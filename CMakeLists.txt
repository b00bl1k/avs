cmake_minimum_required(VERSION 3.5)

project(avs C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)

set(YDER_MIN_VERSION "1.3.3")
find_package(Yder ${YDER_MIN_VERSION} REQUIRED)
set(LIBS ${LIBS} ${YDER_LIBRARIES})

set(ULFIUS_MIN_VERSION "2.3.6")
find_package(Ulfius ${ULFIUS_MIN_VERSION} REQUIRED)
set(LIBS ${LIBS} ${ULFIUS_LIBRARIES})

set(JANSSON_MIN_VERSION 2.10)
find_package(Jansson ${JANSSON_MIN_VERSION} REQUIRED)
set(LIBS ${LIBS} ${JANSSON_LIBRARIES})

find_package(SQLite3 REQUIRED)
set(LIBS ${LIBS} ${SQLITE3_LIBRARIES})

set(AVS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/db.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/endpoints.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opts.c)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
    execute_process(
        COMMAND git log -1 --format=%h
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
  set(GIT_COMMIT_HASH "")
endif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")

message(STATUS "Git commit hash: ${GIT_COMMIT_HASH}")
message(STATUS "Generating version.h")

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in
    ${CMAKE_BINARY_DIR}/generated/version.h
)

include_directories(${CMAKE_BINARY_DIR}/generated)

add_executable(avs ${AVS_SRC})
target_link_libraries(avs ${LIBS})
