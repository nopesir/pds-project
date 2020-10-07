    cmake_minimum_required(VERSION 3.13)
    project(ServerModule)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lcurl -pthread")

find_package(Qt5Sql REQUIRED)

add_compile_options(-l sqlite3)
add_definitions(-DBOOST_ERROR_CODE_HEADER_ONLY)
add_executable(ServerModule main.cpp Server.cpp Server.h Session.cpp Session.h jsonUtility.cpp jsonUtility.h dbService.cpp dbService.h Client.cpp Client.h Message.cpp Message.h fileUtility.cpp fileUtility.h Symbol.cpp Symbol.h File.cpp File.h SymStyle.h SymStyle.cpp email.cpp email.h SharedEditor.cpp SharedEditor.h)

target_link_libraries(ServerModule LINK_PUBLIC sqlite3 Qt5::Sql curl)
