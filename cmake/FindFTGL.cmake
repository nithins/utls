# search for the ftgl library/ header paths
IF (WIN32)
	FIND_PATH( FTGL_INCLUDE_PATH FTGL/ftgl.h
		$ENV{PROGRAMFILES}/FTGL/include		
		DOC "The directory where FTGL/ftgl.h resides")
	FIND_LIBRARY( FTGL_LIBRARY
		NAMES ftgl FTGL ftgl32 ftgl32s
		PATHS
		$ENV{PROGRAMFILES}/FTGL/lib
		DOC "The FTGL library")
ELSE (WIN32)
	FIND_PATH( FTGL_INCLUDE_PATH FTGL/ftgl.h
		/usr/include
		/usr/local/include
		/sw/include
		/opt/local/include
		DOC "The directory where FTGL/ftgl.h resides")
	FIND_LIBRARY( FTGL_LIBRARY
		NAMES FTGL ftgl
		PATHS
		/usr/lib64
		/usr/lib
		/usr/local/lib64
		/usr/local/lib
		/sw/lib
		/opt/local/lib
		DOC "The FTGL library")
ENDIF (WIN32)

IF (FTGL_INCLUDE_PATH)
	SET( FTGL_FOUND 1 CACHE STRING "Set to 1 if FTGL is found, 0 otherwise")
ELSE (FTGL_INCLUDE_PATH)
	SET( FTGL_FOUND 0 CACHE STRING "Set to 1 if FTGL is found, 0 otherwise")
ENDIF (FTGL_INCLUDE_PATH)

MARK_AS_ADVANCED( FTGL_FOUND )
