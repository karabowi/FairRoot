################################################################################
# Copyright (C) 2014-2022 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH  #
#                                                                              #
#              This software is distributed under the terms of the             #
#              GNU Lesser General Public Licence (LGPL) version 3,             #
#                  copied verbatim in the file "LICENSE"                       #
################################################################################

set(target Emc)

set(sources
EmcData/PndEmcPoint.cxx
EmcData/PndEmcApdPoint.cxx
EmcData/PndEmcHit.cxx  
#EmcData/PndEmcApdHit.cxx
#EmcData/PndEmcWaveformData.cxx 
#EmcData/PndEmcWaveform.cxx 
#EmcData/PndEmcMultiWaveform.cxx 
EmcData/PndEmcDigi.cxx 
EmcData/PndEmcCluster.cxx 
#EmcData/PndEmcPrecluster.cxx #
#EmcData/PndEmcSharedDigi.cxx
#EmcData/PndEmcBump.cxx
EmcData/PndEmcHeader.cxx
#EmcData/PndEmcRecoHit.cxx
#EmcData/PndEmcClusterProperties.cxx
#EmcData/PndEmcAbsClusterProperty.cxx
#EmcData/PndEmcClusterDistances.cxx
#EmcData/PndEmcClusterEnergySums.cxx
#EmcData/PndEmcClusterMoments.cxx
#EmcData/PndEmcXClMoments.cxx
EmcMC/PndGeoEmc.cxx 
EmcMC/PndEmc.cxx 
EmcMC/PndEmcReader.cxx 
EmcMC/PndGeoEmcApd.cxx 
EmcMC/PndEmcApd.cxx
EmcMC/PndEmcGeoPar.cxx
EmcTools/PndEmcTwoCoordIndex.cxx 
EmcTools/PndEmcStructure.cxx 
EmcTools/PndEmcMapper.cxx 
EmcTools/PndEmcXtal.cxx 
EmcTools/PndEmcHdrFiller.cxx
#EmcTools/PndEmcViewClusters.cxx #
#EmcTools/PndEmcInspectClusters.cxx #
)

fair_change_extensions_if_exists(.cxx .h FILES "${sources}" OUTVAR headers)


add_library(${target} SHARED ${sources} ${headers})
add_library(PndRoot::${target} ALIAS ${target})
#set_target_properties(${target} PROPERTIES ${PROJECT_LIBRARY_PROPERTIES})

target_include_directories(${target} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  ${CMAKE_SOURCE_DIR}/examples/PndRoot/tools
  ${CMAKE_SOURCE_DIR}/examples/PndRoot/detectors/emc
  ${CMAKE_SOURCE_DIR}/examples/PndRoot/detectors/emc/EmcData
  ${CMAKE_SOURCE_DIR}/examples/PndRoot/detectors/emc/EmcMC
  ${CMAKE_SOURCE_DIR}/examples/PndRoot/detectors/emc/EmcTools
)

target_link_libraries(${target}
  PUBLIC
    ROOT::MathCore
    ROOT::RIO
    ROOT::Hist

    FairRoot::Base
    FairRoot::ParBase
    FairRoot::TrackBase
    FairRoot::Tools     # FairTools

    PndRoot::PndData
    PndRoot::generalTools

  PRIVATE
    FairLogger::FairLogger
    ${VMCLIB}
)

fairroot_target_root_dictionary(${target}
  HEADERS ${headers}
  LINKDEF EmcLinkDef.h
  EXTRA_INCLUDE_DIRS ${CMAKE_INSTALL_FULL_INCLUDEDIR}
)

install(TARGETS ${target} LIBRARY DESTINATION ${PROJECT_INSTALL_LIBDIR})
install(FILES ${headers} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
