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
  ${CMAKE_SOURCE_DIR}/tracking/CentralTracking
  ${CMAKE_SOURCE_DIR}/tools/
  ${CMAKE_SOURCE_DIR}/tracking/TrkSecondary
  ${CMAKE_SOURCE_DIR}/tracking/TrkStructure
  ${CMAKE_SOURCE_DIR}/tracking/TrkAlgo
  ${CMAKE_SOURCE_DIR}/tracking/TrkData
#  ${CMAKE_SOURCE_DIR}/tracking/TrkQA
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
PndTrkBoundaryParStraws.cxx
PndTrkBoundaryParStraws2.cxx
PndTrkCategorizeStt.cxx
PndTrkChi2Fits.cxx
PndTrkCleanup.cxx
PndTrkComparisonMCtruth.cxx
PndTrkCTFindTrackInXY.cxx
PndTrkCTFindTrackInXY2.cxx
PndTrkCTGeometryCalculations.cxx
PndTrkLegendreFits.cxx
PndTrkMergeSort.cxx
PndTrkSttConformalFilling.cxx
PndTrkPlotMacros.cxx
PndTrkPlotMacros2.cxx
PndTrkPrintouts.cxx
PndTrkTracking2.cxx
PndTrkSttAdjacencies.cxx
PndTrkSttClusterFinder.cxx

)

set(LINKDEF  CentralTrackingLinkDef.h)
set(LIBRARY_NAME CentralTracking)

set(DEPENDENCIES Base ParBase PndData Stt Fts TrkBase )

PANDA_GENERATE_LIBRARY()

Install(FILES
PndTrkConstants.h
PndTrkVectors.h
DESTINATION ${PANDA_INCLUDE})
