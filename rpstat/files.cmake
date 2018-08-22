# list header
set(${PROJECT_NAME}_header_root
    "${PROJECT_SOURCE_DIR}/include/rpplugins/rpstat/plugin.hpp"
)

set(${PROJECT_NAME}_headers
    ${${PROJECT_NAME}_header_root}
)

# grouping
source_group("rpstat" FILES ${${PROJECT_NAME}_header_root})



# list source
set(${PROJECT_NAME}_source_ImGuizmo
    "${PROJECT_SOURCE_DIR}/src/ImGuizmo/imgui_internal.h"
    "${PROJECT_SOURCE_DIR}/src/ImGuizmo/ImGuizmo.cpp"
    "${PROJECT_SOURCE_DIR}/src/ImGuizmo/ImGuizmo.h"
    "${PROJECT_SOURCE_DIR}/src/ImGuizmo/stb_textedit.h"
)

set(${PROJECT_NAME}_source_root
    "${PROJECT_SOURCE_DIR}/src/material_window.cpp"
    "${PROJECT_SOURCE_DIR}/src/material_window.hpp"
    "${PROJECT_SOURCE_DIR}/src/nodepath_window.cpp"
    "${PROJECT_SOURCE_DIR}/src/nodepath_window.hpp"
    "${PROJECT_SOURCE_DIR}/src/plugin.cpp"
    "${PROJECT_SOURCE_DIR}/src/scenegraph_window.cpp"
    "${PROJECT_SOURCE_DIR}/src/scenegraph_window.hpp"
    "${PROJECT_SOURCE_DIR}/src/texture_window.cpp"
    "${PROJECT_SOURCE_DIR}/src/texture_window.hpp"
    "${PROJECT_SOURCE_DIR}/src/window_interface.cpp"
    "${PROJECT_SOURCE_DIR}/src/window_interface.hpp"
)

set(${PROJECT_NAME}_sources
    ${${PROJECT_NAME}_source_ImGuizmo}
    ${${PROJECT_NAME}_source_root}
)

# grouping
source_group("src\\ImGuizmo" FILES ${${PROJECT_NAME}_source_ImGuizmo})
source_group("src" FILES ${${PROJECT_NAME}_source_root})
