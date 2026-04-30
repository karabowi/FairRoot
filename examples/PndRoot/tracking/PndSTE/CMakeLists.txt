# Create a library called "SttTrackExtrapolator" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}   
  ${ROOT_INCLUDE_DIR}
  ${GEANT3_INCLUDE_DIR}
  #${CLHEP_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES}
  ${VMC_INCLUDE_DIRS}
  ${GEANT4_INCLUDE_DIR}
)

Set(INCLUDE_DIRECTORIES 
  ${CMAKE_SOURCE_DIR}/pnddata 
  ${CMAKE_SOURCE_DIR}/pndbase/PndStdUtils
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/pnddata/SdsData
  ${CMAKE_SOURCE_DIR}/pnddata/SciTData  
  ${CMAKE_SOURCE_DIR}/pnddata/GemData
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData  
  ${CMAKE_SOURCE_DIR}/field
  ${CMAKE_SOURCE_DIR}/detectors/passive
  ${CMAKE_SOURCE_DIR}/genfit
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/tools 
  ${CMAKE_SOURCE_DIR}/tools/MasterTasks
  ${CMAKE_SOURCE_DIR}/detectors/mvd 
  ${CMAKE_SOURCE_DIR}/detectors/stt  
  ${CMAKE_SOURCE_DIR}/detectors/gem
  ${CMAKE_SOURCE_DIR}/detectors/scitil
  ${CMAKE_SOURCE_DIR}/tracking/riemannfit 
  ${CMAKE_SOURCE_DIR}/tracking/PndHelixPropagator
  ${CMAKE_SOURCE_DIR}/tracking/PndPropagator
  ${CMAKE_SOURCE_DIR}/tracking/PndSTE
)  

include_directories(${INCLUDE_DIRECTORIES})
include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${SIMPATH}/lib 
${GEANT4_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndSTETask.cxx
PndSTETrackBase.cxx
PndSTESettings.cxx
PndSTETrackCreator.cxx
PndSTEMvd.cxx
PndSTEMvdHitFinder.cxx
PndSTEMvdHelix.cxx
PndSTEMvdIdeal.cxx
PndSTEGem.cxx
PndSTEBTof.cxx
PndSTEAnalysisTask.cxx
PndSTEDraw.cxx
PndSTETools.cxx
)

set(LINKDEF PndSTE_LinkDef.h)
set(LIBRARY_NAME PndSTE)

set(DEPENDENCIES ${ROOT_LIBRARIES} Base GeoBase ParBase RecoTasks PndData TrkBase helixpropagator Field pndpropagator riemann
    FairLogger::FairLogger)

GENERATE_LIBRARY()
