include(FairRootTargetRootDictionary)

set(target EmcTools)

set(sources
BSEmcDetectorID.cxx
BSEmcHistogrammer.cxx
BSEmcJoinBranches.cxx
)

set(headers
BSEmc2DMapper.h
BSEmcCounter.h
BSEmcDetectorID.h
BSEmcHistogrammer.h
BSEmcJoinBranches.h
)

include_directories(
${CMAKE_SOURCE_DIR}/pnddata
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
  Gpad

  Base
  GeoBase
  ParBase
  FairLogger::FairLogger

  PndData
  Panda::ProcessTask
)

fairroot_target_root_dictionary(${target}
  HEADERS ${headers}
  LINKDEF EmcToolsLinkDef.h
)

install(TARGETS ${target} EXPORT PandaEmcExports LIBRARY DESTINATION lib)
install(FILES ${headers} DESTINATION ${PANDA_INCLUDE_DIR})
