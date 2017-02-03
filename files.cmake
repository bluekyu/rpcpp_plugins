set(source_src_restapi
    "${PROJECT_SOURCE_DIR}/src/restapi/config.hpp"
    "${PROJECT_SOURCE_DIR}/src/restapi/resolve_message.cpp"
    "${PROJECT_SOURCE_DIR}/src/restapi/resolve_message.hpp"
    "${PROJECT_SOURCE_DIR}/src/restapi/restapi_server.cpp"
    "${PROJECT_SOURCE_DIR}/src/restapi/restapi_server.hpp"
)

set(source_src
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
)

# grouping
source_group("src\\restapi" FILES ${source_src_restapi})
source_group("src" FILES ${source_src})

set(rpcpp_server_sources
    ${source_src_restapi}
    ${source_src}
)
