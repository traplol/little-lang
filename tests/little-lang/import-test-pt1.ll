import "import-test-pt2.ll" as it

println("this is ran when import-test-pt1 is loaded")
it.Init()

def func(a) {
    println("func(", a, ")")
    it.Print(a)
}

def pi {
    println("PI:", it.PI)
}
