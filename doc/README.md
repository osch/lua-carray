# lua-carray Documentation
<!-- ---------------------------------------------------------------------------------------- -->
##   Contents
<!-- ---------------------------------------------------------------------------------------- -->

   * [Overview](#overview)
   * [Module Functions](#module-functions)
        * [carray.new()](#carray_new)
   * [Element Type Names](#element-type-names)
   * [Array Methods](#array-methods)
        * [array:get()](#array_get)
        * [array:set()](#array_set)
        * [array:append()](#array_append)
        * [array:insert()](#array_insert)
        * [array:setsub()](#array_setsub)
        * [array:appendsub()](#array_appendsub)
        * [array:insertsub()](#array_insertsub)
        * [array:remove()](#array_remove)
        * [array:len()](#array_len)
        * [array:type()](#array_type)
        * [array:basetype()](#array_basetype)
        * [array:bitwidth()](#array_bitwidth)
        * [array:reset()](#array_reset)
        * [array:setlen()](#array_setlen)
        * [array:reserve()](#array_reserve)
        * [array:tostring()](#array_tostring)
        * [array:equals()](#array_equals)
        * [array:appendfile()](#array_appendfile)
        
<!-- ---------------------------------------------------------------------------------------- -->
##   Overview
<!-- ---------------------------------------------------------------------------------------- -->

The [Lua] module *carray* provides an array data type and the implementation of the [Carray C API] 
for handling arrays of primitive numeric C data types in Lua script code and also in native 
C code for enhancing native Lua module interoperability and performance. 

This module can be installed from Luarocks: https://luarocks.org/modules/osch/carray.

The module name to require is *"carray"*:

```
local carray = require("carray")
```


<!-- ---------------------------------------------------------------------------------------- -->
##   Module Functions
<!-- ---------------------------------------------------------------------------------------- -->

* <span id="carray_new">**`carray.new(type[, count])
  `**</span>
  
  Creates a new array object with the specified element type.
  
  * *type*  - string, type name of the elements, see [Element Type Names](#element-type-names)
  
  * *count* - optional integer, number of the elements. If not given the created array 
              has no elements. All elements are initialized with zero.

<!-- ---------------------------------------------------------------------------------------- -->
##   Element Type Names
<!-- ---------------------------------------------------------------------------------------- -->

This section lists the possible element type names that can be specified in a call 
to [carray.new()](#carray_new).

* Element type names for corresponding C data types:

  * **Signed chararacters** can be specified by one of the equivalent type names:
    *"signed char"* or *"schar"*.

  * **Unsigned chararacters** can be specified by one of the equivalent type names:
    *"unsigned char"*, *"uchar"* or *"char"*.

  * **Signed short integers**: *"short"*.

  * **Unsigned short integers**: *"unsigned short"* or *"ushort"*.

  * **Signed integers**: *"int"*.

  * **Unsigned integers**: *"unsigned int"* or *"uint"*.

  * **Long integers**: *"long"*.

  * **Unsigned long integers**: *"unsigned long"* or *"ulong"*.

  * **Long long integers**: *"long long"* or *"llong"*.

  * **Unsigned long long integers**: *"unsigned long long"*, *"unsigned llong"* or *"ullong"*.

  * **Float numbers**: *"float"*.
  
  * **Double numbers**: *"double"*.

* Element type names for corresponding [Lua] data types:
  
  * **Integer with same size as *lua_Integer***: *"integer"*.
  * **Float number with same size as *lua_Number***: *"number"*.

* Element type names for corresponding bit size:

  * **Smallest signed integer with at least n bits**: *"int8"*, *"int16"*, *"int32"*, *"int64"*.
  * **Smallest unsigned integer with at least n bits**: *"uint8"*, *"uint16"*, *"uint32"*, *"uint64"*.

  * **Smallest float number with at least n bits**: *"float32"*, *"float64"*.

<!-- ---------------------------------------------------------------------------------------- -->
##   Array Methods
<!-- ---------------------------------------------------------------------------------------- -->

Array objects can be created by calling the module function [carray.new()](#carray_new).

If not other specified, array methods return the array object to allow method chaining, 
e.g.
```lua
    local a = carray.new("char"):append("testdata:")
```
creates a new array with data from string and file content and assigns this array to the
variable *a*.

<!-- ---------------------------------------------------------------------------------------- -->

* <span id="array_get">**`array:get(pos1[, pos2])
  `** </span>
  
  Get elements from the array.
  
  * *pos1* - integer position of the first element to get from the array.
  * *pos2* - optional integer position of the last element of the array to get. If not given this
             equals *pos1*.
  
  The first element in the array has position 1, i.e. *array:get(1)* returns the first element of
  the array.

  *pos1* or *pos2* may be negative to denote positions from behind, e.g. *array:get(-3,-1)* 
  returns the last three elements of the array.

<!-- ---------------------------------------------------------------------------------------- -->

* <span id="array_set">**`array:set(pos, ...)
  `** </span>

  Sets the given elements at the specified position of the array object.

  * *pos*  - integer position of the first element that is to be set into the array.
             It must be: *1 <= pos <= [array:len()](#array_len)* or 
             *-[array:len()](#array_len) <= pos <= -1*. Negative positions are specifying
             the last n-th element, e.g. -1 denotes the position of the last element.
  * *...*  - list of elements or array objects that are set into the array beginning at 
             the specified position *pos*.
             If the array element type is *signed char* or *unsigned char* 
             (see [Element Type Names](#element-type-names)), it is also possible to set 
             string values into the array.
             
  The given elements must fit into the current array length. 
  
<!-- ---------------------------------------------------------------------------------------- -->

* <span id="array_append">**`array:append(...)
  `** </span>

  Appends the given elements to the end of the array object.

  * *...*  - list of elements or array objects that are appended to the end of the array object.
             If the array element type is *signed char* or *unsigned char* 
             (see [Element Type Names](#element-type-names)), it is also possible to append 
             string values.
  
  
<!-- ---------------------------------------------------------------------------------------- -->

* <span id="array_insert">**`array:insert(pos, ...)
  `** </span>

  Inserts the given elements at the specified position of the array object.

  * *pos*  - integer position where the first element is to be inserted. It must
             be: *1 <= pos <= [array:len()](#array_len) + 1*
  * *...*  - list of elements or array objects that are inserted at the specified
             position *pos* of the array object.
             If the array element type is *signed char* or *unsigned char* 
             (see [Element Type Names](#element-type-names)), it is also possible to insert 
             string values.
  
<!-- ---------------------------------------------------------------------------------------- -->

* <span id="array_setsub">**`array:setsub(pos0, array2, pos1, pos2)
  `** </span>

  Sets elements of another array *array2* to the specified position *pos0*
  into the array object.

  * *pos0*   - integer position where the first element is to be set. It must
               be: *1 <= pos0 <= [array:len()](#array_len) + 1*
  * *array2* - source array. 
               If the destination array element type is *signed char* or *unsigned char* 
               (see [Element Type Names](#element-type-names)), it is also possible to give 
               a string value here.
  * *pos1*   - integer position of the first element to be set from *array2*.
  * *pos2*   - integer position of the last element of *array2* to be set.
  
  The first element in the array has position 1, i.e. *array:setsub(1, array2, 1, 3)* sets
  the first three elements of *array2* to the array at position 1.

  *pos1* or *pos2* may be negative to denote positions from behind, e.g. 
  *array:setsub(1, array2, -3, -1)* sets the last three elements of *array2* into
  the array object at position 1.
  
  The given elements must fit into the current array length. 

<!-- ---------------------------------------------------------------------------------------- -->

* <span id="array_appendsub">**`array:appendsub(array2, pos1, pos2)
  `** </span>

  Appends elements of another array *array2* to the end of the array object.

  * *array2* - source array.
               If the destination array element type is *signed char* or *unsigned char* 
               (see [Element Type Names](#element-type-names)), it is also possible to give 
               a string value here.
  * *pos1*   - integer position of the first element to append from *array2*.
  * *pos2*   - integer position of the last element of *array2* to append.
  
  The first element in the array has position 1, i.e. *array:appendsub(array2, 1, 3)* appends the 
  first three elements of *array2* to the array.

  *pos1* or *pos2* may be negative to denote positions from behind, e.g. 
  *array:appendsub(array2,-3,-1)* appends the last three elements of *array2* to the array object.
  
  
<!-- ---------------------------------------------------------------------------------------- -->

* <span id="array_insertsub">**`array:insertsub(pos0, array2, pos1, pos2)
  `** </span>

  Inserts elements of another array *array2* to the specified position *pos0*
  into the array object.

  * *pos0*   - integer position where the first element is to be inserted. It must
               be: *1 <= pos0 <= [array:len()](#array_len) + 1*
  * *array2* - source array.
               If the destination array element type is *signed char* or *unsigned char* 
               (see [Element Type Names](#element-type-names)), it is also possible to give 
               a string value here.
  * *pos1*   - integer position of the first element to insert from *array2*.
  * *pos2*   - integer position of the last element of *array2* to insert.
  
  The first element in the array has position 1, i.e. *array:insertsub(1, array2, 1, 3)* inserts
  the first three elements of *array2* to the array at position 1.

  *pos1* or *pos2* may be negative to denote positions from behind, e.g. 
  *array:insertsub(1, array2, -3, -1)* inserts the last three elements of *array2* into
  the array object at position 1.
  
<!-- ---------------------------------------------------------------------------------------- -->

* <span id="array_remove">**`array:remove(pos1[, pos2])
  `** </span>

  Removes elements from the array.
  
  * *pos1* - integer position of the first element to remove from the array.
  * *pos2* - optional integer position of the last element of the array to remove. If not given this
             equals *pos1*.
  
  The first element in the array has position 1, i.e. *array:remove(1)* removes the first element of
  the array.

  *pos1* or *pos2* may be negative to denote positions from behind, e.g. *array:remove(-3,-1)* 
  removes the last three elements of the array.


<!-- ---------------------------------------------------------------------------------------- -->

* <span id="array_len">**`array:len()
  `** </span>

  Returns the number of elements in the array object.
  
<!-- ---------------------------------------------------------------------------------------- -->

* <span id="array_type">**`array:type()
  `** </span>

  Returns the element type name as string value. Possible values are:
  
  * *"unsigned char"*, *"signed char"*
  * *"short"*, *"unsigned short"*
  * *"int"*, *"unsigned int"*
  * *"long"*, *"unsigned long"*
  * *"long long"*, *"unsigned long long"*
  * *"float"*, *"double"*
  
  See also [Element Type Names](#element-type-names).
  
<!-- ---------------------------------------------------------------------------------------- -->

* <span id="array_basetype">**`array:basetype()
  `** </span>

  Returns the element base type name as string value. Possible values are:
  
  * *"int"*
  * *"uint"*
  * *"float"*
  
  See also [Element Type Names](#element-type-names) or [array:bitwidth()](#array_bitwidth).
  
<!-- ---------------------------------------------------------------------------------------- -->

* <span id="array_bitwidth">**`array:bitwidth()
  `** </span>

  Returns the number of bits per element as integer value.

  See also [Element Type Names](#element-type-names).

<!-- ---------------------------------------------------------------------------------------- -->

* <span id="array_reset">**`array:reset([shrink])
  `** </span>
  
  Resets the array to length 0.
  
  * *shrink* - optional boolean, if true, the internal capacity of the array is also
               set to 0, i.e. the memory for elements is released.
  
  See also [array:reserve()](#array_reserve).

<!-- ---------------------------------------------------------------------------------------- -->

* <span id="array_setlen">**`array:setlen(newlen[, shrink])
  `** </span>

  Sets how many elements can be stored in the array. 
  
  * *newlen* - integer, new length of the array
  
  * *shrink* - optional boolean, if true, the internal capacity of the array is reduced
               to match the length, i.e. reserve is set to zero, see also 
               [array:reserve()](#array_reserve).

  If the new length is larger than the old length, new elements are initialized with zero.
  If the new length is lesser than the old length, existing elements are discarded from behind.
  
<!-- ---------------------------------------------------------------------------------------- -->

* <span id="array_reserve">**`array:reserve([count])
  `** </span>
  
  Sets or gets the reserve count. The reserve count denotes the number of new elements that
  can be appended to the array without the need to re-allocate the array's memory.
  
  * *count* - optional integer, if greater zero this methods assures that the reserve is at 
              least *count* number of elements. If zero or less the reserve is
              released, i.e. the allocated memory for this array object is not greater than
              needed for the [arry:len()](#array_len) number of elements in this array.
  
  This method returns the current reserve count if no argument *count* is given, 
  otherwise the array object itself is returned to allow method chaining.
  
<!-- ---------------------------------------------------------------------------------------- -->

* <span id="array_tostring">**`array:tostring([pos1, pos2])
  `** </span>

  Returns array elements as string value for arrays that have element type *signed char*
  or *unsigned char*, see also [Element Type Names](#element-type-names).
  
  * *pos1* - integer position of the first element to get from the array.
  * *pos2* - optional integer position of the last element of the array to get. If not given this
             equals *pos1*.
  
  The first element in the array has position 1, i.e. *array:tostring(1,2)* returns the first 
  two chars of the array as string.

  *pos1* or *pos2* may be negative to denote positions from behind, e.g. *array:tostring(-3,-1)* 
  returns the last three chars of the array as string.

  
<!-- ---------------------------------------------------------------------------------------- -->

* <span id="array_equals">**`array:equals(array2)
  `** </span>

  Returns *true* if the two arrays are having the same number and type of elements.
  
<!-- ---------------------------------------------------------------------------------------- -->

* <span id="array_appendfile">**`array:appendfile(file[, max])
  `** </span>

  Appends the content of the given file to the array.
  
  * *file* - a file name string or open file handle.
  * *max*  - optional integer, the maximal number of elements that are read from the
             given file.

  Returns the number of elements that were appended.

<!-- ---------------------------------------------------------------------------------------- -->

[Lua]:          https://www.lua.org
[Carray C API]: https://github.com/lua-capis/lua-carray-capi

End of document.

<!-- ---------------------------------------------------------------------------------------- -->
