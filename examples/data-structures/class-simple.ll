class Simple {
    mut MyVal = 42
}

mut simple = Simple.new()
println(simple.MyVal)
simple.MyVal = 54
println(simple.MyVal)

mut other = Simple.new()
other.MyVal = simple.MyVal
println(other.MyVal, "==", simple.MyVal, other.MyVal == simple.MyVal)

