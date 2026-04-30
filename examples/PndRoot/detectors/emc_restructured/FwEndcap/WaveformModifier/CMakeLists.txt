include(FairRootTargetRootDictionary)

set(target FwEndcapWaveformModifier)

set(sources
BSEmcBaselineAdder.cxx
BSEmcShapingNoiseAdder.cxx
BSEmcWaveformClipper.cxx
BSEmcWaveformDigitizer.cxx
BSEmcSamplingNoiseAdder.cxx

)

set(headers
BSEmcBaselineAdder.h
BSEmcShapingNoiseAdder.h
BSEmcWaveformClipper.h
BSEmcWaveformDigitizer.h
BSEmcSamplingNoiseAdder.h
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
  Panda::EmcWaveformGenerationBase

)

fairroot_target_root_dictionary(${target}
  HEADERS ${headers}
  LINKDEF FwEndcapWaveformModifierLinkDef.h
)

install(TARGETS ${target} EXPORT PandaEmcExports LIBRARY DESTINATION lib)
install(FILES ${headers} DESTINATION ${PANDA_INCLUDE_DIR})
