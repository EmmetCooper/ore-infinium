# - Find the yaml-cpp include file and library
#
#  YAMLCPP_FOUND - system has yaml-cpp library
#  YAMLCPP_LIBRARIES - The libraries needed to use yaml-cpp
#  YAMLCPP_INCLUDE_DIRS - Include paths needed to use yaml-cpp
#

find_path(YAMLCPP_INCLUDE_DIR yaml-cpp/yaml.h)
set(YAMLCPP_INCLUDE_DIRS ${YAMLCPP_INCLUDE_DIR})

if(WIN32)
    find_library(YAMLCPP_LIBRARY_RELEASE libyaml-cppmd)
    find_library(YAMLCPP_LIBRARY_DEBUG libyaml-cppmdd)

    include(SelectLibraryConfigurations)
    select_library_configurations(YAMLCPP)
else()
    find_library(YAMLCPP_LIBRARY yaml-cpp)
    set(YAMLCPP_LIBRARIES ${YAMLCPP_LIBRARY})
endif()


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(YamlCpp DEFAULT_MSG YAMLCPP_LIBRARIES YAMLCPP_INCLUDE_DIR)
