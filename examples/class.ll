class MyClass {
    mut name, age
    mut language = "English"
    const theAnswer = 42

    def New(name, age) {
        name = name
        age = age
    }

    def Greet {
        println("Hello,", name)
    }
}

