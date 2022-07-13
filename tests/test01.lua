local carray = require("carray")

local format = string.format

local function PRINT(s)
    print(s.." ("..debug.getinfo(2).currentline..")")
end
local function msgh(err)
    return debug.traceback(err, 2)
end
local function pcall(f, ...)
    return xpcall(f, msgh, ...)
end

local function assertArrayEqual(a1, a2)
    assert(a1:len() == a2:len())
    for i = 1, a1:len() do
        assert(a1:get(i), a2:get(i))
    end
end
local function maxIndex(a)
    local rslt = 0
    for k, v in pairs(a) do
        if k > rslt then
            rslt = k
        end
    end
    return rslt
end
local function assertListEqual(a1, a2)
    local l1 = maxIndex(a1)
    local l2 = maxIndex(a2)
    assert(l1 == l2, format("%d <> %d", l1, l2))
    for i = 1, l1 do
        assert(a1[i] == a2[i])
    end
end
local function assertArrayIs(a1, ...)
    assert(a1:len() == select("#", ...))
    for i = 1, a1:len() do
        assert(a1:get(i), select(i, ...))
    end
end
local function nlist(...)
    local rslt = {}
    rslt.n = select("#", ...)
    for i = 1, rslt.n do
        rslt[i] = select(i, ...)
    end
    return rslt
end
local function assertNlistEquals(len, nlist, a)
    assert(len == nlist.n)
    local alen = maxIndex(a)
    assert(alen <= len)
    for i = 1, len do
        assert(nlist[i] == a[i])
    end
end

PRINT("==================================================================================")
do
    local a1 = carray.new("int", 3)
    assert(a1:len() == 3)
    a1:set(1, 101, 102, 103)
    assertArrayIs(a1, 101, 102, 103)
    assertListEqual({a1:get( 1,  3)}, {101, 102, 103})
    assertListEqual({a1:get( 2,  3)}, {102, 103})
    assertListEqual({a1:get( 1, -1)}, {101, 102, 103})
    assertListEqual({a1:get(-3, -1)}, {101, 102, 103})
    assertListEqual({a1:get( 1,  4)}, {101, 102, 103})
    assertListEqual({a1:get( 1, 10)}, {101, 102, 103})
    assertListEqual({a1:get( 1,  0)}, {})
    assertListEqual({a1:get( 1, -3)}, {101})
    assertListEqual({a1:get( 1, -4)}, {})
    assertListEqual({a1:get( 1,-10)}, {})
    assertListEqual({a1:get(-3,  3)}, {101, 102, 103})
    assertListEqual({a1:get(-3,  3)}, {101, 102, 103})

    assertNlistEquals(4, nlist(a1:get(-4, 3)), {nil, 101, 102, 103})

    assertNlistEquals(4, nlist(a1:get(0, 3)), {nil, 101, 102, 103})

    a1:set(1)
    assertArrayIs(a1, 101, 102, 103)

    a1:set(1, 201)
    assertArrayIs(a1, 201, 102, 103)

    local ok, err = pcall(function() a1:set(0) end)
    assert(not ok and err:match("bad argument %#1 to 'set' %(index out of bounds%)"))
    
    local ok, err = pcall(function() a1:set(0, 201) end)
    assert(not ok and err:match("bad argument %#1 to 'set' %(index out of bounds%)"))
    
    local r = a1:reserve() + a1:len()
    a1:reset()
    assert(a1:reserve() == r)
    assert(a1:len() == 0)

    local ok, err = pcall(function() a1:set(1, 201) end)
    assert(not ok and err:match("bad argument %#1 to 'set' %(index out of bounds%)"))
    
    a1:reset():setlen(3):set(-3, 301,302,303)
    assertNlistEquals(3, nlist(a1:get(1,-1)), {301,302,303})    

    a1:reset():setlen(3):set(1, 301,302,303)
    assertNlistEquals(3, nlist(a1:get(1,-1)), {301,302,303})    
    a1:set(-1, 403)
    assertNlistEquals(3, nlist(a1:get(1,-1)), {301,302,403})    

    a1:reset():setlen(3):set(-3, 301,302,303)
    local ok, err = pcall(function() a1:set(-1, 503,504) end)
    assert(not ok and err:match("bad argument %#3 to 'set' %(index out of bounds%)"))

    a1:reset():setlen(3):set(-3, 301,302,303):set(-3, 501)
    assertNlistEquals(3, nlist(a1:get(1,-1)), {501,302,303})    

    a1:reset():setlen(3):set(-3, 301,302,303)
    local ok, err = pcall(function() a1:set(-4, 501) end)
    assert(not ok and err:match("bad argument %#1 to 'set' %(index out of bounds%)"))

