#include "lexer.h"
#include "result.h"

#include "helpers/strings.h"
#include "helpers/io.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

const char *REPLPrompt_Begin = ">";
const char *REPLPrompt_Secondary = ".";


#define STR_EQ(a,b) (0 == strcmp(a, b))
#define STRN_EQ(a,b,n) (0 == strncmp(a, b, n))

#define LEX_ADVN(l, n)                          \
    do {                                        \
        (l)->CurrentColumnNumber += (n);        \
        if('\n' == *(l)->Pos) {                 \
            (l)->CurrentLineNumber++;           \
            (l)->CurrentColumnNumber = 1;       \
        }                                       \
        (l)->Pos += (n);                        \
    } while (0)

#define LEX_ADV(l) LEX_ADVN(l, 1)

#define LEX_ADVE(l, e)                          \
    do {                                        \
        LEX_ADV(l);                             \
        (e++);                                  \
    } while (0)
        

/************************ Helpers ************************/
int LexerIsValid(struct Lexer *lexer) {
    return lexer && lexer->Filename;
}

int LexerIsInvalid(struct Lexer *lexer) {
    return !LexerIsValid(lexer);
}

int IsWhitespace(int c) {
    if ('\n' == c) return 0;
    return isspace(c);
}
int IsDigit(int c) {
    return '0' <= c && c <= '9';
}
int IsHexDigit(int c) {
    return IsDigit(c) ||
        ('a' <= c && c <= 'f') ||
        ('A' <= c && c <= 'F');
}
int IsAlpha(int c) {
    return ('a' <= c && c <= 'z') ||
        ('A' <= c && c <= 'Z');
}
int IsAlphaNum(int c) {
    return IsAlpha(c) || IsDigit(c);
}
int IsIdentStartChar(int c) {
    return '_' == c || IsAlpha(c);
}
int IsIdentChar(int c) {
    return IsIdentStartChar(c) || IsDigit(c);
}

