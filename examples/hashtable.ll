class HashTable {
    mut table

    def new(self, size) {
        self.table = Vector.new(size)
    }

    def index(self, key) {
        hash(key) % self.table.length()
    }

    def put(self, key, value) {
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
h.put("a", 42)
h.put("b", 123)
h.put("c", 444)

println(h.get("a"))
println(h.get("b"))
println(h.get("c"))

println(h)
