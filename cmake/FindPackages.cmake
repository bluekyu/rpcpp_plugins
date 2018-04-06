# Author: Younguk Kim (bluekyu)

function(_find_boost required_component)
    if(NOT TARGET Boost::boost)
        option(Boost_USE_STATIC_LIBS    "Boost uses static libraries" OFF)
        option(Boost_USE_MULTITHREADED  "Boost uses multithreaded"  ON)
        option(Boost_USE_STATIC_RUNTIME "Boost uses static runtime" OFF)

        set(BOOST_ROOT "" CACHE PATH "Hint for finding boost root directory")
    endif()

    set(missed_component "")
    foreach(component ${required_component})
        if(NOT TARGET Boost::${component})
            list(APPEND missed_component ${component})
        endif()
    endforeach()

    if(missed_component)
        find_package(Boost REQUIRED ${missed_component})
    endif()

    if(NOT TARGET Boost::boost)
        find_package(Boost REQUIRED)
    endif()
endfunction()

_find_boost("filesystem")

# find panda3d
set(panda3d_ROOT "" CACHE PATH "Hint for finding panda3d root directory")
find_package(panda3d REQUIRED p3framework p3direct)

if(NOT TARGET render_pipeline)
    # find render_pipeline
    find_package(render_pipeline REQUIRED)
endif()
