# Create a library called "tpcreco" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${CLHEP_INCLUDE_DIR} 
  ${BASE_INCLUDE_DIRECTORIES}
  ${VMC_INCLUDE_DIRS}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/field
  ${CMAKE_SOURCE_DIR}/tracking/PndPropagator
  ${CMAKE_SOURCE_DIR}/tracking/PndHelixPropagator
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${SIMPATH}/lib
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndHelixPropagator.cxx
)

set(LINKDEF helixPropagatorLinkDef.h)
set(LIBRARY_NAME helixpropagator)

set(DEPENDENCIES Base Core TrkBase Physics Field pndpropagator
    FairLogger::FairLogger)

PANDA_GENERATE_LIBRARY()
