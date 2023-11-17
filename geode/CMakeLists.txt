cmake_minimum_required(VERSION 3.10.0)

# remove old files
file(REMOVE 
${CMAKE_CURRENT_BINARY_DIR}/../mod.json
${CMAKE_CURRENT_BINARY_DIR}/../about.md
${CMAKE_CURRENT_BINARY_DIR}/../changelog.md
${CMAKE_CURRENT_BINARY_DIR}/../support.md
${CMAKE_CURRENT_BINARY_DIR}/../logo.png
${CMAKE_CURRENT_BINARY_DIR}/../${developer}.${PROJECT_NAME}.dll
)

# remove old Resources folder
file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/../Resources)

#configure mod.json - requried by geode
configure_file(mod.json ${CMAKE_CURRENT_BINARY_DIR}/../mod.json)

#configure about.md
if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/about.md) #can be deleted so escape "not found"
    set(about ${CMAKE_CURRENT_BINARY_DIR}/../about.md)
    configure_file(about.md ${about})
endif() 

#configure changelog.md
if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/changelog.md) #can be deleted so escape "not found"
    set(changelog ${CMAKE_CURRENT_BINARY_DIR}/../changelog.md)
    configure_file(changelog.md ${changelog})
endif()

#configure support.md
if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/support.md) #can be deleted so escape "not found"
    set(support ${CMAKE_CURRENT_BINARY_DIR}/../support.md)
    configure_file(support.md ${support})
endif()

#configure logo.png
if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/logo.png) #can be deleted so escape "not found"
    set(logo ${CMAKE_CURRENT_BINARY_DIR}/../logo.png)
    configure_file(logo.png ${logo})
    file(COPY
    ${CMAKE_CURRENT_LIST_DIR}/logo.png
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../)
endif()

if(EXISTS ${LIBRARY_OUTPUT_PATH}/${PROJECT_NAME}.dll)
    file(COPY
    ${LIBRARY_OUTPUT_PATH}/${PROJECT_NAME}.dll
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../)

    if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/Resources) #can be deleted so escape "not found"
        file(COPY Resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../)
        set(Resources "Resources")
    endif()

    file(COPY ${LIBRARY_OUTPUT_PATH}/${PROJECT_NAME}.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../)
    file(RENAME ${CMAKE_CURRENT_BINARY_DIR}/../${PROJECT_NAME}.dll ${CMAKE_CURRENT_BINARY_DIR}/../${developer}.${PROJECT_NAME}.dll)

    file(ARCHIVE_CREATE
        OUTPUT "${CMAKE_CURRENT_LIST_DIR}/release/${developer}.${PROJECT_NAME}.geode"
        PATHS 
        ${CMAKE_CURRENT_BINARY_DIR}/../mod.json
        ${about}
        ${changelog}
        ${support}
        ${logo}
        ${Resources}
        ${developer}.${PROJECT_NAME}.dll
        FORMAT "zip")

    file(COPY "${CMAKE_CURRENT_LIST_DIR}/release/${developer}.${PROJECT_NAME}.geode" DESTINATION 
    ${GEODE_MOD_OUTPUT_PATH})

    message("geode makeup done: see at ${GEODE_MOD_OUTPUT_PATH} or ${CMAKE_CURRENT_LIST_DIR}/release/${developer}.${PROJECT_NAME}.geode")
else()
    message("geode makeup failed: ${LIBRARY_OUTPUT_PATH}/${PROJECT_NAME}.dll isnt exists yet (build is first).")
endif()
