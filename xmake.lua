add_rules("mode.debug", "mode.release")
set_languages("c++17")

target("JiYuUnlock")
    set_kind("binary")
    add_files("src/*.cpp")
    add_includedirs("include")
    
    -- 链接 Windows 核心库
    add_syslinks("ws2_32", "user32", "advapi32", "psapi", "shlwapi")
    
    -- 定义宏，避免 Winsock 冲突
    add_defines("WIN32_LEAN_AND_MEAN", "_WINSOCKAPI_")
    if is_mode("debug") then
        add_defines("DEBUG")
    end
    
    -- 设置运行时库为多线程 (/MT)
    set_runtimes("MT")