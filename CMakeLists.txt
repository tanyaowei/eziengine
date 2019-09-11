cmake_minimum_required(VERSION 3.5)

project (eziengine VERSION 1.0.0)

SET(BUILD_STATIC ON CACHE BOOL "Build rttr static library" FORCE)
SET(BUILD_WITH_STATIC_RUNTIME_LIBS OFF CACHE BOOL "Build with static runtime library" FORCE)
SET(BUILD_RTTR_DYNAMIC OFF CACHE BOOL "Disable rttr dynamic library" FORCE)
SET(BUILD_EXAMPLES OFF CACHE BOOL "Disable rttr examples" FORCE)
SET(BUILD_UNIT_TESTS OFF CACHE BOOL "Disable rttr unittests" FORCE)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/dependencies/glm)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/dependencies/rttr)

file(GLOB SOURCES 
"${CMAKE_CURRENT_LIST_DIR}/source/graphics/*.cpp"
"${CMAKE_CURRENT_LIST_DIR}/source/reflection/*.cpp" 
"${CMAKE_CURRENT_LIST_DIR}/source/serialization/*.cpp")

add_library(eziengine STATIC ${SOURCES})

target_link_libraries(eziengine glm_static RTTR::Core_Lib)

target_include_directories(eziengine PRIVATE ${CMAKE_CURRENT_LIST_DIR}/dependencies/glm)
target_include_directories(eziengine PRIVATE ${CMAKE_CURRENT_LIST_DIR}/dependencies/rttr/src)
target_include_directories(eziengine PUBLIC ${CMAKE_CURRENT_LIST_DIR}/dependencies/ArduinoJson/src)

target_include_directories(eziengine PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

add_executable(ezitest ${CMAKE_CURRENT_LIST_DIR}/test/main.cpp)

target_link_libraries(ezitest eziengine)

install(TARGETS ezitest
  DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

install(TARGETS eziengine
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
  FILES_MATCHING PATTERN "*.h"
)




