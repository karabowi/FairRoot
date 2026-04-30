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
// -----                   DrawHits header file               -----
// -----               Created 19/10/09  by Dipanwita Dutta            -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------

/**  DrawHits.h

 **
 ** Class for Analysising DRC Hits


 **/

#ifndef PNDDRCHITANA_H
#define PNDDRCHITANA_H

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

#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TGraph2D.h"

#include "TColor.h"
#include "TStyle.h"

#ifndef ROOT_TParticlePDG
#include "TParticlePDG.h"
#endif
#ifndef ROOT_TDatabasePDG
#include "TDatabasePDG.h"
#endif

class DrawHits : public FairTask {

 public:
  /** Default constructor **/
  DrawHits();

  /** Constructor with verbosity **/
  DrawHits(Int_t verbose);

  /** Destructor **/
  virtual ~DrawHits();

  virtual InitStatus Init();

  /** Executed task **/
  virtual void Exec(Option_t *option);

  /** Finish task **/
  virtual void Finish();

  /** Rotation in Bar Co-ordinate System **/
  void RotBarCoordinate(TVector3 &vector, Int_t barID);

  void SetTreeName(TString str) { fTreeName = str; }

 protected:
 private:
  void ProcessPhotonHit();
  void ProcessPhotonMC();
  void WriteToFile();
  void CreateHisto();
  void DrawHisto();
  void ProcessBarHit();

  Int_t fDetectorID;
  Double_t fMass;
  Double_t fThetaC, fErrThetaC;
  Int_t fRefIndex;

  TClonesArray *fBarPointArray; // DRC MC points in the bars
  TClonesArray *fPDPointArray;  // DRC points in the photon detector
  TClonesArray *fHitArray;      // DRC Hits in the bars
  TClonesArray *fPDHitArray;    // DRC Hits in the photon detector
  TClonesArray *fMCArray;       // DRC Hits in the photon detector
  TList *fHistoList;

  PndGeoDrcPar *fPar;

  PndGeoDrc *fGeo; //!< Basic geometry data of barrel DRC.

  /** Set the parameters to the default values. **/
  void SetDefaultParameters();

  /** Verbosity level **/
  Int_t fVerbose;

  /* nice plits*/
  void SetPlotStyle()
  {
    const Int_t NRGBs = 5;
    const Int_t NCont = 255;

    Double_t stops[NRGBs] = {0.00, 0.34, 0.61, 0.84, 1.00};
    Double_t red[NRGBs] = {0.00, 0.00, 0.87, 1.00, 0.51};
    Double_t green[NRGBs] = {0.00, 0.81, 1.00, 0.20, 0.00};
    Double_t blue[NRGBs] = {0.51, 1.00, 0.12, 0.00, 0.00};
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->SetNumberContours(NCont);
  }

  Bool_t InsideBar(Double_t, Double_t, Double_t, Double_t, Double_t, Double_t, Double_t, Double_t, Double_t, Double_t);

  Int_t nevents;

  /* tree and its variables */
  Double_t xmcp;
  Double_t ymcp;
  Double_t zmcp;
  Double_t pxmc;
  Double_t pymc;
  Double_t pzmc;
  Float_t xhit;
  Float_t yhit;
  Double_t zhit;
  Double_t thit;
  Double_t xbar;
  Double_t ybar;
  Double_t zbar;
  Double_t pxMo;
  Double_t pyMo;
  Double_t pzMo;
  Double_t pxPho;
  Double_t pyPho;
  Double_t pzPho;
  Double_t xEnt;
  Double_t yEnt;
  Double_t zEnt;
  Double_t lam;

  Double_t fPhiRot;
  TVector3 fStartVertex;
  Double_t fPx;
  Double_t fPy;
  Double_t fPz;
  Double_t fXcross;
  Double_t fYcross;
  TVector3 fPphoB;
  Double_t fkxBar;
  Double_t fkyBar;
  Double_t fkzBar;

  TString fTreeName;
  TTree *phoTree;
  std::vector<TH1F *> histos;
  TH1F *currh;
  TGraph2D *map;
  TGraph2D *pop;
  TVector3 fPMo;

  TH1D *fhThetaC;
  TH2D *fhThetaCMass;
  TH2D *fhMomAng;
  TH2D *fhThetaCMomK;
  TH2D *fhThetaCMomP;
  TH2D *fhThetaCMomM;
  TH2D *fhThetaCMomE;

  TH1F *fhPhoTheta;
  TH2F *fhPDPlane;
  TH1D *fhCHrealMC;
  TH1D *fhCHreal;
  TH2D *fhCHlam;
  TH2D *fhCHlamMC;
  TH2D *fhCHlamE;

  TH2F *fhHitsD;

  TH1D *fhLambda;
  TH2D *fhXYPDHit;
  TH1D *fhLambdaMC;
  TH2D *fhXYPDMCPt;
  TH1D *fhPDTime;

  TH2D *fhXYPDHitKp;
  TH2D *fhXYPDHitKn;
  TH2D *fhXYPDHitpip;
  TH2D *fhXYPDHitpin;
  TH2D *fhXYPDHitmp;
  TH2D *fhXYPDHitmn;

  TH2D *fhXYPDMCPtKp;
  TH2D *fhXYPDMCPtKn;
  TH2D *fhXYPDMCPtpip;
  TH2D *fhXYPDMCPtpin;
  TH2D *fhXYPDMCPtmp;
  TH2D *fhXYPDMCPtmn;
  ClassDef(DrawHits, 1)
};

#endif
