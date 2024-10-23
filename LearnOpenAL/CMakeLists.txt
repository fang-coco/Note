cmake_minimum_required(VERSION 3.5)

project(OpenALWithFFmpeg)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER clang++)

include_directories(
  /opt/homebrew/Cellar/ffmpeg/7.1/include/
  /opt/homebrew/Cellar/openal-soft/1.23.1/include/
)

link_directories(
  /opt/homebrew/Cellar/openal-soft/1.23.1/lib/
  /opt/homebrew/Cellar/ffmpeg/7.1/lib/
)

set(LIBS
  avformat
  avcodec
  swresample
  avutil
  openal
)

if(APPLE)
  INCLUDE_DIRECTORIES(/System/Library/Frameworks)
  FIND_LIBRARY(COCOA_LIBRARY Cocoa)
  FIND_LIBRARY(IOKit_LIBRARY IOKit)
  FIND_LIBRARY(CoreVideo_LIBRARY CoreVideo)
  MARK_AS_ADVANCED(COCOA_LIBRARY OpenGL_LIBRARY)
  SET(APPLE_LIBS ${COCOA_LIBRARY} ${IOKit_LIBRARY} ${OpenGL_LIBRARY} ${CoreVideo_LIBRARY})
  SET(APPLE_LIBS ${APPLE_LIBS} ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY} ${FREETYPE_LIBRARIES})
  set(LIBS ${LIBS} ${APPLE_LIBS})
endif()

# # 查找 OpenAL 库
# find_package(OpenAL REQUIRED)
#
# # 显示找到的 OpenAL 库信息
# if(OpenAL_FOUND)
#     message(STATUS "Found OpenAL: ${OpenAL_LIBRARIES}")
# endif()


# 设置头文件路径
include_directories(${OpenAL_INCLUDE_DIRS})


add_executable(demo ./demo1.cc)

# 链接 OpenAL 库
target_link_libraries(demo PRIVATE ${LIBS})
