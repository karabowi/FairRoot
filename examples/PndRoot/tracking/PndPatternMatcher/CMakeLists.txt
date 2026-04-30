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
  ${CMAKE_SOURCE_DIR}/tracking/PndPatternMatcher
  ${CMAKE_SOURCE_DIR}/detectors/stt
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${SIMPATH}/lib
)

link_directories( ${LINK_DIRECTORIES})


########################### create libPatternMatcher ######################################
set(SRCS

PndPattern.cxx
PndPatternDBGenerator.cxx
PndPatternDBMerger.cxx
PndPatternMatcher.cxx

)

set(LINKDEF  PndPatternMatcherLinkDef.h)
set(LIBRARY_NAME PatternMatcher)

set(DEPENDENCIES Base PndData Stt ParBase RIO Tree)
#set(DEPENDENCIES Base GeoBase ParBase PndData generalTools)

PANDA_GENERATE_LIBRARY()
 
