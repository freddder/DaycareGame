#include "LuaCallable.h"

#include "Lua.hpp"

#include "Engine.h"
#include "cUIManager.h"

int ShowDialogLine(lua_State* L)
{
    std::string s = lua_tostring(L, 1);

    Manager::ui.ShowDialogLine(s);

    return 0;
}

int ShowDialogOptions(lua_State* L)
{
    std::vector<std::string> options;

    int len = luaL_len(L, 1);

    for (int i = 1; i <= len; ++i) {
        lua_geti(L, 1, i);

        if (!lua_isstring(L, -1)) {
            lua_pop(L, 1);
            return luaL_error(L, "Expected all elements to be strings");
        }

        std::string str = lua_tostring(L, -1);
        options.emplace_back(str);

        lua_pop(L, 1);
    }

    Manager::ui.ShowDialogOptions(options);

    return 0;
}

int EndDialog(lua_State* L)
{
    Manager::ui.EndDialog();
    return 0;
}
