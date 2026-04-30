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
// -----                   PndDrcRecoLookupMapS header file                   -----
// -----               Created 30/09/11 by Maria Patsyuk               -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------

/**  PndDrcRecoLookupMapS.h

**
** Class for Analysising DRC Cherenkov Photons with lookup tables for CHerenkov
   photons directions

**/

#ifndef PNDDRCRECOLOOKUPMAPS_H
#define PNDDRCRECOLOOKUPMAPS_H

#include "FairTask.h"
#include "TClonesArray.h"
#include "PndMCTrack.h"
#include "PndDrcHit.h"
#include "PndDrcPDHit.h"
#include "PndStack.h"
#include "FairBaseParSet.h"
#include "PndGeoDrcPar.h"
#include "TString.h"
#include <string>
#include "PndDrcDigiPar.h"
#include "PndDrcDigi.h"
#include "PndGeoHandling.h"

#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TGraph2D.h"
#include "TColor.h"
#include "TStyle.h"
#include "TLine.h"
#include "TPolyLine.h"

#ifndef ROOT_TParticlePDG
#include "TParticlePDG.h"
#endif
#ifndef ROOT_TDatabasePDG
#include "TDatabasePDG.h"
#endif

class PndDrcRecoLookupMapS : public FairTask {

 public:
  /** Default constructor **/
  PndDrcRecoLookupMapS();

  /** Constructor with verbosity **/
  PndDrcRecoLookupMapS(Int_t verbose);

  /** Destructor **/
  virtual ~PndDrcRecoLookupMapS();

  virtual InitStatus Init();

  /** Executed task **/
  virtual void Exec(Option_t *option);

  /** Finish task **/
  virtual void Finish();

  void SetHistoWidth(double wid = 0.01) { fWidth = wid; } // 0.01
  void SetNSigma(double nsig = 3.) { fNSigma = nsig; }
  void SetMagneticField(double mag = 0.) { fB = mag; } // [Tesla]
  void SetOutputFile(TString outName = "PndDrcRecoLookupMapS_output.root") { fOutputName = outName; }

 protected:
 private:
  void ProcessPhotonHit();
  void ProcessPhotonMC();
  void WriteToFile();
  void CreateHisto();
  void DrawHisto();
  void ProcessBarHit();

  // parameters:
  Double_t fpi;
  Double_t fR;
  Double_t fzup;
  Double_t fzdown;
  Double_t fHThick;
  Double_t fBboxNum;
  Double_t fBarNum;
  Double_t fPipehAngle;
  Double_t fBarBoxGap;
  Double_t fLSide;
  Double_t fBarWidth;
  Double_t fDphi;

  // output trees
  TTree *mapstree;
  Float_t fxx, fyy, fchdiff;

  // look up table
  virtual void SetParContainers();
  PndDrcDigiPar *fDigiPar;

  Double_t InBarCoordSyst(TVector3, TVector3 *, TVector3 *, TVector3 *, TVector3 *);
  Int_t NumberOfBounces(TVector3, TVector3, Int_t);
  Double_t FindOutPoint(Double_t, Double_t, Double_t, Double_t *, Bool_t);
  Double_t CherenkovPhi(TVector3, TVector3, Double_t *, Double_t *);
  Double_t RecoAmbigTime(TVector3, TVector3, Double_t *, Bool_t);
  void DrawBarBox(TVector3, TVector3, TVector3, TVector3);
  Int_t fDetectorID;

  TClonesArray *fBarPointArray; // DRC MC points in the bars
  TClonesArray *fPDPointArray;  // DRC points in the photon detector
  TClonesArray *fHitArray;      // DRC Hits in the bars
  TClonesArray *fPDHitArray;    // DRC Hits in the photon detector
  TClonesArray *fMCArray;       // DRC Hits in the photon detector
  TClonesArray *fDigiArray;

  // TClonesArray* fChPhoArray; // output array of PndChPho
  TClonesArray *fDrcLutInfoArray;

  TList *fHistoList;

  PndGeoDrc *fGeo;       //!< Basic geometry data of barrel DRC.
  PndGeoHandling *fGeoH; //!

  /** Set the parameters to the default values. **/
  void SetDefaultParameters();

  /** Verbosity level **/
  Int_t fVerbose;

  Int_t nevents;

  TString fOutputName;
  Double_t fWidth;
  Double_t fPixelSize;
  Double_t fHAngleInBDeg;

  Double_t fCHrecoMin;
  Double_t fCHrecoMax;
  Double_t fNSigma;
  Double_t fB;

  TVector3 fnX1;
  TVector3 fnY1;

  Double_t flambdah;
  Double_t fpixID;
  Double_t fxbar;
  Double_t fybar;
  Double_t fxPHit;
  Double_t fyPHit;
  Double_t fzPHit;
  Double_t ftime;
  Double_t ftime0;
  Double_t fPhiRot;
  TVector3 fPMo;
  TVector3 fPMoB;
  TVector3 fPphoInit;
  TVector3 fkBar;
  Double_t fWeight;
  Int_t fBarId;

  Bool_t print;

  // look up parameters
  Int_t NHitPix;
  Int_t NPixPar;
  Int_t NAmb;

  Double_t fkxBar, fkyBar, fkzBar;
  Double_t fPxMoBar, fPyMoBar, fPzMoBar;
  Double_t fxnew, fynew;
  Double_t CHexp;
  Double_t CHreal;
  TVector3 fPphoB; // kBar
  TVector3 fStartVertex;
  TVector3 fPphoPD;
  Double_t fPath;
  Double_t fPhiMap;
  Double_t fThetaMap;

  // vertices of hit BarBox
  TVector3 fBBver1;
  TVector3 fBBver2;
  TVector3 fBBver3;
  TVector3 fBBver4;

  ClassDef(PndDrcRecoLookupMapS, 1)
};

#endif
