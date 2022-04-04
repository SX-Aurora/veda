ADD_SUBDIRECTORY(veda)
ADD_SUBDIRECTORY(device)

## VERA ------------------------------------------------------------------------
OPTION(VEDA_WITH_VERA "Enable vera" ON)
IF(VEDA_WITH_VERA)
	ADD_SUBDIRECTORY(vera)
ENDIF()

## SMI -------------------------------------------------------------------------
OPTION(VEDA_WITH_SMI "Enable veda-smi" ON)
IF(VEDA_WITH_SMI)
	ADD_SUBDIRECTORY(smi)
ENDIF()