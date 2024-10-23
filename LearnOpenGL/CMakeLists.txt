cmake_minimum_required(VERSION 3.5)

project(LearnOpenGL)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)

add_library(GLAD ${CMAKE_SOURCE_DIR}/src/glad.c)
add_library(STB_IMAGE ${CMAKE_SOURCE_DIR}/src/stb_image.c)

add_subdirectory(third_party/imgui)

include_directories(
  inc
  /opt/homebrew/include/
  /opt/homebrew/include/freetype2/
  third_party/imgui/
  third_party/imgui/backends/
)


link_directories(
  lib
  /opt/homebrew/lib/
)

set(LIBS
  glfw3
  glm
  assimp
  freetype
  GLAD
  STB_IMAGE
  IMGUI
)

if(APPLE)
  INCLUDE_DIRECTORIES(/System/Library/Frameworks)
  FIND_LIBRARY(COCOA_LIBRARY Cocoa)
  FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
  FIND_LIBRARY(IOKit_LIBRARY IOKit)
  FIND_LIBRARY(CoreVideo_LIBRARY CoreVideo)
  MARK_AS_ADVANCED(COCOA_LIBRARY OpenGL_LIBRARY)
  SET(APPLE_LIBS ${COCOA_LIBRARY} ${IOKit_LIBRARY} ${OpenGL_LIBRARY} ${CoreVideo_LIBRARY})
  SET(APPLE_LIBS ${APPLE_LIBS} ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY} ${FREETYPE_LIBRARIES})
  set(LIBS ${LIBS} ${APPLE_LIBS})
endif()

# add_executable(loadModel ${CMAKE_SOURCE_DIR}/src/loadModel.cc)
# target_link_libraries(loadModel ${LIBS})
#
# add_executable(pbrLighting ${CMAKE_SOURCE_DIR}/src/pbrLighting.cc)
# target_link_libraries(pbrLighting ${LIBS})

#
# add_executable(loadImgui ${CMAKE_SOURCE_DIR}/src/loadImgui.cc)
# target_link_libraries(loadImgui PRIVATE ${LIBS})
#
# add_executable(loadTexture ${CMAKE_SOURCE_DIR}/src/loadTexture.cc)
# target_link_libraries(loadTexture PRIVATE ${LIBS})
#
# add_executable(loadFonts ${CMAKE_SOURCE_DIR}/src/loadFonts.cc)
# target_link_libraries(loadFonts PRIVATE ${LIBS})
#
# add_executable(fontsImgui ${CMAKE_SOURCE_DIR}/src/fontswithimgui.cc)
# target_link_libraries(fontsImgui PRIVATE ${LIBS})
add_executable(hello3DSpace ${CMAKE_CURRENT_SOURCE_DIR}/src/hello3DSpace.cc)
target_link_libraries(hello3DSpace PRIVATE ${LIBS})
