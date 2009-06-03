SET(
	PUBLIC_CLASSES
	CollectionInterface
	Container
	MimeData
	PlayerInterface
	PlaylistInterface
	Plugin
	TagReader
	TrackData
	TrackData_p
	Utilities
)
SET(
	PRIVATE_CLASSES
	PlayerInterface_p
	PlaylistInterface_p
)

IF("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	LIST(APPEND PRIVATE_CLASSES modeltest)
ENDIF()

SET(SOURCES)
SET(HEADERS)

FOREACH(class ${PUBLIC_CLASSES})
	LIST(APPEND SOURCES ${class}.cpp)
	LIST(APPEND HEADERS ../../include/${class}.h)
ENDFOREACH()

FOREACH(class ${PRIVATE_CLASSES})
	LIST(APPEND SOURCES ${class}.cpp)
	LIST(APPEND HEADERS ${class}.h)
ENDFOREACH()

#### DON'T EDIT BELOW HERE (probably) #####

# Generate moc files
QT4_WRAP_CPP( MOC_SOURCES ${HEADERS} )

# Make it so the compiler can find generated files
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

# Taglib includes
FIND_PATH( TAGLIB_INCLUDE_DIR taglib/tag.h )
INCLUDE_DIRECTORIES( ${TAGLIB_INCLUDE_DIR} ${TAGLIB_INCLUDE_DIR}/taglib )

# Build the core library from the above
ADD_LIBRARY(
	core
	${SOURCES}
	${MOC_SOURCES}
	${UI_HEADERS}
)
