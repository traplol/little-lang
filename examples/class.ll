class MyClass {
    mut name, age = "Default", -1

    def New(name, age) {
        name = name
        age = age
    }

    def Greet {
        println("Hello,", name)
    }
}
