# CMakeList.txt : CMake project for WaveTransformer, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (WaveTransformer "main.cpp"  "include/WaveFile.h" "include/InputParser.h" "include/Options.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET WaveTransformer PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
