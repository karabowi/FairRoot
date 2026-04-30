include(FairRootTargetRootDictionary)


set(target EmcClustering)

set(sources
BSEmcClusteringPar.cxx
BSEmcClusteringTask.cxx
Processes/BSEmcRecursiveClustering.cxx
Processes/BSEmcClusteringProcess.cxx
Processes/BSEmcExpClusterSplittingProcess.cxx
Processes/BSEmcLocalMaxFindingProcess.cxx
Processes/BSEmcAddLinksToCluster.cxx
Processes/BSEmcRemoveSplitOffMax.cxx
Processes/BSEmcMCPropagationProcess.cxx
Processes/BSEmcMakePreclusters.cxx
Processes/BSEmcMergePreclusters.cxx
Processes/BSEmcOnlineBumpSplitter.cxx

BSEmcClusteringContFact.cxx
)

set(headers
BSEmcClusteringPar.h
BSEmcClusteringTask.h
Algorithms/BSEmcClusterRadiusAlgo.h

Processes/BSEmcRecursiveClustering.h
Processes/BSEmcClusteringProcess.h
Processes/BSEmcExpClusterSplittingProcess.h
Processes/BSEmcLocalMaxFindingProcess.h
Processes/BSEmcAddLinksToCluster.h
Processes/BSEmcRemoveSplitOffMax.h
Processes/BSEmcMCPropagationProcess.h

Processes/BSEmcMakePreclusters.h
Processes/BSEmcMergePreclusters.h
Processes/BSEmcOnlineBumpSplitter.h

BSEmcClusteringContFact.h
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
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Algorithms>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Processes>
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
  Panda::EmcGeometry 
  Panda::EmcTools 
  Panda::EmcClusterProperties 
  ${VMCLIB} 
  FairLogger::FairLogger
)

fairroot_target_root_dictionary(${target}
  HEADERS ${headers}
  LINKDEF EmcClusteringLinkDef.h
)

install(TARGETS ${target} EXPORT PandaEmcExports LIBRARY DESTINATION lib)
install(FILES ${headers} DESTINATION ${PANDA_INCLUDE_DIR})
