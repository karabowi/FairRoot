# Create a library called "libtrackrep" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${CLHEP_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES} 
  ${GEANT3_INCLUDE_DIR}
  ${VMC_INCLUDE_DIRS}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/tracking/GenfitTools/trackrep
  ${CMAKE_SOURCE_DIR}/tracking/GenfitTools/trackrep/GeaneTrackRep
  ${CMAKE_SOURCE_DIR}/tracking/GenfitTools/trackrep/RKTrackRep
  ${CMAKE_SOURCE_DIR}/genfit
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/geane
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})


set(SRCS 
GeaneTrackRep/GeaneTrackRep.cxx
RKTrackRep/RKTrackRep.cxx
)

set(LINKDEF  trackrepLinkDef.h)
set(LIBRARY_NAME trackrep)

set(DEPENDENCIES genfit Geane TrkBase)

PANDA_GENERATE_LIBRARY()
