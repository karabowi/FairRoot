#---------------------------------------------------------------------
# Author: Mustafa Schmidt (Mustafa.A.Schmidt@physik.uni-giessen.de)
# Changes:
# Date: 30.09.2015
# Description: CMakeLists.txt for Panda Disc DIRC
#---------------------------------------------------------------------

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${BASE_INCLUDE_DIRECTORIES}
  ${GEANT3_INCLUDE_DIR}
  ${VMC_INCLUDE_DIRS}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/pnddata 
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/pnddata/DiscData
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/detectors/disc
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${SIMPATH}/lib
)
 
link_directories( ${LINK_DIRECTORIES} )

set(SRCS
  PndDisc.cxx
  PndDiscNoiseGeneration.cxx
  PndDiscPhotodetector.cxx
  PndDiscTaskDigitization.cxx
  PndDiscTaskReconstruction.cxx
  PndDiscTaskPID.cxx
  PndDiscWriteoutBuffer.cxx
  PndDiscSensorGrid.cxx
  PndDiscSensorGridPhotodetector.cxx
)

set(LINKDEF DiscLinkDef.h)

set(LIBRARY_NAME Disc)

set(DEPENDENCIES Base PndData GeoBase ParBase Passive Geane MathCore MathMore FairTools)

PANDA_GENERATE_LIBRARY()
