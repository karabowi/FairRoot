include(FairRootTargetRootDictionary)
include(FairMacros)

set(target EmcTimeSortingBase)

set(sources
BSEmcDigiTimeBuncherProcess.cxx
BSEmcPreclusterTimeBuncherProcess.cxx
BSEmcDigiTimeBuncherProcess.cxx
BSEmcFetchDigisFromPreclusterLinks.cxx

)

set(headers
BSEmcDigiTimeBuncherProcess.h
BSEmcPreclusterTimeBuncherProcess.h
BSEmcDigiTimeBuncherProcess.h
BSEmcFetchDigisFromPreclusterLinks.h

BSEmcRingSorter.h
BSEmcSorterTask.h
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

target_link_directories(${target} PUBLIC
  ${FAIRROOT_LIBRARY_DIR}
)

target_link_libraries(${target} PUBLIC
  Base
  GeoBase
  ParBase
  Geom
  FairLogger::FairLogger

  PndData
  generalTools
  Panda::ProcessTask
  Panda::PndContainer
  Panda::EmcMC
  Panda::EmcClustering
  ${VMCLIB}
)

fairroot_target_root_dictionary(${target}
  HEADERS ${headers}
  LINKDEF TimeSortingLinkDef.h
)

install(TARGETS ${target} EXPORT PandaEmcExports LIBRARY DESTINATION lib)
install(FILES ${headers} DESTINATION ${PANDA_INCLUDE_DIR})
