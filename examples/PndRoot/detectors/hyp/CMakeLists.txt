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
  ${CMAKE_SOURCE_DIR}/pnddata 
  ${CMAKE_SOURCE_DIR}/pnddata/HypData
  ${CMAKE_SOURCE_DIR}/pnddata/PidData
  ${CMAKE_SOURCE_DIR}/detectors/passive
  ${CMAKE_SOURCE_DIR}/tracking/riemannfit
  ${CMAKE_SOURCE_DIR}/tools
  ${CMAKE_SOURCE_DIR}/detectors/hyp
  ${CMAKE_SOURCE_DIR}/detectors/hyp/hypData
  ${CMAKE_SOURCE_DIR}/detectors/hyp/hypDigi
  ${CMAKE_SOURCE_DIR}/detectors/hyp/hypTools
  ${CMAKE_SOURCE_DIR}/detectors/hyp/hypReco
#${CMAKE_SOURCE_DIR}/detectors/hyp/hypTracking
#${CMAKE_SOURCE_DIR}/detectors/hyp/hypFullAna
#${CMAKE_SOURCE_DIR}/genfit
#${CMAKE_SOURCE_DIR}/recotasks
#${CMAKE_SOURCE_DIR}/trackbase
#${CMAKE_SOURCE_DIR}/tracking/GenfitTools/trackrep/GeaneTrackRep
#${CMAKE_SOURCE_DIR}/tracking/GenfitTools/trackrep/LSLtrackRep
#${CMAKE_SOURCE_DIR}/geane
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
PndGeoHyp.cxx 
PndHyp.cxx 
PndGeoHypPar.cxx 
PndHypHitProducerIdeal.cxx    
PndHypContFact.cxx 
THParticle.cxx 
#PndHypDecayer.cxx 
#PndHypDecayReader.cxx
#PndHypAnaIdeal.cxx  
HypStatDecay.cxx  

#hypData/PndHypCluster.cxx
#hypData/PndHypDigiPixel.cxx
#hypData/PndHypDigiStrip.cxx
#hypData/PndHypDigi.cxx
#hypData/PndHypClusterCand.cxx
#hypData/PndHypStripCluster.cxx
#hypData/PndHypRecoHit.cxx
#hypData/PndHypRecoSPHit.cxx

hypDigi/PndHypStripHitProducer.cxx
#hypDigi/PndHypPixelHitProducer.cxx
#hypDigi/PndHypCalcPixel.cxx
hypDigi/PndHypCalcStrip.cxx
#hypDigi/PndHypCalcFePixel.cxx
#hypDigi/PndHypGeoMappingPar.cxx
hypDigi/PndHypDigiPar.cxx
hypDigi/PndHypStripDigiPar.cxx

hypReco/PndHypIdealRecoTask.cxx
#hypReco/PndHypRecoTask.cxx
hypReco/PndHypStripClusterBuilder.cxx
hypReco/PndHypStripClusterTask.cxx

#hypFullAna/PndHypFullAna.cxx
#hypFullAna/PndHypSimpleAna.cxx
#hypFullAna/PndHypMicroWriter.cxx
#hypFullAna/PndHypFullIdealAna.cxx
#hypFullAna/PndHypMicroIdealWriter.cxx

hypTools/PndHypGeoHandling.cxx
hypTools/PndStringVector.cxx
#hypTools/PndHypMSAnaTask.cxx

#hypTracking/PndHypIdealTrackFinderTask.cxx
#hypTracking/PndHypRiemannTrackFinderTask.cxx
#hypTracking/PndHypIdealTrackingTask.cxx
#hypTracking/PndHypKalmanTask.cxx
#hypTracking/PndHypDKalmanTask.cxx
#hypTracking/PndHypDPatternRecoTask.cxx
#hypTracking/PndHypIdealPRTask.cxx
#hypTracking/PndFieldAdaptor.cxx

hypPID/PndHypAdvancedPidAlgo.cxx
hypPID/PndHypPidIdealTask.cxx

)

set(LINKDEF  HypLinkDef.h)
set(LIBRARY_NAME Hyp)

set(DEPENDENCIES Base GeoBase ParBase PndData TrkBase AnalysisTools
generalTools Geane RhoFitter DecayTreeFitter Rho PndData)

PANDA_GENERATE_LIBRARY()
