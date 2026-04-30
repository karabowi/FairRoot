include(FairRootTargetRootDictionary)

set(target EmcGeometry)

set(sources
BSEmcPositionPar.cxx
BSEmcCrystalPositionPar.cxx
BSEmcGeoItem.cxx
BSEmcGeoNeighbouringRelationPar.cxx
BSEmcParameterLoadingProcess.cxx
BSEmcStructure.cxx
BSEmcXtal.cxx
BSEmcPositionContFact.cxx
)

set(headers
BSEmcPositionPar.h
BSEmcCrystalPositionPar.h
BSEmcGeoItem.h
BSEmcGeoNeighbouringRelationPar.h
BSEmcParameterLoadingProcess.h
BSEmcStructure.h
BSEmcXtal.h
BSEmcPositionContFact.h
)

include_directories(
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
  FairLogger::FairLogger
  
  PndData
  generalTools
  Panda::ProcessTask
  Panda::EmcTools
)

fairroot_target_root_dictionary(${target}
  HEADERS ${headers}
  LINKDEF EmcGeometryLinkDef.h
)

install(TARGETS ${target} EXPORT PandaEmcExports LIBRARY DESTINATION lib)
install(FILES ${headers} DESTINATION ${PANDA_INCLUDE_DIR})