end
PRINT("==================================================================================")
do
    local a1 = carray.new("int")
    assert(a1:len() == 0)

    local ok, err = pcall(function() a1:set(1, 101) end)
    assert(not ok and err:match("bad argument %#1 to 'set' %(index out of bounds%)"))
    
    a1:setlen(3)
    assert(a1:len() == 3)
    
    assertArrayIs(a1, 0, 0, 0)

    local ok, err = pcall(function() a1:set(2, 3, 4, 5) end)
    assert(not ok and err:match("bad argument %#4 to 'set' %(index out of bounds%)"))
    
    a1:set(2, 3, 4)

    assertArrayIs(a1, 0, 3, 4)

    a1:setlen(4)
    assertArrayIs(a1, 0, 3, 4, 0)

    a1:setlen(0)
    assert(a1:len() == 0)
end
PRINT("==================================================================================")
do
    local a1 = carray.new("int", 3)
    assert(a1:len() == 3)
    assertArrayIs(a1, 0, 0, 0)

    local r = a1:reserve() + a1:len()
    a1:reset()
    assert(a1:reserve() == r)
    assert(a1:len() == 0)
    
    a1:setlen(4)
    assertArrayIs(a1, 0, 0, 0, 0)
    
    a1:set(3, 103, 104)
    assertArrayIs(a1, 0, 0, 103, 104)
end
PRINT("==================================================================================")
do
    local a1 = carray.new("int", 3)
    assert(a1:len() == 3)
    assertArrayIs(a1, 0, 0, 0)

    a1:reset(true)
    assert(a1:len() == 0)
    assert(a1:reserve() == 0)
    
    a1:setlen(4)
    assertArrayIs(a1, 0, 0, 0, 0)
    
    a1:set(3, 103, 104)
    assertArrayIs(a1, 0, 0, 103, 104)
end
PRINT("==================================================================================")
do
    local a1 = carray.new("int", 3)
    assert(a1:len() == 3)
    assertArrayIs(a1, 0, 0, 0)

    a1:reset(nil)
    assert(a1:len() == 0)
    
    a1:setlen(4)
    assertArrayIs(a1, 0, 0, 0, 0)
    
    a1:set(3, 103, 104)
    assertArrayIs(a1, 0, 0, 103, 104)
