# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.30

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.30.4/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.30.4/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/fanglee/Notes/LearnOpenGL

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/fanglee/Notes/LearnOpenGL/build

# Include any dependencies generated for this target.
include CMakeFiles/STB_IMAGE.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/STB_IMAGE.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/STB_IMAGE.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/STB_IMAGE.dir/flags.make

CMakeFiles/STB_IMAGE.dir/src/stb_image.c.o: CMakeFiles/STB_IMAGE.dir/flags.make
CMakeFiles/STB_IMAGE.dir/src/stb_image.c.o: /Users/fanglee/Notes/LearnOpenGL/src/stb_image.c
CMakeFiles/STB_IMAGE.dir/src/stb_image.c.o: CMakeFiles/STB_IMAGE.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/fanglee/Notes/LearnOpenGL/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/STB_IMAGE.dir/src/stb_image.c.o"
	clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/STB_IMAGE.dir/src/stb_image.c.o -MF CMakeFiles/STB_IMAGE.dir/src/stb_image.c.o.d -o CMakeFiles/STB_IMAGE.dir/src/stb_image.c.o -c /Users/fanglee/Notes/LearnOpenGL/src/stb_image.c

CMakeFiles/STB_IMAGE.dir/src/stb_image.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/STB_IMAGE.dir/src/stb_image.c.i"
	clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/fanglee/Notes/LearnOpenGL/src/stb_image.c > CMakeFiles/STB_IMAGE.dir/src/stb_image.c.i

CMakeFiles/STB_IMAGE.dir/src/stb_image.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/STB_IMAGE.dir/src/stb_image.c.s"
	clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/fanglee/Notes/LearnOpenGL/src/stb_image.c -o CMakeFiles/STB_IMAGE.dir/src/stb_image.c.s

# Object files for target STB_IMAGE
STB_IMAGE_OBJECTS = \
"CMakeFiles/STB_IMAGE.dir/src/stb_image.c.o"

# External object files for target STB_IMAGE
STB_IMAGE_EXTERNAL_OBJECTS =

libSTB_IMAGE.a: CMakeFiles/STB_IMAGE.dir/src/stb_image.c.o
libSTB_IMAGE.a: CMakeFiles/STB_IMAGE.dir/build.make
libSTB_IMAGE.a: CMakeFiles/STB_IMAGE.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/fanglee/Notes/LearnOpenGL/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libSTB_IMAGE.a"
	$(CMAKE_COMMAND) -P CMakeFiles/STB_IMAGE.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/STB_IMAGE.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/STB_IMAGE.dir/build: libSTB_IMAGE.a
.PHONY : CMakeFiles/STB_IMAGE.dir/build

CMakeFiles/STB_IMAGE.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/STB_IMAGE.dir/cmake_clean.cmake
.PHONY : CMakeFiles/STB_IMAGE.dir/clean

CMakeFiles/STB_IMAGE.dir/depend:
	cd /Users/fanglee/Notes/LearnOpenGL/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/fanglee/Notes/LearnOpenGL /Users/fanglee/Notes/LearnOpenGL /Users/fanglee/Notes/LearnOpenGL/build /Users/fanglee/Notes/LearnOpenGL/build /Users/fanglee/Notes/LearnOpenGL/build/CMakeFiles/STB_IMAGE.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/STB_IMAGE.dir/depend

