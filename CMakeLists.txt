cmake_minimum_required(VERSION 3.5)

project(avs C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)

set(ORCANIA_VERSION_DOWNLOAD "1.2.3")
set(YDER_VERSION_DOWNLOAD "1.3.3")
set(ULFIUS_VERSION_DOWNLOAD "2.3.6")

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

set(Orcania_FIND_QUIETLY ON)
include(FindOrcania)
find_package(Orcania 1.1 QUIET)
if (NOT ORCANIA_FOUND)
    include(DownloadProject)
    option(INSTALL_HEADER "specific param" off)
    download_project(PROJ orcania
            URL "https://github.com/babelouest/orcania/archive/v${ORCANIA_VERSION_DOWNLOAD}.tar.gz"
            QUIET)
    add_subdirectory(${orcania_SOURCE_DIR} ${orcania_BINARY_DIR})
    include_directories(${orcania_SOURCE_DIR}/include)
    add_dependencies(avs orcania)
		set(ORCANIA_LIBRARIES orcania)
endif ()
set(LIBS ${LIBS} ${ORCANIA_LIBRARIES})

set(Yder_FIND_QUIETLY ON)
set(YDER_MIN_VERSION "1.3.3")
find_package(Yder ${YDER_MIN_VERSION} QUIET)
if (NOT YDER_FOUND)
    include(DownloadProject)
    option(CHECK_ORCANIA "specific param" off)
    option(INSTALL_HEADER "specific param" off)
    download_project(PROJ yder
            URL "https://github.com/babelouest/yder/archive/v${YDER_VERSION_DOWNLOAD}.tar.gz"
            QUIET)
    add_subdirectory(${yder_SOURCE_DIR} ${yder_BINARY_DIR})
    include_directories(${yder_SOURCE_DIR}/include)
    include_directories(${orcania_SOURCE_DIR}/include)
    add_dependencies(avs yder)
		set(YDER_LIBRARIES yder)
endif ()
set(LIBS ${LIBS} ${YDER_LIBRARIES})

set(Ulfius_FIND_QUIETLY ON)
set(ULFIUS_MIN_VERSION "2.3.6")
find_package(Ulfius ${ULFIUS_MIN_VERSION} QUIET)
if (NOT ULFIUS_FOUND)
    include(DownloadProject)
    option(WITH_WEBSOCKET "specific param" off)
    option(INSTALL_HEADER "specific param" off)
    option(CHECK_YDER "specific param" off)
    download_project(PROJ ulfius
            URL "https://github.com/babelouest/ulfius/archive/v${ULFIUS_VERSION_DOWNLOAD}.tar.gz"
            QUIET)
    add_subdirectory(${ulfius_SOURCE_DIR} ${ulfius_BINARY_DIR})
    include_directories(${yder_SOURCE_DIR}/include)
    include_directories(${orcania_SOURCE_DIR}/include)
    include_directories(${ulfius_SOURCE_DIR}/include)
    add_dependencies(avs ulfius)
		set(ULFIUS_LIBRARIES ulfius)
endif ()
set(LIBS ${LIBS} ${ULFIUS_LIBRARIES})

set(JANSSON_MIN_VERSION 2.7)
find_package(Jansson ${JANSSON_MIN_VERSION} REQUIRED)
set(LIBS ${LIBS} ${JANSSON_LIBRARIES})

find_package(SQLite3 REQUIRED)
set(LIBS ${LIBS} ${SQLITE3_LIBRARIES})

target_link_libraries(avs ${LIBS})
