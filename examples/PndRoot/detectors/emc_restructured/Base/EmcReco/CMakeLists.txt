include(FairRootTargetRootDictionary)

set(target EmcReco)

set(sources

Processes/BSEmcCreateRecoHitProcess.cxx
Processes/BSEmcOutOfClusterCorrectionProcess.cxx
Processes/BSEmcFinishRecoHitProcess.cxx
BSEmcPhiRecoHitSplitter.cxx

BSEmcCorrection.cxx
BSEmcThetaEnergyECF.cxx
BSEmcErrorCalculationPar.cxx
BSEmcErrorMatrixCalculation.cxx

BSEmcSplitOffPar.cxx
Processes/BSEmcFlagSplitOffs.cxx
Processes/BSEmcMergeRecoHits.cxx

BSEmcRecoContFact.cxx #Compile this last
)

set(headers
Processes/BSEmcCreateRecoHitProcess.h
Processes/BSEmcOutOfClusterCorrectionProcess.h
Processes/BSEmcFinishRecoHitProcess.h
BSEmcPhiRecoHitSplitter.h

BSEmcCorrection.h
BSEmcThetaEnergyECF.h
BSEmcErrorCalculationPar.h
BSEmcErrorMatrixCalculation.h

BSEmcSplitOffPar.h
Processes/BSEmcFlagSplitOffs.h
Processes/BSEmcMergeRecoHits.h

BSEmcRecoContFact.h #Compile this last
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
  Panda::EmcMC
  Panda::EmcClustering
  ${VMCLIB} 
  FairLogger::FairLogger
)

fairroot_target_root_dictionary(${target}
  HEADERS ${headers}
  LINKDEF EmcRecoLinkDef.h
)

install(TARGETS ${target} EXPORT PandaEmcExports LIBRARY DESTINATION lib)
install(FILES ${headers} DESTINATION ${PANDA_INCLUDE_DIR})
