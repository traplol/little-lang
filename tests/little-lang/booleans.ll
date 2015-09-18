import "assert.ll" as t

# logical not
t.assert(true, true, "true")
t.assert(false, !true, "!true")
t.assert(true, !!true, "!!true")
t.assert(false, false, "false")
t.assert(true, !false, "!false")
t.assert(false, !!false, "!!false")

# logical or
t.assert(true, true || true, "true || true")
t.assert(true, true || false, "true || false")
t.assert(true, false || true, "false || true")
t.assert(false, false || false, "false || false")

# logical and
t.assert(true, true && true, "true && true")
t.assert(false, true && false, "true && false")
t.assert(false, false && true, "false && true")
t.assert(false, false && false, "false && false")
