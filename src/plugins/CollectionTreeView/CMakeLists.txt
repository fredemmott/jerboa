SET(
	CLASSES
	CollectionFilter
	CollectionTreeView
	CollectionWidget
	SearchLineEdit
)

if(USE_CARBON)
	list(
		APPEND CLASSES
		QtCFString
		SearchLineEdit_mac
		SearchLineEdit_mac_p
	)
endif()

SET(SOURCES)
SET(HEADERS)
FOREACH(class ${CLASSES})
	LIST(APPEND SOURCES ${class}.cpp)
	LIST(APPEND HEADERS ${class}.h)
ENDFOREACH()

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
QT4_WRAP_CPP(MOC_SOURCES ${HEADERS})
ADD_LIBRARY(
	CollectionTreeView
	${SOURCES}
	${MOC_SOURCES}
)
