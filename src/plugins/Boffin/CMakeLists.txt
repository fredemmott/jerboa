SET(
    CLASSES
    Boffin
    CollectionTagFetcher
    LastFmTagFetcher
    LastFmTagFetcher_Track
    TagCloudModel
    TagCloudView
    TagDelegate
    TagsPane
    WeightedTag
)

SET(SOURCES)
SET(HEADERS)
FOREACH(class ${CLASSES})
	LIST(APPEND SOURCES ${class}.cpp)
	LIST(APPEND HEADERS ${class}.h)
ENDFOREACH()

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
QT4_ADD_RESOURCES(RESOURCE_SOURCES Boffin.qrc)
QT4_WRAP_CPP(MOC_SOURCES ${HEADERS})
ADD_LIBRARY(
	Boffin
	${SOURCES}
	${RESOURCE_SOURCES}
	${MOC_SOURCES}
)
