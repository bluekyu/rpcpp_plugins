set(${PROJECT_NAME}_header_include
    "${PROJECT_SOURCE_DIR}/include/rpeditor_server_plugin.hpp"
    "${PROJECT_SOURCE_DIR}/include/api_server_interface.hpp"
)

# grouping
source_group("include" FILES ${${PROJECT_NAME}_header_include})

set(${PROJECT_NAME}_headers
    ${header_include}
)


set(${PROJECT_NAME}_source_src_restapi_resources
    "${PROJECT_SOURCE_DIR}/src/restapi/resources/common.cpp"
    "${PROJECT_SOURCE_DIR}/src/restapi/resources/common.hpp"
    "${PROJECT_SOURCE_DIR}/src/restapi/resources/geomnode.cpp"
    "${PROJECT_SOURCE_DIR}/src/restapi/resources/material.cpp"
    "${PROJECT_SOURCE_DIR}/src/restapi/resources/nodepath.cpp"
    "${PROJECT_SOURCE_DIR}/src/restapi/resources/showbase.cpp"
)

set(${PROJECT_NAME}_source_src_restapi
    "${PROJECT_SOURCE_DIR}/src/restapi/config.hpp"
    "${PROJECT_SOURCE_DIR}/src/restapi/resolve_message.cpp"
    "${PROJECT_SOURCE_DIR}/src/restapi/resolve_message.hpp"
    "${PROJECT_SOURCE_DIR}/src/restapi/restapi_server.cpp"
    "${PROJECT_SOURCE_DIR}/src/restapi/restapi_server.hpp"
)

set(${PROJECT_NAME}_source_src
    "${PROJECT_SOURCE_DIR}/src/rpeditor_server_plugin.cpp"
)

# grouping
source_group("src\\restapi\\resources" FILES ${${PROJECT_NAME}_source_src_restapi_resources})
source_group("src\\restapi" FILES ${${PROJECT_NAME}_source_src_restapi})
source_group("src" FILES ${${PROJECT_NAME}_source_src})

set(${PROJECT_NAME}_sources
    ${${PROJECT_NAME}_source_src_restapi_resources}
    ${${PROJECT_NAME}_source_src_restapi}
    ${${PROJECT_NAME}_source_src}
)
