SET(
    SOURCES
    ReplayGain.cpp
)
SET(
    HEADERS
    ReplayGain.h
)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
QT4_WRAP_CPP(MOC_SOURCES ${HEADERS})
ADD_LIBRARY(
	ReplayGain
    ${SOURCES}
    ${MOC_SOURCES}
)