end
PRINT("==================================================================================")
do
    local a1 = carray.new("int")
    assert(a1:reserve() == 0)
    assert(a1:len() == 0)
    a1:reserve(100)
    PRINT(format("Len: %d, Reserve: %d", a1:len(), a1:reserve()))
    assert(a1:reserve() == 100)
    a1:add(101, 102, 103)
    PRINT(format("Len: %d, Reserve: %d", a1:len(), a1:reserve()))
    assert(a1:len() == 3)
    assert(a1:reserve() == 100 - 3)
    assertArrayIs(a1, 101, 102, 103)
    a1:reset()
    PRINT(format("Len: %d, Reserve: %d", a1:len(), a1:reserve()))
    assert(a1:len() == 0)
    for i = 1, 999 do
        a1:add(1000 + i)
    end
    PRINT(format("Len: %d, Reserve: %d", a1:len(), a1:reserve()))
    a1:reserve(0)
    PRINT(format("Len: %d, Reserve: %d", a1:len(), a1:reserve()))
    assert(a1:reserve() == 0)
    assert(a1:len() == 999)
    for i = 1, 999 do
        assert(1000 + i == a1:get(i))
    end
    assert(a1:get(1000) == nil)
    assert(a1:get(1001) == nil)
    assert(a1:get(2001) == nil)
    assert(a1:get(-1)   == 1999)
    assert(a1:get(-2)   == 1998)
    assert(a1:get(-999) == 1001)
    
    assertNlistEquals(1, nlist(a1:get(0)), {nil})
    assertNlistEquals(1, nlist(a1:get(-1000)), {nil})
    assertNlistEquals(2, nlist(a1:get(-1001, -1000)), {nil, nil})
    assertNlistEquals(2, nlist(a1:get(0, 1)), {nil, 1001})
    assertNlistEquals(0, nlist(a1:get(-1, 2)), {})
    assertNlistEquals(1, nlist(a1:get(-1, -1)), {1999})
    assertNlistEquals(0, nlist(a1:get(-1, 0)), {})
    assertNlistEquals(1, nlist(a1:get(-999, 1)), {1001})
    assertNlistEquals(2, nlist(a1:get(-1000, 1)), {nil, 1001})
    assertNlistEquals(3, nlist(a1:get(-1001, 1)), {nil, nil, 1001})
    assertNlistEquals(0, nlist(a1:get(999, 1)), {})
    assertNlistEquals(1, nlist(a1:get(999, -1)), {1999})
    assertNlistEquals(1, nlist(a1:get(999, 999)), {1999})
    assertNlistEquals(1, nlist(a1:get(999, 1000)), {1999})
    assertNlistEquals(1, nlist(a1:get(999, 1001)), {1999})
    
    a1:reset()
    local ok, err = pcall(function() a1:add(11,12,13,"14",15,16) end)
    assert(not ok and err:match("bad argument %#4 to 'add' %(integer or carray expected%)"))
    assert(a1:len() == 3)
    assertNlistEquals(3, nlist(a1:get(1,-1)), {11,12,13})

    a1:reset()
    local ok, err = pcall(function() a1:add(11,12,13,"14",15) end)
    assert(not ok and err:match("bad argument %#4 to 'add' %(integer or carray expected%)"))
    assert(a1:len() == 3)
    assertNlistEquals(3, nlist(a1:get(1,-1)), {11,12,13})

    a1:reset()
    local ok, err = pcall(function() a1:add(11,12,13,"14") end)
    assert(not ok and err:match("bad argument %#4 to 'add' %(integer or carray expected%)"))
    assert(a1:len() == 3)
    assertNlistEquals(3, nlist(a1:get(1,-1)), {11,12,13})

    a1:reset()
    local ok, err = pcall(function() a1:add(11,"12") end)
    assert(not ok and err:match("bad argument %#2 to 'add' %(integer or carray expected%)"))
    assert(a1:len() == 1)
    assertNlistEquals(1, nlist(a1:get(1,-1)), {11})

    a1:reset()
    local ok, err = pcall(function() a1:add("11") end)
    assert(not ok and err:match("bad argument %#1 to 'add' %(integer or carray expected%)"))
    assert(a1:len() == 0)
    assertNlistEquals(0, nlist(a1:get(1,-1)), {})

    a1:reset()
    local ok, err = pcall(function() a1:add("11",12) end)
    assert(not ok and err:match("bad argument %#1 to 'add' %(integer or carray expected%)"))
    assert(a1:len() == 0)
    assertNlistEquals(0, nlist(a1:get(1,-1)), {})
    
    a1:reset()
    a1:add(101, 102, 103)
    local a2 = carray.new("int")
    a2:add(201, 202, a1, 203)
    assertNlistEquals(6, nlist(a2:get(1,-1)), {201,202,101,102,103,203})

    a1 = carray.new("int"):add(101, 102, 103)
    local a2 = carray.new("int"):add(a1)
    assertNlistEquals(3, nlist(a2:get(1,-1)), {101,102,103})

    a1 = carray.new("int")
    local a2 = carray.new("int"):add(201, 202, a1, 203)
    assertNlistEquals(3, nlist(a2:get(1,-1)), {201,202,203})
