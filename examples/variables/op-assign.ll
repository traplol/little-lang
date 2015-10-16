class Test {
    mut var = 42
    def __str__(self) {
        string(self.var)
    }
}

# usage with member access
mut t = Test.new()
println(t)
t.var += 10
println(t)

# usage with index access
mut v = Vector.new()
v[0] = 3
println(v)
v[0] **= 3
println(v)

# standard usage
mut i = 1
println("i =", i)

i += 1
println("i += 1:", i)

i -= 1
println("i -= 1:", i)

i *= 5
println("i *= 5:", i)

i /= 2
println("i /= 2:", i)

i %= 2
println("i %= 2:", i)

i ^= 4
println("i ^= 4:", i)

i &= 3
println("i &= 3:", i)

i |= 3
println("i |= 3:", i)

i <<= 3
println("i <<= 3:", i)

i >>= 3
println("i >>=3:", i)

i **= i
println("i **= i:", i)

