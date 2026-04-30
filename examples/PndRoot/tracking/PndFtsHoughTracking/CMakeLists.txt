# Create a library called "libFts" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${GEANT3_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/field
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/FtsData
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/detectors/fts
  ${CMAKE_SOURCE_DIR}/tracking/FtsHoughTracking
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndFtsTrackerIdeal.cxx
PndFtsHoughTracklet.cxx
PndFtsHoughTrackCand.cxx
PndFtsHoughSpace.cxx
PndFtsHoughSpaceBinning.cxx
PndFtsHoughSpacePeak.cxx
PndFtsHoughTrackFinderParams.cxx
PndFtsHoughTrackFinder.cxx
PndFtsHoughTrackerTask.cxx
PndFtsHitProducerMcPointCoordinates.cxx
)

set(LINKDEF  PndFtsHoughTrackingLinkDef.h)
set(LIBRARY_NAME FtsHoughTrkacking)

set(DEPENDENCIES Base GeoBase ParBase PndData TrkBase Spectrum)

PANDA_GENERATE_LIBRARY()
