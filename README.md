# lua-carray 
[![Licence](http://img.shields.io/badge/Licence-MIT-brightgreen.svg)](LICENSE)
[![Install](https://img.shields.io/badge/Install-LuaRocks-brightgreen.svg)](https://luarocks.org/modules/osch/carray)

<!-- ---------------------------------------------------------------------------------------- -->

This [Lua] module provides an array data type together with a [C API](./src/carray_capi.h)
for handling arrays of primitive numeric C data types in Lua script code and also in native 
C code for enhancing native Lua module interoperability. 

[Lua]: https://www.lua.org

<!-- ---------------------------------------------------------------------------------------- -->

## First Example

 ```lua
local carray = require"carray"

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

local c = carray.new("char", 10)
c:setstring(1, "1234567890")

assert(c:get(1) == string.byte("1"))
assert(c:tostring() == "1234567890")
assert(c:tostring(1) == "1")
assert(c:tostring(2,4) == "234")

c:setstring(4, "abc")
assert(c:tostring() == "123abc7890")
```

<!-- ---------------------------------------------------------------------------------------- -->