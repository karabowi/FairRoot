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
  ${CMAKE_SOURCE_DIR}/pnddata/SdsData
  ${CMAKE_SOURCE_DIR}/pnddata/FtsData
  ${CMAKE_SOURCE_DIR}/detectors/fts
  ${CMAKE_SOURCE_DIR}/tools/
  ${CMAKE_SOURCE_DIR}/tracking/ftscatracking
  ${CMAKE_SOURCE_DIR}/tracking/ftscatracking/code
  ${CMAKE_SOURCE_DIR}/tracking/ftscatracking/code/Performance
  ${CMAKE_SOURCE_DIR}/tracking/ftscatracking/code/KFParticle
  #${CMAKE_SOURCE_DIR}/detectors/ftscatracking/code/Common
  )

  Include_Directories(${INCLUDE_DIRECTORIES})
  Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

  set(LINK_DIRECTORIES
  ${ROOT_LIBRARY_DIR}
  ${FAIRROOT_LIBRARY_DIR}
  ${SIMPATH}/lib
  )

  link_directories( ${LINK_DIRECTORIES} )

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3")


  set(HEADERS
  code/Performance/PndCATrackFtsMCPointContainer.h
  PndFtsCATracking.h
  )

  set(SRCS
  PndFtsCATracking.cxx

  code/PndFTSCAGBTracker.cxx
  ##code/PndFTSCAGBTrack.cxx #
  #code/PndFTSCADisplay.cxx
  ##code/PndFTSCAClusterData.cxx #
  ##code/PndFTSCAParameters.cxx #
  code/PndFTSCAGBHit.cxx
  code/PndFTSCAParam.cxx
  code/PndFTSCATrackParam.cxx
  code/PndFTSCATrackParamVector.cxx
  code/CAFunctionality.cxx

  #code/Performance/PndFTSCAPerformance.cxx
  #code/Performance/PndFTSPerformanceBase.cxx
  #code/Performance/PndFTSCATrackPerformanceBase.cxx
  #code/Performance/PndFTSCAGlobalPerformance.cxx
  #code/Performance/PndFTSCAMCTrack.cxx
  #code/Performance/PndFTSCAMCPoint.cxx
  #code/Performance/PndCATrackFtsMCPointContainer.cxx
  )

  #add_definitions(-DENABLE_VECTORIZATION)
  #If(APPLE)
  #  ADD_DEFINITIONS( -Wall -Wabi -Wsign-promo -Wno-pmf-conversions -Wctor-dtor-privacy -Wreorder -Wno-deprecated -Wstrict-null-sentinel   -Wno-parentheses -DSTAR_HFT -DDRAW -DALICE_ITS -DUSE_MC_PV -DDRIFT_TUBES
  #  -DPANDA_FTS -DDRIFT_TUBES -DKFPARTICLE -DKFPARTICLEFINDER -DCONVERTER -DDO_TPCCATRACKER_EFF_PERFORMANCE) #-DPANDA_STT
  #Else()
  #  ADD_DEFINITIONS( -Wall -Wabi -fabi-version=0 -Wsign-promo -Wno-pmf-conversions -Wctor-dtor-privacy -Wreorder -Wno-deprecated   -Wstrict-null-sentinel -Wno-parentheses -DSTAR_HFT -DDRAW -DALICE_ITS -DUSE_MC_PV -DDRIFT_TUBES
  #  -DPANDA_FTS -DDRIFT_TUBES -DKFPARTICLE -DKFPARTICLEFINDER -DCONVERTER -DDO_TPCCATRACKER_EFF_PERFORMANCE) #-DPANDA_STT
  #EndIf()

  If(APPLE)
    ADD_DEFINITIONS( -Wall -Wabi -Wsign-promo -Wno-pmf-conversions -Wctor-dtor-privacy -Wreorder -Wno-deprecated -Wstrict-null-sentinel   -Wno-parentheses -DDRIFT_TUBES -DPANDA_FTS)
    #  -DUSE_MC_PV -DKFPARTICLE -DKFPARTICLEFINDER -DDO_TPCCATRACKER_EFF_PERFORMANCE -DDRAW -DUSE_IDEAL_TF
  Else()
    ADD_DEFINITIONS( -Wall -Wabi -fabi-version=0 -Wsign-promo -Wno-pmf-conversions -Wctor-dtor-privacy -Wreorder -Wno-deprecated   -Wstrict-null-sentinel -Wno-parentheses -DDRIFT_TUBES -DPANDA_FTS)
    #  -DUSE_MC_PV -DKFPARTICLE -DKFPARTICLEFINDER -DDO_TPCCATRACKER_EFF_PERFORMANCE -DDRAW -DUSE_IDEAL_TF
  EndIf()


  set(LINKDEF  FtsCATrackingLinkDef.h)
  set(LIBRARY_NAME CAFtsTracking)

  set(DEPENDENCIES PndData VcLegacy)

  PANDA_GENERATE_LIBRARY()

Else(VCSUPPORT)
  Message(STATUS "VC is not supported or switched off: Skipping FTS CATracking package.")
EndIf(VCSUPPORT)