int LexerParseString(struct Lexer *lexer, enum TokenType *out_type, char **out_str) {
    char *begin, *end, *str, *tmp;
    unsigned int len;
    end = begin = lexer->Pos;
    LEX_ADVE(lexer, end); /* Eat the beginning " */
    while (*end && *end != '"') {
        if ('\\' == *end && '"' == *(end+1)) {
            LEX_ADVE(lexer, end);
        }
        LEX_ADVE(lexer, end);
    }
    LEX_ADVE(lexer, end);
    len = end - begin;
    str = malloc(len+1);
    tmp = str;
    /* Copy into str */
    while (begin < end) {
        *tmp++ = *begin++;
    }
    *tmp = 0;
    lexer->Pos = begin;
    *out_type = TokenStringLiteral;
    *out_str = str;
    return 0;
}
int LexerParseIdentOrKeyword(struct Lexer *lexer, enum TokenType *out_type, char **out_str) {
    char *begin, *end, *str, *tmp;
    unsigned int len;
    enum TokenType type;
    end = begin = lexer->Pos;
    while (*end && IsIdentChar(*end)) {
        LEX_ADVE(lexer, end);
    }
    len = end - begin;
    str = malloc(len+1);
    tmp = str;
    /* Copy into str */
    while (begin < end) {
        *tmp++ = *begin++;
    }
    *tmp = 0;
    if (STR_EQ(str, "def")) { type = TokenDef; }
    else if (STR_EQ(str, "import")) { type = TokenImport; }
    else if (STR_EQ(str, "as")) { type = TokenAs; }
    else if (STR_EQ(str, "mut")) { type = TokenMut; }
    else if (STR_EQ(str, "const")) { type = TokenConst; }
    else if (STR_EQ(str, "return")) { type = TokenReturn; }
    else if (STR_EQ(str, "break")) { type = TokenBreak; }
    else if (STR_EQ(str, "continue")) { type = TokenContinue; }
    else if (STR_EQ(str, "class")) { type = TokenClass; }
    else if (STR_EQ(str, "if")) { type = TokenIf; }
    else if (STR_EQ(str, "else")) { type = TokenElse; }
    else if (STR_EQ(str, "for")) { type = TokenFor; }
    else if (STR_EQ(str, "while")) { type = TokenWhile; }
    else if (STR_EQ(str, "true")) { type = TokenTrue; }
    else if (STR_EQ(str, "false")) { type = TokenFalse; }
    else if (STR_EQ(str, "nil")) { type = TokenNil; }
    else { type = TokenIdentifer; }
    lexer->Pos = begin;
    *out_type = type;
    *out_str = str;
    return R_OK;
}
int LexerParseNumber(struct Lexer *lexer, enum TokenType *out_type, char **out_str) {
    char *begin, *end, *str, *tmp;
    unsigned int len;
    enum TokenType type;
    int fp = 0;
    end = begin = lexer->Pos;

    while (IsDigit(*end) || ('.' == *end && IsDigit(*(end+1)))) {
        if ('.' == *end && IsDigit(*(end+1))) {
            LEX_ADVE(lexer, end);
            fp = 1;
            while (IsDigit(*end)) {
                LEX_ADVE(lexer, end);
            }
            goto done;
        }
        LEX_ADVE(lexer, end);
    }

done:
    len = end - begin;
    str = malloc(len+1);
    tmp = str;
    /* Copy into str */
    while (begin < end) {
        *tmp++ = *begin++;
    }
    *tmp = 0;
    type = fp ? TokenRealConstant : TokenIntegerConstant;

    lexer->Pos = begin;
    *out_type = type;
    *out_str = str;
    return R_OK;
}
int LexerParseOther(struct Lexer *lexer, enum TokenType *out_type, char **out_str) {
    char *str;
    unsigned int adv = 0;
    enum TokenType type;
    str = lexer->Pos;
    while (*str && IsWhitespace(*str)) {
        LEX_ADVE(lexer, str);
    }
    
    if (STRN_EQ("..", str, 2)) { type = TokenDotDot; adv = 2;}
    else if (STRN_EQ("==", str, 2)) { type = TokenEqEq; adv = 2;}
    else if (STRN_EQ("!=", str, 2)) { type = TokenBangEq; adv = 2;}
    else if (STRN_EQ("&&", str, 2)) { type = TokenAmpAmp; adv = 2;}
    else if (STRN_EQ("||", str, 2)) { type = TokenBarBar; adv = 2;}
    else if (STRN_EQ("<=", str, 2)) { type = TokenLtEq; adv = 2;}
    else if (STRN_EQ(">=", str, 2)) { type = TokenGtEq; adv = 2;}
    else if (STRN_EQ("**", str, 2)) { type = TokenStarStar; adv = 2;}
    else if (STRN_EQ("<<", str, 2)) { type = TokenLtLt; adv = 2;}
    else if (STRN_EQ(">>", str, 2)) { type = TokenGtGt; adv = 2;}
    else if (STRN_EQ(",", str, 1)) { type = TokenComma; adv = 1;}
    else if (STRN_EQ(";", str, 1)) { type = TokenSemicolon; adv = 1;}
    else if (STRN_EQ("{", str, 1)) { type = TokenLeftCurlyBrace; adv = 1;}
    else if (STRN_EQ("}", str, 1)) { type = TokenRightCurlyBrace; adv = 1;}
    else if (STRN_EQ("(", str, 1)) { type = TokenLeftParen; adv = 1;}
    else if (STRN_EQ(")", str, 1)) { type = TokenRightParen; adv = 1;}
    else if (STRN_EQ("[", str, 1)) { type = TokenLeftSqBracket; adv = 1;}
    else if (STRN_EQ("]", str, 1)) { type = TokenRightSqBracket; adv = 1;}
    else if (STRN_EQ(".", str, 1)) { type = TokenDot; adv = 1;}
    else if (STRN_EQ("=", str, 1)) { type = TokenEquals; adv = 1;}
    else if (STRN_EQ("!", str, 1)) { type = TokenBang; adv = 1;}
    else if (STRN_EQ("+", str, 1)) { type = TokenPlus; adv = 1;}
    else if (STRN_EQ("-", str, 1)) { type = TokenMinus; adv = 1;}
    else if (STRN_EQ("*", str, 1)) { type = TokenAsterisk; adv = 1;}
    else if (STRN_EQ("/", str, 1)) { type = TokenSlash; adv = 1;}
    else if (STRN_EQ("%", str, 1)) { type = TokenPercent; adv = 1;}
    else if (STRN_EQ("^", str, 1)) { type = TokenCaret; adv = 1;}
    else if (STRN_EQ("&", str, 1)) { type = TokenAmp; adv = 1;}
    else if (STRN_EQ("|", str, 1)) { type = TokenBar; adv = 1;}
    else if (STRN_EQ("<", str, 1)) { type = TokenLt; adv = 1;}
    else if (STRN_EQ(">", str, 1)) { type = TokenGt; adv = 1;}
    else if (STRN_EQ("\n", str, 1)) { goto handle_newline; }
    else if (!*str) { goto end_of_stream; }
    else { type = TokenUnknown; }

    str = strndup(str, adv);
    LEX_ADVN(lexer, adv);
    *out_type = type;
    *out_str = str;
    return R_OK;

handle_newline:
    LEX_ADVN(lexer, 1);
    *out_type = TokenNewline;
    *out_str = strdup("<newline>");
    return R_OK;

end_of_stream:
    str = strdup("<EOS>");
    *out_type = TokenEOS;
    *out_str = str;
    lexer->REPLPrompt = REPLPrompt_Begin;
    return R_OK;
}

