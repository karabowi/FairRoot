include(FairRootTargetRootDictionary)

set(target NewEmcBwEndcap)

set(sources
BSEmcBwEndcapContFact.cxx
BSEmcBwEndcap2DMapper.cxx
BSEmcBwEndcapECF.cxx
BSEmcBwEndcapSensorNameIdMap.cxx
Timebased/BSEmcBwEndcapDigi.cxx
Timebased/BSEmcBwEndcapDigiPar.cxx
Timebased/BSEmcBwEndcapDigitizer.cxx
Timebased/BSEmcBwEndcapNoiseAdder.cxx
Timebased/BSEmcBwEndcapPulseAmplifier.cxx
Timebased/BSEmcBwEndcapTimebasedWaveforms.cxx
Timebased/BSEmcBwPulseshape.cxx
Timebased/BSEmcMultiPSA.cxx
Timebased/BSEmcPSAOverflowCombinator.cxx
Timebased/BSEmcPSATmaxAnalyser.cxx
Timebased/BSEmcBwEndcapOnlineFeatureExtraction.cxx
)

set(headers
BSEmcBwEndcapContFact.h
BSEmcBwEndcap2DMapper.h
BSEmcBwEndcapECF.h
BSEmcBwEndcapSensorNameIdMap.h
Timebased/BSEmcBwEndcapDigi.h
Timebased/BSEmcBwEndcapDigiPar.h
Timebased/BSEmcBwEndcapDigitizer.h
Timebased/BSEmcBwEndcapNoiseAdder.h
Timebased/BSEmcBwEndcapPulseAmplifier.h
Timebased/BSEmcBwEndcapTimebasedWaveforms.h
Timebased/BSEmcBwPulseshape.h
Timebased/BSEmcMultiPSA.h
Timebased/BSEmcPSACombinator.h
Timebased/BSEmcPSAOverflowCombinator.h
Timebased/BSEmcPSATmaxAnalyser.h
Timebased/BSEmcBwEndcapOnlineFeatureExtraction.h
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
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Timebased>
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
  Panda::EmcWaveformGenerationBase 
  Panda::EmcFeatureExtractionBase
  Panda::EmcReco  
  
  ${VMCLIB} 
  FairLogger::FairLogger 
)

fairroot_target_root_dictionary(${target}
  HEADERS ${headers}
  LINKDEF BSEmcBwEndcapLinkDef.h
)

install(TARGETS ${target} EXPORT PandaEmcExports LIBRARY DESTINATION lib)
install(FILES ${headers} DESTINATION ${PANDA_INCLUDE_DIR})

