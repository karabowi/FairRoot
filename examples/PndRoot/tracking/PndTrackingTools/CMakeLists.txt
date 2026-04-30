# Create a library called "libMCMatch" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${BASE_INCLUDE_DIRECTORIES}
  ${VMC_INCLUDE_DIRS}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/pndbase
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/detectors/stt
  ${CMAKE_SOURCE_DIR}/detectors/stt2
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/tracking/PndTrackingTools
  
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
PndMissingPzCleanerTask.cxx
PndTrackSmearTask.cxx
PndTrackEvaluatorDetI.cxx
PndTrackEvaluatorDetStt.cxx
)

set(LINKDEF  PndTrackingToolsLinkDef.h)
set(LIBRARY_NAME TrackingTools)

set(DEPENDENCIES Base MathCore ParBase PndData FairTools Stt PndStt2)

PANDA_GENERATE_LIBRARY()