int LexerParseThing(struct Lexer *lexer, enum TokenType *out_type, char **out_str) {
    char *p = lexer->Pos;
    if ('"' == *p) {
        return LexerParseString(lexer, out_type, out_str);
    }
    else if (IsIdentStartChar(*p)) {
        return LexerParseIdentOrKeyword(lexer, out_type, out_str);
    }
    else if (IsDigit(*p)) {
        return LexerParseNumber(lexer, out_type, out_str);
    }
    return LexerParseOther(lexer, out_type, out_str);
}

struct Token *LexerGetNextToken(struct Lexer *lexer, int consumeToken) {
    struct Token *token;
    int line, column;
    char *out_str, *oldPos;
    unsigned int oldCurrentLineNumber, oldCurrentColumnNumber;
    enum TokenType out_type;
non_recursive_call:
    while (*lexer->Pos && IsWhitespace(*lexer->Pos)) 
        LEX_ADV(lexer);
    if (*lexer->Pos == '#') {
        while (*lexer->Pos && '\n' != *lexer->Pos) {
            LEX_ADV(lexer);
        }
        goto non_recursive_call;
    }
    token = malloc(sizeof *token);
    oldPos = lexer->Pos;
    oldCurrentLineNumber = line = lexer->CurrentLineNumber;
    oldCurrentColumnNumber = column = lexer->CurrentColumnNumber;
    if (R_OK != LexerParseThing(lexer, &out_type, &out_str)) {
        free(token);
        return NULL;
    }
    if (!consumeToken) {
        lexer->Pos = oldPos;
        lexer->CurrentLineNumber = oldCurrentLineNumber;
        lexer->CurrentColumnNumber = oldCurrentColumnNumber;
    }
    TokenMake(token, out_type, out_str, lexer->Filename, line, column);
    free(out_str);
    return token;
}

int LexerSharedGetNext(struct Lexer *lexer, struct Token **out_token, int consume) {
    struct Token *token;
    if (LexerIsInvalid(lexer)) {
        return R_InvalidArgument;
    }
    token = LexerGetNextToken(lexer, consume);
    if (!out_token) {
        TokenFree(token);
        free(token);
        return -1; /* TODO: Maybe this should be fine? */
    }
    *out_token = token;
    return R_OK;
}
int LexerSharedGetNextREPL(struct Lexer *lexer, struct Token **out_token, int consume) {
    char *buf;
    const int inc = 1024;
    int c, i, len, size = inc;
    while (lexer->Pos && IsWhitespace(*lexer->Pos)) {
        LEX_ADV(lexer);
    }
    if (lexer->Pos && *lexer->Pos) {
        return LexerSharedGetNext(lexer, out_token, consume);
    }
    free(lexer->Code);
    buf = malloc(size);
    printf("%03d > ", lexer->CurrentLineNumber);
    lexer->REPLPrompt = REPLPrompt_Secondary;
    for (i = 0; EOF != (c = getchar());) {
        if (i + 1 >= size) {
            size += inc;
            buf = realloc(buf, inc);
        }
        buf[i++] = c;
        if ('\n' == c) {
            break;
        }
    }
    buf[i] = 0;
    len = strlen(buf);
    buf = realloc(buf, 1 + len);
    lexer->Code = buf;
    lexer->Pos = lexer->Code;
    lexer->Length = len;
    return LexerSharedGetNext(lexer, out_token, consume);
}

/*********************** Public Functions ************************/

int LexerMake(struct Lexer *lexer, char *filename, char *code) {
    if (!lexer || !filename) {
        return R_InvalidArgument;
    }
    lexer->Filename = strdup(filename);
    lexer->Code = code ? strdup(code) : NULL;
    lexer->Pos = lexer->Code;
    lexer->Length = !lexer->Code ? 0 : strlen(lexer->Code);
    lexer->CurrentLineNumber = 1;
    lexer->CurrentColumnNumber = 1;
    lexer->REPL = 0;
    lexer->REPLPrompt = REPLPrompt_Begin;
    return R_OK;
}

int LexerFree(struct Lexer *lexer) {
    if (LexerIsInvalid(lexer)) {
        return R_InvalidArgument;
    }
    //free(lexer->Filename); /* TODO: Handle freeing the filenames better */
    free(lexer->Code);
    return R_OK;
}

int LexerThrowAwayCode(struct Lexer *lexer) {
    if (LexerIsInvalid(lexer)) {
        return R_InvalidArgument;
    }
    free(lexer->Code);
    lexer->Code = NULL;
    lexer->Pos = lexer->Code;
    lexer->Length = 0;
    return R_OK;
}

int LexerNextToken(struct Lexer *lexer, struct Token **out_token) {
    if (!lexer->REPL) {
        return LexerSharedGetNext(lexer, out_token, 1);
    }
    return LexerSharedGetNextREPL(lexer, out_token, 1);
}

int LexerPeekToken(struct Lexer *lexer, struct Token **out_token) {
    if (!lexer->REPL) {
        return LexerSharedGetNext(lexer, out_token, 0);
    }
    return LexerSharedGetNextREPL(lexer, out_token, 0);
}
