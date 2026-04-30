include(FairRootTargetRootDictionary)

set(target FwEndcapTools)

set(sources
BSEmcFwEndcap2DMapper.cxx
BSEmcFwEndcapSensorNameIdMap.cxx
BSEmcFwEndcap_AgThomaBonnVersion_SensorNameIdMap.cxx

BSEmcFwEndcapECF.cxx
BSEmcFwEndcapDetIdToAlveoleMap.cxx
)

set(headers
BSEmcFwEndcap2DMapper.h
BSEmcFwEndcapSensorNameIdMap.h
BSEmcFwEndcap_AgThomaBonnVersion_SensorNameIdMap.h

BSEmcFwEndcapECF.h
BSEmcFwEndcapDetIdToAlveoleMap.h
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
  ${ROOT_LIBRARY_DIR}
  ${FAIRROOT_LIBRARY_DIR}
  ${SIMPATH}/lib
)

target_link_libraries(${target} PUBLIC
  Base
  FairLogger::FairLogger

  Panda::EmcTools
  Panda::EmcReco
)

fairroot_target_root_dictionary(${target}
  HEADERS ${headers}
  LINKDEF FwEndcapToolsLinkDef.h
)

install(TARGETS ${target} EXPORT PandaEmcExports LIBRARY DESTINATION lib)
install(FILES ${headers} DESTINATION ${PANDA_INCLUDE_DIR})
