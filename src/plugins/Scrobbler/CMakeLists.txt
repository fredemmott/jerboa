SET(
    SOURCES
    Scrobbler.cpp
    Scrobbler_FirstRunWizard.cpp
)
SET(
    HEADERS
    Scrobbler.h
    Scrobbler_FirstRunWizard.h
)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
QT4_ADD_RESOURCES(RESOURCE_SOURCES Scrobbler.qrc)
QT4_WRAP_CPP(MOC_SOURCES ${HEADERS})
ADD_LIBRARY(
    Scrobbler 
    ${SOURCES}
    ${RESOURCE_SOURCES}
    ${MOC_SOURCES}
)
