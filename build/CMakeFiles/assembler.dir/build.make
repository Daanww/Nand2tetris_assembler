# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/daan/Documents/vscodeprojects/assembler

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/daan/Documents/vscodeprojects/assembler/build

# Include any dependencies generated for this target.
include CMakeFiles/assembler.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/assembler.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/assembler.dir/flags.make

CMakeFiles/assembler.dir/main.c.o: CMakeFiles/assembler.dir/flags.make
CMakeFiles/assembler.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/daan/Documents/vscodeprojects/assembler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/assembler.dir/main.c.o"
	/bin/x86_64-linux-gnu-gcc-9 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/assembler.dir/main.c.o   -c /home/daan/Documents/vscodeprojects/assembler/main.c

CMakeFiles/assembler.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/assembler.dir/main.c.i"
	/bin/x86_64-linux-gnu-gcc-9 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/daan/Documents/vscodeprojects/assembler/main.c > CMakeFiles/assembler.dir/main.c.i

CMakeFiles/assembler.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/assembler.dir/main.c.s"
	/bin/x86_64-linux-gnu-gcc-9 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/daan/Documents/vscodeprojects/assembler/main.c -o CMakeFiles/assembler.dir/main.c.s

# Object files for target assembler
assembler_OBJECTS = \
"CMakeFiles/assembler.dir/main.c.o"

# External object files for target assembler
assembler_EXTERNAL_OBJECTS =

assembler: CMakeFiles/assembler.dir/main.c.o
assembler: CMakeFiles/assembler.dir/build.make
assembler: CMakeFiles/assembler.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/daan/Documents/vscodeprojects/assembler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable assembler"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/assembler.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/assembler.dir/build: assembler

.PHONY : CMakeFiles/assembler.dir/build

CMakeFiles/assembler.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/assembler.dir/cmake_clean.cmake
.PHONY : CMakeFiles/assembler.dir/clean

CMakeFiles/assembler.dir/depend:
	cd /home/daan/Documents/vscodeprojects/assembler/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/daan/Documents/vscodeprojects/assembler /home/daan/Documents/vscodeprojects/assembler /home/daan/Documents/vscodeprojects/assembler/build /home/daan/Documents/vscodeprojects/assembler/build /home/daan/Documents/vscodeprojects/assembler/build/CMakeFiles/assembler.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/assembler.dir/depend

