# little-lang (actual name TBD)
little-lang is a simple interpreted programming language inspired by an amalgamation of Ruby, Go, Python, and Lisp.
Currently, it provides duck typing and mutable/const variables with plans to include a class/object and inheritance 
system, garbage collection, and dynamically scoped closures.

## Overview

### Basics

At the moment little-lang can only interpret whole source programs but a REPL will be implemented.

```
$ cat hello-world.ll
println("Hello", "World!")
$ little-lang ./hello-world.ll
Hello World!
```

Expressions are separated by either a semicolon or until the expression fails to parse. The only type of comments
are ```# until end of line``` comments.

### Compiling

So far, testing has only been done with clang version 3.6.0 on Ubuntu 15.04 x86_64.

```
$ git clone git@github.com:traplol/little-lang.git
$ cd little-lang
$ make
$ ./bin/little-lang tests/little-lang/fizzbuzz.ll
```

### Builtins

#### Nil
```nil``` -- anything may point to it, it's a long value that only evaluates to itself.

#### Booleans
little-lang has no concept if "truthy" values, booleans only evaluate to themselves, e.g:
	```1, 0, "true", "false", nil```
none of these evaluate to a boolean value.

Either ```true``` or ```false```, these values can only be acquired through literal assignment or logical 
comparisons.

#### Reals
64-bit floating point values.

#### Integers
32-bit signed integers that can currently only be represented in decimal.

#### Strings
Strings are immutable, all string manipulation is done through copying.

### Builtin functions

```string(x)``` -- Returns a String object, takes one argument, and attempts to represent the value as a 
string. If ```string``` is unable to represent the value as a string it returns the value's type name.

```print(...)``` -- Returns ```nil```, takes any arguments, and prints the ```string``` representation of each 
argument separated with a space.

```println(...)``` -- Returns ```nil```, takes any arguments, calls ```print``` and adds a ```newline``` character
to the end.

```type(x)``` -- Returns a String object, takes one argument, and returns the value's type name.