end
PRINT("==================================================================================")
do
    a1 = carray.new("int")

    local ok, err = pcall(function() a1:insert(0,101) end)
    assert(not ok and err:match("bad argument %#1 to 'insert' %(index out of bounds%)"))

    local ok, err = pcall(function() a1:insert(-1,101) end)
    assert(not ok and err:match("bad argument %#1 to 'insert' %(index out of bounds%)"))

    local ok, err = pcall(function() a1:insert(2,101) end)
    assert(not ok and err:match("bad argument %#1 to 'insert' %(index out of bounds%)"))

    a1:add(101,102,103)
    
    local ok, err = pcall(function() a1:insert(5,201,202) end)
    assert(not ok and err:match("bad argument %#1 to 'insert' %(index out of bounds%)"))

    a1:insert(4,201,202)
    assertNlistEquals(5, nlist(a1:get(1,-1)), {101,102,103,201,202})


    a1:reset():add(101,102,103):insert(3, 201, 202)
    assertNlistEquals(5, nlist(a1:get(1,-1)), {101,102,201,202,103})
    
    local a1 = carray.new("int"):add(101, 102, 103)
    local a2 = carray.new("int"):insert(1, a1)
    assertNlistEquals(3, nlist(a2:get(1,-1)), {101,102,103})

    local a1 = carray.new("int"):add(101, 102, 103)
    local a2 = carray.new("int"):add(a1)
    assertNlistEquals(3, nlist(a2:get(1,-1)), {101,102,103})

    local a1 = carray.new("int")
    local a2 = carray.new("int"):insert(1, 201, 202, a1, 203)
    assertNlistEquals(3, nlist(a2:get(1,-1)), {201,202,203})

    local a1 = carray.new("int"):add(101,102)
    local a2 = carray.new("int"):insert(1, 201, 202, a1, 203)
    assertNlistEquals(5, nlist(a2:get(1,-1)), {201,202,101,102,203})

    local a1 = carray.new("int"):add(101,102)
    local a2 = carray.new("int"):add(201, 202, 203):insert(2, a1)
    assertNlistEquals(5, nlist(a2:get(1,-1)), {201,101,102,202,203})

    local a1 = carray.new("int"):add(101,102)
    local a2 = carray.new("int"):add(201, 202, 203):insert(2, 301, a1)
    assertNlistEquals(6, nlist(a2:get(1,-1)), {201,301,101,102,202,203})

    local a1 = carray.new("int"):add(101,102)
    local a2 = carray.new("int"):add(201, 202, 203):insert(2, 301, a1, 302)
    assertNlistEquals(7, nlist(a2:get(1,-1)), {201,301,101,102,302,202,203})

    local a1 = carray.new("int"):add(101,102)
    local a2 = carray.new("int"):add(201, 202, 203):insert(2, a1, 302)
    assertNlistEquals(6, nlist(a2:get(1,-1)), {201,101,102,302,202,203})

    local a1 = carray.new("int"):add(101,102)
    local a2 = carray.new("int"):add(201, 202, 203)
    local a3 = carray.new("int"):insert(1, a1, a2)
    assertNlistEquals(5, nlist(a3:get(1,-1)), {101,102,201,202,203})

    local a1 = carray.new("int"):add(101,102)
    local a2 = carray.new("int"):add(201, 202, 203)
    local a3 = carray.new("int"):add(a1, a2)
    assertNlistEquals(5, nlist(a3:get(1,-1)), {101,102,201,202,203})

    local a1 = carray.new("int"):add(101,102)
    local a2 = carray.new("int"):add(201, 202, 203)
    local a3 = carray.new("int"):add(301):insert(1, a1, a2)
    assertNlistEquals(6, nlist(a3:get(1,-1)), {101,102,201,202,203,301})

    local a1 = carray.new("int"):add(101,102)
    local a2 = carray.new("int"):add(201, 202, 203)
    local a3 = carray.new("int"):add(301):insert(2, a1, a2)
    assertNlistEquals(6, nlist(a3:get(1,-1)), {301,101,102,201,202,203})

    local a1 = carray.new("int"):add(101,102)
    local a2 = carray.new("int"):add(201,202,203)
    local a3 = carray.new("int"):add(301,302):insert(2, a1, a2)
    assertNlistEquals(7, nlist(a3:get(1,-1)), {301,101,102,201,202,203,302})
