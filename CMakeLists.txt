cmake_minimum_required( VERSION 3.5 )

project( Platon )
set(DEFAULT_SYSTEM_INCLUDE_FOLDERS ${CMAKE_SOURCE_DIR}/platonlib/include ${CMAKE_SOURCE_DIR}/libc++/libcxx/include ${CMAKE_SOURCE_DIR}/libc/musl/include ${CMAKE_SOURCE_DIR}/boost/include ${CMAKE_SOURCE_DIR}/rapidjson/include)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

include(FindWasm)
include(wasm)


add_subdirectory(libc++)
add_subdirectory(libc)
add_subdirectory(platonlib)
add_subdirectory(hello)
add_subdirectory(user)

if (TESTS)
    add_subdirectory(test)
endif()
