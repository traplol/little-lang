import "assert.ll" as t

mut x, i = 1, 1

while i <= 10 {
    x = x * i
    i = i + 1
}
t.assert(3628800, x, "10!")

x = 1
i = 10
while i > 0 {
    x = x * i
    i = i - 1
}
t.assert(3628800, x, "10!")