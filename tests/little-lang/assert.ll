def assert(expected, actual, msg) {
    print("(", msg, ") ==", actual)
    if (expected == actual) {
         println(": PASSED")
    }
    else {
         println(": FAILED")
    }
}
