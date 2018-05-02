cmake_minimum_required(VERSION 3.7.0)
project(XML-Beispiel VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 14)

include(FindXercesC)
find_package(XercesC REQUIRED)
add_library(XercesC SHARED IMPORTED)
set_target_properties(XercesC PROPERTIES IMPORTED_LOCATION "${XercesC_LIBRARIES}")
set_target_properties(XercesC PROPERTIES INCLUDE_DIRECTORIES "${XercesC_INCLUDE_DIRS}")

add_executable(xerxes-beispiel xerces.cpp)
target_link_libraries(xerxes-beispiel XercesC pthread)




