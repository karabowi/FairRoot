//****************************************************************************
//*                   This file is part of PandaRoot.                        *
//*                                                                          *
//*            PandaRoot is distributed under the terms of the               *
//*              GNU General Public License (GPL) version 3,                 *
//*                 copied verbatim in the file "LICENSE".                   *
//*                                                                          *
//*  Copyright (C) 2006 - 2024 FAIR GmbH and copyright holders of PandaRoot  *
//*     The copyright holders are listed in the file "COPYRIGHTHOLDERS".     *
//*               The authors are listed in the file "AUTHORS".              *
//****************************************************************************

// -------------------------------------------------------------------------
// -----                  PndDetectorList.header file                  -----
// -----                 Created 11/02/09  by M. Al-Turany                -----
// -------------------------------------------------------------------------

/** Unique identifier for all Panda detector systems **/

#ifndef PNDDETECTORLIST_H
#define PNDDETECTORLIST_H 1

enum class PndDetectorId  {
  /** kRICH must be the 1st id, and kHYP must be the last one. Please put new detectors in between!! **/
  kRICH,
  kDRC,
  kDSK,
  kEMC,
  kGEM,
  kLUMI,
  kMDT,
  kMVD,
  kRPC,
  kSTT,
  kSTT2,
  kFTOF,
  kTOF,
  kFTS,
  kHYPG,
  kOT,
  kHYP
};

/** Beware! each new detector should be added also in PndMCTrack **/

/** Unique identifier for all Panda Point and Hit types **/

enum class DetectorType {
  kUnknown,
  kMCTrack,
  kMVDPoint,
  kMVDDigiStrip,
  kMVDDigiPixel,
  kMVDClusterPixel,
  kMVDClusterStrip,
  kMVDHitsStrip,
  kMVDHitsPixel,
  kSttPoint,
  kStt2Point,
  kSttHit,
  kStt2Hit,
  kSttHelixHit,
  kSttTrackCand,
  kSttTrack,
  kGemPoint,
  kGemDigi,
  kGemCluster,
  kGemHit,
  kFtsPoint,
  kFtsDigi,
  kFtsHit,
  kOtPoint,
  kOtDigi,
  kOtHit,
  kMdtPoint,
  kMdtHit,
  kMdtTrack,
  kEmcPoint,
  kEmcHit,
  kEmcDigi,
  kEmcCluster,
  kEmcBump,
  kEmcRecoHit,
  kLheTrack,
  kTrackCand,
  kTrack,
  kPidChargedCandidate,
  kPidNeutralCandidate
};

enum class SensorSide { kTOP, kBOTTOM };

#endif
