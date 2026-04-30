################################################################################
# Copyright (C) 2014-2022 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH  #
#                                                                              #
#              This software is distributed under the terms of the             #
#              GNU Lesser General Public Licence (LGPL) version 3,             #
#                  copied verbatim in the file "LICENSE"                       #
################################################################################

set(target PndData)

set(sources
  PndStack.cxx
  PndMCTrack.cxx
  PndMCPoint.cxx

  DskData/PndDskCerenkov.cxx
  DskData/PndDskParticle.cxx
  DskData/PndDskTrackPoint.cxx
  DskData/PndDskFLGHit.cxx

  DiscData/PndDiscDigitizedHit.cxx
  DiscData/PndDiscParticleMCPoint.cxx
  DiscData/PndDiscSensorMCPoint.cxx
  DiscData/PndDiscReconResult.cxx
  DiscData/PndDiscPID.cxx

  DrcData/PndDrcEVPoint.cxx
  DrcData/PndDrcPDPoint.cxx
  DrcData/PndDrcBarPoint.cxx
  DrcData/PndDrcHit.cxx
  DrcData/PndDrcPDHit.cxx
  DrcData/PndDrcDigi.cxx
  DrcData/PndDrcLutNode.cxx
  DrcData/PndDrcLutInfo.cxx
  DrcData/PndDrcTrackInfo.cxx
  DrcData/PndDrcPhotonInfo.cxx
  DrcData/PndDrcAmbiguityInfo.cxx

  GemData/PndGemMCPoint.cxx
  GemData/PndGemDigi.cxx
  GemData/PndGemCluster.cxx
  GemData/PndGemHit.cxx

  MdtData/PndMdtPoint.cxx
  MdtData/PndMdtDigi.cxx
  MdtData/PndMdtCluster.cxx
  MdtData/PndMdtHit.cxx
  MdtData/PndMdtTrk.cxx
  MdtData/PndMdtID.cxx

  SttData/PndSttGeomPoint.cxx
  SttData/PndSttHelixHit.cxx
  SttData/PndSttHit.cxx
  SttData/PndSttSkewedHit.cxx
  SttData/PndSttHitInfo.cxx
  SttData/PndSttPoint.cxx
  SttData/PndSttTrack.cxx
  SttData/PndStt2Point.cxx
  SttData/PndStt2Digi.cxx
  SttData/PndStt2Hit.cxx

  FtsData/PndFtsPoint.cxx
  FtsData/PndFtsHit.cxx

  HypData/PndHypCluster.cxx
  HypData/PndHypDigi.cxx
  HypData/PndHypDigiStrip.cxx
  HypData/PndHypHit.cxx
  HypData/PndHypHitInfo.cxx
  HypData/PndHypPidCand.cxx
  HypData/PndHypPoint.cxx

  SdsData/PndSdsMCPoint.cxx
  SdsData/PndSdsDigi.cxx
  SdsData/PndSdsPixel.cxx
  SdsData/PndSdsDigiPixel.cxx
  SdsData/PndSdsDigiTopix4.cxx
  SdsData/PndSdsDigiTopix4Header.cxx
  SdsData/PndSdsStrip.cxx
  SdsData/PndSdsDigiStrip.cxx
  SdsData/PndSdsCluster.cxx
  SdsData/PndSdsClusterPixel.cxx
  SdsData/PndSdsClusterStrip.cxx
  SdsData/PndSdsHit.cxx
  #SdsData/PndSdsHitInfo.cxx
  SdsData/PndSdsPidCand.cxx
  SdsData/PndSdsRadDamHit.cxx
  SdsData/PndSdsDigiPixelMCInfo.cxx

  RichData/PndRichPhoton.cxx
  RichData/PndRichPDPoint.cxx
  RichData/PndRichBarPoint.cxx
  RichData/PndRichDigi.cxx
  RichData/PndRichHit.cxx
  RichData/PndRichPDHit.cxx
  RichData/PndRichMirrorSegment.cxx

  MvdData/PndMvdApvHit.cxx
  MvdData/PndMvdPidCand.cxx
  MvdData/PndMvdRadDamHit.cxx

  OtData/PndOtPoint.cxx
  OtData/PndOtHit.cxx

  SciTData/PndSciTPoint.cxx
  SciTData/PndSciTHit.cxx

  TrackData/PndTrackCandHit.cxx
  TrackData/PndTrackCand.cxx
  TrackData/PndTrack.cxx
  TrackData/PndSimpleTrack.cxx
  TrackData/PndTrackID.cxx
  #TrackData/PndVertex.cxx

  PidData/PndPidCandidate.cxx
  PidData/PndPidProbability.cxx
  PidData/PndPidBremCorrected4Mom.cxx

  EmcData/BSEmcMCPoint.cxx
  EmcData/BSEmcMCDeposit.cxx
  EmcData/BSEmcDigi.cxx
  EmcData/BSEmcCluster.cxx
  EmcData/BSEmcSubCluster.cxx
  EmcData/BSEmcRecoHit.cxx
  EmcData/BSEmcPrecluster.cxx
  EmcData/BSEmcWaveform.cxx
  EmcData/BSEmcMultiWaveform.cxx

  PndPersistencyTask.cxx
)

fair_change_extensions_if_exists(.cxx .h FILES "${sources}" OUTVAR headers)

add_library(${target} SHARED ${sources} ${headers})
add_library(PndRoot::${target} ALIAS ${target})
set_target_properties(${target} PROPERTIES ${PROJECT_LIBRARY_PROPERTIES})

target_include_directories(${target} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/DrcData>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/DskData>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/DiscData>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/FtsData>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/GemData>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/HypData>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/HypGData>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/MdtData>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/MvdData>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/OtData>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PidData>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/RichData>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/SciTData>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/SdsData>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/SttData>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scitil>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/TrackData>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/EmcData>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(${target}
  PUBLIC
    FairRoot::Base      # FairRunIdGenerator, etc.
    FairRoot::GeoBase   # FairGeoSet
    FairRoot::ParBase   # FairRuntimeDb
    FairRoot::TrackBase # track base classes

    ROOT::Core          # Rtypes
    ROOT::Matrix
    ROOT::Graf3d
    ROOT::Hist
    ROOT::Physics       # TVector3, TLorentzVector

  PRIVATE
    FairLogger::FairLogger
    ${VMCLIB}
)

fairroot_target_root_dictionary(${target}
  HEADERS ${headers}
  LINKDEF PndDataLinkDef.h
  EXTRA_INCLUDE_DIRS ${CMAKE_INSTALL_FULL_INCLUDEDIR}
)

install(TARGETS ${target} LIBRARY DESTINATION ${PROJECT_INSTALL_LIBDIR})
install(FILES ${headers} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})