end
PRINT("==================================================================================")
do
    local a1 = carray.new("int"):add(101,102,103):remove(5)
    assertNlistEquals(3, nlist(a1:get(1,-1)), {101,102,103})

    local a1 = carray.new("int"):add(101,102,103):remove(4)
    assertNlistEquals(3, nlist(a1:get(1,-1)), {101,102,103})

    local a1 = carray.new("int"):add(101,102,103):remove(3)
    assertNlistEquals(2, nlist(a1:get(1,-1)), {101,102})

    local a1 = carray.new("int"):add(101,102,103):remove(2)
    assertNlistEquals(2, nlist(a1:get(1,-1)), {101,103})

    local a1 = carray.new("int"):add(101,102,103):remove(1)
    assertNlistEquals(2, nlist(a1:get(1,-1)), {102,103})

    local a1 = carray.new("int"):add(101,102,103):remove(0)
    assertNlistEquals(3, nlist(a1:get(1,-1)), {101,102,103})

    local a1 = carray.new("int"):add(101,102,103):remove(-1)
    assertNlistEquals(2, nlist(a1:get(1,-1)), {101,102})

    local a1 = carray.new("int"):add(101,102,103):remove(-2)
    assertNlistEquals(2, nlist(a1:get(1,-1)), {101,103})

    local a1 = carray.new("int"):add(101,102,103):remove(-3)
    assertNlistEquals(2, nlist(a1:get(1,-1)), {102,103})

    local a1 = carray.new("int"):add(101,102,103):remove(-4)
    assertNlistEquals(3, nlist(a1:get(1,-1)), {101,102,103})
    
    local a1 = carray.new("int"):add(101,102,103):remove(-5)
    assertNlistEquals(3, nlist(a1:get(1,-1)), {101,102,103})
    
    local a1 = carray.new("int"):add(101,102,103):remove(3,2)
    assertNlistEquals(3, nlist(a1:get(1,-1)), {101,102,103})

    local a1 = carray.new("int"):add(101,102,103):remove(3,3)
    assertNlistEquals(2, nlist(a1:get(1,-1)), {101,102})

    local a1 = carray.new("int"):add(101,102,103):remove(2,3)
    assertNlistEquals(1, nlist(a1:get(1,-1)), {101})

    local a1 = carray.new("int"):add(101,102,103):remove(2,4)
    assertNlistEquals(1, nlist(a1:get(1,-1)), {101})

    local a1 = carray.new("int"):add(101,102,103):remove(2,-1)
    assertNlistEquals(1, nlist(a1:get(1,-1)), {101})

    local a1 = carray.new("int"):add(101,102,103):remove(2,-2)
    assertNlistEquals(2, nlist(a1:get(1,-1)), {101,103})

    local a1 = carray.new("int"):add(101,102,103):remove(1,-1)
    assertNlistEquals(0, nlist(a1:get(1,-1)), {})

    local a1 = carray.new("int"):add(101,102,103):remove(-1,-1)
    assertNlistEquals(2, nlist(a1:get(1,-1)), {101,102})

    local a1 = carray.new("int"):add(101,102,103):remove(-2,-1)
    assertNlistEquals(1, nlist(a1:get(1,-1)), {101})

    local a1 = carray.new("int"):add(101,102,103):remove(-3,-1)
    assertNlistEquals(0, nlist(a1:get(1,-1)), {})
    
    local a1 = carray.new("int"):add(101,102,103):remove(-3,-2)
    assertNlistEquals(1, nlist(a1:get(1,-1)), {103})
    
    local a1 = carray.new("int"):add(101,102,103):remove(-4,-2)
    assertNlistEquals(1, nlist(a1:get(1,-1)), {103})

    local a1 = carray.new("int"):add(101,102,103):remove(-4,-3)
    assertNlistEquals(2, nlist(a1:get(1,-1)), {102,103})

    local a1 = carray.new("int"):add(101,102,103):remove(-5,-3)
    assertNlistEquals(2, nlist(a1:get(1,-1)), {102,103})

    local a1 = carray.new("int"):add(101,102,103):remove(0,-3)
    assertNlistEquals(2, nlist(a1:get(1,-1)), {102,103})

    local a1 = carray.new("int"):add(101,102,103):remove(-5,-2)
    assertNlistEquals(1, nlist(a1:get(1,-1)), {103})

    local a1 = carray.new("int"):add(101,102,103):remove(0,-2)
    assertNlistEquals(1, nlist(a1:get(1,-1)), {103})

    local a1 = carray.new("int"):add(101,102,103):remove(-5,0)
    assertNlistEquals(3, nlist(a1:get(1,-1)), {101,102,103})

    local a1 = carray.new("int"):add(101,102,103):remove(-5,5)
    assertNlistEquals(0, nlist(a1:get(1,-1)), {})
end
PRINT("==================================================================================")
do
    local a1 = carray.new("char")
    assert(a1:tostring()   == "")

    local ok, err = pcall(function() a1:tostring(1) end)
    assert(not ok and err:match("bad argument %#2 to 'tostring' %(number expected, got no value%)"))

    local ok, err = pcall(function() a1:tostring(nil) end)
    assert(not ok and err:match("bad argument %#1 to 'tostring' %(number expected, got nil%)"))

    assert(a1:tostring(1,-1) == "")
    assert(a1:tostring(-1,-1) == "")
    assert(a1:tostring(1,1)  == "")
    assert(a1:tostring(1,0)  == "")
    assert(a1:tostring(0,0)  == "")
    assert(a1:tostring(0,-1)  == "")
    assert(a1:tostring(0,1)  == "")
    assert(a1:tostring(0,-2)  == "")
    assert(a1:tostring(1,-2)  == "")
    assert(a1:tostring(1,2)  == "")
    assert(a1:tostring(1,3)  == "")

    local a1 = carray.new("char"):add(string.byte("a"))
    assert(a1:tostring() == "a")
    assert(a1:tostring(1,-1) == "a")
    assert(a1:tostring(1,1) == "a")
    assert(a1:tostring(-1,-1) == "a")
    assert(a1:tostring(1,0) == "")
    assert(a1:tostring(2,2) == "")
    assert(a1:tostring(2,1) == "")
    assert(a1:tostring(2,0) == "")
    assert(a1:tostring(0,1) == "a")
    assert(a1:tostring(-1000,-1) == "a")

    local a1 = carray.new("char",3):set(1, "abc")
    assert(a1:tostring() == "abc")
    assert(a1:tostring(1,-1) == "abc")
    assert(a1:tostring(-1,-1) == "c")
    assert(a1:tostring(1,1) == "a")
    assert(a1:tostring(0,1) == "a")
    assert(a1:tostring(-1,1) == "")
    assert(a1:tostring(-2,1) == "")
    assert(a1:tostring(-3,1) == "a")
    assert(a1:tostring(-4,1) == "a")
    assert(a1:tostring(-5,1) == "a")
    assert(a1:tostring(-6,1) == "a")
    assert(a1:tostring(-6,2) == "ab")
    assert(a1:tostring(-6,3) == "abc")
    assert(a1:tostring(-6,4) == "abc")
    assert(a1:tostring(-6,5) == "abc")
    
    a1:add("xyz")
    assert(a1:tostring() == "abcxyz")
    assert(a1:tostring(1,-1) == "abcxyz")
    assert(a1:tostring(1,-2) == "abcxy")
    assert(a1:tostring(2,-2) == "bcxy")

    a1:add(string.byte("1"), "23", string.byte("4"), "", "567", string.byte("890",1,-1))
    assert(a1:tostring() == "abcxyz1234567890")
    
    a1:insert(4, "_A_")
    assert(a1:tostring() == "abc_A_xyz1234567890")
