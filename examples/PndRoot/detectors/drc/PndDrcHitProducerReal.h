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
// -----                   PndDrcHitProducer header file               -----
// -----               Created 30/10/09  by Dipanwita Dutta
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------

/**  PndDrcHitProducerReal.h

 **
 ** Class for producing DRC hits directly from MCPoints

 **/

#ifndef PNDDRCHITPRODUCERREAL_H
#define PNDDRCHITPRODUCERREAL_H

#include <PndPersistencyTask.h>
#include "TClonesArray.h"
#include "PndMCTrack.h"
#include "PndDrcHit.h"
#include "PndDrcPDHit.h"
#include "PndStack.h"
#include "FairBaseParSet.h"
#include "PndGeoDrcPar.h"
#include "PndGeoDrc.h"
#include "TString.h"
#include <string>
#include "TH1.h"
#include "TH2.h"

#ifndef ROOT_TParticlePDG
#include "TParticlePDG.h"
#endif
#ifndef ROOT_TDatabasePDG
#include "TDatabasePDG.h"
#endif

class PndDrcHitProducerReal : public PndPersistencyTask {

 public:
  /** Default constructor **/
  PndDrcHitProducerReal();

  /** Constructor with verbosity **/
  PndDrcHitProducerReal(Int_t verbose, Int_t det_type);

  /** Destructor **/
  virtual ~PndDrcHitProducerReal();

  /** Initialization of the task **/
  //  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Executed task **/
  virtual void Exec(Option_t *option);

  /** Finish task **/
  virtual void Finish();

  /** method AddHit
   **
   ** Adds a DrcHit to the HitCollection
   **/
  PndDrcHit *AddHit(Int_t detID, TVector3 posHit, TVector3 dPosHit, Double_t thetaC, Double_t errThetaC, Int_t index);

  /** method AddPDHit
   **
   ** Adds a DrcPDHit to the HitCollection
   **/
  PndDrcPDHit *AddPDHit(Int_t detID, TVector3 posPDHit, TVector3 dPosPDHit, Double_t time, Double_t timeThreshold, Int_t index1);

  /** method: FindDrcHitPosition-> finds hit position in PMT plane **/
  void FindDrcHitPosition(Double_t xPoint, Double_t yPoint, Double_t &xHit, Double_t &yHit, Int_t &pmtID);
  void FindDrcHitPositionTilt(Double_t xPoint, Double_t yPoint, Double_t &xHit, Double_t &yHit, Int_t &pmtID);

  /**  Set Photon Detector Parameter **/
  void SetPhotonDetEff(Double_t &lambda_min, Double_t &lambda_max, Double_t &lambda_step, Double_t efficiency[]);
  void SetFakeDetEff(Double_t &lambda_min, Double_t &lambda_max, Double_t &lambda_step, Double_t efficiency[]);
  void SetPhotonDetEffOld(Double_t &lambda_min, Double_t &lambda_max, Double_t &lambda_step, Double_t efficiency[]);
  void SetPhotonDetEffNew(Double_t &lambda_min, Double_t &lambda_max, Double_t &lambda_step, Double_t efficiency[]);

  /** Set Photon Transport Efficiency **/
  void SetPhotonTransportEff(Double_t &, Double_t &, Double_t &, Double_t &, Int_t &, Double_t fEfficiency[]);
  /** Auxiliraly functions **/
  void NumberOfBounces(TVector3, TVector3, Int_t *, Int_t *, Double_t *, Double_t *);
  Double_t FindPhiRot(Double_t, Double_t);
  Double_t FindOutPoint(Double_t, Double_t, Double_t, Double_t *, Bool_t);

  void SetIsDetEfficiency(Bool_t isDetEff) { fisDetEff = isDetEff; }
  void SetIsPixelization(Bool_t isPixel) { fisPixel = isPixel; }
  void SetTransportEfficiency(Bool_t isTran = 0) { fisTransportEff = isTran; }

  //########################################
  void SetTilt(Double_t tilt = 0.) { ftilt = tilt; } // degrees
  //########################################

  void DrawDetEfficiency(Bool_t dr = kFALSE) { fDrawing = dr; }

 protected:
 private:
  /**   Process MC Points **/
  void ProcessPhotonPoint();
  void ProcessBarPoint();
  void Smear(Double_t &time, Double_t sigt);

  // basic parameters of DIRC
  Double_t fpi;
  Double_t fzup;
  Double_t fzdown;
  Double_t fradius;
  Double_t fhthick;
  Double_t fpipehAngle;
  Double_t fbbGap;
  Double_t fbbnum;
  Double_t fbarnum;
  Double_t fphi0;
  Double_t fdphi;
  Double_t flside;
  Double_t fbarwidth;

  Bool_t fisDetEff;
  Bool_t fisPixel;
  Bool_t fisTransportEff;
  Bool_t fDrawing;
  Double_t ftilt;
  Int_t fDetectorID;
  TVector3 fPosHit;
  TVector3 fDPosHit;
  TVector3 fPosPDHit;
  TVector3 fDPosPDHit;
  Double_t fThetaC, fErrThetaC;
  Double_t fTime, fTimeThreshold;
  Int_t fRefIndex;
  Int_t fPDRefIndex;

  TClonesArray *fBarPointArray; // DRC MC points in the bars
  TClonesArray *fPDPointArray;  // DRC MC points in the photon plane
  TClonesArray *fHitArray;      // DRC hits
  TClonesArray *fPDHitArray;    // DRC Photon Detectorhits
  TClonesArray *fMCArray;       // DRC Hits in the photon detector

  // TObjArray *fVolumeArray;
  // TClonesArray* fListStack;     // Tracks

  PndGeoDrcPar *fPar;

  PndGeoDrc *fGeo; //!< Basic geometry data of barrel DRC.

  /** Verbosity level **/
  Int_t fVerbose;

  /** Parameters of photodetector **/
  Int_t fDetType; // detector type

  Double_t nRefrac;          // Refractive index of photon detector
  Double_t fPixelDim;        // Pixel Diemsion of photocathode
  Double_t fSigmat;          // Time Resolution in ps
  Double_t fCollectionEff;   // Collection Efficiency
  Double_t fPackingFraction; // Packing Fraction or Active Area Ratio
  Double_t fRoughness;       // Surface roughness (bars)
  Int_t detection;
  Int_t nevents;

  /** Set the parameters to the default values. **/
  void SetParameters();

  ClassDef(PndDrcHitProducerReal, 1)
};

#endif
