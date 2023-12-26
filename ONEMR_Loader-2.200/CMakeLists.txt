cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /MANIFEST:NO")
set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}")
cmake_policy(SET CMP0057 NEW)

project ("ONEMR_Loader")
set(developer "user95401")
set(description "gd 2.2 mod that load dlls in all ur game directory")
set(lib_output "D:\\Games\\user666\\2.2 PC GDPS but actually that is lol\\") # post build copy

#curly-eureka
set(GAME_VERSION "2.200")
include("C:\\Users\\Lenovo\\source\\curly-eureka\\CURLY-EUREKA-INCL.cmake")# "\" must be escaped by \\

#version
set(FILEVERSION "1, 3, 0, 0")
set(LegalCopyright "user666's original")
configure_file("Version.rc" "../../../src/Version.rc")

#mod
file(GLOB_RECURSE SRC
src/*
)
add_library(${PROJECT_NAME} SHARED ${SRC} ${CURLY_EUREKA_SRC})

#POST_BUILD
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND #POST_BUILD UPDATE USES POWERSHELL!
"${CMAKE_CURRENT_LIST_DIR}/CMakePostBuild.bat" ${developer} ${PROJECT_NAME} ${LIBRARY_OUTPUT_PATH} ${lib_output})

#libcurl distributing zip makeup
if(EXISTS ${LIBRARY_OUTPUT_PATH}/${PROJECT_NAME}.dll) #can be deleted so escape "not found"
    #ONEMR_Loader
    file(COPY ${LIBRARY_OUTPUT_PATH}/${PROJECT_NAME}.dll
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
        ${CMAKE_CURRENT_BINARY_DIR}/copy.to.game.folder/${PROJECT_NAME}.dll
        ${CMAKE_CURRENT_BINARY_DIR}/copy.to.game.folder/libcurl.dll
        ${CMAKE_CURRENT_BINARY_DIR}/libcurldist_readme.md
        FORMAT "zip")
    message("${PROJECT_NAME}_dist makeup done: see at ${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}_dist.zip")
endif() 