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

/** PndSttTrackFitter
 *@author R.Castelijns <r.castelijns@fz-juelich.de>
 **
 ** Abstract base class for concrete STT track fitting algorithm.
 ** Each derived class must implement the method DoFit. This has
 ** to operate on an object of type PndSttTrack and fill the
 ** parameters fPidHypo, fParamFirst, fParamLast, fFlag and fChi2.
 **/

#ifndef PNDSTTHELIXTRACKFITTER
#define PNDSTTHELIXTRACKFITTER 1

#include "PndSttTrack.h"
//#include "PndTrackCand.h"
//#include "PndSttPoint.h"
//#include "FairTrackParam.h"
#include "PndSttTrackFitter.h"

//#include "TH2F.h"
//#include "TCanvas.h"
#include "TList.h"

#include <map>

class FairTrackParam;
class PndSttHit;
// class PndSttTrackFitter;
class TCanvas;
class TH2F;
class TH1F;

void fcnHelix(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);
void fcnHelix2(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);

class PndSttHelixTrackFitter : public PndSttTrackFitter {
 private:
  Int_t fEventCounter;

  PndSttTrack *fTrack;
  PndTrackCand *fTrackCand;
  PndSttTrack currentTrack;

  TClonesArray *fHitArray;
  TClonesArray *fPointArray;
  TObjArray *ZPointsArray;

  TH2F *h1, *h2, *h3, *h4, *houg;
  TH1F *hougcon;
  TCanvas *eventCanvas, *eventDetails;
  Bool_t rootoutput;
  Int_t fVerbose;

 public:
  PndSttHelixTrackFitter();
  PndSttHelixTrackFitter(Int_t verbose);
  ~PndSttHelixTrackFitter();
  void Init();

  // xy plane ----------------------
  // intersection finder
  Bool_t IntersectionFinder(PndTrackCand *pTrackCand);
  // fit
  Int_t XYFit(PndTrackCand *pTrackCand, Int_t whatToFit);
  Int_t XYFitThroughOrigin(PndTrackCand *pTrackCand, Int_t whatToFit);
  Int_t MinuitFit(PndTrackCand *pTrackCand, Int_t whatToFit);
  Int_t SetUpFitVector(PndTrackCand *pTrackCand, TMatrixT<Double32_t> &fitvect);

  // z track length plane ----------
  // zfinder
  Bool_t ZFinder(PndTrackCand *pTrackCand, Int_t whatToFit);
  Bool_t ZFinderThroughOrigin(PndTrackCand *pTrackCand, Int_t whatToFit);

  // hough
  void Hough(TVector3 *choice, Double_t Phi0, Double_t x0, Double_t y0, Double_t R);
  TVector3 GetHoughResponse();
  void HoughThroughOrigin(TVector3 *choice, Double_t Phi0, Double_t x0, Double_t y0, Double_t R);
  TVector3 GetHoughResponseThroughOrigin();

  // zfit
  Int_t ZFit(PndTrackCand *pTrackCand, Int_t whatToFit);
  Int_t ZFitThroughOrigin(PndTrackCand *pTrackCand, Int_t whatToFit);
  TVector3 FindCorrectZ(TObjArray *choices, Double_t x_0, Double_t y_0, Double_t x0, Double_t y0, Double_t R);
  Int_t DoFit(PndTrackCand *pTrackCand, PndSttTrack *pTrack, Int_t pidHypo = 211);
  // plain = with no constraint
  Int_t DoFitPlain(PndTrackCand *pTrackCand, PndSttTrack *pTrack, Int_t pidHypo = 211);
  // through origin = force the track to pass in 0, 0, 0.
  //  Int_t DoFitThroughOrigin(PndTrackCand* pTrackCand, PndSttTrack* pTrack, Int_t pidHypo = 211);

  // charge reconstruction from xy fit
  Int_t GetCharge(Double_t dCenter, Double_t phiCenter, Double_t radius);
  void OrderHitsByR(std::map<Double_t, Int_t> &hitMap);
  Double_t GetHitAngle(Int_t hitNo, Double_t dCenter, Double_t phiCenter, Double_t radius);

  PndSttHit *GetHitFromCollections(Int_t hitCounter) const;
  TList fHitCollectionList;
  virtual void AddHitCollection(TClonesArray *mHitArray) { fHitCollectionList.Add(mHitArray); }

  virtual void Extrapolate(PndSttTrack *track, Double_t r, FairTrackParam *param);

  // all: kTRUE = also z param; kFALSE = only xy param
  // void SetParameters(PndSttTrack* pTrack, Bool_t all); /* commented out by JGM, 16/01/2010 */

  // marray reset
  void ResetMArray();

  PndSttTrack *GetTrack() const { return fTrack; };
  PndTrackCand *GetTrackCand() const { return fTrackCand; };
  TClonesArray *GetHitArray() const { return fHitArray; };

  void SetConstraint(Int_t con) { fConstraint = con; };
  Int_t GetConstraint() { return fConstraint; };

  Double_t refAngle;

  // 0 = no contraint, 1 = the track passes through (0, 0, 0)
  Int_t fConstraint;

  TClonesArray *fTubeArray;
  void SetTubeArray(TClonesArray *tubeArray) { fTubeArray = tubeArray; };

  Int_t fDisplayLevel;
  void SetDisplayLevel(Int_t display = 2) { fDisplayLevel = display; };
  void InitEventDisplay();
  Bool_t RunEventDisplay(PndTrackCand *trackCand);
  void FinishEventDisplay(PndSttTrack *track);

  ClassDef(PndSttHelixTrackFitter, 1);
};

#endif
