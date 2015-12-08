# Locate gdal
# This module defines
# OSG_LIBRARY
# OSG_FOUND, if false, do not try to link to gdal 
# OSG_INCLUDE_DIR, where to find the headers
#
# $OSG_DIR is an environment variable that would
# correspond to the ./configure --prefix=$OSG_DIR
#
# Created by Robert Osfield. 

FIND_PATH(OSG_INCLUDE_DIR osg/Node
    ${OSG_DIR}/include
    $ENV{OSG_DIR}/include
    $ENV{OSG_DIR}
    $ENV{OSGDIR}/include
    $ENV{OSGDIR}
    $ENV{OSG_ROOT}/include
    NO_DEFAULT_PATH
)

FIND_PATH(OSG_INCLUDE_DIR osg/Node)

MACRO(FIND_OSG_LIBRARY MYLIBRARY MYLIBRARYNAME)

    FIND_LIBRARY("${MYLIBRARY}_DEBUG"
        NAMES "${MYLIBRARYNAME}${CMAKE_DEBUG_POSTFIX}"
        PATHS
        ${OSG_DIR}/lib/Debug
        ${OSG_DIR}/lib64/Debug
        ${OSG_DIR}/lib
        ${OSG_DIR}/lib64
        $ENV{OSG_DIR}/lib/debug
        $ENV{OSG_DIR}/lib64/debug
        $ENV{OSG_DIR}/lib
        $ENV{OSG_DIR}/lib64
        $ENV{OSG_DIR}
        $ENV{OSGDIR}/lib
        $ENV{OSGDIR}/lib64
        $ENV{OSGDIR}
        $ENV{OSG_ROOT}/lib
        $ENV{OSG_ROOT}/lib64
        NO_DEFAULT_PATH
    )

    FIND_LIBRARY("${MYLIBRARY}_DEBUG"
        NAMES "${MYLIBRARYNAME}${CMAKE_DEBUG_POSTFIX}"
        PATHS
        ~/Library/Frameworks
        /Library/Frameworks
        /usr/local/lib
        /usr/local/lib64
        /usr/lib
        /usr/lib64
        /sw/lib
        /opt/local/lib
        /opt/csw/lib
        /opt/lib
        [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSG_ROOT]/lib
        /usr/freeware/lib64
    )
    
    FIND_LIBRARY(${MYLIBRARY}
        NAMES "${MYLIBRARYNAME}${CMAKE_RELEASE_POSTFIX}"
        PATHS
        ${OSG_DIR}/lib/Release
        ${OSG_DIR}/lib64/Release
        ${OSG_DIR}/lib
        ${OSG_DIR}/lib64
        $ENV{OSG_DIR}/lib/Release
        $ENV{OSG_DIR}/lib64/Release
        $ENV{OSG_DIR}/lib
        $ENV{OSG_DIR}/lib64
        $ENV{OSG_DIR}
        $ENV{OSGDIR}/lib
        $ENV{OSGDIR}/lib64
        $ENV{OSGDIR}
        $ENV{OSG_ROOT}/lib
        $ENV{OSG_ROOT}/lib64
        NO_DEFAULT_PATH
    )

    FIND_LIBRARY(${MYLIBRARY}
        NAMES "${MYLIBRARYNAME}${CMAKE_RELEASE_POSTFIX}"
        PATHS
        ~/Library/Frameworks
        /Library/Frameworks
        /usr/local/lib
        /usr/local/lib64
        /usr/lib
        /usr/lib64
        /sw/lib
        /opt/local/lib
        /opt/csw/lib
        /opt/lib
        [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSG_ROOT]/lib
        /usr/freeware/lib64
    )
    
    IF( NOT ${MYLIBRARY}_DEBUG)
        IF(MYLIBRARY)
            SET(${MYLIBRARY}_DEBUG ${MYLIBRARY})
         ENDIF(MYLIBRARY)
    ENDIF( NOT ${MYLIBRARY}_DEBUG)
           
ENDMACRO(FIND_OSG_LIBRARY LIBRARY LIBRARYNAME)

