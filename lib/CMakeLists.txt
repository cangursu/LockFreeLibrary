
cmake_minimum_required(VERSION 3.12)

project(lfl)

add_subdirectory(test)

set(PROJECT_VERSION 0)
set(PROJECT_VERSION_MAJOR 1)

include_directories(inc)

file(GLOB_RECURSE PUBHEADER  "inc/*.hpp" "inc/*.h")
file(GLOB_RECURSE HEADER "inc/*.hpp" "inc/*.h")
list (APPEND HEADER ${PUBHEADER})

file(GLOB_RECURSE SOURCE "src/*.cpp" "src/*.c")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library   (${PROJECT_NAME} SHARED ${HEADER} ${SOURCE})

set_target_properties(${PROJECT_NAME}     PROPERTIES VERSION       ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME}     PROPERTIES SOVERSION     ${PROJECT_VERSION_MAJOR})
set_target_properties(${PROJECT_NAME}     PROPERTIES PUBLIC_HEADER "${PUBHEADER}")


# source_group("Header include" FILES ${HEADER})
# source_group("Source src"     FILES ${SOURCE})

# link libraries
target_include_directories(${PROJECT_NAME} PRIVATE  "inc" )
# target_link_libraries(${PROJECT_NAME} -lpthread -lboost_system -lssl -lcrypto -lrt)

install(TARGETS ${PROJECT_NAME}
    LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

