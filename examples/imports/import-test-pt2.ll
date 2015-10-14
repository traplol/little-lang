println("this is ran when import-test-pt2 is loaded!")

const PI = 3.14159265359

def Init {
    println("hello from import-test-pt2.Init()!")
    println("import-test-pt2 initialized!")
}

def Print(p) {
    println("You sent me a", type(p), "=", p)
}

