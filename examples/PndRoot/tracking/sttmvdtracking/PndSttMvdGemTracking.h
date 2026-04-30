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

#ifndef PNDSTTMVDGEMTRACKING_H
#define PNDSTTMVDGEMTRACKING_H 1

#include "PndGeoSttPar.h"
#include "PndGemDigiPar.h"
#include "PndGemHit.h"
#include "PndSttHit.h"
#include "PndTrackCand.h"
#include "PndTrack.h"
#include "PndPersistencyTask.h"
#include "FairGeanePro.h"

#include "TCanvas.h"
#include "TH2F.h"

class TClonesArray;

class PndSttMvdGemTracking : public PndPersistencyTask {

 public:
  /** Default constructor **/
  PndSttMvdGemTracking();

  PndSttMvdGemTracking(Int_t verbose);

  /** Destructor **/
  ~PndSttMvdGemTracking();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  /** set persistence flag **/
  void SetPersistenc(Bool_t persistence) { SetPersistency(persistence); }

  virtual void SetParContainers();

  // -------- MVD + STT ---> + GEM --------------
  void Copy(PndTrackCand *completeCand, PndTrack *completeTrack, PndTrackCand *sttmvdCand, PndTrack *sttmvd);
  void SetTimes(Int_t times) { fTimes = times; }
  void SetMaxDistance(Double_t maxdistance) { fMaxDistance = maxdistance; }
  void SwitchOnDisplay() { fDisplayOn = true; }

  void SetupGEMPlanes();
  void Reset(Int_t nhits, Int_t ntracks);
  void OrderGemHits(Int_t nhits);

  Int_t GetTrackIndex(Int_t i);
  Int_t GetHitIndex(Int_t i);
  Int_t GetPosIndex(PndGemHit *hit);

  Int_t CountTracks();
  Int_t CountHitsInTrack(Int_t itrk);
  void AddHitToTrack(Int_t ihit, Int_t itrk);
  void DeleteHitFromTrack(Int_t ihit, Int_t itrk);
  std::vector<int> GetTracksAssociatedToHit(Int_t ihit);
  std::vector<int> GetHitsAssociatedToTrack(Int_t itrk);
  std::vector<int> GetHitsAssociatedToTrackOnPlane(Int_t itrk, Int_t ipos);
  void AddRemainingHits(Int_t ntracks);

  void CheckCombinatorial(Int_t nhits, Int_t ntracks);
  void ForbidMultiAssignedHits(Int_t nhits, Int_t ntracks);
  void OnlyOneHitToEachTrack(Int_t nhits, Int_t ntracks);
  void Retrack();

  Bool_t PropagateToGemPlane(FairTrackParP *tmppar, FairTrackParP *gempar, Int_t ipos);
  Bool_t PropagateToGemPlaneAsHelix(PndTrack *sttmvd, FairTrackParP *gempar, Int_t ipos);

  Double_t IsAssignable(FairTrackParP *gempar, PndGemHit *gemhit);
  std::vector<int> AssignHits(Int_t itrk, FairTrackParP *gempar, Int_t ipos);
  void EvaluatePerformances(Int_t nhits, Int_t ntracks);
  void FillTrueDistances();
  // -----------------------------------------------
  Int_t SelectPdgCode(PndTrack *sttmvd);

  void Kalman(TMatrixT<double> extrap, TMatrixT<double> measurement, TMatrixT<double> H, TMatrixT<double> extrap_cov, TMatrixT<double> measurement_cov, TMatrixT<double> &kalman,
              TMatrixT<double> &kalman_cov);
  FairTrackParP SetStartParameters(PndTrack *sttmvd, PndTrackCand *sttmvdCand);

  Int_t GetClosestOnFirst(FairTrackParP *gempar, Int_t ipos, Double_t &closestdistance);

  // PREFIT -----------------------------
  Bool_t Prefit(PndTrack *sttmvdTrack, PndTrackCand *sttmvdCand, TVector3 &lastpos, TVector3 &lastmom);
  Bool_t IntersectionFinder(Double_t xc, Double_t yc, Double_t radius, PndSttHit *stthit, TVector3 &xyz, TVector3 &dxyz);
  Bool_t Fit(TMatrixT<double> points, Double_t &outxc, Double_t &outyc, Double_t &outradius);
  Bool_t ZFit(TMatrixT<double> points, Int_t charge, Double_t xc, Double_t yc, Double_t radius, Double_t &fitm, Double_t &fip);
  Bool_t GetInitialParams(PndTrack *sttmvd, Double_t &xc, Double_t &yc, Double_t &radius, Double_t &fitm, Double_t &fitp);
  Double_t CalculatePhi(TVector2 v, TVector2 p, double alpha, double Phi0, int charge);
  Double_t CompareToPreviousPhi(Double_t Fi, Double_t Fi_pre, int charge);
  Bool_t ZFind(Int_t nhits, TMatrixT<double> points, Double_t xc, Double_t yc, Double_t radius);

  // COMBINATORIAL EFFECS ----------------
  void ConsiderCombinatorialEffect(Int_t nhits);
  void SetCombinatorialDistance(Double_t combidistance) { fCombiDistance = combidistance; }
  void UpdateMCTrackId(PndTrackCand *completeCand);

