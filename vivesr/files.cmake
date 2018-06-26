# list header
set(${PROJECT_NAME}_header_root
    "${PROJECT_SOURCE_DIR}/include/rpplugins/vivesr/plugin.hpp"
    "${PROJECT_SOURCE_DIR}/include/rpplugins/vivesr/see_through_module.hpp"
)

set(${PROJECT_NAME}_headers
    ${${PROJECT_NAME}_header_root}
)

# grouping
source_group("vivesr" FILES ${${PROJECT_NAME}_header_root})



# list source
set(${PROJECT_NAME}_source_root
    "${PROJECT_SOURCE_DIR}/src/plugin.cpp"
    "${PROJECT_SOURCE_DIR}/src/see_through_module.cpp"
)

set(${PROJECT_NAME}_sources
    ${${PROJECT_NAME}_source_root}
)

# grouping
source_group("src" FILES ${${PROJECT_NAME}_source_root})
