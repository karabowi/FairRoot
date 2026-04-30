include(FairRootTargetRootDictionary)

set(target ProcessTask)

set(sources
PndProcessHandler.cxx
PndProcessTask.cxx
)

set(headers
  PndParameterRegister.h
  PndContainerRegister.h
  PndProcess.h
  PndProcessHandler.h
  PndProcessTask.h
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
  ${ROOT_LIBRARY_DIR}
)

target_link_libraries(${target} PUBLIC
  Base
  ParBase
  FairLogger::FairLogger
  Matrix

  PndData
  Panda::PndContainer

  ${VMCLIB}
)

fairroot_target_root_dictionary(${target}
  HEADERS ${headers}
  LINKDEF ProcessTaskLinkDef.h
)

install(TARGETS ${target} EXPORT ProcessTaskExports LIBRARY DESTINATION lib)
install(FILES ${headers} DESTINATION ${PANDA_INCLUDE_DIR})
# install(EXPORT ProcessTaskExports DESTINATION lib)

if(NOT BUILD_PANDA_WITHOUT_TESTS)
add_subdirectory(tests)
endif(NOT BUILD_PANDA_WITHOUT_TESTS)
