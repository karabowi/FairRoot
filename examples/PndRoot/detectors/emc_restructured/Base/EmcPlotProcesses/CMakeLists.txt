include(FairRootTargetRootDictionary)

set(target EmcPlotProcesses)

set(sources
BSEmcPlotProcess.cxx
BSEmcPlotTask.cxx

BSEmcPlotBarrelFwEndcapOverlap.cxx
BSEmcPlotRecoHitVsMC.cxx
BSEmcPlotMCPoints.cxx
BSEmcPlotMCDeposits.cxx
BSEmcPlotDigis.cxx
BSEmcPlotCluster.cxx
BSEmcPlotSubCluster.cxx
BSEmcPlotRecoHits.cxx
BSEmcPlotAllRecoHits.cxx
BSEmcPlotMultiHitCluster.cxx
BSEmcPlotWaveforms.cxx
)

set(headers
BSEmcPlotProcess.h
BSEmcPlotTask.h

BSEmcPlotBarrelFwEndcapOverlap.h
BSEmcPlotRecoHitVsMC.h
BSEmcPlotMCPoints.h
BSEmcPlotMCDeposits.h
BSEmcPlotDigis.h
BSEmcPlotCluster.h
BSEmcPlotSubCluster.h
BSEmcPlotRecoHits.h
BSEmcPlotAllRecoHits.h
BSEmcPlotMultiHitCluster.h
BSEmcPlotWaveforms.h
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


target_link_libraries(${target} PUBLIC
  Base
  ParBase
  FairLogger::FairLogger

  PndData
  Panda::PndContainer
  Panda::ProcessTask
  Panda::EmcGeometry 
  Panda::EmcTools 
  Panda::EmcClusterProperties 
)

fairroot_target_root_dictionary(${target}
  HEADERS ${headers}
  LINKDEF EmcPlotProcessesLinkDef.h
)


install(TARGETS ${target} EXPORT PandaEmcExports LIBRARY DESTINATION lib )
install(FILES ${headers} DESTINATION ${PANDA_INCLUDE_DIR})
