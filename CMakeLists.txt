# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

set(LAME_DYN_PATH ""  CACHE STRING "Lame dynamic link path")
set(LAME_LIB_PATH ""  CACHE STRING "Lame dynamic library path")
set(LAME_INCLUDE_PATH ""  CACHE STRING "Lame include path")
set(MPG123_DYN_PATH "" CACHE STRING "Mpg123 dynamic link path")
set(MPG123_LIB_PATH "" CACHE STRING "Mpg123 dynamic library path")
set(MPG123_INCLUDE_PATH "" CACHE STRING "Mpg123 include path")

message("----------PATHS----------")
message("LAME DYNAMIC LIBRARY   : ${LAME_DYN_PATH}")
message("LAME LINK LIBRARY      : ${LAME_LIB_PATH}")
message("LAME INCLUDE           : ${LAME_INCLUDE_PATH}")
message("MPG123 DYNAMIC LIBRARY : ${MPG123_DYN_PATH}")
message("MPG123 LINK LIBRARY    : ${MPG123_LIB_PATH}")
message("MPG123 INCLUDE         : ${MPG123_INCLUDE_PATH}")
message("")
message("Compiler: ${CMAKE_CXX_COMPILER}")

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("WaveTransformer")

# Include sub-projects.
add_subdirectory ("WaveTransformer")

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "wavtrans")