end
PRINT("==================================================================================")
do
    local a1 = carray.new("int")
    
    assert(a1:len() == 0)
    assert(a1:reserve() == 0)
    
    a1:add(101)
    assert(a1:len() == 1)
    assert(a1:reserve() == 1)

    a1:add(102)
    assert(a1:len() == 2)
    assert(a1:reserve() == 0)

    a1:add(103)
    assert(a1:len() == 3)
    assert(a1:reserve() == 3)

    a1:add(104, 105, 106)
    assert(a1:len() == 6)
    assert(a1:reserve() == 0)

    a1:add(107)
    assert(a1:len() == 7)
    assert(a1:reserve() == 7)

    a1:add(108,109,110,111,112,113)
    assert(a1:len() == 13)
    assert(a1:reserve() == 1)

    a1:add(114,115)
    assert(a1:len() == 15)
    assert(a1:reserve() == 15)

    a1:reserve(0)
    assert(a1:len() == 15)
    assert(a1:reserve() == 0)
    
    a1:add(116)
    assert(a1:len() == 16)
    assert(a1:reserve() == 16)
    
    a1:reserve(2)
    assert(a1:len() == 16)
    assert(a1:reserve() == 16)

    a1:reserve(17)
    assert(a1:len() == 16)
    assert(a1:reserve() == 17)

    a1:reserve(-1)
    assert(a1:len() == 16)
    assert(a1:reserve() == 0)

    a1:reserve(0)
    assert(a1:len() == 16)
    assert(a1:reserve() == 0)

    a1:setlen(5)
    assert(a1:len() == 5)
    assert(a1:reserve() == 11)

    a1:setlen(5, true)
    assert(a1:len() == 5)
    assert(a1:reserve() == 0)

    a1:setlen(1, false)
    assert(a1:len() == 1)
    assert(a1:reserve() == 4)

    a1:reset()
    assert(a1:len() == 0)
    assert(a1:reserve() == 5)

    a1:reset(true)
    assert(a1:len() == 0)
    assert(a1:reserve() == 0)
end
PRINT("==================================================================================")
do
    local a1 = carray.new("int")
    local a2 = carray.new("int")
    a1:addsub(a2,1,-1)
    assert(a1:len() == 0)

    local a1 = carray.new("int"):add(101,102,103)
    local a2 = carray.new("int")
    a1:addsub(a2,1,-1)
    assertNlistEquals(3, nlist(a1:get(1,-1)), {101,102,103})
    
    a1:addsub(a1,1,-1)
    assertNlistEquals(6, nlist(a1:get(1,-1)), {101,102,103,101,102,103})

    a1:addsub(a1,1,0)
    assertNlistEquals(6, nlist(a1:get(1,-1)), {101,102,103,101,102,103})

    a1:addsub(a1,1,1)
    assertNlistEquals(7, nlist(a1:get(1,-1)), {101,102,103,101,102,103,101})

    a1:add(999):addsub(a1,-1,-1)
    assertNlistEquals(9, nlist(a1:get(1,-1)), {101,102,103,101,102,103,101,999,999})

    a1:addsub(a1,-3,-2)
    assertNlistEquals(11, nlist(a1:get(1,-1)), {101,102,103,101,102,103,101,999,999,101,999})

    a1:reset():addsub(a1, 1, -1)
    assertNlistEquals(0, nlist(a1:get(1,-1)), {})

    a2:add(201,202,203)
    a1:addsub(a2, 1, -1)
    assertNlistEquals(3, nlist(a1:get(1,-1)), {201,202,203})

    a1:addsub(a2, -4, 1)
    assertNlistEquals(4, nlist(a1:get(1,-1)), {201,202,203,201})
    assertNlistEquals(3, nlist(a2:get(1,-1)), {201,202,203})

    a1:addsub(a2, 2, 100)
    assertNlistEquals(6, nlist(a1:get(1,-1)), {201,202,203,201,202,203})
    assertNlistEquals(3, nlist(a2:get(1,-1)), {201,202,203})
