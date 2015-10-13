class MyClass {
    mut name, age
    mut language = "English"
    const theAnswer = 42

    def new(self, name, age) {
        self.name = name
        self.age = age
    }

    def Greet(self) {
       println("Hello,", self.name)
       if self.age == 1 {
           println("You are 1 year old!")
       }
       else {
           println("You are", self.age, "years old!")
       }
       println("You speak " + self.language + ".")
       println("The answer is " + string(self.theAnswer) + ".")
    }
}

mut mc = MyClass.new("Max", 24)
mc.Greet()

