# Create a library called "libMCMatch" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${BASE_INCLUDE_DIRECTORIES}
  ${VMC_INCLUDE_DIRS} 
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/pndbase 
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/SdsData
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/pnddata/GemData
  ${CMAKE_SOURCE_DIR}/pnddata/FtsData
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/pndbase/PndStdUtils
  ${CMAKE_SOURCE_DIR}/field 
  ${CMAKE_SOURCE_DIR}/trackrep
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/geane
  ${CMAKE_SOURCE_DIR}/genfit
  ${CMAKE_SOURCE_DIR}/tracking/riemannfit
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcData
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcReco
  ${CMAKE_SOURCE_DIR}/detectors/stt
  ${CMAKE_SOURCE_DIR}/tracking/PndIdealTrackFinder
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
PndIdealTrackFinder.cxx
PndCloneGenerator.cxx
PndTrackFunctor.cxx
)

set(LINKDEF  PndIdealTrackFinderLinkDef.h)
set(LIBRARY_NAME IdealTrackFinder)

set(DEPENDENCIES Base riemann Emc ParBase PndData)

PANDA_GENERATE_LIBRARY()
