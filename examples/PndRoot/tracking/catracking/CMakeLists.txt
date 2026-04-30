# Create a library called "libTracking" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.
If(VCSUPPORT)

  Set(SYSTEM_INCLUDE_DIRECTORIES
    ${SYSTEM_INCLUDE_DIRECTORIES}
    ${BASE_INCLUDE_DIRECTORIES}
    ${VMC_INCLUDE_DIRS}
  )

  set(INCLUDE_DIRECTORIES
  ${VCLEGACY_INCLUDE_DIRS}
  ${ROOT_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES}
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/field
  ${CMAKE_SOURCE_DIR}/detectors/stt
  ${CMAKE_SOURCE_DIR}/detectors/stt2
  ${CMAKE_SOURCE_DIR}/pnddata/SdsData
  ${CMAKE_SOURCE_DIR}/tools/
  ${CMAKE_SOURCE_DIR}/tracking/catracking
  ${CMAKE_SOURCE_DIR}/tracking/catracking/code
  ${CMAKE_SOURCE_DIR}/tracking/catracking/code/Performance
  ${CMAKE_SOURCE_DIR}/trackrep
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/tracking/PndIdealTrackFinder
  ${CMAKE_SOURCE_DIR}/tracking/TrackingQA
  ${CMAKE_SOURCE_DIR}/analysis/rho
  ${CMAKE_SOURCE_DIR}/analysis/rho/RhoBase
  )

  Include_Directories(${INCLUDE_DIRECTORIES})
  Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

  set(LINK_DIRECTORIES
  ${ROOT_LIBRARY_DIR}
  ${FAIRROOT_LIBRARY_DIR}
  ${SIMPATH}/lib
  )

  link_directories( ${LINK_DIRECTORIES} )
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -DDO_TPCCATRACKER_EFF_PERFORMANCE -DCATRACKER_DISPLAY -DCATRACKER_TIMING")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -DDO_TPCCATRACKER_EFF_PERFORMANCE -DCATRACKER_TIMING")
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3")


  set(HEADERS
  code/Performance/PndCATrackMCPointContainer.h
  PndCATracking.h
  #PndTrackingQualityBarrelTaskNewLinks.h
  #PndTrackingQualityBarrelAnalysisNewLinks.h
  )

  set(SRCS
  PndCATracking.cxx
  code/PndCAFunctionality.cxx
  code/PndCAGBTrack.cxx
  code/PndCAParam.cxx
  code/PndCATrackParam.cxx
  code/PndCAGBHit.cxx
  code/PndCAGBTracker.cxx
  code/PndCAParameters.cxx
  code/PndCATrackParamVector.cxx
  code/Performance/PndCAGlobalPerformance.cxx
  code/Performance/PndCAPerformanceBase.cxx
  code/Performance/PndCAMCPoint.cxx
  code/Performance/PndCAPerformance.cxx
  code/Performance/PndCAMCTrack.cxx
  code/Performance/PndCAMCVertex.cxx
  code/Performance/PndCATrackMCPointContainer.cxx
  code/Performance/PndCATrackPerformanceBase.cxx
  code/Performance/PndCADisplay.cxx
  #PndTrackingQualityBarrelTaskNewLinks.cxx
  #PndTrackingQualityBarrelAnalysisNewLinks.cxx
  )

  set(LINKDEF  CATrackingLinkDef.h)
  set(LIBRARY_NAME CATracking)

  set(DEPENDENCIES PndData Stt PndStt2 VcLegacy Base IdealTrackFinder Rho TrackingQA)

  PANDA_GENERATE_LIBRARY()

Else(VCSUPPORT)
  Message(STATUS "VC is not supported or switched off: Skipping CATracking package.")
EndIf(VCSUPPORT)
