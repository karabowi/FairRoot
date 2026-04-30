# Create a library called "libgenfitAdapter" which includes the source files given in
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
  ${CMAKE_SOURCE_DIR}/tracking/GenfitTools/trackrep/LSLtrackRep
  ${CMAKE_SOURCE_DIR}/genfit
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/geane
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
PndGenfitAdapters.cxx
PndGenfitField.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${GFADAPTER_SRCS}" "${INCLUDE_DIRECTORIES}" GFADAPTER_RULES)
endif(RULE_CHECKER_FOUND)

set(LIBRARY_NAME genfitAdapters)

set(DEPENDENCIES trackrep PndData)

PANDA_GENERATE_LIBRARY()
