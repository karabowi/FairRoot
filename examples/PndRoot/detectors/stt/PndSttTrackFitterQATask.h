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

#ifndef PNDSTTTRACKFITTERQATASK_H
#define PNDSTTTRACKFITTERQATASK_H 1

#include "FairTask.h"
#include "PndSttHelixHit.h"
#include "PndGeoSttPar.h"

#include "TVector3.h"
#include "TH1F.h"
#include "TH2F.h"

class TClonesArray;
class TObjectArray;

class PndSttTrackFitterQATask : public FairTask {

 public:
  /** Default constructor **/
  PndSttTrackFitterQATask();

  PndSttTrackFitterQATask(Int_t verbose);

  /** Destructor **/
  ~PndSttTrackFitterQATask();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  void WriteHistograms();

  void SetParContainers();

 private:
  /** Input array of PndSttPoints **/
  TClonesArray *fPointArray;
  /** Input array of PndSttHits **/
  TClonesArray *fHitArray;
  /** Input array of PndSttTracks **/
  TClonesArray *fTrackArray;
  /** Input array of PndTracks **/
  TClonesArray *fFoundTrackArray;
  /** Input array of PndTrackCands **/
  TClonesArray *fTrackCandArray;
  /** Input array of MCTracks **/
  TClonesArray *fMCTrackArray;
  /** Output array of PndSttHelixHits **/
  TClonesArray *fHelixHitArray;

  /** from parameters array of PndSttTube **/ //  CHECK added
  TClonesArray *fTubeArray;

  // histograms
  TH1F *hptfit, *hplfit, *hptotfit, *hptfound, *hplfound, *hptotfound, *hptfit_perc, *hplfit_perc, *hptotfit_perc, *hptfound_perc, *hplfound_perc, *hptotfound_perc, *hresx, *hresy,
    *hresz, *hx, *hy, *hz, *hxs, *hys, *hzs, *hDist, *hRad, *hPhi, *hTanL, *hZ, *hpxfit, *hpyfit, *hpzfit, *hQ;

  PndGeoSttPar *fSttParameters; //  CHECK added

  ClassDef(PndSttTrackFitterQATask, 1);
};

#endif
