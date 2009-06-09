##############################################
# You probably don't want to edit below here #
##############################################
# The plugins list is in ../CMakeLists.txt

FOREACH(Plugin ${ACTIVE_PLUGINS})
	ADD_SUBDIRECTORY(${Plugin})
ENDFOREACH()
