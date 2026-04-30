include(FairRootTargetRootDictionary)

set(target CmpPndBarrelSonderslice)

set(sources
CmpPndBarrelSonderslice2DMapper.cxx
CmpPndBarrelSondersliceSensorNameIdMap.cxx
)

set(headers
CmpPndBarrelSonderslice2DMapper.h
CmpPndBarrelSondersliceSensorNameIdMap.h
)

include_directories(
${CMAKE_SOURCE_DIR}/tools
)

add_library(${target} SHARED ${sources} ${headers})
add_library(Panda::${target} ALIAS ${target})

target_include_directories(${target} PUBLIC
${ROOT_INCLUDE_DIR}
${VMC_INCLUDE_DIRS}
${FAIRROOT_INCLUDE_DIR}
${PANDAROOT_INCLUDE_DIR}
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
$<INSTALL_INTERFACE:include>
)

target_link_directories(${target} PUBLIC
${SIMPATH}/lib
${FAIRROOT_LIBRARY_DIR}
${PANDAROOT_LIBRARY_DIR}
)

target_link_libraries(${target} PUBLIC
  Base
  FairLogger::FairLogger
  NewEmcBarrel
  # EmcTools
  generalTools
)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

fairroot_target_root_dictionary(${target}
  HEADERS ${headers}
  LINKDEF CmpPndBarrelSondersliceLinkDef.h
)

install(TARGETS ${target} EXPORT ${target} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${headers} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
#install(EXPORT ${target} DESTINATION ${CMAKE_INSTALL_LIBDIR})


