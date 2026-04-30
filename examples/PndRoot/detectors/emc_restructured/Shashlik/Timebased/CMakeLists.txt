include(FairRootTargetRootDictionary)

set(target ShashlykTimebasedDigi)

set(sources
BSEmcPSAOptimalFilterAnalyser.cxx
BSEmcShashlykDigiPar.cxx
BSEmcShashlykNoiseAdder.cxx
BSEmcShashlykPulseshape.cxx

BSEmcShashlykDigi.cxx
BSEmcShashlykTimebasedWaveforms.cxx

BSEmcShashlykDigiContFact.cxx

)

set(headers
BSEmcPSAOptimalFilterAnalyser.h
BSEmcShashlykDigiPar.h
BSEmcShashlykNoiseAdder.h
BSEmcShashlykPulseshape.h

BSEmcShashlykDigi.h
BSEmcShashlykTimebasedWaveforms.h

BSEmcShashlykDigiContFact.h
)


include_directories(
${CMAKE_SOURCE_DIR}/tools
${CMAKE_SOURCE_DIR}/pnddata/
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
  FairLogger::FairLogger

  PndData
  Panda::EmcWaveformGenerationBase
  Panda::EmcFeatureExtractionBase
  Panda::NewEmcBwEndcap

)

fairroot_target_root_dictionary(${target}
  HEADERS ${headers}
  LINKDEF ShashlikTimebasedLinkDef.h
)

install(TARGETS ${target} EXPORT PandaEmcExports LIBRARY DESTINATION lib)
install(FILES ${headers} DESTINATION ${PANDA_INCLUDE_DIR})
