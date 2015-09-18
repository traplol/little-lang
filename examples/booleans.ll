def test(expected, actual) {
    mut passed = "Passed"
    if actual != expected {
        passed = "Failed"
    }
    println("Got:", actual,"Expected:", expected, passed)
}

test(true, true || true && false)
test(true, true || false || true)
test(true, false || true && true)
test(false, false || false && false)

test(false, true && false)
test(false, false && true)
test(false, false && false)
test(true, true && true || true)
test(true, true && false || true)
test(true, false && true || true)
test(true, false && false || true)

