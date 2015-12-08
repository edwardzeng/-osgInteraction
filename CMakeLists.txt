# set project's name
PROJECT( osgInteraction )

#ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)

# Path to additional CMake modules
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules ${CMAKE_MODULE_PATH})

###############################################################################
# CMake settings
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)

ADD_DEFINITIONS(-DUNICODE)

FIND_PACKAGE(Qt4 REQUIRED)


SET(QT_USE_QTMAIN TRUE)
SET(QT_USE_QTOPENGL TRUE)
SET(QT_USE_QTXML TRUE)
INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
###############################################################################
FIND_PACKAGE(OSG REQUIRED)

IF (OSG_FOUND)
	MESSAGE(${OSG_INCLUDE_DIR})
	MESSAGE(${OSG_LIBRARIES})
ENDIF (OSG_FOUND)
#apply them to following projects
INCLUDE_DIRECTORIES(${OSG_INCLUDE_DIR})


# Setup output directories
SET(LIBRARY_OUTPUT_PATH 
	${PROJECT_BINARY_DIR}/bin
	CACHE
	PATH
	"Single directory for all libraries")
	
SET(EXECUTABLE_OUTPUT_PATH 
	${PROJECT_BINARY_DIR}/bin
	CACHE
	PATH
	"Single directory for all executables")

ADD_SUBDIRECTORY(example)



