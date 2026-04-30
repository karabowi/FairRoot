# Create a library called "TrackingQA" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${BASE_INCLUDE_DIRECTORIES}
  ${VMC_INCLUDE_DIRS}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/detectors/stt
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/trackrep
  ${CMAKE_SOURCE_DIR}/trackbase
#  ${CMAKE_SOURCE_DIR}/PndMCMatch
  ${CMAKE_SOURCE_DIR}/tracking/PndPropagator
  ${CMAKE_SOURCE_DIR}/tracking/PndHelixPropagator
  ${CMAKE_SOURCE_DIR}/tracking/PndIdealTrackFinder
  ${CMAKE_SOURCE_DIR}/tracking/TrackingQA
  ${CMAKE_SOURCE_DIR}/analysis/rho
  ${CMAKE_SOURCE_DIR}/analysis/rho/RhoBase
  ${CMAKE_SOURCE_DIR}/analysis/rho/RhoHistogram
  ${CMAKE_SOURCE_DIR}/tools
  
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
PndTrackingQA.cxx
PndTrackingQATask.cxx
PndTrackingQualityMCInfo.cxx
PndTrackingQualityRecoInfo.cxx
PndTrackingCloneInfo.cxx
PndTrackingEventInfo.cxx
PndTrackingQAMCInfo.cxx
PndTrackingQARecoInfo.cxx
PndTrackingQAQualityNumbers.cxx
PndTrackingQASummary.cxx
PndTrackingQAAna.cxx
PndTrackingQABranchEnum.cxx
)

set(LINKDEF  TrackingQualityAnalysis_LinkDef.h)
set(LIBRARY_NAME TrackingQA)

set(DEPENDENCIES Base PndData IdealTrackFinder Rho helixpropagator)

PANDA_GENERATE_LIBRARY()
