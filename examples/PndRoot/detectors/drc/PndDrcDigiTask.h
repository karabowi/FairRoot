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

#ifndef PNDDRCDIGITASK_H
#define PNDDRCDIGITASK_H

#include <PndPersistencyTask.h>
#include "TClonesArray.h"
#include "PndMCTrack.h"
#include "PndDrcDigi.h"
#include "PndStack.h"
#include "FairBaseParSet.h"
#include "PndGeoDrcPar.h"
#include "TString.h"
#include <string>
#include "TH1.h"
#include "TH2.h"
#include "PndGeoDrc.h"
#include "PndGeoHandling.h"
#include "FairEventHeader.h"

#include "PndDrcPDPoint.h"
#include "PndDrcBarPoint.h"
#include "PndMCTrack.h"
#include "PndDrcDigiWriteoutBuffer.h"

#ifndef ROOT_TParticlePDG
#include "TParticlePDG.h"
#endif
#ifndef ROOT_TDatabasePDG
#include "TDatabasePDG.h"
#endif

#include <map>

class PndDrcDigiTask : public PndPersistencyTask {

 public:
  /** Default constructor **/
  PndDrcDigiTask();

  /** Constructor with verbosity **/
  PndDrcDigiTask(Int_t verbose);

  /** Destructor **/
  virtual ~PndDrcDigiTask();

  /** Initialization of the task **/
  //  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Executed task **/
  virtual void Exec(Option_t *option);

  /** Reset eventwise counters **/
  void Reset();

  /** Finish task **/
  virtual void Finish();

  /** method: FindDrcHitPosition-> finds hit position in PMT plane **/
  void FindDrcHitPosition(Double_t xPoint, Double_t yPoint, Double_t zPoint, Double_t &xHit, Double_t &yHit, Double_t &zHit, Int_t pmtID);

  void RunTimeBased() { fTimeOrderedDigi = kTRUE; }
  void SetChargeSharing(Bool_t ct = 0) { fChargeSharing = ct; }
  void SetDeadTime(Double_t var) { fDeadTime = var; }
  void SetTimeResolution(Double_t var) { fSigmat = var; }

  TVector3 GetSensorDimensions(Int_t sensorID);

 protected:
  PndGeoHandling *fGeoH; //! ///< converter for detector names

 private:
  /**   Process MC Points **/
  void ProcessPhotonPoint();
  void Smear(Double_t &time, Double_t sigt);
  void ActivatePixel(Int_t sensorId, Int_t k, Int_t csflag);

  // basic parameters of DIRC
  Double_t fMcpActiveArea;
  Double_t fPixelSize;
  Int_t fNpix; // in one column/row
  Double_t fPixelGap;
  Double_t fPixelStep;
  Double_t fPixelSigma;
  Double_t fDeadTime;        //[ns]
  Double_t fThreshold;       // in % of the total probability of 1 to detect a hit
  Double_t fTimeGranularity; // granularity of the time signal [ns]

  Bool_t fChargeSharing;
  Bool_t fTimeOrderedDigi;
  Int_t fDetectorID;
  TVector3 fPosHit;
  TVector3 fDPosHit;
  TVector3 fPosPDHit;
  Double_t fThetaC, fErrThetaC;
  Double_t fTime;
  Int_t fRefIndex;
  Int_t fPixelID;
  Int_t fBarId;

  Int_t fNDigis;
  TClonesArray *fDigis; /** Output array of PndDrcDigi **/

  std::map<Int_t, Int_t> fPixelMap; // Map of active pixels  to index of PndDrcDigis

  TClonesArray *fBarPointArray; // DRC MC points in the bars
  TClonesArray *fPDPointArray;  // DRC MC points in the photon plane
  TClonesArray *fDrcDigiArray;  // DRC digis
  TClonesArray *fMCArray;       // DRC Hits in the photon detector

  PndGeoDrcPar *fPar;
  PndGeoDrc *fGeo; // Basic geometry data of barrel DRC.

  Int_t fVerbose; // Verbosity level

  /** Parameters of photodetector **/
  Int_t fDetType; // detector type

  Double_t nRefrac; // Refractive index of photon detector
  Double_t fSigmat; // Time Resolution in ps
  Int_t nevents;

  PndDrcPDPoint *fPpt;
  PndMCTrack *fMCtrk;
  PndDrcBarPoint *fBarPoint;
  PndDrcDigiWriteoutBuffer *fDataBuffer;

  /** Set the parameters to the default values. **/
  void SetParameters();

  ClassDef(PndDrcDigiTask, 1)
};

#endif
