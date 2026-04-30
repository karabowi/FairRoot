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
// -----                   PndPhoGunShort header file                  -----
// -----               Created 12/10/10 by Maria Patsyuk               -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------

/**  PhoGun.h

 **
 ** Class for Analysising DRC Cherenkov Photons fired in the DIRC barrel directly


 **/

#ifndef PNDPHOGUNSHORT_H
#define PNDPHOGUNSHORT_H

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

#include "PndDrcBarPoint.h"
#include "PndDrcPDPoint.h"
#include "PndDrcEVPoint.h"
#include "PndDrcHit.h"
#include "PndDrcPDHit.h"
#include "PndDrcDigi.h"
#include "PndGeoHandling.h"

#include "PndDrcLutNode.h"

#ifndef ROOT_TParticlePDG
#include "TParticlePDG.h"
#endif
#ifndef ROOT_TDatabasePDG
#include "TDatabasePDG.h"
#endif

class PndPhoGunShort : public FairTask {

 public:
  /** Default constructor **/
  PndPhoGunShort();

  /** Constructor with verbosity **/
  PndPhoGunShort(Int_t verbose);

  /** Destructor **/
  virtual ~PndPhoGunShort();

  virtual InitStatus Init();

  /** Executed task **/
  virtual void Exec(Option_t *option);

  /** Finish task **/
  virtual void Finish();

  void SetOutputFile(TString outName = "lutnode.root") { fOutputName = outName; }

  void SetEVDepth(Float_t EVdepth = 30.) { fEVdz = EVdepth; }
  void SetNmcp(Float_t nmcp = 5.) { fNmcp = nmcp; }
  void SetShiftedPix(Bool_t spix = kFALSE) { fShiftPix = spix; }

 private:
  void ProcessPhotonHit();
  void InitLut();

  Int_t fDetectorID;

  TClonesArray *fBarPointArray; // DRC MC points in the bars
  TClonesArray *fPDPointArray;  // DRC points in the photon detector
  TClonesArray *fPDHitArray;    // DRC Hits in the photon detector
  TClonesArray *fMCArray;       // DRC MCPoints in the photon detector
  TClonesArray *fEVPointArray;  // DRC MCPoints in the photon detector
  TClonesArray *fDigiArray;

  // TClonesArray* fChPhoArray; // output array of PndChPho

  TList *fHistoList;
  TFile *froot;

  PndDrcDigi *fDigi;
  PndGeoDrc *fGeo; //!< Basic geometry data of barrel DRC.
  PndGeoHandling *fGeoH;

  /** Set the parameters to the default values. **/
  void SetDefaultParameters();

  Double_t InBarCoordSyst(TVector3, TVector3 *, TVector3 *, TVector3 *, TVector3 *);
  Double_t FindReflectionType(Double_t, Double_t, Double_t, TString);

  /** Verbosity level **/
  Int_t fVerbose;

  Int_t nevents;
  Int_t ambiguity;

  TString fOutputName;
  TString fTableName;
  Double_t ftilt;
  Int_t fBarId;

  // parameters:
  Double_t fR;
  Double_t fRBottom;
  Double_t fHThick;
  Double_t fBboxNum;
  Double_t fPipehAngle;
  Double_t fBarBoxGap;
  Double_t fLength;
  Double_t fDphi;
  Double_t fEVlen;
  Double_t fEVdz;
  Double_t fpi;
  Double_t fEVdrop;
  Double_t fPixelSize;
  Float_t fNmcp;
  Bool_t fShiftPix;

  Double_t fNoDD;
  Double_t fNoU0, fNoU1, fNoU2, fNoU3;
  Double_t fNoB;
  Double_t fNoUB;
  Double_t fNoBU0, fNoBU1, fNoBU2;
  Double_t fNoUU0, fNoUU1, fNoUU2, fNoUU3;
  Double_t fNoUUU0, fNoUUU1, fNoUUU2, fNoUUU3, fNoUUU4;
  Double_t fNoBUU0, fNoBUU1, fNoBUU2;
  Double_t fNoBUB;
  Double_t fNoUBU;
  Double_t fNoTotal;
  Double_t fNweirdPhotons;

  Double_t flambdah;
  Double_t fPixIndex;
  Double_t ftime;
  Double_t fPhiRot;
  TVector3 fPphoInit;
  TVector3 fStartVertex;
  TVector3 fPDSec, fEVSec, fEvSec;
  Double_t fPDPhi, fEVPhi, fPhiRotEV;
  Double_t Ang_pipe, Rout1, Rin1, Rin2, PlanB[9], PlanU[9], PlanR[6];
  Double_t determint1, determint2, determint3, determint4;
  TString ReflectionType, ReflName;
  TArrayD fmatrixdata;
  Double_t fZin;
  Double_t fLowZ;

  Double_t fkxBar, fkyBar, fkzBar;
  TVector3 fPphoB; // kBar

  // vertices of hit BarBox
  TVector3 fBBver1;
  TVector3 fBBver2;
  TVector3 fBBver3;
  TVector3 fBBver4;

  PndDrcEVPoint *EVpt;
  PndDrcEVPoint *EVt;
  PndDrcPDPoint *Ppt;
  PndDrcPDHit *pdhit;
  PndMCTrack *tr;

  TClonesArray *fLut[5];
  TFile *fFile;
  TTree *fTree;

  ClassDef(PndPhoGunShort, 1)
};

#endif
