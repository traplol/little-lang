def vector_test(n) {
    mut v = Vector.new()
    for mut i = 0; i < n; i = i + 1 {
        v << i
    }
    for mut i = 0; i < v.length(); i = i + 1 {
        println(i)
    }
}

vector_test(20)
