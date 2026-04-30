# Create a library called "tpcreco" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${CLHEP_INCLUDE_DIR} 
  ${BASE_INCLUDE_DIRECTORIES}
  ${VMC_INCLUDE_DIRS}
  ${GEANT3_INCLUDE_DIR}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/field
  ${CMAKE_SOURCE_DIR}/tracking/PndPropagator
  ${CMAKE_SOURCE_DIR}/tracking/PndGeanePropagator
  ${CMAKE_SOURCE_DIR}/tools/MasterTasks
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${SIMPATH}/lib
${GEANT3_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndGeanePro.cxx
PndGeaneGeometryMethods.cxx
)

set(LINKDEF GeanePropagatorLinkDef.h)
set(LIBRARY_NAME geanepropagator)

set(DEPENDENCIES Base Core TrkBase Physics Field pndpropagator geant321
    FairLogger::FairLogger)

GENERATE_LIBRARY()
