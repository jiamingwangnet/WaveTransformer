# CMakeList.txt : CMake project for WaveTransformer, include source and define
# project specific logic here.
#

# Add source to this project's executable.

file(GLOB exec_src CONFIGURE_DEPENDS
        "*.cpp"
)

file(GLOB exec_head CONFIGURE_DEPENDS 
		"include/*.h"
		${LAME_INCUDE_PATH}
        ${MPG123_INCLUDE_PATH}
)

add_executable (WaveTransformer ${exec_src} ${exec_head})

target_include_directories(WaveTransformer PRIVATE "${LAME_INCLUDE_PATH}")
target_include_directories(WaveTransformer PRIVATE "${MPG123_INCLUDE_PATH}")
target_link_libraries(WaveTransformer "${LAME_LIB_PATH}")
target_link_libraries(WaveTransformer "${MPG123_LIB_PATH}")

add_custom_command(TARGET WaveTransformer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${LAME_DYN_PATH}
            $<TARGET_FILE_DIR:WaveTransformer>/libmp3lame.dll
)

add_custom_command(TARGET WaveTransformer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${MPG123_DYN_PATH}
            $<TARGET_FILE_DIR:WaveTransformer>/libmpg123-0.dll
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET WaveTransformer PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
