add_subdirectory(Timebased)

include(FairRootTargetRootDictionary)

set(target NewShashlik)


set(sources
BSEmcShashlik2DMapper.cxx
BSEmcShashlikSensorNameIdMap.cxx
BSEmcShashlikIdealDigitizationProcess.cxx
BSEmcShashlikECF.cxx
BSEmcShashlikDetector.cxx
)

set(headers
BSEmcShashlik2DMapper.h
BSEmcShashlikSensorNameIdMap.h
BSEmcShashlikIdealDigitizationProcess.h
BSEmcShashlikECF.h
BSEmcShashlikDetector.h
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
  Base 
  
  generalTools 
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
  LINKDEF BSEmcShashlikLinkDef.h
)

install(TARGETS ${target} EXPORT PandaEmcExports LIBRARY DESTINATION lib)
install(FILES ${headers} DESTINATION ${PANDA_INCLUDE_DIR})
