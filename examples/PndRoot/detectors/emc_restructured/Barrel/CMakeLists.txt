add_subdirectory(LateralClusterSplittingProcess)

include(FairRootTargetRootDictionary)

set(target NewEmcBarrel)

set(sources
BSEmcBarrel2DMapper.cxx
BSEmcBarrelSensorNameIdMap.cxx
BSEmcBarrelZDependentMCDepositProductionProcess.cxx
BSEmcBarrelErrorMatrixCalculation.cxx
BSEmcBarrelDigiNonuniformityPar.cxx
BSEmcBarrelECF.cxx
BSEmcBarrelContFact.cxx
)

set(headers
BSEmcBarrel2DMapper.h
BSEmcBarrelSensorNameIdMap.h
BSEmcBarrelZDependentMCDepositProductionProcess.h
BSEmcBarrelErrorMatrixCalculation.h
BSEmcBarrelDigiNonuniformityPar.h
BSEmcBarrelECF.h
BSEmcBarrelContFact.h
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
  Panda::ProcessTask 
  Panda::PndContainer 
  Panda::EmcMC 
  Panda::EmcTools 
  Panda::EmcReco  
#  ${VMCLIB} 
  FairLogger::FairLogger 
)

fairroot_target_root_dictionary(${target}
  HEADERS ${headers}
  LINKDEF BSEmcBarrelLinkDef.h
)

install(TARGETS ${target} EXPORT PandaEmcExports LIBRARY DESTINATION lib)
install(FILES ${headers} DESTINATION ${PANDA_INCLUDE_DIR})

