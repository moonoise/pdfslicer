set (SOURCES
	main.cpp
	command.addfiles.cpp
	command.move.cpp
	command.remove.cpp
	document.addfile.cpp
	document.addfiles.cpp
	document.move.cpp
	document.remove.cpp
	tempfile.cpp)

add_executable (pdfslicer_tests ${SOURCES})
target_link_libraries_system (pdfslicer_tests Catch2)

if (MINGW)
	target_link_libraries (pdfslicer_tests backend)
else ()
	target_link_libraries_system (pdfslicer_tests backend)
endif ()

target_compile_options(pdfslicer_tests PUBLIC $<$<CONFIG:DEBUG>:${SLICER_DEBUG_FLAGS}>)

file (GLOB SLICER_TEST_MATERIALS "materials/*.pdf")
file (COPY ${SLICER_TEST_MATERIALS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
