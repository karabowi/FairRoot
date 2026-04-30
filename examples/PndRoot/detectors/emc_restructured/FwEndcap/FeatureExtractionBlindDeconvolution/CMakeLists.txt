include(FairRootTargetRootDictionary)

set(target FwEndcapEmcFeatureExtractionBlindDeconvolutionBase)

set(sources
PSA/BSEmcPSAFPGABaselineFinder.cxx
PSA/BSEmcPSAFPGABlindDeconv.cxx
PSA/BSEmcPSAFPGAPeakfinder.cxx
PSA/BSEmcPSAFPGAPeakfinderTime.cxx
PSA/BSEmcPSAFPGAPileUpDetection.cxx
BSEmcFwEndcapBlindDeconvProcess.cxx
BSEmcFwEndcapBlindDeconvPar.cxx
BSEmcFwEndcapBlindDeconvContFact.cxx
)

set(headers
PSA/BSEmcPSAFPGABaselineFinder.h
PSA/BSEmcPSAFPGABlindDeconv.h
PSA/BSEmcPSAFPGAPeakfinder.h
PSA/BSEmcPSAFPGAPeakfinderTime.h
PSA/BSEmcPSAFPGAPileUpDetection.h
PSA/BSEmcPSAFPGAPeakFeatures_t.h
BSEmcFwEndcapBlindDeconvProcess.h
BSEmcFwEndcapBlindDeconvPar.h
BSEmcFwEndcapBlindDeconvContFact.h
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

install(TARGETS ${target} LIBRARY DESTINATION lib)
install(FILES ${headers} DESTINATION ${PANDA_INCLUDE_DIR})
