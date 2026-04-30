# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${GEANT3_INCLUDE_DIR}
  ${CLHEP_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES}
  ${Boost_INCLUDE_DIR}
  ${VMC_INCLUDE_DIRS}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/field
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/SdsData
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/detectors/passive
  ${CMAKE_SOURCE_DIR}/tools/
  ${CMAKE_SOURCE_DIR}/tools/buffers
  ${CMAKE_SOURCE_DIR}/tracking/riemannfit
  ${CMAKE_SOURCE_DIR}/tools
  ${CMAKE_SOURCE_DIR}/detectors/sds/SdsMC
  ${CMAKE_SOURCE_DIR}/detectors/sds/SdsDigi
  ${CMAKE_SOURCE_DIR}/detectors/sds/SdsReco
  ${CMAKE_SOURCE_DIR}/detectors/sds/SdsTools
  ${CMAKE_SOURCE_DIR}/detectors/sds/SdsFEModel
  ${CMAKE_SOURCE_DIR}/detectors/sds
  ${CMAKE_SOURCE_DIR}/detectors/lmd/LmdMC
  ${CMAKE_SOURCE_DIR}/detectors/lmd/LmdDigi
  ${CMAKE_SOURCE_DIR}/detectors/lmd/LmdSensorAlignment

  # ${CMAKE_SOURCE_DIR}/detectors/lmd/LmdSensorAlignment/SensorAligner
  ${CMAKE_SOURCE_DIR}/detectors/lmd/LmdReco
  ${CMAKE_SOURCE_DIR}/detectors/lmd/LmdTrack
  ${CMAKE_SOURCE_DIR}/detectors/lmd/LmdTools
  ${CMAKE_SOURCE_DIR}/detectors/lmd/LmdQA
  ${CMAKE_SOURCE_DIR}/detectors/lmd

  # ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdDigi # to do: get rid of that dependency!
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/geane
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
  ${ROOT_LIBRARY_DIR}
  ${FAIRROOT_LIBRARY_DIR}
  ${SIMPATH}/lib
  ${Boost_LIBRARY_DIRS}
)

link_directories(${LINK_DIRECTORIES})

# ########################## create libLmd ######################################
set(SRCS

  LmdTools/PndLmdContFact.cxx
  LmdTools/PndLmdAlignPar.cxx
  LmdDigi/PndLmdPixelHitProducerFast.cxx
  LmdDigi/PndLmdHybridHitProducer.cxx
  LmdDigi/PndLmdStripHitProducer.cxx
  LmdDigi/PndLmdHitProducerIdeal.cxx
  LmdDigi/PndLmdDigiTask.cxx

  # LmdDigi/PndLmdNoiseProducer.cxx
  LmdMC/PndLmdDetector.cxx
  LmdMC/PndLmdGeo.cxx
  LmdMC/PndLmdGeometryFactory.cxx
  LmdMC/PndLmdGeometryHelper.cxx
)

set(LINKDEF LmdLinkDef.h)
set(LIBRARY_NAME Lmd)

set(DEPENDENCIES Base ParBase GeoBase TrkBase Sds PndData) # to do: Get rid of the Mvd dependency which is only due to the noise producer. It is not derived from SDS!

PANDA_GENERATE_LIBRARY()

# ########################## create libLmdReco ##################################
set(SRCS

  LmdReco/PndLmdStripClusterTask.cxx
  LmdReco/PndLmdPixelClusterTask.cxx
  LmdReco/PndLmdHitMergeTask.cxx
  LmdReco/PndLmdSimplePixelClusterFinder.cxx
  LmdReco/PndLmdChargeWeightedPixelMapping.cxx
  LmdReco/PndSdsMergedHit.cxx
  LmdReco/PndSdsCell.cxx

  # LmdReco/PndLmdIdealRecoTask.cxx
)

set(LINKDEF LmdRecoLinkDef.h)
set(LIBRARY_NAME LmdReco)

