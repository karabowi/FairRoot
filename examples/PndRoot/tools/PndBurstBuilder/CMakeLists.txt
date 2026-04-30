# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${GEANT3_INCLUDE_DIR}
  ${CLHEP_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES}
  ${VMC_INCLUDE_DIRS} 
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/tools/PndBurstBuilder
  #${CMAKE_SOURCE_DIR}/recotasks
  #${CMAKE_SOURCE_DIR}/trackbase
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${SIMPATH}/lib
)

link_directories( ${LINK_DIRECTORIES})


########################### create libSds ######################################
set(SRCS

PndBranchBurstBuilder.cxx
PndBranchBurstBuilder_eventBased.cxx
PndBranchBurstBuilder_timeCut_STB.cxx
PndBranchBurstBuilder_timeCut.cxx
)

set(LINKDEF  PndBurstBuilderLinkDef.h)
set(LIBRARY_NAME PndBurstBuilder)

set(DEPENDENCIES Base Core)
#set(DEPENDENCIES Base GeoBase ParBase PndData generalTools Core)

PANDA_GENERATE_LIBRARY()
 
