file(GLOB_RECURSE MYMC_SOURCES CONFIGURE_DEPENDS *.cpp *.cc *.c)
file(GLOB_RECURSE MYMC_HEADERS CONFIGURE_DEPENDS *.h *.hpp)

# 将源代码添加到此项目的可执行文件。
add_executable(Minecraft-cpp main.cpp ${MYMC_SOURCES} ${MYMC_HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
        src
)

# 将运行时 DLL 复制到可执行文件目录，确保 CLion/命令行调试时能加载
# 1) GLEW 共享库（glew32d.dll）
# 2) MinGW 运行时库（libstdc++/libgcc_s/winpthread），MSVC 构建无此依赖
if (WIN32)
    add_custom_command(TARGET Minecraft-cpp POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:glew>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
    if (MINGW)
        get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
        add_custom_command(TARGET Minecraft-cpp POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${MINGW_BIN_DIR}/libstdc++-6.dll"
                "${MINGW_BIN_DIR}/libgcc_s_seh-1.dll"
                "${MINGW_BIN_DIR}/libwinpthread-1.dll"
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
        )
    endif ()
endif ()