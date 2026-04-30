# Create a library called "libStt" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.


Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${BASE_INCLUDE_DIRECTORIES}
  ${GEANT3_INCLUDE_DIR}
  ${VMC_INCLUDE_DIRS}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/pnddata 
  ${CMAKE_SOURCE_DIR}/pnddata/SciTData
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData  
  ${CMAKE_SOURCE_DIR}/genfit2/core/include
  ${CMAKE_SOURCE_DIR}/genfit2/trackReps/include
  ${CMAKE_SOURCE_DIR}/tracking/GenfitTools/adapters2
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcData
  ${CMAKE_SOURCE_DIR}/tracking/propagation
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
PndPropagationTest.cxx
IdealTubeDetector.cxx
PndElectronCorrectionTest.cxx
)

set(LINKDEF  PropLinkDef.h)
set(LIBRARY_NAME Prop)

set(DEPENDENCIES Base FairTools MathCore Hist RIO Tree Gpad GeoBase ParBase PndData Passive ParBase ExMCStack Hist MathCore Gpad Graf Geane genfit2 genfitAdapters2 generalTools GeoBase ParBase PndData TrkBase Minuit  FairLogger ${VMCLIB})

GENERATE_LIBRARY()
