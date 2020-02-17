cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME eziengine)
project(${PROJECT_NAME})

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set(CMAKE_CXX_STANDARD 17)

#add_definitions(-Wno-implicit-int-float-conversion)

file(GLOB SOURCES 
"${CMAKE_CURRENT_LIST_DIR}/graphics/*.cpp"
"${CMAKE_CURRENT_LIST_DIR}/reflection/*.cpp" 
"${CMAKE_CURRENT_LIST_DIR}/serialization/*.cpp")

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME} CONAN_PKG::glm)
target_link_libraries(${PROJECT_NAME} CONAN_PKG::arduinojson)
target_link_libraries(${PROJECT_NAME} CONAN_PKG::rttr)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})

#add_executable(ezitest ${CMAKE_CURRENT_LIST_DIR}/test/main.cpp)

#target_link_libraries(ezitest ${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

install(TARGETS ${PROJECT_NAME}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
  FILES_MATCHING PATTERN "*.h"
)




