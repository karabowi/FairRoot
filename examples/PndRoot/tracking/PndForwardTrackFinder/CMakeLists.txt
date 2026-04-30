# Create a library called "PndForwardTrackFinder" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${CLHEP_INCLUDE_DIR} 
  ${BASE_INCLUDE_DIRECTORIES} 
  ${VMC_INCLUDE_DIRS}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/pnddata/FtsData
  ${CMAKE_SOURCE_DIR}/pnddata/SdsData
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/detectors/stt
  ${CMAKE_SOURCE_DIR}/field 
  ${CMAKE_SOURCE_DIR}/detectors/fts 
  ${CMAKE_SOURCE_DIR}/detectors/passive
  ${CMAKE_SOURCE_DIR}/genfit
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/tracking/riemannfit
  ${CMAKE_SOURCE_DIR}/tracking/SttCellTrackFinder
  ${CMAKE_SOURCE_DIR}/tracking/PndForwardTrackFinder
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
PndForwardTrackFinderTask.cxx
PndFtsCellTrackletGenerator.cxx
PndFtsLineApproximator.cxx
PndFtsExpandedTrackCand.cxx
PndFtsLineComparator.cxx
Tuple.cxx
PndLineApproximation.cxx
PndLine.cxx
PndPlane.cxx
PndTrackCombiner.cxx
PndLineCombination.cxx
PndAnalysisForwardTask.cxx
PndModuleCombiner.cxx
PndWayFollower.cxx
PndTrackCollection.cxx
)

set(LINKDEF PndForwardTrackFinderLinkDef.h)
set(LIBRARY_NAME PndForwardTrackFinder)

set(DEPENDENCIES Base PndData TrkBase Stt Fts)

PANDA_GENERATE_LIBRARY()
