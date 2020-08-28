IF(NOT _NEC_HELPER)
	SET(_NEC_HELPER TRUE)
	
	MACRO(_NEC_FIND_FOLDER INPATH OUTPATH VERSION)
		FILE(GLOB PATHS ${INPATH})
		LIST(SORT PATHS)
		LIST(LENGTH PATHS LEN)

		SET(${OUTPATH} "")
		SET(${VERSION} "")

		IF(LEN GREATER 0)
			LIST(REVERSE PATHS)

			FOREACH(PATH ${PATHS})
				STRING(REGEX MATCH "[0-9]+.[0-9]+.[0-9]+" VER ${PATH})
				IF(NOT VER STREQUAL "")
					SET(${OUTPATH} ${PATH})
					SET(${VERSION} ${VER})
					BREAK()
				ENDIF()
			ENDFOREACH()
		ENDIF()

		UNSET(PATHS)
		UNSET(LEN)
	ENDMACRO()
ENDIF()