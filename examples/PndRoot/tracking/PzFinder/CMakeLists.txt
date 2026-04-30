# Create a library called "tpcreco" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.


Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${CLHEP_INCLUDE_DIR} 
  ${BASE_INCLUDE_DIRECTORIES}
  ${VMC_INCLUDE_DIRS} 
)

Set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CLHEP_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
${Boost_INCLUDE_DIR}
${SIMPATH}/include
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/SttData
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/detectors/passive
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/detectors/stt
${CMAKE_SOURCE_DIR}/detectors/stt2
#${CMAKE_SOURCE_DIR}/tracking/sttmvdtracking
#${CMAKE_SOURCE_DIR}/tracking/SttCellTrackFinder
${CMAKE_SOURCE_DIR}/tracking/riemannfit
#${CMAKE_SOURCE_DIR}/tracking/PndIdealTrackFinder
#${CMAKE_SOURCE_DIR}/tracking/HelixPropagator
${CMAKE_SOURCE_DIR}/tracking/PzFinder
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
PndSttSkewStrawPzFinderData.cxx
PndSttSkewStrawPzFinderTask.cxx
PndSttSkewStrawPzFinder.cxx
PndSttSkewStrawPzFinderAnalysisTask.cxx
)

set(LINKDEF  PndPzFinderLinkDef.h)
set(LIBRARY_NAME pzfinder)

set(DEPENDENCIES Base PndData TrkBase Stt PndStt2 riemann)

PANDA_GENERATE_LIBRARY()
