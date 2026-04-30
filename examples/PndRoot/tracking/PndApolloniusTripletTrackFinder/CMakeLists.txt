# Create a library called "apolloniusTriplet" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${CLHEP_INCLUDE_DIR} 
  ${BASE_INCLUDE_DIRECTORIES}
  ${ROOT_INCLUDE_DIRS}
  ${VMC_INCLUDE_DIRS}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/trackbase 
  ${CMAKE_SOURCE_DIR}/tracking/PndApolloniusTripletTrackFinder
  ${CMAKE_SOURCE_DIR}/tracking/riemannfit
  ${CMAKE_SOURCE_DIR}/tracking/PndTrackingTools
  ${CMAKE_SOURCE_DIR}/tracking/PndPreselectSttHits
  ${CMAKE_SOURCE_DIR}/tracking/PndHoughTrackFinder
  ${CMAKE_SOURCE_DIR}/tracking/SttCA
  ${CMAKE_SOURCE_DIR}/tracking/SttCellTrackFinder
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/detectors/stt
  ${CMAKE_SOURCE_DIR}/detectors/stt2
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndApolloniusTriplet.cxx
PndApolloniusTripletTrackFinder.cxx
PndApolloniusTripletTrackFinderTask.cxx
)

set(LINKDEF  PndApolloniusTripletTrackFinderLinkDef.h)
set(LIBRARY_NAME apolloniusTriplet)

set(DEPENDENCIES Base Core TrkBase Physics TrackingTools preselectStt Stt PndStt2 SttCA SttCellTrackFinder Hough)

PANDA_GENERATE_LIBRARY()
