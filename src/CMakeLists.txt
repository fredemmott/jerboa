# Add the subdirectories
ADD_SUBDIRECTORY(core)

# Put Jerboa in the top level
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Sources
ADD_EXECUTABLE(
	jerboa 
	WIN32
	core/main.cpp
)
TARGET_LINK_LIBRARIES(
	jerboa
	core
	${PHONON_LIBRARY}
	${QT_LIBRARIES}
	${TAGLIB_LIBRARY}
)
