include(FairRootTargetRootDictionary)

set(target EmcMC)

set(sources
BSEmcDetector.cxx
DepositCreation/BSEmcMCDepositProducerTask.cxx
DepositCreation/Processes/BSEmcSortMCTruthAlgo.cxx
DepositCreation/Processes/BSEmcMCDepositProductionProcess.cxx
DepositCreation/BSEmcMCPar.cxx

DigiCreation/BSEmcDigitizerTask.cxx
DigiCreation/BSEmcDigiPar.cxx
DigiCreation/BSEmcDataConcentratorLookUp.cxx
DigiCreation/BSEmcCalibrationPar.cxx
DigiCreation/Processes/BSEmcIdealDigitizationProcess.cxx
DigiCreation/Processes/BSEmcApplyCalibrationProcess.cxx
DigiCreation/Processes/BSEmcDigiDCSetterProcess.cxx

BSEmcMCContFact.cxx #Compile this last
)

set(headers
DepositCreation/BSEmcMCDepositProducerTask.h
DigiCreation/BSEmcDigitizerTask.h

BSEmcDetector.h
DepositCreation/Processes/BSEmcSortMCTruthAlgo.h
DepositCreation/Processes/BSEmcMCDepositProductionProcess.h
DepositCreation/BSEmcMCPar.h

DigiCreation/BSEmcDigiPar.h
DigiCreation/BSEmcDataConcentratorLookUp.h
DigiCreation/BSEmcCalibrationPar.h
DigiCreation/Processes/BSEmcIdealDigitizationProcess.h
DigiCreation/Processes/BSEmcApplyCalibrationProcess.h
DigiCreation/Processes/BSEmcDigiDCSetterProcess.h

BSEmcMCContFact.h #Compile this last
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
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/DepositCreation>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/DepositCreation/Processes>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/DigiCreation>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/DigiCreation/Processes>
  $<INSTALL_INTERFACE:${PANDA_INCLUDE_DIR}>
)

target_link_directories(${target} PUBLIC
  ${FAIRROOT_LIBRARY_DIR}
)

target_link_libraries(${target} PUBLIC
  MathCore

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
  ${VMCLIB}
  FairLogger::FairLogger
)

fairroot_target_root_dictionary(${target}
  HEADERS ${headers}
  LINKDEF EmcMCLinkDef.h
)

install(TARGETS ${target} EXPORT PandaEmcExports LIBRARY DESTINATION lib)
install(FILES ${headers} DESTINATION ${PANDA_INCLUDE_DIR})
