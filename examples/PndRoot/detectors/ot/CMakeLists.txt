# Create a library called "libOt" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${GEANT3_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES}
  ${VMC_INCLUDE_DIRS}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/field
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/OtData
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/detectors/ot
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${SIMPATH}/lib
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndGeoOtPar.cxx
PndGeoOt.cxx
PndOtHitInfo.cxx
PndOt.cxx
PndOtContFact.cxx
PndOtMapCreator.cxx
PndOtTube.cxx
PndOtSingleStraw.cxx
PndOtSignalOverlap.cxx
PndOtHitProducerRealFast.cxx
PndGeoOtPar.cxx
)

set(LINKDEF  LinkDef.h)
set(LIBRARY_NAME Ot)

set(DEPENDENCIES Base RIO MathCore GeoBase ParBase PndData Spectrum FairTools)

PANDA_GENERATE_LIBRARY()
