import "assert.ll" as t

mut x = 1

for mut i = 1; i <= 10; i = i + 1 {
    x = x * i
}
t.assert(3628800, x, "10!")

x = 1
for mut i = 10; i > 0; i = i - 1 {
    x = x * i
}
t.assert(3628800, x, "10!")