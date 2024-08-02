cmake_minimum_required(VERSION 3.12)

project(lflLib_test)

file(GLOB_RECURSE HEADER_TEST "inc/*.hpp" "inc/*.h")
file(GLOB_RECURSE SOURCE_TEST "src/*.cpp" "src/*.c")

file(GLOB_RECURSE HEADER_SCR "../inc/*.hpp" "../inc/*.h")   # Recompile Source for testing
file(GLOB_RECURSE SOURCE_SCR "../src/*.cpp" "../src/*.c")   # Recompile Source for testing

foreach(item ${SOURCE_SCR})
    if(${item} MATCHES "src/main.cpp")
        list(REMOVE_ITEM SOURCE_SCR ${item})
    endif(${item} MATCHES "src/main.cpp")
endforeach(item)

find_package(GTest REQUIRED)

add_executable(${PROJECT_NAME}  ${SOURCE_SCR} ${SOURCE_TEST}  )

target_include_directories(${PROJECT_NAME} PRIVATE "../inc" "inc" )
target_link_libraries(${PROJECT_NAME} PRIVATE GTest::GTest)

