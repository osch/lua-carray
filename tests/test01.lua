local carray = require("carray")

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
local function assertListEqual(a1, a2)
    assert(#a1 == #a2)
    for i = 1, #a1 do
        assert(a1[i], a2[i])
    end
end
local function assertArrayIs(a1, ...)
    assert(a1:len() == select("#", ...))
    for i = 1, a1:len() do
        assert(a1:get(i), select(i, ...))
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

    local ok, err = pcall(function() a1:get(-4, 3) end)
    assert(not ok and err:match("bad argument %#1 to 'get' %(invalid index%)"))

    local ok, err = pcall(function() a1:get(0, 3) end)
    assert(not ok and err:match("bad argument %#1 to 'get' %(invalid index%)"))
    
    a1:set(1)
    assertArrayIs(a1, 101, 102, 103)

    a1:set(1, 201)
    assertArrayIs(a1, 201, 102, 103)

    local ok, err = pcall(function() a1:set(0) end)
    assert(not ok and err:match("bad argument %#1 to 'set' %(index out of bounds%)"))
    
    local ok, err = pcall(function() a1:set(0, 201) end)
    assert(not ok and err:match("bad argument %#1 to 'set' %(index out of bounds%)"))
    
    a1:reset()
    assert(a1:len() == 0)

    local ok, err = pcall(function() a1:set(1, 201) end)
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

    a1:reset()
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
print("OK.")
