/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup  pkg_lua
 * @ingroup  pkg
 * @ingroup  sys
 * @{
 * @file
 *
 * @brief   Definitions for including built-in modules.
 * @author  Juan Carrano <j.carrano@fu-berlin.de>
 *
 */

#ifndef LUA_BUILTIN_H
#define LUA_BUILTIN_H

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Entry describing a pure lua module whose source is built into the
 * application binary.
 */
struct luaR_builtin_lua {
    const char *name;   /** Name of the module */
    const char *code;   /** Lua source code buffer*/
    size_t code_size;   /** Size of the source code buffer. */
};

/**
 * Entry describing a c lua module built into the
 * application binary.
 */
struct luaR_builtin_c {
    const char *name;
    int (*luaopen)(lua_State *);
};

/** Table containing all built in pure lua modules */
extern const struct luaR_builtin_lua *const luaR_builtin_lua_table;
/** Number of elements of luaR_builtin_lua_table */
extern const size_t luaR_builtin_lua_table_len;

/** Table containing all built in c lua modules */
extern const struct luaR_builtin_c *const luaR_builtin_c_table;
/** Number of elements of luaR_builtin_c_table */
extern const size_t luaR_builtin_c_table_len;

#ifdef __cplusplus
extern "C" }
#endif

#endif /* LUA_BUILTIN_H */

/** @} */
