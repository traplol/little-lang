#include "../src/token.c"

#include "c_test.h"
#include "test_helpers.h"

TEST(parse_escaped_string) {
    char *str = parse_escaped_string("\"hello world\"");
    assert_str_eq("hello world", str, "Did not parse \"hello world\" correctly.");
    free(str);

    str = parse_escaped_string("\"\\\\\"");
    assert_str_eq("\\", str, "Did not parse \"\\\\\" correctly.");
    free(str);

    str = parse_escaped_string("\"\\ttest\\n\"");
    assert_str_eq("\ttest\n", str, "Did not parse \"\\ttest\\n\" correctly.");
    free(str);
}

int main() {
    TEST_RUN(parse_escaped_string);
    return 0;
}
