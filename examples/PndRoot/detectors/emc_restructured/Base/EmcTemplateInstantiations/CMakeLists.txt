include(FairRootTargetRootDictionary)

set(target EmcTemplateInstantiations)

set(sources
)

set(headers
BSEmcBaseTemplateInstantiations.h
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
  GeoBase 
  ParBase 
  Geom 
  
  PndData 
  generalTools 
  Panda::ProcessTask 
  Panda::PndContainer 
  Panda::EmcMC 
  Panda::EmcGeometry 
  Panda::EmcTools 
  Panda::EmcClusterProperties 
  Panda::EmcClustering 
  Panda::EmcPlotProcesses 
  ${VMCLIB} 
  FairLogger::FairLogger
)

fairroot_target_root_dictionary(${target}
  HEADERS ${headers}
  LINKDEF EmcTemplateInstantiationsLinkDef.h
)

install(TARGETS ${target} EXPORT PandaEmcExports LIBRARY DESTINATION lib)
#install(FILES ${headers} DESTINATION ${PANDA_INCLUDE_DIR})