end
PRINT("==================================================================================")
do
    local a1 = carray.new("int"):add(101,102,103)
    local a2 = carray.new("int"):add(201,202,203,204,205)
    
    a1:insertsub(2,a2,3,4)
    assertNlistEquals(5, nlist(a1:get(1,-1)), {101,203,204,102,103})
    assertNlistEquals(5, nlist(a2:get(1,-1)), {201,202,203,204,205})

    a1:reset():add(101,102,103,104,105,106)
    a1:insertsub(1,a1,3,5)
    assertNlistEquals(9, nlist(a1:get(1,-1)), {103,104,105,101,102,103,104,105,106})

    a1:reset():add(101,102,103,104,105,106)
    a1:insertsub(2,a1,3,5)
    assertNlistEquals(9, nlist(a1:get(1,-1)), {101,103,104,105,102,103,104,105,106})

    a1:reset():add(101,102,103,104,105,106)
    a1:insertsub(3,a1,3,5)
    assertNlistEquals(9, nlist(a1:get(1,-1)), {101,102,103,104,105,103,104,105,106})

    a1:reset():add(101,102,103,104,105,106)
    a1:insertsub(4,a1,3,5)
    assertNlistEquals(9, nlist(a1:get(1,-1)), {101,102,103,103,104,105,104,105,106})

    a1:reset():add(101,102,103,104,105,106)
    a1:insertsub(5,a1,3,5)
    assertNlistEquals(9, nlist(a1:get(1,-1)), {101,102,103,104,103,104,105,105,106})

    a1:reset():add(101,102,103,104,105,106)
    a1:insertsub(6,a1,3,5)
    assertNlistEquals(9, nlist(a1:get(1,-1)), {101,102,103,104,105,103,104,105,106})

    a1:reset():add(101,102,103,104,105,106)
    a1:insertsub(7,a1,3,5)
    assertNlistEquals(9, nlist(a1:get(1,-1)), {101,102,103,104,105,106,103,104,105})

    a1:reset():add(101,102,103,104,105,106)
    local ok, err = pcall(function() a1:insertsub(8,a1,3,5) end)
    assert(not ok and err:match("bad argument %#1 to 'insertsub' %(index out of bounds%)"))
    
    a1:reset():add(101,102,103,104,105,106)
    local ok, err = pcall(function() a1:insertsub(0,a1,3,5) end)
    assert(not ok and err:match("bad argument %#1 to 'insertsub' %(index out of bounds%)"))
    
    a1:reset():add(101,102,103,104,105,106)
    local ok, err = pcall(function() a1:insertsub(-1,a1,3,5) end)
    assert(not ok and err:match("bad argument %#1 to 'insertsub' %(index out of bounds%)"))
    
    a2:reset():add(101,102,103,104,105,106,107)

    a1:reset():add(101,102,103,104,105,106)
    a1:insertsub(1,a2,3,5)
    assertNlistEquals(9, nlist(a1:get(1,-1)), {103,104,105,101,102,103,104,105,106})

    a1:reset():add(101,102,103,104,105,106)
    a1:insertsub(2,a2,3,5)
    assertNlistEquals(9, nlist(a1:get(1,-1)), {101,103,104,105,102,103,104,105,106})

    a1:reset():add(101,102,103,104,105,106)
    a1:insertsub(3,a2,3,5)
    assertNlistEquals(9, nlist(a1:get(1,-1)), {101,102,103,104,105,103,104,105,106})

    a1:reset():add(101,102,103,104,105,106)
    a1:insertsub(4,a2,3,5)
    assertNlistEquals(9, nlist(a1:get(1,-1)), {101,102,103,103,104,105,104,105,106})

    a1:reset():add(101,102,103,104,105,106)
    a1:insertsub(5,a2,3,5)
    assertNlistEquals(9, nlist(a1:get(1,-1)), {101,102,103,104,103,104,105,105,106})

    a1:reset():add(101,102,103,104,105,106)
    a1:insertsub(6,a2,3,5)
    assertNlistEquals(9, nlist(a1:get(1,-1)), {101,102,103,104,105,103,104,105,106})

    a1:reset():add(101,102,103,104,105,106)
    a1:insertsub(7,a2,3,5)
    assertNlistEquals(9, nlist(a1:get(1,-1)), {101,102,103,104,105,106,103,104,105})

    assertNlistEquals(7, nlist(a2:get(1,-1)), {101,102,103,104,105,106,107})
