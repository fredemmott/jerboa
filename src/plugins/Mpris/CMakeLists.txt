SET(
	SOURCES
	Mpris.cpp
	MprisIdentification.cpp
	MprisPlayer.cpp
	MprisTracklist.cpp
)
SET(
	HEADERS
	Mpris.h
	MprisIdentification.h
	MprisPlayer.h
	MprisTracklist.h
)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

QT4_ADD_DBUS_ADAPTOR(
	ADAPTOR_SOURCES
	org.freedesktop.MediaPlayer.root.xml
	MprisIdentification.h
	MprisIdentification
	MprisIdentificationAdaptor
	MprisIdentificationAdaptor
)
QT4_ADD_DBUS_ADAPTOR(
	ADAPTOR_SOURCES
	org.freedesktop.MediaPlayer.player.xml
	MprisPlayer.h
	MprisPlayer
	MprisPlayerAdaptor
	MprisPlayerAdaptor
)
QT4_ADD_DBUS_ADAPTOR(
	ADAPTOR_SOURCES
	org.freedesktop.MediaPlayer.tracklist.xml
	MprisTracklist.h
	MprisTracklist
	MprisTracklistAdaptor
	MprisTracklistAdaptor
)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
QT4_WRAP_CPP(MOC_SOURCES ${HEADERS})
ADD_LIBRARY(
	Mpris
	${ADAPTOR_SOURCES}
	${SOURCES}
	${MOC_SOURCES}
)
