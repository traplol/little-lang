class HashTable {
    mut table

    def new(self, size) {
        self.table = Vector.new(size)
    }

    def index(self, key) {
        hash(key) % self.table.length()
    }

    def set(self, key, value) {
        mut idx = self.index(key)
        self.table[idx] = value
    }

    def get(self, key) {
        mut idx = self.index(key)
        self.table[idx]
    }

    def __str__(self) {
        string(self.table)
    }
}

mut h = HashTable.new(10)
h.set("a", 42)
h.set("b", 123)
h.set("c", 444)

println(h.get("a"))
println(h.get("b"))
println(h.get("c"))

println(h)
