# little-lang (actual name TBD)
little-lang is a simple interpreted programming language inspired by an amalgamation of Ruby, Go, Python, and Lisp.
Currently, it provides duck typing and mutable/const variables with plans to include a class/object and inheritance 
system, garbage collection, and dynamically scoped closures.

## Overview

### Basics

little-lang can interpret whole source programs or be used as a REPL.

```
$ cat hello-world.ll
println("Hello", "World!")
$ little-lang ./hello-world.ll
Hello World!
$ little-lang
001 > println("Hello world!")
Hello world!
 => nil
002 > 
```

The little-lang REPL can also be preloaded with a program.

```
$ cat examples/fib.ll 
def fib(n) {
    if n < 2 {
        n
    }
    else {
        fib(n - 2) + fib(n - 1)
    }
}

println(fib(30))

$ little-lang examples/fib.ll -i
832040
011 > fib(26)
 => 121393
012 > 
```

*NOTE:* Due to the ambiguity of parsing something like:
```
001 > if 1 + 1 == 2 {
002 > 	print("yes!")
003 > }
004 > 
[...]
009 > else {
010 >   print("wut?")
011 > }
yes! => nil
```
solo ```if``` expressions should be "closed" with a semicolon like:
```
001 > if 1 + 1 == 2 {
002 > 	print("yes!")
003 > }; # Notice this semicolon!
yes! => nil
```
*NOTE:* This is only necessary within the REPL.

Expressions are separated by either a semicolon or a newline. The only type of comments
are ```# until end of line``` comments.

### Compiling

So far, testing has only been done with clang version 3.6.0 on Ubuntu 15.04 x86_64.

```
$ git clone git@github.com:traplol/little-lang.git
$ cd little-lang
$ make
$ ./bin/little-lang examples/fizzbuzz.ll
```

### Builtins

#### Nil
```nil``` -- anything may point to it, it's a lone value that only evaluates to itself.

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

### Constructs

#### Defining variables
All variables must be defined as either ```mut``` or ```const``` and have an initial value assigned to them. 
```mut``` variables are mutable and can have their values and even types changed. ```const``` variables, on the 
other hand, cannot change value or be modified.

```mut``` can take a comma separated list of names and values for assignment and will fail to parse if there is
a mismatched number of values/names.

```
mut x, y, z = 1, 2, 3
print(x, y, z)        # prints "1 2 3"

mut a, b, c = 4, 5    # throws an error before program execution.

mut x, x, z = 1, 2, 3 # throws an error at runtime for trying to redeclare an existing variable.
```

```const```, on the other hand, expects only one name and one value.

```
const fifty = 50
fifty = 40 # throws a runtime error.
```

#### Control flow
Standard ```if/else/else if``` construct.

```
if x < y {
	println("x is greater!")
}
else if x > y {
	println("y is greater!")
}
else if x == y {
	println("x and y are the same!")
}
else {
	println("this shouldn't happen...")
}
```

Note that the ```else``` isn't required.

```
if x == y {
	println("x is equal to y")
}
```

#### Loops

The two looping constructs are ```for``` and ```while``` loops.
```
for mut x = 1; x < 10; x = x + 1 {
	println(x)
}
# notice that this doesn't throw a runtime error because the `x' 
# declared in the for loop has gone out of scope.
mut x = 10
while x > 0 {
	println(x)
	x = x - 1
}
```

#### Function definitions

```
def MyPrint(a, b, c) {
	print(a, b, c)
}
```

or optionally, the parameters list may be omitted

```
def HelloWorld {
	println("Hello world!")
}
```

The return value of a function is the value of the last statement executed, even from loops
or conditional blocks.

```
def returns_42 {
	42
}
println(returns_42()) # prints "42"

def max(x, y) {
    if x > y {
        x
    }
    else {
        y
    }
}
println(max(99, 100), max(50, 49), max(10, 10)) # prints "100 50 10"

def returns_x {
    mut x = 0
    while x < 10 {
        x = x + 1
    }
}
println(returns_x()) # prints "10"
```

#### Imports
A rudimentary module import system is in place to allow for multiple source file programs where each file may be
considered as a namespace. All imports will happen in the order they appear in the source code, however they will execute before the main program does.

```
import "path/to/file.ll" as libName

libName.SomeFunc()
```

*NOTE:* There are currently no cycle/loop checks for recursive imports which will result in a stack overflow in the 
interpreter.
