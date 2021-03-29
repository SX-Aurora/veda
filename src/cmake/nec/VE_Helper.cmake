IF(NOT _NEC_HELPER)
	SET(_NEC_HELPER TRUE)
	
	MACRO(_NEC_FIND_FOLDER INPATH OUTPATH VERSION)
		FILE(GLOB PATHS ${INPATH})
		LIST(LENGTH PATHS LEN)
		
		SET(${OUTPATH} "")
		SET(${VERSION} "0;0;0")

		IF(LEN GREATER 0)
			LIST(REVERSE PATHS)

			FOREACH(PATH ${PATHS})
				STRING(REGEX MATCH "[0-9]+.[0-9]+.[0-9]+" VER ${PATH})
				IF(NOT VER STREQUAL "")
					STRING(REPLACE "." ";" VER ${VER})
					LIST(LENGTH VER VLEN)
					MATH(EXPR VLEN "${VLEN} - 1")
					FOREACH(I RANGE ${VLEN})
						LIST(GET VER ${I} A)
						LIST(GET ${VERSION} ${I} B)

						IF(A GREATER B)
							SET(${OUTPATH} ${PATH})
							SET(${VERSION} ${VER})
							BREAK()
						ENDIF()
					ENDFOREACH()
				ENDIF()
			ENDFOREACH()
		ENDIF()

		STRING(REPLACE ";" "." ${VERSION} "${${VERSION}}")

		UNSET(PATHS)
		UNSET(VLEN)
		UNSET(LEN)
		UNSET(VER)
	ENDMACRO()
ENDIF()