# lua-carray 
[![Licence](http://img.shields.io/badge/Licence-MIT-brightgreen.svg)](LICENSE)
[![build status](https://github.com/osch/lua-carray/workflows/test/badge.svg)](https://github.com/osch/lua-carray/actions/workflows/test.yml)
[![Build status](https://ci.appveyor.com/api/projects/status/v8t6rsf45dwt60pl/branch/master?svg=true)](https://ci.appveyor.com/project/osch/lua-carray/branch/master)
[![Install](https://img.shields.io/badge/Install-LuaRocks-brightgreen.svg)](https://luarocks.org/modules/osch/carray)

<!-- ---------------------------------------------------------------------------------------- -->

This [Lua] module provides an array data type and the implementation of the [Carray C API] 
for handling arrays of primitive numeric C data types in Lua script code and also in native 
C code for enhancing native Lua module interoperability and performance. 

   * [Documentation](./doc/README.md#lua-carray-documentation)

[Lua]:          https://www.lua.org
[Carray C API]: https://github.com/lua-capis/lua-carray-capi

<!-- ---------------------------------------------------------------------------------------- -->

## First Example

 ```lua
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

local c = carray.new("char"):append("1234567890")

assert(c:get(1) == string.byte("1"))
assert(c:tostring() == "1234567890")
assert(c:tostring(1,1) == "1")
assert(c:tostring(2,4) == "234")

c:set(4, "ab", string.byte("c"))
assert(c:tostring() == "123abc7890")
```

<!-- ---------------------------------------------------------------------------------------- -->
