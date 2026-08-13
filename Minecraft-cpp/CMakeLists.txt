file(GLOB_RECURSE MYMC_SOURCES CONFIGURE_DEPENDS *.cpp *.cc *.c)
file(GLOB_RECURSE MYMC_HEADERS CONFIGURE_DEPENDS *.h *.hpp)

# 将源代码添加到此项目的可执行文件。
add_executable (Minecraft-cpp main.cpp ${MYMC_SOURCES} ${MYMC_HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE 
    src
)