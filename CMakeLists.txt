cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(vdb LANGUAGES CXX)

add_executable(vdb vdb/main.cpp
        vdb/core/threadpool.h vdb/core/threadpool.cpp
        vdb/core/corotask.h
        vdb/core/pooltask.h
        vdb/core/sqlquerybuilder.h
        vdb/core/sqlresponse.cpp
        vdb/core/sqlresponse.h
)

target_include_directories(vdb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

include(GNUInstallDirs)
install(TARGETS vdb
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
