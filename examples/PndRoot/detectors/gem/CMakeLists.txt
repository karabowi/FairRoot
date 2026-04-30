################################################################################
# Copyright (C) 2014-2022 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH  #
#                                                                              #
#              This software is distributed under the terms of the             #
#              GNU Lesser General Public Licence (LGPL) version 3,             #
#                  copied verbatim in the file "LICENSE"                       #
################################################################################

set(target Gem)

set(sources
  PndGemContFact.cxx
  PndGemDetector.cxx
  PndGemDigiAna.cxx
  PndGemDigiPar.cxx
  PndGemDigiRingSorter.cxx
  PndGemDigiSorterTask.cxx
  PndGemDigiWriteoutBuffer.cxx
  PndGemDigitize.cxx
  PndGemEmpty.cxx
  #PndGemEventBuilderOnTracks.cxx
  PndGemFindClusters.cxx
  PndGemFindClustersTB.cxx
  PndGemFindHits.cxx
  PndGemFindHitsQA.cxx
  PndGemFindHitsAna.cxx
  PndGemFindTracks.cxx
  PndGemGeo.cxx
  PndGemGeoPar.cxx
  PndGemIdealHitProducer.cxx
  PndGemIdealPatternRecoTask.cxx
  PndGemMagneticFieldVsTrackParameters.cxx
  PndGemMatchHits.cxx
  PndGemMonitor.cxx
  PndGemSensor.cxx
  PndGemSensorMonitor.cxx
  PndGemSmearingTask.cxx
  PndGemStation.cxx
  PndGemTrackFinder.cxx
  PndGemTrackFinderIdeal.cxx
  PndGemTrackFinderOnHits.cxx
  PndGemTrackFinderOnHitsTB.cxx
  PndGemTrackFinderQA.cxx
  #PndGemTrackWriteoutBuffer.cxx
)

fair_change_extensions_if_exists(.cxx .h FILES "${sources}" OUTVAR headers)


add_library(${target} SHARED ${sources} ${headers})
add_library(PndRoot::${target} ALIAS ${target})
#set_target_properties(${target} PROPERTIES ${PROJECT_LIBRARY_PROPERTIES})

target_include_directories(${target} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  ${CMAKE_SOURCE_DIR}/examples/PndRoot/pnddata/GemData
)

target_link_libraries(${target}
  PUBLIC
    FairRoot::Base
    FairRoot::ParBase
    FairRoot::TrackBase
    FairRoot::Tools     # FairTools
    PndRoot::PndData

    ROOT::MathCore
    ROOT::RIO
    ROOT::Hist

  PRIVATE
    FairLogger::FairLogger
    ${VMCLIB}
)

fairroot_target_root_dictionary(${target}
  HEADERS ${headers}
  LINKDEF GemLinkDef.h
  EXTRA_INCLUDE_DIRS ${CMAKE_INSTALL_FULL_INCLUDEDIR}
)

install(TARGETS ${target} LIBRARY DESTINATION ${PROJECT_INSTALL_LIBDIR})
install(FILES ${headers} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
