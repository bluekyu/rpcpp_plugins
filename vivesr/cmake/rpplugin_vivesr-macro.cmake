include(CMakeFindDependencyMacro)
if(NOT TARGET ViveSR::ViveSR)
    find_package(ViveSR REQUIRED)
endif()
