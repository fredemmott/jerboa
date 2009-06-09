##############################################
# You probably don't want to edit below here #
##############################################
# The plugins list is in ../CMakeLists.txt

FOREACH(Plugin ${PLUGINS})
	IF(WITH_${Plugin}_PLUGIN)
		ADD_SUBDIRECTORY(${Plugin})
	ENDIF(WITH_${Plugin}_PLUGIN)
ENDFOREACH()
