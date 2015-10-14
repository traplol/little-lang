class Node {
    mut next, data
    def new(self, data) {
        self.data = data
    }

    def __str__(self) {
        string(self.data)
    }
}

class LinkedList {
    mut head, tail

    def push_back(self, data) {
        mut node = Node.new(data)
        if self.head == nil {
            self.head = node
            self.tail = self.head
        }
        else {
            self.tail.next = node
            self.tail = node
        }
    }

    def pop_back(self) {
        self.delete(self.tail)
    }

    def delete(self, node) {
        if node == self.tail {
            mut c = self.head
            while c.next != self.tail {
                c = c.next
            }
            c.next = nil
            self.tail = c
        }
        else {
            node.data = node.next.data
            node.next = node.next.next
        }
    }

    def find(self, object) {
        mut cur = self.head
        while cur.data != object {
            cur = cur.next
        }
        cur
    }

    def print(self) {
        mut cur = self.head
        while cur != nil {
            print(cur)
            if cur.next != nil {
                print(", ")
            }
            cur = cur.next
        }
        println()
    }
}

mut ll = LinkedList.new()
for mut i = 0; i < 10; i = i + 1 {
    ll.push_back(i)
}
ll.print()
ll.pop_back()
ll.print()

mut n = ll.find(2)
ll.delete(n)
ll.print()
