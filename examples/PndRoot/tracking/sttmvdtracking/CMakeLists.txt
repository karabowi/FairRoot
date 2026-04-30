# Create a library called "libSttMvdTracking" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.


Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${GEANT3_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES}
  ${VMC_INCLUDE_DIRS}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/pnddata 
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/tracking/riemannfit
  ${CMAKE_SOURCE_DIR}/tools
  ${CMAKE_SOURCE_DIR}/field 
  ${CMAKE_SOURCE_DIR}/detectors/stt
  ${CMAKE_SOURCE_DIR}/tracking/sttmvdtracking
  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdTracking
  ${CMAKE_SOURCE_DIR}/pnddata/SdsData
  ${CMAKE_SOURCE_DIR}/pnddata/SciTData
  ${CMAKE_SOURCE_DIR}/detectors/gem 
  ${CMAKE_SOURCE_DIR}/pnddata/GemData 
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
PndSttMvdGemTracking.cxx
#PndSttMvdGemTrackingIdeal.cxx
PndMixBackgroundEvents.cxx
PndMvdSttGemRiemannTrackFinder.cxx
)

set(LINKDEF  SttMvdTrackingLinkDef.h)
set(LIBRARY_NAME SttMvdTracking)

set(DEPENDENCIES Base GeoBase ParBase PndData Geane Gem Stt riemann)

PANDA_GENERATE_LIBRARY()
