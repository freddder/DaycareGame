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

int EndDialog(lua_State* L)
{
    Manager::ui.EndDialog();
    return 0;
}
