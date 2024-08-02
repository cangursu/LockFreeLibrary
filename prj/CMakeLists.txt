
cmake_minimum_required(VERSION 3.12)

project(lflPrj)

add_subdirectory(test)

include_directories(inc)

file(GLOB_RECURSE HEADER "inc/*.hpp" "inc/*.h")
file(GLOB_RECURSE SOURCE "src/*.cpp" "src/*.c")

set (lflLib "${CMAKE_SOURCE_DIR}/lib/")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} ${SOURCE} ${HEADER})
set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME lfl)

# source_group("Header include" FILES ${HEADER})
# source_group("Source src"     FILES ${SOURCE})

# link libraries
target_include_directories(${PROJECT_NAME}
                    PRIVATE  "inc"
                    PUBLIC "${lflLib}/inc")
# target_link_libraries(${PROJECT_NAME} -lpthread -lboost_system -lssl -lcrypto -lrt)
target_link_libraries(${PROJECT_NAME} lfl)

install(TARGETS ${PROJECT_NAME}
    LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR}
#    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