set(DEPENDENCIES SdsReco Lmd)

PANDA_GENERATE_LIBRARY()

# ########################## create PndLmdSensorAligner ######################################
set(SRCS
  LmdSensorAlignment/PndLmdPairFinderTask.cxx
  LmdSensorAlignment/PndLmdHitPair.cxx
)

set(HEADERS
  LmdSensorAlignment/PndLmdPairFinderTask.h
  LmdSensorAlignment/PndLmdHitPair.h
)

set(LINKDEF LmdSensorAlignerLinkDef.h)
set(LIBRARY_NAME LmdSensorAligner)

set(DEPENDENCIES Base Sds Lmd boost_system boost_thread boost_regex boost_filesystem)

PANDA_GENERATE_LIBRARY()

# ########################## create libLmdTrk ##################################
set(SRCS

  LmdTrack/PndLmdTrackFinderTask.cxx
  LmdTrack/PndLmdTrackFinderCATask.cxx
  LmdTrack/PndLmdLinFitTask.cxx
  LmdTrack/PndLmdLineTask.cxx
  LmdTrack/PndLmdIdealFitTask.cxx
  LmdTrack/PndLinTrack.cxx

  # LmdTrack/PndLmdKalmanTask.cxx
  LmdTrack/PndLmdGeaneTask.cxx

  # LmdTrack/PndLmdBPRungeKuttaTask.cxx
  LmdTrack/PndLmdTrksFilterTask.cxx
  LmdTrack/PndLmdSigCleanTask.cxx
  LmdTrack/PndLmdNoiseTrkSuppressionTask.cxx
)

set(LINKDEF LmdTrkLinkDef.h)
set(LIBRARY_NAME LmdTrk)

# set(DEPENDENCIES genfitAdapters RecoHits TrkBase Geane LmdReco TMVA)
set(DEPENDENCIES TrkBase Geane LmdReco TMVA Tree)

PANDA_GENERATE_LIBRARY()

# ########################## create libLmdTools ##################################
set(SRCS

  LmdQA/PndLmdTrackQ.cxx
  LmdQA/PndLmdTrkQTask.cxx
  LmdQA/PndLmdDigiQ.cxx
  LmdQA/PndLmdDigiQTask.cxx
  LmdQA/PndLmdQATask.cxx
)

set(LINKDEF LmdToolLinkDef.h)
set(LIBRARY_NAME LmdTool)

set(DEPENDENCIES Sds LmdTrk Gpad)

PANDA_GENERATE_LIBRARY()

# ########################## create tests ##################################
ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK)

set(TEST_SRCS

  # test_SensorAligner.cxx
)

foreach(testSrc ${TEST_SRCS})
  # Extract the filename without an extension (NAME_WE)
  get_filename_component(fileName ${testSrc} NAME_WE)
  SET(testName "Lmd_${fileName}")

  # Add compile target
  add_executable(${testName} tests/${testSrc})

  # Link to Boost libraries AND your targets and dependencies
  target_link_libraries(${testName}
    MathMore
    LmdSensorAligner
    Core
    Geom
    MathCore
    Physics
    boost_unit_test_framework
  )

  target_include_directories(${testName}
    PUBLIC ${Boost_INCLUDE_DIR})

  # Move testing binaries into a testBin directory
  set_target_properties(${testName}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/test/
  )

  # Finally add it to test execution -
  # Notice the WORKING_DIRECTORY and COMMAND
  add_test(NAME ${testName}
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/bin/test/
    COMMAND ${PROJECT_BINARY_DIR}/bin/test/${testName})
endforeach(testSrc)

# ----
Install(FILES
  LmdSensorAlignment/PndLmdAlignStructs.h
  DESTINATION ${PANDA_INCLUDE})

install(FILES ${CMAKE_SOURCE_DIR}/macro/detectors/lmd/lmd-geo-params.json DESTINATION ${PANDA_SHARE}/macro/detectors/lmd)
