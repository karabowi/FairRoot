# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${GEANT3_INCLUDE_DIR}
  ${CLHEP_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES} 
  ${Boost_INCLUDE_DIR}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/field
  ${CMAKE_SOURCE_DIR}/detectors/passive
  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdTestBeam/Tools
  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdTestBeam/PastaTB
  ${CMAKE_SOURCE_DIR}/trackbase
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${SIMPATH}/lib
${Boost_LIBRARY_DIRS}
)

link_directories( ${LINK_DIRECTORIES})


########################### create libMvd ######################################
set(SRCS
  PndMvdPastaDigi.cxx
  PndMvdPasta.cxx
  PndMvdPastaTorino.cxx
)

set(LINKDEF MvdPastaLinkDef.h)
set(LIBRARY_NAME MvdPasta)

set(DEPENDENCIES Base GeoBase ParBase MvdTestBeamTools boost_serialization boost_thread MathCore MathMore
  # fairmq_logger
  boost_timer
  boost_system
  boost_filesystem
  boost_program_options
  boost_random
  boost_chrono)

PANDA_GENERATE_LIBRARY()
