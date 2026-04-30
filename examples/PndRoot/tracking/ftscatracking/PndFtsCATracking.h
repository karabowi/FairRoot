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

#ifndef PNDFTSCATRACKING_HH
#define PNDFTSCATRACKING_HH

#include "PndPersistencyTask.h"
#include "FairTrackParP.h"

#include "TString.h"
#include "PndFTSCAGBHit.h"

#include <vector>
#include <fstream>
#include <map>

using std::map;

class TClonesArray;
class FairTrackParP;
class PndGeoHandling;
class PndFTSCATrackParam;
class TFile;
class PndFTSCAPerformance;
class PndFTSCAGBTracker;

class PndFtsCATracking : public PndPersistencyTask {
 public:
  // Constructors/Destructors ---------
  PndFtsCATracking(const char *name = "FtsCATracking", Int_t iVerbose = 0);
  ~PndFtsCATracking();
  void SetFtsHitsBranchName(const TString &name) { fFtsHitsBranchName = name; }
  void SetFtsTracksBranchName(const TString &name) { fFtsTracksBranchName = name; }

  virtual InitStatus Init();
  void SetParContainers();
  virtual void Exec(Option_t *opt);
  virtual void Finish();

  static bool CATrackParToFairTrackParP(FairTrackParP *fairParam, const PndFTSCATrackParam *caParam);

  bool NonReconstructableEvent();

 private:
  static Int_t fVerbose;
  TString fFtsHitsBranchName;
  TString fFtsTracksBranchName;
  TString fFtsTrackCandsBranchName;
  Int_t fBranchID;
  // TClonesArray*  fMCTracks;      //! Array of PndMCTrack
  // TClonesArray*  fMCPoints;      //! Array of event's points
  TClonesArray *fHits; //! Array of event's hits

  TClonesArray *fTracks;     //! Array of found tracks
  TClonesArray *fTrackCands; //! Array of found track hit collection

  bool fDoPerformance;
  // PndGeoFtsPar *fFtsParameters;
  TClonesArray *fTubeArrayFts;

  void WriteFTSHits(/*PndFTSCAGBHit* vHits,*/ std::vector<PndFTSCAGBHit> &vHits,
                    /*std::fstream &outH, std::fstream &outHL, std::fstream &outMCT, std::fstream &outMCP,*/ int &iHit /*, map<int, unsigned int> &nHitsInMCTrack*/);

  PndFTSCAGBTracker *fTracker;

  // for performance
  TFile *fPerfHistoFile;
  PndFTSCAPerformance *fPerformance;

  ClassDef(PndFtsCATracking, 1);
};
#endif
