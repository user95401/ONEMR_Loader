cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /MANIFEST:NO")
cmake_policy(SET CMP0057 NEW)

project ("ONEMR_Loader")
set(developer "user95401")
set(version "v1.2.0")
set(description "load dlls in all ur game directory")
set(repository "https://github.com/user95401/ONEMR_Loader")
set(issues_url "https://discord.gg/89VGJH6QQg")
set(issues_info "U can write about it on <cb>user666's discord...</c>")
set(LIBRARY_OUTPUT_PATH "D:\\Games\\user666\\Geometry Dash 2.2 [modloader, unlock mod, steam crack]\\") # dll output
set(GEODE_MOD_OUTPUT_PATH "D:\\Games\\user666\\Geometry.Dash.v07.24.2021\\geode\\mods") # geode mod output

#curly-eureka
include("C:\\Users\\Lenovo\\source\\curly-eureka\\CURLY-EUREKA-INCL.cmake")

#mod
file(GLOB_RECURSE SRC
src/*
)
add_library(${PROJECT_NAME} SHARED ${SRC} ${CURLY_EUREKA_SRC})

#geode-makeup
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/geode geode-build)

#libcurl distributing zip makeup
if(EXISTS ${LIBRARY_OUTPUT_PATH}/ONEMR_Loader.dll) #can be deleted so escape "not found"
    #ONEMR_Loader
    file(COPY ${LIBRARY_OUTPUT_PATH}/ONEMR_Loader.dll
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/copy.to.game.folder/)
    #libcurl
    file(COPY ${CMAKE_CURRENT_LIST_DIR}/libcurl.dll
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/copy.to.game.folder/)
    #libcurldist_readme
    configure_file(${CMAKE_CURRENT_LIST_DIR}/libcurldist_readme.md ${CMAKE_CURRENT_BINARY_DIR}/libcurldist_readme.md)
    #ARCHIVE_CREATE
    file(ARCHIVE_CREATE
        OUTPUT "${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}_dist.zip"
        PATHS 
        ${CMAKE_CURRENT_BINARY_DIR}/copy.to.game.folder/ONEMR_Loader.dll
        ${CMAKE_CURRENT_BINARY_DIR}/copy.to.game.folder/libcurl.dll
        ${CMAKE_CURRENT_BINARY_DIR}/libcurldist_readme.md
        ${CMAKE_CURRENT_BINARY_DIR}/about.md
        FORMAT "zip")
    message("${PROJECT_NAME}_dist makeup done: see at ${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}_dist.zip")
endif() 