FIND_OSG_LIBRARY(OSG_LIBRARY osg)
FIND_OSG_LIBRARY(OSGGA_LIBRARY osgGA)
FIND_OSG_LIBRARY(OSGUTIL_LIBRARY osgUtil)
FIND_OSG_LIBRARY(OSGDB_LIBRARY osgDB)
FIND_OSG_LIBRARY(OSGTEXT_LIBRARY osgText)
FIND_OSG_LIBRARY(OSGWIDGET_LIBRARY osgWidget)
FIND_OSG_LIBRARY(OSGTERRAIN_LIBRARY osgTerrain)
FIND_OSG_LIBRARY(OSGFX_LIBRARY osgFX)
FIND_OSG_LIBRARY(OSGVIEWER_LIBRARY osgViewer)
FIND_OSG_LIBRARY(OSGVOLUME_LIBRARY osgVolume)
FIND_OSG_LIBRARY(OPENTHREADS_LIBRARY OpenThreads)
FIND_OSG_LIBRARY(OSGSHADOW_LIBRARY osgShadow)
FIND_OSG_LIBRARY(OSGQT_LIBRARY osgQt)

FIND_OSG_LIBRARY(OSG_DEBUG_LIBRARY osgd)
FIND_OSG_LIBRARY(OSGGA_DEBUG_LIBRARY osgGAd)
FIND_OSG_LIBRARY(OSGUTIL_DEBUG_LIBRARY osgUtild)
FIND_OSG_LIBRARY(OSGDB_DEBUG_LIBRARY osgDBd)
FIND_OSG_LIBRARY(OSGTEXT_DEBUG_LIBRARY osgTextd)
FIND_OSG_LIBRARY(OSGWIDGET_DEBUG_LIBRARY osgWidgetd)
FIND_OSG_LIBRARY(OSGTERRAIN_DEBUG_LIBRARY osgTerraind)
FIND_OSG_LIBRARY(OSGFX_DEBUG_LIBRARY osgFXd)
FIND_OSG_LIBRARY(OSGVIEWER_DEBUG_LIBRARY osgViewerd)
FIND_OSG_LIBRARY(OSGVOLUME_DEBUG_LIBRARY osgVolumed)
FIND_OSG_LIBRARY(OPENTHREADS_DEBUG_LIBRARY OpenThreadsd)
FIND_OSG_LIBRARY(OSGSHADOW_DEBUG_LIBRARY osgShadowd)
FIND_OSG_LIBRARY(OSGQT_DEBUG_LIBRARY osgQtd)

SET(OSG_LIBRARIES 
	debug ${OSG_DEBUG_LIBRARY}             optimized ${OSG_LIBRARY}
	debug ${OSGGA_DEBUG_LIBRARY}           optimized ${OSGGA_LIBRARY}
	debug ${OSGUTIL_DEBUG_LIBRARY}         optimized ${OSGUTIL_LIBRARY}
	debug ${OSGDB_DEBUG_LIBRARY}           optimized ${OSGDB_LIBRARY}
	debug ${OSGTEXT_DEBUG_LIBRARY}         optimized ${OSGTEXT_LIBRARY}
	debug ${OSGWIDGET_DEBUG_LIBRARY}       optimized ${OSGWIDGET_LIBRARY}
	debug ${OSGTERRAIN_DEBUG_LIBRARY}      optimized ${OSGTERRAIN_LIBRARY}
	debug ${OSGFX_DEBUG_LIBRARY}           optimized ${OSGFX_LIBRARY}
	debug ${OSGVIEWER_DEBUG_LIBRARY}       optimized ${OSGVIEWER_LIBRARY}
	debug ${OSGVOLUME_DEBUG_LIBRARY}       optimized ${OSGVOLUME_LIBRARY}
    debug ${OPENTHREADS_DEBUG_LIBRARY}     optimized ${OPENTHREADS_LIBRARY}
	debug ${OSGSHADOW_DEBUG_LIBRARY}       optimized ${OSGSHADOW_LIBRARY}
    debug ${OSGQT_DEBUG_LIBRARY}           optimized ${OSGQT_LIBRARY}
)

SET(OSG_FOUND "NO")
IF(OSG_LIBRARY AND OSG_INCLUDE_DIR)
    SET(OSG_FOUND "YES")
ENDIF(OSG_LIBRARY AND OSG_INCLUDE_DIR)
