# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${BASE_INCLUDE_DIRECTORIES} 
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/field 
  ${CMAKE_SOURCE_DIR}/pnddata 
  ${CMAKE_SOURCE_DIR}/detectors/hypGe
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndGeoHypGe.cxx 
PndHypGe.cxx 
PndGeoHypGePar.cxx 
PndHypGePoint.cxx 
PndHypGeHit.cxx 
PndHypGeHitProducerIdeal.cxx 
PndHypGeContFact.cxx 
PndHypGeReader.cxx 
GeCluster.cxx
PndHypGeIdealTrackFinderTask.cxx
geometry/PndGeoHypGeCrystal.cxx
geometry/PndGeoHypGeCluster.cxx
geometry/PndGeoHypGeSingleCluster.cxx
geometry/PndGeoHypGeTripleCluster.cxx
geometry/PndGeoHypGeDoubleCluster.cxx
geometry/PndGeoHypGeModifiedCTFrame.cxx
geometry/PndGeoHypGeModifiedBeamPipe.cxx
geometry/PndGeoHypGeBase.cxx
geometry/PndGeoHypGeTripleClusterCrystalsOnly.cxx
geometry/PndGeoHypGeCrystalWithoutCapsule.cxx
geometry/PndGeoHypGeDEGAS.cxx
hypGeAna/PndHypGeGammaAna.cxx
hypGeAna/PndHypGeGammaAnaStorage.cxx
#hypGeAna/PndHypGeCOSYBackgroundAna.cxx
#hypGeSpecAnalyser/PndHypGeSpectrumAnalyser.cxx
#hypGeSpecAnalyser/PndHypGePeakFitFunction.cxx
)

set(LINKDEF  HypGeLinkDef.h)
set(LIBRARY_NAME HypGe)

set(DEPENDENCIES Base GeoBase ParBase PndData)

PANDA_GENERATE_LIBRARY()
install(FILES geometry/PndGeoHypGeDEGASParfile.txt geometry/PndGeoHypGeDoubleClusterParfile.txt geometry/PndGeoHypGeTripleClusterParfile.txt
        DESTINATION detectors/hypGe/geometry
        )
