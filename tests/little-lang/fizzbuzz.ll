def fizzbuzz(n) {
    for mut x = 1; x <= n; x = x + 1 {
        if x % 15 == 0 {
            println("fizzbuzz")
        }
        else if x % 3 == 0 {
            println("fizz")
        }
        else if x % 5 == 0 {
            println("buzz")
        }
        else {
            println(x)
        }
    }
}

#fizzbuzz(100)
