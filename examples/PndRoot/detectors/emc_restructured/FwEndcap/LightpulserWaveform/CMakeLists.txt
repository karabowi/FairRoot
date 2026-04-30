include(FairRootTargetRootDictionary)

set(target FwEndcapLightpulserWaveformGeneration)

set(sources
BSEmcFittedPulseshape.cxx
BSEmcFittedPulseshape_nw.cxx
BSEmcFwEndcapTimebasedWaveforms.cxx
BSEmcFwEndcapWaveformGeneration.cxx
BSEmcFwEndcapWaveformGenPar.cxx
BSEmcFwEndcapLightpulserContFact.cxx
)

set(headers
BSEmcFittedPulseshape.h
BSEmcFittedPulseshape_nw.h
BSEmcFwEndcapTimebasedWaveforms.h
BSEmcFwEndcapWaveformGeneration.h
BSEmcFwEndcapWaveformGenPar.h
BSEmcFwEndcapLightpulserContFact.h
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
  LINKDEF FwEndcapLightpulserWaveformGenerationLinkDef.h
)

install(TARGETS ${target} EXPORT PandaEmcExports LIBRARY DESTINATION lib)
install(FILES ${headers} DESTINATION ${PANDA_INCLUDE_DIR})
