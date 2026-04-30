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
  ${CMAKE_SOURCE_DIR}/field
  ${CMAKE_SOURCE_DIR}/detectors/passive
  ${CMAKE_SOURCE_DIR}/tools
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdData
  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdTools
  ${CMAKE_SOURCE_DIR}/pnddata/MvdData
  ${CMAKE_SOURCE_DIR}/pnddata/SdsData
  ${CMAKE_SOURCE_DIR}/detectors/stt
  ${CMAKE_SOURCE_DIR}/detectors/stt2
  ${CMAKE_SOURCE_DIR}/detectors/stt/sttreco
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/detectors/gem
  ${CMAKE_SOURCE_DIR}/pnddata/GemData
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/genfit
  ${CMAKE_SOURCE_DIR}/recotasks
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/tracking/GenfitTools/trackrep/LSLtrackRep
  ${CMAKE_SOURCE_DIR}/tracking/GenfitTools/trackrep/GeaneTrackRep
  ${CMAKE_SOURCE_DIR}/tracking/GenfitTools/recohits
  ${CMAKE_SOURCE_DIR}/geane
#  ${CMAKE_SOURCE_DIR}/global
  ${CMAKE_SOURCE_DIR}/tracking/PndBarrelTrackFinder
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
PndBarrelTrackFinder.cxx
)

set(LINKDEF  BarrelTrackFinderLinkDef.h)
set(LIBRARY_NAME BarrelTrackFinder)

set(DEPENDENCIES Base ParBase PndData TrkBase Gem Stt PndStt2 Geane)

PANDA_GENERATE_LIBRARY()
