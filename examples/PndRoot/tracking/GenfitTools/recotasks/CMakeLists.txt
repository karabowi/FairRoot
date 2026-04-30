# Create a library called "libRecoTasks" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${CLHEP_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES} 
  ${GEANT3_INCLUDE_DIR}
  ${VMC_INCLUDE_DIRS}
)

set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/tracking/GenfitTools/trackrep
  ${CMAKE_SOURCE_DIR}/tracking/GenfitTools/trackrep/GeaneTrackRep
  ${CMAKE_SOURCE_DIR}/tracking/GenfitTools/trackrep/RKTrackRep
  ${CMAKE_SOURCE_DIR}/genfit
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/detectors/stt
  ${CMAKE_SOURCE_DIR}/detectors/stt2
  ${CMAKE_SOURCE_DIR}/detectors/fts
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/GemData
  ${CMAKE_SOURCE_DIR}/pnddata/MdtData
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/pnddata/SdsData
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/pnddata/FtsData
  ${CMAKE_SOURCE_DIR}/tools
  ${CMAKE_SOURCE_DIR}/geane
  ${CMAKE_SOURCE_DIR}/tracking/GenfitTools/adapters
  ${CMAKE_SOURCE_DIR}/tracking/GenfitTools/recohits
  ${CMAKE_SOURCE_DIR}/tracking/GenfitTools/recotasks
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
PndRecoKalmanFit.cxx
PndRecoDafFit.cxx
PndRecoKalmanTask.cxx
PndRecoMultiKalmanTask.cxx
)

set(LINKDEF  RecoTasksLinkDef.h)
set(LIBRARY_NAME RecoTasks)

set(DEPENDENCIES Base ParBase PndData generalTools Stt PndStt2 Fts TrkBase
Geane RecoHits genfitAdapters)

PANDA_GENERATE_LIBRARY()
