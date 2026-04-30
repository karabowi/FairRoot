# Create a library called "libTracking" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.


Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${BASE_INCLUDE_DIRECTORIES}
  ${VMC_INCLUDE_DIRS} 
)
Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/pnddata 
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/pnddata/FtsData
  ${CMAKE_SOURCE_DIR}/pnddata/SciTData
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/field 
  ${CMAKE_SOURCE_DIR}/detectors/stt
  ${CMAKE_SOURCE_DIR}/detectors/fts
  ${CMAKE_SOURCE_DIR}/tracking/sttmvdtracking
  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdTracking
  ${CMAKE_SOURCE_DIR}/pnddata/SdsData
  ${CMAKE_SOURCE_DIR}/detectors/gem 
  ${CMAKE_SOURCE_DIR}/pnddata/GemData 
  ${CMAKE_SOURCE_DIR}/geane 
  ${CMAKE_SOURCE_DIR}/analysis/AnalysisTools
  ${CMAKE_SOURCE_DIR}/analysis/rho
  ${CMAKE_SOURCE_DIR}/analysis/rho/RhoBase
  ${CMAKE_SOURCE_DIR}/analysis/rho/RhoMath
  ${CMAKE_SOURCE_DIR}/analysis/rho/RhoFitter
  ${CMAKE_SOURCE_DIR}/tracking/SecondariesTracking
  ${CMAKE_SOURCE_DIR}/tools/
  ${CMAKE_SOURCE_DIR}/tracking/SecondariesTracking/TrkSecondary
  ${CMAKE_SOURCE_DIR}/tracking/SecondariesTracking/TrkStructure
  ${CMAKE_SOURCE_DIR}/tracking/SecondariesTracking/TrkAlgo
  ${CMAKE_SOURCE_DIR}/tracking/SecondariesTracking/TrkData
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
TrkAlgo/PndTrkLegendreTransform.cxx
TrkAlgo/PndTrkConformalTransform.cxx
TrkAlgo/PndTrkTools.cxx
TrkAlgo/PndTrkClean.cxx
TrkAlgo/PndTrkFitter.cxx
TrkAlgo/PndTrkGemCombinatorial.cxx
#
TrkData/PndTrkHit.cxx
TrkData/PndTrkIndivisibleHit.cxx
TrkData/PndTrkSkewHit.cxx
TrkData/PndTrkHitList.cxx
TrkData/PndTrkSdsHitList.cxx
TrkData/PndTrkSttHitList.cxx
TrkData/PndTrkGemHitList.cxx
TrkData/PndTrkSciTHitList.cxx
TrkData/PndTrkCluster.cxx
TrkData/PndTrkLegendreCluster.cxx 
TrkData/PndTrkClusterList.cxx
TrkData/PndTrkTrackList.cxx      
TrkData/PndTrkTrack.cxx
TrkData/PndTrkConformalHit.cxx
TrkData/PndTrkConformalHitList.cxx
TrkData/PndTrkSkewHitList.cxx
TrkData/PndTrkNeighboringMap.cxx
#
TrkSecondary/PndTrkTrackFinder.cxx
TrkSecondary/PndTrkAddTCA.cxx
TrkSecondary/PndTrkGemCombiTask.cxx

)

set(LINKDEF  SecondariesTrackingLinkDef.h)
set(LIBRARY_NAME SecondariesTracking)

set(DEPENDENCIES Base ParBase PndData Stt Fts TrkBase )

PANDA_GENERATE_LIBRARY()

Install(FILES
TrkStructure/PndTrkParameters.h
DESTINATION ${PANDA_INCLUDE})
