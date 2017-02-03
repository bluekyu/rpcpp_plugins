set(source_src
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
)

# grouping
source_group("src" FILES ${source_src})

set(rpcpp_server_sources
    ${source_src}
)
