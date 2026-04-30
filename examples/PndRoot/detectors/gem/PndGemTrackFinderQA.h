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
// -----                   PndGemTrackFinderQA header file          -----
// -----                  Created 02.06.2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** \class PndGemTrackFinderQA
 *  \author R. Karabowicz <r.karabowicz@gsi.de>
 *  \date 19.03.2009
 *  \brief track finding quality assesment task
 *
 *  Track finding QA, efficiency, mom. resolution and so on
 **/

#ifndef PNDGEMTRACKFINDERQA_H
#define PNDGEMTRACKFINDERQA_H

#include "TH1F.h"
#include "TH2F.h"

#include "FairTask.h"
#include "PndMCTrack.h"

#include "PndGemHit.h"
#include "PndGemDigiPar.h"

#include <vector>

class PndGemTrackFinderQA : public FairTask {

 public:
  /** Default constructor **/
  PndGemTrackFinderQA();

  /** Default constructor **/
  PndGemTrackFinderQA(Int_t iVerbose);

  /** Destructor **/
  virtual ~PndGemTrackFinderQA();

  /** Execution **/
  virtual void Exec(Option_t *opt);

  /** Public modifiers **/
  void SetVerbose(const Int_t &verbose) { fVerbose = verbose; };

 private:
  PndGemDigiPar *fDigiPar;

  /** Arrays of MC information **/
  TClonesArray *fMCTrackArray;
  TClonesArray *fMCPointArray;

  TClonesArray *fGemHitArray;   //
  TClonesArray *fGemTrackArray; ///< Output array of PndGemTracks

  /** Array of GEM-related data **/
  TClonesArray *fGemData[10];
  Int_t fGemDataPointer[1000];
  Int_t fGemPointNumber;

  /** Event counter **/
  Int_t fNofEvents; ///< event counter

  TVector3 fTargetPos;
  std::vector<Int_t> fMCTrackNofCrossedGemStations;
  std::vector<Int_t> fMCTrackNofGemPoints;
  std::vector<Int_t> fRecoTrackMCMatch;
  Int_t fNeededStationsToRecoTrack;
  Double_t fMinQuota;

  Int_t fNofMCAll;
  Int_t fNofMCAcc;
  Int_t fNofMCPrim;
  Int_t fNofMCSec;
  Int_t fNofMCRef; // primary, mom.mag > 0.5GeV/c

  Int_t fNofRecoAcc;
  Int_t fNofRecoPrim;
  Int_t fNofRecoSec;
  Int_t fNofRecoRef; // primary, mom.mag > 0.5GeV/c

  Int_t fNofRecoGhosts;
  Int_t fNofRecoClones;

  TList *fHistoList;
  // number of mc tracks, reco tracks, efficiency as function of MOMENTUM
  TH1F *fhMCAllVsP, *fhMCAccVsP, *fhMCPrimVsP, *fhMCSecVsP, *fhMCRefVsP;
  TH1F *fhRecoAccVsP, *fhRecoPrimVsP, *fhRecoSecVsP, *fhRecoRefVsP;
  TH1F *fhEffAccVsP, *fhEffPrimVsP, *fhEffSecVsP, *fhEffRefVsP;
  // number of mc tracks, reco tracks, efficiency as function of THETA
  TH1F *fhMCAllVsT, *fhMCAccVsT, *fhMCPrimVsT, *fhMCSecVsT, *fhMCRefVsT;
  TH1F *fhRecoAccVsT, *fhRecoPrimVsT, *fhRecoSecVsT, *fhRecoRefVsT;
  TH1F *fhEffAccVsT, *fhEffPrimVsT, *fhEffSecVsT, *fhEffRefVsT;
  // number of mc tracks, reco tracks, efficiency as function of PHI
  TH1F *fhMCAllVsA, *fhMCAccVsA, *fhMCPrimVsA, *fhMCSecVsA, *fhMCRefVsA;
  TH1F *fhRecoAccVsA, *fhRecoPrimVsA, *fhRecoSecVsA, *fhRecoRefVsA;
  TH1F *fhEffAccVsA, *fhEffPrimVsA, *fhEffSecVsA, *fhEffRefVsA;
  // number of mc tracks, reco tracks, efficiency as function of NUMBER OF POINTS
  TH1F *fhMCAllVsN, *fhMCAccVsN, *fhMCPrimVsN, *fhMCSecVsN, *fhMCRefVsN;
  TH1F *fhRecoAccVsN, *fhRecoPrimVsN, *fhRecoSecVsN, *fhRecoRefVsN;
  TH1F *fhEffAccVsN, *fhEffPrimVsN, *fhEffSecVsN, *fhEffRefVsN;

  // MOMENTUM, THETA and (PHI)ANGLE reco tracks distribution
  TH1F *fhRecoAllP, *fhRecoPrimP, *fhRecoSecP;
  TH1F *fhRecoAllT, *fhRecoPrimT, *fhRecoSecT;
  TH1F *fhRecoAllA, *fhRecoPrimA, *fhRecoSecA;

  // momentum resolution vs MOMENTUM
  TH2F *fhMomResAccVsP, *fhMomResPrimVsP, *fhMomResSecVsP, *fhMomResRefVsP;
  // momentum resolution vs THETA
  TH2F *fhMomResAccVsT, *fhMomResPrimVsT, *fhMomResSecVsT, *fhMomResRefVsT;
  // momentum resolution vs THETA
  TH2F *fhMomResAccVsA, *fhMomResPrimVsA, *fhMomResSecVsA, *fhMomResRefVsA;

  TH1F *fhNofHitsPerTrack;
  TH1F *fhNofHitsPerRecoTrack;
  TH1F *fhNofHitsPerGhost;
  TH1F *fhNofHitsPerClone;

  TH1F *fhNofCorrHitsPerRecoTrack;
  TH1F *fhNofOthTHitsPerRecoTrack;
  TH1F *fhNofNoTrHitsPerRecoTrack;

  TH1F *fhNofMCTracksPerEvent, *fhNofRecoTracksPerEvent;

  void CreateHistos();
  void PrepareMCTracks();
  void MatchRecoTracks();

  /** Get parameter containers **/
  virtual void SetParContainers();

  void DivideHistos(TH1 *hist1, TH1 *hist2, TH1 *hist3);

  Int_t GetPointVector(Int_t arrayId, Int_t entryId, std::vector<Int_t> &pointVector, Bool_t printInfo = kFALSE);
  Int_t FindMatchingPoint(Int_t gemHitIndex);

  /** Finish **/
  virtual void Finish();

  /** Intialisation **/
  virtual InitStatus Init();

  /** Reinitialisation **/
  virtual InitStatus ReInit();

  ClassDef(PndGemTrackFinderQA, 1);
};

#endif
