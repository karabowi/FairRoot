include(FairRootTargetRootDictionary)

set(target EmcClusterProperties)

set(sources
BSEmcPositionAlgo.cxx
BSEmcEnergyCalculationAlgo.cxx
BSEmcXClMoments.cxx
BSEmcClusterPropertiesPar.cxx
BSEmcClusterPropertiesContFact.cxx
)

set(headers
BSEmcPositionAlgo.h
BSEmcEnergyCalculationAlgo.h
BSEmcXClMoments.h
BSEmcClusterPropertiesPar.h
BSEmcClusterPropertiesContFact.h
)

include_directories(
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/EmcData
${CMAKE_SOURCE_DIR}/tools
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
  ParBase 
  
  PndData 
  Panda::EmcGeometry 
  FairLogger::FairLogger
)

fairroot_target_root_dictionary(${target}
  HEADERS ${headers}
  LINKDEF ClusterPropertiesLinkDef.h
)

install(TARGETS ${target} EXPORT PandaEmcExports LIBRARY DESTINATION lib)
install(FILES ${headers} DESTINATION ${PANDA_INCLUDE_DIR})