end
PRINT("==================================================================================")
do
    local a = carray.new("char", 3)
    
    assert(a:tostring() == "\0\0\0")
    a:set(1, "abc")
    assert(a:tostring() == "abc")
    a:set(1, string.byte("xyz", 1, -1))
    assert(a:tostring() == "xyz")
    a:reset():add("123456")
    assert(a:tostring() == "123456")
    a:set(3, "ABC")
    assert(a:tostring() == "12ABC6")
    a:set(-1, "D")
    assert(a:tostring() == "12ABCD")
    
    local ok, err = pcall(function() a:set(-1, "ab") end)
    assert(not ok and err:match("bad argument %#2 to 'set' %(index out of bounds%)"))

    local ok, err = pcall(function() a:set(0, "ab") end)
    assert(not ok and err:match("bad argument %#1 to 'set' %(index out of bounds%)"))

    local ok, err = pcall(function() a:set(1, "ab", "c", string.byte("d"), "efg") end)
    assert(not ok and err:match("bad argument %#5 to 'set' %(index out of bounds%)"))
    assert(a:tostring() == "abcdCD")
    
    local a2 = carray.new("char"):add("123")
    
    a:set(-3, a2)
    assert(a:tostring() == "abc123")
end
PRINT("==================================================================================")
do
    local a = carray.new("float", 3)
    a:set(1, 101,102,103)
    
    assertNlistEquals(3, nlist(a:get(1,-1)), {101,102,103})

    local ok, err = pcall(function() a:set(1, "22") end)
    assert(not ok and err:match("bad argument %#2 to 'set' %(number or carray expected%)"))

    local ok, err = pcall(function() a:add("22") end)
    assert(not ok and err:match("bad argument %#1 to 'add' %(number or carray expected%)"))

    local ok, err = pcall(function() a:insert(1, "22") end)
    assert(not ok and err:match("bad argument %#2 to 'insert' %(number or carray expected%)"))
    
    local a2 = carray.new("int", 3)
    
    local ok, err = pcall(function() a:set(1, a2) end)
    assert(not ok and err:match("bad argument %#2 to 'set' %(carray type mismatch, expected: carray%<float%>%)"))
    
    local ok, err = pcall(function() a:add(a2) end)
    assert(not ok and err:match("bad argument %#1 to 'add' %(carray type mismatch, expected: carray%<float%>%)"))
    
    local ok, err = pcall(function() a:insert(1, a2) end)
    assert(not ok and err:match("bad argument %#2 to 'insert' %(carray type mismatch, expected: carray%<float%>%)"))
end
PRINT("==================================================================================")
do
    local a = carray.new("int", 3)
    a:set(1, 101,102,103)
    
    assertNlistEquals(3, nlist(a:get(1,-1)), {101,102,103})

    local ok, err = pcall(function() a:set(1, "22") end)
    assert(not ok and err:match("bad argument %#2 to 'set' %(integer or carray expected%)"))
    
    local ok, err = pcall(function() a:add("22") end)
    assert(not ok and err:match("bad argument %#1 to 'add' %(integer or carray expected%)"))

    local ok, err = pcall(function() a:insert(1, "22") end)
    assert(not ok and err:match("bad argument %#2 to 'insert' %(integer or carray expected%)"))

    local a2 = carray.new("float", 3)

    local ok, err = pcall(function() a:set(1, a2) end)
    assert(not ok and err:match("bad argument %#2 to 'set' %(carray type mismatch, expected: carray%<int%>%)"))

    local ok, err = pcall(function() a:add(a2) end)
    assert(not ok and err:match("bad argument %#1 to 'add' %(carray type mismatch, expected: carray%<int%>%)"))

    local ok, err = pcall(function() a:insert(1, a2) end)
    assert(not ok and err:match("bad argument %#2 to 'insert' %(carray type mismatch, expected: carray%<int%>%)"))
end
PRINT("==================================================================================")
do
    local a = carray.new("char")
    
    a:addsub("abcdefg", 2, 4)
    assert(a:tostring() == "bcd")

    a:insertsub(2, "_XYZ_", 2, 4)
    assert(a:tostring() == "bXYZcd")
    
    a:insertsub(1, a, 2, 4)
    assert(a:tostring() == "XYZbXYZcd")

    a:reset():add("1234567890")
    a:insertsub(4, a, 2, 8)
    assert(a:tostring() == "12323456784567890")

    a:setsub(3, "_abc_", 2, 4)
    assert(a:tostring() == "12abc456784567890")
    
    a:reset():add("1234567890")
    a:setsub(4, a, 2, 8)
    assert(a:tostring() == "1232345678")
end
PRINT("==================================================================================")
print("OK.")
