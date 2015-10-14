
def test(a, b) {
    if a < b {
        "if"
    }
    else if a > b {
        "else if"
    }
    else {
        "else"
    }
}


def a(a, b, c) {
    a
}

def b(a, b, c) {
    b
}

def c(a, b, c) {
    c
}

def abc() {
    println("hello world!")
    string(a(1, 2, 3) + b(4, 5, 6) + c(7, 8, 9))
}

println(test(1,2))
println(test(2,1))
println(test(1,1))

println(abc(), 1 + 5 + 9)
println("type(true) =", type(true))
println("type(false) =", type(false))
println("type(abc) =", type(abc))
println("type(abc()) =", type(abc()))
println("type(1) =", type(1))
println("type(3.14159) =", type(3.14159))
