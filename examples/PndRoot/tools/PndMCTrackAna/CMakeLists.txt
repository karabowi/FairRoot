# Create a library called "generalTools" which includes the source files given in
# the array .
# The header extension is already found. Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${BASE_INCLUDE_DIRECTORIES} 
  ${CLHEP_INCLUDE_DIR}
  ${VMC_INCLUDE_DIRS} 
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/pnddata 
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/pnddata/PidData
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/tools/PndMCTrackAna
  ${CMAKE_SOURCE_DIR}/tools
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
  PndEventRequirements.cxx
  PndEventCheck.cxx
  PndMCTrackInfo.cxx
  PndParticleRequirements.cxx
  PndMCTrackInfoAnaTask.cxx
  PndMCTrackInfoTask.cxx
)

set(LINKDEF  mcTrackAnaToolsLinkDef.h)
set(LIBRARY_NAME mcTrackAnaTools)

set(DEPENDENCIES Base ParBase PndData generalTools)

PANDA_GENERATE_LIBRARY()
