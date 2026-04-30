# Create a library called "libRecoHits" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${CLHEP_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES} 
  ${GEANT3_INCLUDE_DIR}
  ${VMC_INCLUDE_DIRS}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/tracking/GenfitTools/trackrep
  ${CMAKE_SOURCE_DIR}/tracking/GenfitTools/trackrep/GeaneTrackRep
  ${CMAKE_SOURCE_DIR}/tracking/GenfitTools/trackrep/RKTrackRep
  ${CMAKE_SOURCE_DIR}/genfit
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/GemData
  ${CMAKE_SOURCE_DIR}/pnddata/MdtData
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/pnddata/SdsData
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/pnddata/FtsData
  ${CMAKE_SOURCE_DIR}/detectors/stt
  ${CMAKE_SOURCE_DIR}/detectors/fts
  ${CMAKE_SOURCE_DIR}/tools
  ${CMAKE_SOURCE_DIR}/geane
  ${CMAKE_SOURCE_DIR}/tracking/GenfitTools/recohits
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
PndGemRecoHit.cxx
PndMdtRecoHit.cxx
PndSdsRecoHit.cxx
PndSttRecoHit.cxx
PndSttRecoHitProducer.cxx
PndFtsRecoHit.cxx
PndFtsRecoHitProducer.cxx
PndHitRecoHit.cxx
)

set(LINKDEF  RecoHitsLinkDef.h)
set(LIBRARY_NAME RecoHits)

set(DEPENDENCIES Base Tree ParBase PndData generalTools Stt Fts genfit trackrep ${VMCLIB})

PANDA_GENERATE_LIBRARY()
