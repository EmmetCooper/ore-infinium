# - Find the yaml-cpp include file and library
#
#  YAMLCPP_FOUND - system has yaml-cpp library
#  YAMLCPP_LIBRARIES - The libraries needed to use yaml-cpp
#  YAMLCPP_INCLUDE_DIRS - Include paths needed to use yaml-cpp
#

find_path(YAMLCPP_INCLUDE_DIR yaml-cpp/yaml.h)
find_library(YAMLCPP_LIBRARY yaml-cpp)

set(YAMLCPP_INCLUDE_DIRS ${YAMLCPP_INCLUDE_DIR})
set(YAMLCPP_LIBRARIES ${YAMLCPP_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(YamlCpp DEFAULT_MSG YAMLCPP_LIBRARY YAMLCPP_INCLUDE_DIR)
