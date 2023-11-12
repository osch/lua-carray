c=require"carray"
a=c.new"char"
assert(10==a:appendfile("test02.data"))
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

n = c.new("char")
assert(n:appendfile("test02.data") == 10)
assert(a:equals(n))

n = c.new("char")
assert(n:appendfile("test02.data", 3) == 3)
assert(not a:equals(n))
assert(c.new("char"):appendsub(a, 1, 3):equals(n))
assert(n:tostring() == "123")

n = c.new("char")
f = io.open("test02.data", "rb")
assert(n:appendfile(f) == 10)
assert(a:equals(n))
assert(f:close())


n = c.new("char")
f = io.open("test02.data", "rb")
assert(n:appendfile(f, 4) == 4)
assert(not a:equals(n))
assert(c.new("char"):appendsub(a, 1, 4):equals(n))
assert(n:tostring() == "1234")

n:reset()
assert(n:appendfile(f) == 6)
assert(not a:equals(n))
assert(n:tostring() == "56789\n")
assert(c.new("char"):appendsub(a, 5, 10):equals(n))

assert(n:appendfile(f) == 0)
assert(f:close())

ok, err = pcall(function() n:appendfile(f) end)
assert(not ok and err:match('invalid file'))

assert(n:tostring() == "56789\n")

print("test02 OK.")
