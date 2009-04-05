SET(TAGLIB_MINIMUM_VERSION 1.5)
############################################################
# You probably don't want to edit anything below this line #
############################################################
# See the CMakeLists.txt in src/, src/core, src/plugins

PROJECT(Jerboa)
# Stop cmake 2.6 from whining
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
IF(COMMAND cmake_policy)
	CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

ADD_DEFINITIONS(-DQT_STATICPLUGIN)

# Docs
#CONFIGURE_FILE(Doxyfile.cmake Doxyfile)
#ADD_CUSTOM_TARGET(doc doxygen)
#ADD_CUSTOM_TARGET(pdfdoc gmake DEPENDS doc WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/doc/latex)

# Find Qt4
FIND_PACKAGE( Qt4 REQUIRED )

SET( QT_USE_QTNETWORK TRUE )
SET( QT_USE_QTOPENGL TRUE ) # dependency of phonon
SET( QT_USE_QTSQL TRUE )
SET( QT_USE_QTMAIN TRUE )
SET( QT_USE_QTDBUS TRUE )
SET( QT_USE_QTSVG TRUE )

# Include the cmake file needed to use qt4
INCLUDE( ${QT_USE_FILE} )

# Phonon
FIND_LIBRARY( PHONON_LIBRARY phonon REQUIRED PATHS "${QT_LIBRARY_DIR}" )
FIND_PATH( PHONON_INCLUDE_DIR phonon/mediaobject.h PATHS "${QT_INCLUDE_DIR}" )
INCLUDE_DIRECTORIES( ${PHONON_INCLUDE_DIR} )

# Make it so the compiler can find generated files
INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR}/include/ )

# Link against taglib
OPTION(HAVE_TAGLIB_ASF "Mark as ON if your TagLib has ASF support" OFF)
FIND_LIBRARY( TAGLIB_LIBRARY tag REQUIRED )

# Taglib
FIND_FILE( TAGLIB_CONFIG NAMES taglib-config taglib-config.exe DOC "Path to taglib-config binary" )
EXECUTE_PROCESS(COMMAND ${TAGLIB_CONFIG} --version OUTPUT_VARIABLE TAGLIB_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE )
IF(${TAGLIB_VERSION} EQUAL ${TAGLIB_MINIMUM_VERSION} OR ${TAGLIB_VERSION} GREATER ${TAGLIB_MINIMUM_VERSION})
	FIND_PATH( TAGLIB_INCLUDE_DIR taglib/tag.h )
	INCLUDE_DIRECTORIES( ${TAGLIB_INCLUDE_DIR}/taglib/.. ${TAGLIB_INCLUDE_DIR}/taglib )
	MESSAGE(STATUS "Found taglib version ${TAGLIB_VERSION}")
ELSE(${TAGLIB_VERSION} EQUAL ${TAGLIB_MINIMUM_VERSION} OR ${TAGLIB_VERSION} GREATER ${TAGLIB_MINIMUM_VERSION})
	MESSAGE(FATAL_ERROR "Found taglib version ${TAGLIB_VERSION}, but at least version ${TAGLIB_MINIMUM_VERSION} is required.")
ENDIF(${TAGLIB_VERSION} EQUAL ${TAGLIB_MINIMUM_VERSION} OR ${TAGLIB_VERSION} GREATER ${TAGLIB_MINIMUM_VERSION})

# Add include directories
INCLUDE_DIRECTORIES("include")

# If building for release, don't include QDebug stuff
IF("x${CMAKE_BUILD_TYPE}" STREQUAL "x")
	SET(CMAKE_BUILD_TYPE "Release")
ENDIF()
IF("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
	ADD_DEFINITIONS("-DQT_NO_DEBUG_OUTPUT -DQT_NODEBUG -DQT_NO_DEBUG")
ENDIF("${CMAKE_BUILD_TYPE}" STREQUAL "Release")

#IF(APPLE)
#	INCLUDE(OSXPackaging)
#ENDIF(APPLE)

#INSTALL(
#	FILES
#	images/jerboa.svg
#	DESTINATION
#	${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps
#)
#INSTALL(
#	FILES
#	Jerboa.desktop
#	DESTINATION
#	${CMAKE_INSTALL_PREFIX}/share/applications
#)

ADD_SUBDIRECTORY(include)
ADD_SUBDIRECTORY(src)
