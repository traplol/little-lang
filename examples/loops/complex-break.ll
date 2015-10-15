mut itrs, prints = 0, 0

for mut i = 0; i < 10; i = i + 1 {
    itrs = itrs + 1
    println("i =", i) # 0..9
    prints = prints + 1
    for mut j = 0; j < 10; j = j + 1 {
        itrs = itrs + 1
        if j == 5 {
            break
        }
        println("\tj =", j) # should be odds
        prints = prints + 1
        for mut k = 0; k < 10; k = k + 1 {
            itrs = itrs + 1
            break
            println("\t\tk =", k) # never runs
            prints = prints + 1
        }
    }
}

println("Total iterations:", itrs)
println("Total prints:", prints)