  // CHECK delete this when everything is ok
  void UseMonteCarlo() { fUseMC = kTRUE; }
  void WriteHistograms();

  void SetEvaluateFlag(Bool_t flag) { fEvaluate = flag; }

  // PDG stuff
  Int_t GetPdgFromMC(int trackid);
  Int_t GetChargeCorrectedPdgFromMC(int trackid, int charge);
  void SetPdgFromMC() { fPdgFromMC = kTRUE; }
  void SetPdgFromMC(TString trackid)
  {
    fStartTrackIDBranchName = trackid;
    fPdgFromMC = kTRUE;
  }
  void SetDefaultPdg(int pdg) { fDefaultPdgCode = pdg; }

  void SetBranchNames(TString mvdpixel, TString mvdstrip, TString stt, TString gem);
  void SetTrackBranchNames(TString startingtrack, TString startingtrackcand);

 private:
  /** Input array of GEMHits **/
  TClonesArray *fGemHitArray;
  /** Input array of GEMPoints **/
  TClonesArray *fGemPointArray;

  /** Input array of MVDHitsPixel **/
  TClonesArray *fMvdPixelHitArray;
  /** Input array of MVDHitsStrip **/
  TClonesArray *fMvdStripHitArray;
  /** Input array of STTHits **/
  TClonesArray *fSttHitArray;

  /** Input array of MVDPoints **/
  TClonesArray *fMvdPointArray;
  /** Input array of STTPoints **/
  TClonesArray *fSttPointArray;

  /** Input array of mctracks **/
  TClonesArray *fMCTrackArray;
  /** Input array of mvd + stt tracks **/
  TClonesArray *fTrackArray;
  /** Input array of mvd + stt trackID **/
  TClonesArray *fTrackIDArray;
  /** Input array of mvd + stt track cand **/
  TClonesArray *fTrackCandArray;

  /** Output array of PndTrackCands **/
  TClonesArray *fCompleteTrackCandArray;
  /** Output array of PndTracks **/
  TClonesArray *fCompleteTrackArray;

  /** from parameters array of PndSttTube **/
  TClonesArray *fTubeArray;

  Bool_t fPdgFromMC; //!

  PndGeoSttPar *fSttParameters;
  PndGemDigiPar *fGemParameters;

  /** GEANE propagator **/
  FairGeanePro *fPro;

  Int_t countgood, countbad, countdoubt, countreconstructablehit, countprinotassigned, countsecnotassigned, countreconstructablepoint;
  Int_t countplane[8], countprop[2][8], countsttmvd, countsttmvdusable, countnohitonplane[8], evt;

  std::vector<int> usabletracks;

  /** map for the GEM hit ordering **/
  std::vector<int> fOrdering;
  std::vector<int>::iterator fOrderingIterator;

  /** position of the planes x, y, z**/
  TClonesArray *fSensPositions;

  /** number of GEM planes (nstations * nsensors on each station) **/
  Int_t fNPositions;

  /** plane of the sensor **/
  TVector3 fDj, fDk;

  /** PDG Code **/
  Int_t fPdgCode, fDefaultPdgCode;

  // CHECK added -------------------------
  std::vector<std::pair<int, int>> trackvector;
  std::vector<int> trackindexes;
  std::vector<int> notassignedhits;
  std::vector<int> notassignedtracks;

  /** hit to plane map **/
  std::map<int, bool> towhichplane;

  /** map which tracks can(not) be extrapolated to GEM **/
  std::map<int, bool> fProTracks;

  /** hitmap:
      rows = position in ordering (plane: 0, 1, 2, ...)
      cols = number of hits in that position (if I have
      3 hits on the i-th plane there will be 3 cols filled
      for row i-th) ... filled with hitID of each hit **/
  TMatrixT<float> hitmap;

  /** hitcounter: vector of number of hits for each
      position in ordering (e.g. 4 hits on 1st plane,
      0 on 2nd, 3 on 3rd, ...) **/
  TMatrixT<float> hitcounter;

  /** map of distances **/
  TMatrixT<double> distancemap;
  // ----------------------------------------

  Double_t fMaxDistance;
  Int_t fTimes;
  Int_t fTurn;

  // DISPLAY
  Bool_t fDisplayOn;
  TCanvas *display;
  TH2F *h[8], *hnotskewed, *hskewed;
  TH1F *hdist[8], *hdist2[8], *hsigma[8], *hsigma2[8], *hchosen[8], *hchosen2[8], *hmcdist[8], *hmcx[8], *hmcy[8];

  // MonteCarlo
  // CHECK delete this when everything is ok
  Bool_t fUseMC;

  TString fMvdPixelBranchName, fMvdStripBranchName, fSttBranchName, fGemBranchName;
  TString fStartTrackBranchName, fStartTrackCandBranchName, fStartTrackIDBranchName;

  /** combimap: hitID <-> 1/0 whether it is combinatorial or not **/
  std::map<int, int> fCombiMap;
  Double_t fCombiDistance;

  // evaluate performances? yes/no
  Bool_t fEvaluate;

  ClassDef(PndSttMvdGemTracking, 1);
};

#endif
