--[[
   @file repl.lua
   @brief   Read-eval-print loop for LUA
   @author  Juan Carrano <j.carrano@fu-berlin.de>
]]

local _R_EVAL = 0
local _R_CONT = 1
local _R_EXIT = 2
local _R_ERROR = 3

--[[ Read code from standard input (whatever stdin means for lua)
    @return action_code     what the eval loop should do
    @return code_or_msg     either code (_R_EVAL) or a message (_R_ERROR) or nil
                            (_R_CONT, _R_EXIT).
]]
local function re()
    io.write("L> ")
    local ln = io.read()

    if not ln then
        return _R_EXIT
    elseif ln == "\n" then
        return _R_CONT
    end

    local maybe_code, compile_err = load("return "..ln)

    if maybe_code == nil then
        maybe_code, compile_err = load(ln)
    end

    if maybe_code == nil then
        local _get_multiline = coroutine.create(
            function ()
                coroutine.yield(ln.."\n")
                while 1 do
                    io.write("L.. ");
                    local l = io.read()
                    if #l ~= 0 then
                        l = l.."\n"
                    end
                    coroutine.yield(l)
                end
            end
            )
        local get_multiline = function()
                local a, b = coroutine.resume(_get_multiline)
                if a then
                    return b
                else
                    return nil
                end
            end

        maybe_code, compile_err = load(get_multiline)
    end

    if maybe_code == nil then
        return _R_ERROR, compile_err
    else
        return _R_EVAL, maybe_code
    end
end

local function repl()
    while 1 do
        local action, fn_or_message = re()

        if action == _R_EVAL then
            local success, msg_or_ret = pcall(fn_or_message)
            if not success then
                print("Runtime error", msg_or_ret)
            elseif msg_or_ret ~= nil then
                print(msg_or_ret)
            end
        elseif action == _R_EXIT then
            print()
            return
        elseif action == _R_ERROR then
            print("Compile error:", msg)
        end -- (action == _R_CONT)
    end
end

repl()
