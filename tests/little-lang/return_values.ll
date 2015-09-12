def returns_42 {
    42
}

def returns_x {
    mut x = 0
    while x < 10 {
        x = x + 1
    }
}

def for_loop_return {
    for mut x = 0; x < 42; x = x + 1 {
        type(x * 1.0)
    }
}

def max(x, y) {
    if x > y {
        x
    }
    else {
        y
    }
}

println(returns_42())
println(returns_x())
println(for_loop_return())
println(max(1, 2), max(2, 1), max(1,1))
