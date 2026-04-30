include(FairRootTargetRootDictionary)

set(target EmcUtilities)

set(sources
PndEmcDetectors.cxx
PndEmcMCDepositProducers.cxx
PndEmcIdealDigitizers.cxx
PndEmcWaveformFeatureExtractions.cxx
PndEmcClusterers.cxx
)

set(headers
PndEmcDetectors.h
PndEmcMCDepositProducers.h
PndEmcIdealDigitizers.h
PndEmcWaveformFeatureExtractions.h
PndEmcClusterers.h
)


include_directories(
${CMAKE_SOURCE_DIR}/tools
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

  generalTools
  PndData

  Panda::EmcMC
  Panda::EmcTools
  Panda::FwEndcapTools
  Panda::NewEmcBwEndcap
  Panda::NewEmcBarrel
  Panda::NewShashlik
  Panda::ShashlykTimebasedDigi

  Panda::FwEndcapIdealDigitizing
  Panda::FwEndcapLightpulserWaveformGeneration
  Panda::FwEndcapEmcFeatureExtractionBase

  ${VMCLIB}
  FairLogger::FairLogger
)

fairroot_target_root_dictionary(${target}
  HEADERS ${headers}
  LINKDEF UtilityLinkDef.h
)

install(TARGETS ${target} EXPORT PandaEmcExports LIBRARY DESTINATION lib)
install(FILES ${headers} DESTINATION ${PANDA_INCLUDE_DIR})
