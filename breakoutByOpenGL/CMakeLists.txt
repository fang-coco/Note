cmake_minimum_required(VERSION 3.5)

project(BreakOut)
set(CMAKE_C_COMPILER /usr/bin/clang)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

include_directories(
  inc/
  ext/
  /opt/homebrew/include/
  /opt/homebrew/include/freetype2/
  /opt/homebrew/Cellar/ffmpeg/7.1/include/
  /opt/homebrew/Cellar/openal-soft/1.23.1/include/
)


link_directories(
  lib/
  /opt/homebrew/Cellar/openal-soft/1.23.1/lib/
  /opt/homebrew/Cellar/ffmpeg/7.1/lib/
  /opt/homebrew/lib/
)

add_library(GLAD ext/glad/glad.c)
add_library(STB_IMAGE ext/stb_image/stb_image.c)


set(LIBS
  glfw3
  avformat
  avcodec
  swresample
  avutil
  openal
  freetype
  GLAD
  STB_IMAGE
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


aux_source_directory(src SRC)


add_executable(BreakOut ${SRC})
target_link_libraries(BreakOut ${LIBS})
