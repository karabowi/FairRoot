include(FairRootTargetRootDictionary)

set(target PndContainer)

set(sources)

set(headers
  PndContainerBaseI.h
  PndContainerI.h
  PndConstContainerI.h
  PndMutableContainerI.h

  PndSTDConstContainer.h
  PndSTDMutableContainer.h
  PndTCAConstContainer.h
  PndTCAMutableContainer.h

  PndConstContainer.h
  PndMutableContainer.h

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
  FairLogger::FairLogger

  ${VMCLIB}
)

fairroot_target_root_dictionary(${target}
  HEADERS ${headers}
  LINKDEF PndContainerLinkDef.h
)

install(TARGETS ${target} EXPORT PndContainerExport LIBRARY DESTINATION lib)
install(FILES ${headers} DESTINATION ${PANDA_INCLUDE_DIR})
# install(EXPORT PndContainerExport DESTINATION lib)

if(NOT BUILD_PANDA_WITHOUT_TESTS)
add_subdirectory(tests)
endif(NOT BUILD_PANDA_WITHOUT_TESTS)
