# test1 should assert the println in the condition
if true && println("test1-cond") == nil {
    println("test1-body")
}

# test2 should not assert the println in the condition
# nor should it print the body
if false && println("test2-cond") == nil {
    println("test2-body")
}

#test3 should not assert the println in the condition
if true || println("test3-cond") == nil {
    println("test3-body")
}

#test4 should assert the println in the condition
if false || println("test4-cond") == nil {
    println("test4-body")
}

