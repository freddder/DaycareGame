#pragma once

struct lua_State;

int ShowDialogLine(lua_State* L);
int ShowDialogOptions(lua_State* L);
int EndDialog(lua_State* L);