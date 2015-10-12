class MyClass {
    mut name, age
    mut language = "English"
    const theAnswer = 42

    def New(self, name, age) {
        self.name = name
        self.age = age
    }

    def Greet(self) {
        println("Hello,", self.name)
        println("You speak " + self.language + ".")
        println("The answer is", self.theAnswer)
    }
}

