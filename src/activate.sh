# Activations script for add the carray module to the lua path when
# building carray using the Makefile.
# Source this into interactive shell by invoking ". activates.sh" from this directory
# This is not necessary if carray is installed, e.g. via luarocks.

this_dir=$(pwd)

luacarray_dir=$(cd "$this_dir"/..; pwd)

if [ ! -e "$luacarray_dir/src/activate.sh" -o ! -e "$luacarray_dir/src/main.c" ]; then

    echo '**** ERROR: ". activate.sh" must be invoked from "src" directory ***'

else

    echo "Setting lua paths for: $luacarray_dir"

    add_lua_path="$luacarray_dir/src/?.lua;$luacarray_dir/src/?/init.lua"
    add_lua_cpath="$luacarray_dir/src/build"

    # unset LUA_PATH_5_4 LUA_CPATH_5_4 LUA_PATH_5_3 LUA_CPATH_5_3 LUA_PATH_5_2 LUA_CPATH_5_2 LUA_PATH LUA_CPATH

    default_version=""
    if which lua > /dev/null 2>&1; then
        default_version=$(lua -e 'v=_VERSION:gsub("^Lua ","");print(v)')
    fi
    
    if [ -n "$default_version" ]; then
        if [ "$default_version" != "5.1" ]; then
            echo "Setting path for lua (version=$default_version)"
            lua_path_vers=$(echo $default_version|sed 's/\./_/')
            eval "export LUA_PATH_$lua_path_vers=\"$add_lua_path;$(lua -e 'print(package.path)')\""
            eval "export LUA_CPATH_$lua_path_vers=\"$add_lua_cpath/lua$default_version/?.so;$(lua -e 'print(package.cpath)')\""
        fi
    fi

    for vers in 5.4 5.3 5.2 5.1; do
        lua_cmd=""
        if which lua$vers > /dev/null 2>&1; then
            lua_cmd="lua$vers"
        elif which lua-$vers > /dev/null 2>&1; then
            lua_cmd="lua-$vers"
        fi
        if [ -n "$lua_cmd" ]; then
            lua_version=$($lua_cmd -e 'v=_VERSION:gsub("^Lua ","");print(v)')
            if [ "$lua_version" != "$default_version" ]; then
                echo "Setting path for $lua_cmd (version=$lua_version)"
                if [ "$lua_version" = "5.1" ]; then
                    export LUA_PATH="$add_lua_path;$($lua_cmd -e 'print(package.path)')"
                    export LUA_CPATH="$add_lua_cpath/lua5.1/?.so;$($lua_cmd -e 'print(package.cpath)')"
                else
                    lua_path_vers=$(echo $lua_version|sed 's/\./_/')
                    eval "export LUA_PATH_$lua_path_vers=\"$add_lua_path;$($lua_cmd -e 'print(package.path)')\""
                    eval "export LUA_CPATH_$lua_path_vers=\"$add_lua_cpath/lua$lua_version/?.so;$($lua_cmd -e 'print(package.cpath)')\""
                fi
            fi
        fi
    done
    
    if [ -n "$default_version" ]; then
        if [ "$default_version" = "5.1" ]; then
            echo "Setting path for lua (version=$default_version)"
            export LUA_PATH="$add_lua_path;$(lua -e 'print(package.path)')"
            export LUA_CPATH="$add_lua_cpath/lua5.1/?.so;$(lua -e 'print(package.cpath)')"
        fi
    fi
fi

unset lua_cmd this_dir luacarray_dir add_lua_path add_lua_cpath lua_version lua_path_vers vers default_version

