def vector_test(n) {
    mut v = Vector.new()
    for mut i = 0; i < n; i = i + 1 {
        v << i
    }
    for mut i = 0; i < v.length(); i = i + 1 {
        println(i)
    }
    println(v)
    v
}

mut v = vector_test(10)

v[0] = true
v[v.length()-1] = 42
println(v)

