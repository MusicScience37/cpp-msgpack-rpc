message("Configure documentation of ${FULL_PROJECT_NAME}")

add_custom_target(${PROJECT_NAME}_doc)

set(${UPPER_PROJECT_NAME}_DOXYGEN_XML_PATH ${CMAKE_CURRENT_BINARY_DIR}/xml)
set(${UPPER_PROJECT_NAME}_HTML_ROOT ${CMAKE_CURRENT_BINARY_DIR}/html)

add_subdirectory(doxygen)

option(${UPPER_PROJECT_NAME}_BUILD_DOC_SPHINX
       "build documentation of ${FULL_PROJECT_NAME} with Sphinx" ON)
if(${UPPER_PROJECT_NAME}_BUILD_DOC_SPHINX)
    add_subdirectory(sphinx)
endif()

configure_file(${${UPPER_PROJECT_NAME}_SOURCE_DIR}/schemas/config_schema_v1.json
               ${${UPPER_PROJECT_NAME}_HTML_ROOT}/schemas/config_schema_v1.json)
configure_file(${${UPPER_PROJECT_NAME}_SOURCE_DIR}/schemas/config_schema_v1.json
               ${${UPPER_PROJECT_NAME}_HTML_ROOT}/schemas/config_schema.json)
