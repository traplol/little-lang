mut itrs = 0
mut never = "never"
def test {
    for mut i = 0; i < 5; i = i + 1 {
        itrs = itrs + 1
        return 42
        never = "happened"
    }
    "wat"
}

println("return in for loop")
println(test())
println(itrs)
println(never)

println("\n### WHILE ###")
println("continue:")
mut i = 0;
while i < 10 {
    if i % 2 == 0 {
        i = i + 1
        continue
    }
    println(i)
    i = i + 1
}

println("\nbreak:")
i = 0;
while i < 10 {
    if i == 5 {
        break
    }
    println(i)
    println("abc")
    i = i + 1
}

println("\n### FOR ###")
println("continue:")
for mut i = 0; i < 10; i = i + 1 {
    if i % 2 == 0 {
        continue
    }
    println(i)
}

println("\nbreak:")
for mut i = 0; i < 10; i = i + 1 {
    if i == 5 {
        break
    }
    println(i)
    println("abc")
}
