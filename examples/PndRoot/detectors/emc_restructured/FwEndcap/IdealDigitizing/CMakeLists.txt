include(FairRootTargetRootDictionary)

set(target FwEndcapIdealDigitizing)

set(sources
BSEmcFwEndcapIdealDigitizerProcess.cxx
)

set(headers
BSEmcFwEndcapIdealDigitizerProcess.h
BSEmcFwEndcapVpttIds.h
)


include_directories(
${CMAKE_SOURCE_DIR}/tools
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
  Panda::PndContainer
  Panda::ProcessTask
  Panda::EmcMC
)

fairroot_target_root_dictionary(${target}
  HEADERS ${headers}
  LINKDEF FwEndcapIdealDigitizingLinkDef.h
)

install(TARGETS ${target} EXPORT PandaEmcExports LIBRARY DESTINATION lib)
install(FILES ${headers} DESTINATION ${PANDA_INCLUDE_DIR})
