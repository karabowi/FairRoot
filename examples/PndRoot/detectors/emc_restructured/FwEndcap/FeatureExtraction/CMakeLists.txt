include(FairRootTargetRootDictionary)

set(target FwEndcapEmcFeatureExtractionBase)

set(sources
BSEmcFwEndcapDigi.cxx
BSEmcFwEndcapDigiPar.cxx
BSEmcFwEndcapOnlineFeatureExtraction.cxx
PSA/BSEmcHighLowPSA.cxx
PSA/BSEmcPSAFPGAFilterCF.cxx
PSA/BSEmcPSAFPGAFilterDelay.cxx
PSA/BSEmcPSAFPGAFilterLine.cxx
PSA/BSEmcPSAFPGAFilterMA.cxx
PSA/BSEmcPSAFPGAFilterMWD.cxx
PSA/BSEmcPSAFPGALinFitter.cxx
PSA/BSEmcPSAFPGAPileupAnalyser.cxx
PSA/BSEmcPSAFPGASampleAnalyser.cxx
BSEmcFwEndcapFeatureExtractionContFact.cxx
)

set(headers
BSEmcFwEndcapDigi.h
BSEmcFwEndcapDigiPar.h
BSEmcFwEndcapOnlineFeatureExtraction.h
PSA/BSEmcHighLowPSA.h
PSA/BSEmcPSAFPGAFilterCF.h
PSA/BSEmcPSAFPGAFilterDelay.h
PSA/BSEmcPSAFPGAFilterLine.h
PSA/BSEmcPSAFPGAFilterMA.h
PSA/BSEmcPSAFPGAFilterMWD.h
PSA/BSEmcPSAFPGALinFitter.h
PSA/BSEmcPSAFPGAPileupAnalyser.h
PSA/BSEmcPSAFPGASampleAnalyser.h
BSEmcFwEndcapFeatureExtractionContFact.h
)

include_directories(
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/EmcData
)

add_library(${target} SHARED ${sources} ${headers})
add_library(Panda::${target} ALIAS ${target})

target_include_directories(${target} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PSA>
  $<INSTALL_INTERFACE:${PANDA_INCLUDE_DIR}>
)

target_link_directories(${target} PUBLIC
  ${FAIRROOT_LIBRARY_DIR}
)

target_link_libraries(${target} PUBLIC
  Base
  FairLogger::FairLogger

  PndData
  Panda::ProcessTask
  Panda::PndContainer
  Panda::EmcFeatureExtractionBase

)

fairroot_target_root_dictionary(${target}
  HEADERS ${headers}
  LINKDEF FwEndcapFeatureExtractionLinkDef.h
)

install(TARGETS ${target} EXPORT PandaEmcExports LIBRARY DESTINATION lib)
install(FILES ${headers} DESTINATION ${PANDA_INCLUDE_DIR})
