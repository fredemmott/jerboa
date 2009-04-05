SET(FIND_AS_YOU_TYPE_INTERVAL 300 CACHE STRING
	"Minimum time after a keypress for find-as-you-type to kick in. Should be increased for devices without a full keyboard."
)

OPTION(
	EMBEDDED_LEFT_BUTTON_CONTEXT_MENU
	"Activate context menus on left click"
	OFF
)

OPTION(
	EMBEDDED_USE_FIXED_PATHS
	"Use hardcoded paths"
	OFF
)
SET(EMBEDDED_FIXED_DATA_PATH "" CACHE PATH
	"Location to store database, cover art, and lyrics"
)
SET(EMBEDDED_FIXED_MUSIC_PATH "" CACHE PATH 
	"Default location to look for music"
)

CONFIGURE_FILE(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)
