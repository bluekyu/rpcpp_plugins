# list header
set(${PROJECT_NAME}_header_utils
    "${PROJECT_SOURCE_DIR}/include/rpflex/utils/helpers.hpp"
    "${PROJECT_SOURCE_DIR}/include/rpflex/utils/shape.hpp"
    "${PROJECT_SOURCE_DIR}/include/rpflex/utils/shape_box.hpp"
    "${PROJECT_SOURCE_DIR}/include/rpflex/utils/triangle_mesh.hpp"
)

set(${PROJECT_NAME}_header_root
    "${PROJECT_SOURCE_DIR}/include/rpflex/flex_buffer.hpp"
    "${PROJECT_SOURCE_DIR}/include/rpflex/instance_interface.hpp"
    "${PROJECT_SOURCE_DIR}/include/rpflex/plugin.hpp"
)

set(${PROJECT_NAME}_headers
    ${${PROJECT_NAME}_header_root}
    ${${PROJECT_NAME}_header_utils}
)

# grouping
source_group("rpflex\\utils" FILES ${${PROJECT_NAME}_header_utils})
source_group("rpflex" FILES ${${PROJECT_NAME}_header_root})



# list source
set(${PROJECT_NAME}_source_root
    "${PROJECT_SOURCE_DIR}/src/plugin.cpp"
)

set(${PROJECT_NAME}_sources
    ${${PROJECT_NAME}_source_root}
)

# grouping
source_group("src" FILES ${${PROJECT_NAME}_source_root})
