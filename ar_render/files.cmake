# list header
set(${PROJECT_NAME}_header_root
    "${PROJECT_SOURCE_DIR}/include/rpplugins/ar_render/plugin.hpp"
    "${PROJECT_SOURCE_DIR}/include/rpplugins/ar_render/ar_composite_stage.hpp"
)

set(${PROJECT_NAME}_headers
    ${${PROJECT_NAME}_header_root}
)

# grouping
source_group("ar_render" FILES ${${PROJECT_NAME}_header_root})



# list source
set(${PROJECT_NAME}_source_root
    "${PROJECT_SOURCE_DIR}/src/ar_composite_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/depthmap_render_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/depthmap_render_stage.hpp"
    "${PROJECT_SOURCE_DIR}/src/plugin.cpp"
)

set(${PROJECT_NAME}_sources
    ${${PROJECT_NAME}_source_root}
)

# grouping
source_group("src" FILES ${${PROJECT_NAME}_source_root})
