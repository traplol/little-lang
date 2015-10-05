# test1
if true && println("test1-cond") == nil {
    println("test1-body")
}

# test2
if false && println("test2-cond") == nil {
    println("test2-body")
}

#test3
if true || println("test3-cond") == nil {
    println("test3-body")
}

#test4
if false || println("test4-cond") == nil {
    println("test4-body")
}
