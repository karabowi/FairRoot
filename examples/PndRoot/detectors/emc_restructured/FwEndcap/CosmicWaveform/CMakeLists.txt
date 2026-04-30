include(FairRootTargetRootDictionary)

set(target FwEndcapCosmicWaveformGeneration)

set(sources
BSEmcLUTBasedPulseshape.cxx
BSEmcFwEndcapCosmicWaveformGeneration.cxx
BSEmcFwEndcapAPDAndVPTTWaveformGenerator.cxx
BSEmcFwEndcapCosmicWaveformGenPar.cxx
BSEmcFwEndcapCosmicWaveformContFact.cxx
)

set(headers
BSEmcLUTBasedPulseshape.h
BSEmcFwEndcapCosmicWaveformGeneration.h
BSEmcFwEndcapAPDAndVPTTWaveformGenerator.h
BSEmcFwEndcapCosmicWaveformGenPar.h
BSEmcFwEndcapCosmicWaveformContFact.h
)

include_directories(
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/EmcData
)

add_library(${target} SHARED ${sources} ${headers})
add_library(Panda::${target} ALIAS ${target})

target_include_directories(${target} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${PANDA_INCLUDE_DIR}>
)

target_link_directories(${target} PUBLIC
  ${FAIRROOT_LIBRARY_DIR}
)

target_link_libraries(${target} PUBLIC
  Base
  FairLogger::FairLogger

  PndData
  generalTools
  Panda::ProcessTask
  Panda::PndContainer
  Panda::EmcWaveformGenerationBase
  Panda::FwEndcapWaveformModifier
)

fairroot_target_root_dictionary(${target}
  HEADERS ${headers}
  LINKDEF FwEndcapCosmicWaveformGenerationLinkDef.h
)

install(TARGETS ${target} EXPORT PandaEmcExports LIBRARY DESTINATION lib)
install(FILES ${headers} DESTINATION ${PANDA_INCLUDE_DIR})
