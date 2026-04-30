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

//-----------------------------------------------------------
//-----------------------------------------------------------
//*******************************************************************************
// List of most recent changes:                                                 *
// 29-05-17  Correction of booking and saving of histograms (Irina Rostovtseva) *
//                                                                              *
//*******************************************************************************

#ifndef PndCATracking_HH
#define PndCATracking_HH

#include "PndPersistencyTask.h"

#include "TString.h"
#include "PndCAGBHit.h"

#include <vector>
#include <fstream>
#include <map>

using std::map;

class TClonesArray;
class PndCATrackParam;
class FairTrackParP;
class PndGeoHandling;
class PndCAPerformance;

class PndCATracking : public PndPersistencyTask {
 public:
  // Constructors/Destructors ---------
  PndCATracking(const char *name = "TrackingCA", Int_t iVerbose = 0);
  ~PndCATracking();

  /// Instance
  static PndCAPerformance &Instance();

  void SetMvdPixelHitsBranchName(const TString &name) { fMvdPixelHitsBranchName = name; }
  void SetMvdStripHitsBranchName(const TString &name) { fMvdStripHitsBranchName = name; }
  void SetSttHitsBranchName(const TString &name) { fSttHitsBranchName = name; }

  virtual InitStatus Init();
  void SetParContainers();
  virtual void Exec(Option_t *opt);
  virtual void Finish();

 private:
  TString fMvdPixelHitsBranchName;
  TString fMvdStripHitsBranchName;
  TString fMvdPixelClusterBranchName;
  TString fMvdStripClusterBranchName;
  TString fMvdPixelDigiBranchName;
  TString fMvdStripDigiBranchName;
  TString fSttHitsBranchName;

  TString fMvdPointsBranchName;
  TString fSttPointsBranchName;
  TString fMCTracksBranchName;

  // hits
  TClonesArray *fMvdPixelHitsArray;
  TClonesArray *fMvdStripHitsArray;
  TClonesArray *fMvdPixelClusterArray;
  TClonesArray *fMvdStripClusterArray;
  TClonesArray *fMvdPixelDigiArray;
  TClonesArray *fMvdStripDigiArray;
  TClonesArray *fSttHitsArray;

  TClonesArray *fMvdPointsArray; // MVD MC points
  TClonesArray *fSttPointsArray; // STT MC points
  TClonesArray *fMCTrackArray;

  // STT Map
  TClonesArray *fTubeArray;
  // for MVD geometry extraction
  PndGeoHandling *fGeoH;

  /** Output array of PndSttMvd   PndTrack **/
  TClonesArray *fSttMvdPndTrackArray;

#ifdef DO_TPCCATRACKER_EFF_PERFORMANCE
  PndCAPerformance *perf;
#endif

  bool fDoPerformance;
  void WriteMVDHits(std::vector<PndCAGBHit> &vHits, std::fstream &outH, std::fstream &outHL, std::fstream &outMCT, std::fstream &outMCP, int &iHit,
                    map<int, unsigned int> &nHitsInMCTrack, bool isPixel);

  static void CATrackParToFairTrackParP(FairTrackParP *fairParam, const PndCATrackParam *caParam);

  ClassDef(PndCATracking, 1);
};

#endif
