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
// -----                   PndDrcRecoLookupMap header file                   -----
// -----               Created 30/09/11 by Maria Patsyuk               -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------

/**  PndDrcRecoLookupMap.h

**
** Class for Analysising DRC Cherenkov Photons with lookup tables for CHerenkov
   photons directions

**/

#ifndef PNDDRCRECOLOOKUPMAP_H
#define PNDDRCRECOLOOKUPMAP_H

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

class PndDrcRecoLookupMap : public FairTask {

 public:
  /** Default constructor **/
  PndDrcRecoLookupMap();

  /** Constructor with verbosity **/
  PndDrcRecoLookupMap(Int_t verbose);

  /** Destructor **/
  virtual ~PndDrcRecoLookupMap();

  virtual InitStatus Init();

  /** Executed task **/
  virtual void Exec(Option_t *option);

  /** Finish task **/
  virtual void Finish();

  void SetHistoWidth(double wid = 0.01) { fWidth = wid; } // 0.01
  void SetNSigma(double nsig = 3.) { fNSigma = nsig; }
  void SetMagneticField(double mag = 0.) { fB = mag; } // [Tesla]
  void SetOutputFile(TString outName = "PndDrcRecoLookupMap_output.root") { fOutputName = outName; }

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

  void DrawDetectorLayout()
  {
    // drawing of detector layout
    Double_t theta = (180. - 2 * fGeo->PipehAngle()) / fGeo->BBoxNum() / 180. * 3.1415; //[rad]
    // std::cout<<"-I- DrawLayout: theta = "<<theta<<std::endl;
    Double_t rad_out = 102.6; // 104.0; // PD outer radius
    Double_t rad_in = 49.15;  // 50.53; // PD inner radius
    Double_t rp_out = rad_out / cos(theta);
    Double_t rp_in = rad_in / cos(theta);
    Double_t xout[4], yout[4], xin[4], yin[4];
    for (Int_t i = 0; i < 4; i++) {
      xout[i] = rp_out * cos(2.0 * (i - 1) * theta);
      yout[i] = rp_out * sin(2.0 * (i - 1) * theta);
      xin[i] = rp_in * cos(2.0 * (i - 1) * theta);
      yin[i] = rp_in * sin(2.0 * (i - 1) * theta);
      TLine *l1 = new TLine(xout[i], yout[i], xin[i], yin[i]);
      l1->Draw("same");
    }
    TPolyLine *p1 = new TPolyLine(4, xout, yout);
    p1->SetFillColor(kYellow);
    p1->Draw("same");
    TPolyLine *p2 = new TPolyLine(4, xin, yin);
    p2->SetFillColor(0);
    p2->Draw("same");
    //--------------------------
  }

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

  // look up table
  virtual void SetParContainers();
  PndDrcDigiPar *fDigiPar;

  Double_t InBarCoordSyst(TVector3, TVector3 *, TVector3 *, TVector3 *, TVector3 *);
  Double_t SectorNum(Double_t, Double_t);
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

  // vertices of hit BarBox
  TVector3 fBBver1;
  TVector3 fBBver2;
  TVector3 fBBver3;
  TVector3 fBBver4;

  std::vector<TH1F *> PhiThetaPoints;
  std::vector<TH1F *> PhiThetaPointsCut;
  std::vector<TH1F *> PhiThetaPointsWeight;
  std::vector<TH1F *> NboPoints;
  TH2F *fMapHist;
  TH2F *fSigHist;
  TH2F *fCheHist;
  TH2F *fkBarXHist;
  TH2F *fkBarYHist;

  // time resolution
  TF1 *timeres;

  TH1D *fhPDTime;
  TH2D *fhPDHits;
  TH1F *fhDiff;

  TH2F *fhRecoT1;
  TH1F *fhCHreal;
  TH1F *fhLam;

  TH2F *fhNboLam;

  ClassDef(PndDrcRecoLookupMap, 1)
};

#endif
