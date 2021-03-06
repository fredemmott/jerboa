SET(
	PLUGINS
	CollectionTreeView
	FileSystemTab
	InfoWidget
	JerboaCollection
	JerboaPlaylist
	NestedCollectionModel
	PhononPlayer
	PlaylistModel
	ReplayGain
	Scrobbler
	SeekToolBar
	StandardContainer
	StandardPlaylistView
	StandardToolBar
	StartPlaybackOnTrackAppend
	TagLibTagReader
	TrayIcon
	VolumeToolBar
)
SET(
	DEFAULT_OFF_PLUGINS
	# Privacy issues:
	Boffin 
	# WIP:
	NetworkClient
	NetworkServer
)

IF(UNIX AND NOT APPLE)
	LIST(APPEND PLUGINS Mpris) # D-Bus requirement
ENDIF()
IF(APPLE AND CMAKE_SIZEOF_VOID_P MATCHES "4")
	SET(USE_CARBON)
	ADD_DEFINITIONS(-DUSE_CARBON)
	LIST(APPEND PLUGINS MacStyle)
ENDIF()

##### DON'T EDIT BELOW HERE (probably) #####

# Add options for each plugin
SET(ACTIVE_PLUGINS)
FOREACH(plugin ${PLUGINS})
	OPTION(WITH_${plugin}_PLUGIN "Build the ${plugin} plugin" ON)
	IF(WITH_${plugin}_PLUGIN)
		SET(ACTIVE_PLUGINS ${ACTIVE_PLUGINS} ${plugin})
	ENDIF()
ENDFOREACH()
FOREACH(plugin ${DEFAULT_OFF_PLUGINS})
	OPTION(WITH_${plugin}_PLUGIN "Build the ${plugin} plugin" OFF)
	IF(WITH_${plugin}_PLUGIN)
		SET(ACTIVE_PLUGINS ${ACTIVE_PLUGINS} ${plugin})
	ENDIF()
ENDFOREACH()

# Library plugins
IF(WITH_NetworkClient_PLUGIN OR WITH_NetworkServer_PLUGIN)
	LIST(APPEND ACTIVE_PLUGINS NetworkCommon)
ENDIF()

# Write the header file defining our plugins
SET(STATIC_PLUGINS_H "${CMAKE_CURRENT_BINARY_DIR}/StaticPlugins.h")
FILE(WRITE ${STATIC_PLUGINS_H} "#include <QtPlugin>\n")
FOREACH(plugin ${ACTIVE_PLUGINS})
	FILE(APPEND ${STATIC_PLUGINS_H} "Q_IMPORT_PLUGIN(Jerboa_${plugin});\n")
ENDFOREACH()
INCLUDE(AddFileDependencies)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
ADD_FILE_DEPENDENCIES(main.cpp ${STATIC_PLUGINS_H})

# Add the subdirectories
ADD_SUBDIRECTORY(core)
ADD_SUBDIRECTORY(plugins)

# Put Jerboa in the top level
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

MESSAGE(STATUS "Active plugins: ${ACTIVE_PLUGINS}")

# Sources
ADD_EXECUTABLE(
	jerboa 
	WIN32
	main.cpp
)
TARGET_LINK_LIBRARIES(
	jerboa
	${ACTIVE_PLUGINS}
	core
	${PHONON_LIBRARY}
	${QT_LIBRARIES}
	${TAGLIB_LIBRARY}
)
INSTALL(TARGETS jerboa RUNTIME DESTINATION bin)
