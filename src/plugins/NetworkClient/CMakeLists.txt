SET(
    CLASSES
    NetworkClient
    NetworkClient_ServerDiscoverer
)

SET(SOURCES)
SET(HEADERS)
FOREACH(class ${CLASSES})
	LIST(APPEND SOURCES ${class}.cpp)
	LIST(APPEND HEADERS ${class}.h)
ENDFOREACH()

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
QT4_WRAP_CPP(MOC_SOURCES ${HEADERS})
ADD_LIBRARY(
	NetworkClient
	${SOURCES}
	${MOC_SOURCES}
)
