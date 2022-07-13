local carray = require("carray")

local a = carray.new("int", 10)

for i = 1, a:len() do
    assert(a:get(i) == 0)
end

for i = 1, a:len() do
    a:set(i, 100 + i)
end

for i = 1, a:len() do
    assert(a:get(i) == 100 + i)
end

local x, y, z = a:get(2, 4)
assert(x == 102)
assert(y == 103)
assert(z == 104)


local x, y, z = a:get(-3, -1)
assert(x == 108)
assert(y == 109)
assert(z == 110)

a:set(2, 202, 203, 204)

local x, y, z = a:get(2, 4)
assert(x == 202)
assert(y == 203)
assert(z == 204)

local c = carray.new("char"):add("1234567890")

assert(c:get(1) == string.byte("1"))
assert(c:tostring() == "1234567890")
assert(c:tostring(1,1) == "1")
assert(c:tostring(2,4) == "234")

c:set(4, "ab", string.byte("c"))
assert(c:tostring() == "123abc7890")

