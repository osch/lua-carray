c=require"carray"
a=c.new"char"
assert(a==a:appendfile("test02.data"))
assert(a:len() == 10)
assert(a:tostring(1,-1) == "123456789\n")
a:reset():appendfile("test02.data", 2)
assert(a:len() == 2)
assert(a:tostring(1,-1) == "12")
f=io.open("test02.data", "rb")

a:reset():appendfile(f, 2)
assert(a:len() == 2)
assert(a:tostring(1,-1) == "12")

a:appendfile(f, 2)
assert(a:len() == 4)
assert(a:tostring(1,-1) == "1234")

a:appendfile(f, 10)
assert(a:len() == 10)
assert(a:tostring(1,-1) == "123456789\n")

assert(a:equals(c.new("char"):appendfile("test02.data")))

assert(not a:equals(c.new("char"):appendfile("test02.data", 3)))

print("test02 OK.")
