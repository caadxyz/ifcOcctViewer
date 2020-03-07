cmake_minimum_required(VERSION 3.6)

project(ifcOcctViewer)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE 1)
set(APP_VERSION_MAJOR 1)
set(APP_VERSION_MINOR 0)
set(APP_TARGET ifcviewer)

file(GLOB IFCVIEWER_H_FILES *.h)
file(GLOB IFCVIEWER_CPP_FILES *.cpp)
set(IFCVIEWER_FILES ${IFCVIEWER_h_FILES} ${IFCVIEWER_CPP_FILES})

# OpenGL
find_package(OpenGL REQUIRED)

# Open CASCADE Technology
# brew install opencascade
if (APPLE)
  set(OpenCASCADE_DIR "/usr/local/Cellar/opencascade/7.4.0/lib/cmake/opencascade")
elseif(UNIX)
  set(OpenCASCADE_DIR "/usr/local/lib/cmake/opencascade")
endif ()
find_package(OpenCASCADE REQUIRED NO_DEFAULT_PATH)
if (OpenCASCADE_FOUND)
  message (STATUS "Using OpenCASCADE from \"${OpenCASCADE_DIR}\"" )
  INCLUDE_DIRECTORIES(${OpenCASCADE_INCLUDE_DIR})
  LINK_DIRECTORIES(${OpenCASCADE_LIBRARY_DIR})
else()
  message (WARNING "Could not find OpenCASCADE, please set OpenCASCADE_DIR variable." )
  set (OCCT_LIBRARY_DIR)
  set (OCCT_BIN_DIR)
endif()
SET(OpenCASCADE_LIBS 
  TKService TKV3d TKOpenGl
  TKernel TKMath TKBRep TKGeomBase TKGeomAlgo TKG3d TKG2d TKShHealing TKTopAlgo TKMesh TKPrim TKBool TKBO
  TKFillet TKSTEP TKSTEPBase TKSTEPAttr TKXSBase TKSTEP209 TKIGES TKOffset
)

# glfw
find_package(glfw3 REQUIRED)
if (glfw3_FOUND)
  MESSAGE (STATUS "Using glfw3 ${glfw3_VERSION}" )
  INCLUDE_DIRECTORIES(${GLFW_INCLUDE_DIRS})
  LINK_DIRECTORIES(${GLFW_LIBRARY_DIRS})
else()
  message (STATUS "glfw3 is not found." )
endif()

# Find Boost
set(BOOST_COMPONENTS system program_options regex thread date_time)
FIND_PACKAGE(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})
MESSAGE(STATUS "Boost include files found in ${Boost_INCLUDE_DIRS}")
MESSAGE(STATUS "Boost libraries found in ${Boost_LIBRARY_DIRS}")

# Find IfcOpenShell
IF("${IFCOPENSHELL_INCLUDE_DIR}" STREQUAL "")
	SET(IFCOPENSHELL_INCLUDE_DIR "/Users/apple/opt/include")
    MESSAGE(STATUS "Looking for IfcOpenShell include files in: ${IFCOPENSHELL_INCLUDE_DIR}")
ENDIF()
FIND_FILE(ifc_geom_api_h "ifcgeom/ifc_geom_api.h" ${IFCOPENSHELL_INCLUDE_DIR})
IF(ifc_geom_api_h)
    INCLUDE_DIRECTORIES(${IFCOPENSHELL_INCLUDE_DIR})
	MESSAGE(STATUS "Header files found")
ELSE()
	MESSAGE(FATAL_ERROR "Unable to find header files, aborting")
ENDIF()

IF("${IFCOPENSHELL_LIBRARY_DIR}" STREQUAL "")
    SET(IFCOPENSHELL_LIBRARY_DIR "/Users/apple/opt/lib")
    MESSAGE(STATUS "Looking for Open CASCADE library files in: ${IFCOPENSHELL_LIBRARY_DIR}")
ENDIF()
FIND_LIBRARY(libIfcGeom NAMES IfcGeom PATHS ${IFCOPENSHELL_LIBRARY_DIR} NO_DEFAULT_PATH)
IF(libIfcGeom)
    LINK_DIRECTORIES(${IFCOPENSHELL_LIBRARY_DIR})
	MESSAGE(STATUS "Library files found")
ELSE()
	MESSAGE(FATAL_ERROR "Unable to find library files, aborting")
ENDIF()

ADD_EXECUTABLE(${APP_TARGET} ${IFCVIEWER_FILES})
TARGET_LINK_LIBRARIES(
    ${APP_TARGET}
    IfcParse
    IfcGeom
    ${OpenCASCADE_LIBS}
    glfw
    ${OPENGL_LIBRARIES}
    ${Boost_LIBRARIES}
 )


