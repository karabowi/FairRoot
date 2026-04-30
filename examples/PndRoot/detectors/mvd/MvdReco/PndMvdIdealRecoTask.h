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
// -----                 PndMvdIdealRecoTask header file             -----
// -----                  Created 20/03/07  by R.Kliemt               -----
// -------------------------------------------------------------------------

/** PndMvdIdealRecoTask.h
 *@author R.Kliemt <r.kliemt@physik.tu-dresden.de>
 **
 ** The ideal tracking produces tracks of type PndSdsHit by using MC information
 ** directly, but with the correct data interface.
 **/

#ifndef PNDMVDIDEALRECOTASK_H
#define PNDMVDIDEALRECOTASK_H

// Root includes
#include "TVector3.h"
#include "TRandom.h"
#include "TString.h"
#include "TGeoManager.h"

// framework includes
#include "FairTask.h"

// PndMvd includes
#include "PndSdsMCPoint.h"
#include "PndGeoHandling.h"

class TClonesArray;

class PndMvdIdealRecoTask : public FairTask {
 public:
  /** Default constructor **/
  PndMvdIdealRecoTask();

  PndMvdIdealRecoTask(Double_t sx, Double_t sy, Double_t sz);

  /** Destructor **/
  virtual ~PndMvdIdealRecoTask();

  PndMvdIdealRecoTask(const PndMvdIdealRecoTask &) = delete;
  PndMvdIdealRecoTask &operator=(const PndMvdIdealRecoTask &) = delete;

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  void SetPersistance(Bool_t p = kTRUE) { fPersistance = p; };
  Bool_t GetPersistance() { return fPersistance; };

 private:
  Bool_t fPersistance; // switch to turn on/off storing the arrays to a file

  TString fBranchName;
  PndGeoHandling *fGeoH;
  /** Input array of Points **/
  TClonesArray *fPointArray;
  TClonesArray *fMctruthArray;

  /** Output array of Hits **/
  TClonesArray *fHitOutputArray;
  /** Properties **/
  Double_t fSigmaX; // Variance vector of sensor smearing
  Double_t fSigmaY; // Variance vector of sensor smearing
  Double_t fSigmaZ; // Variance vector of sensor smearing
  PndSdsMCPoint *fCurrentPndMvdMCPoint;
  TGeoHMatrix *fCurrentTransMat;
  TMatrixT<Double_t> fHitCovMatrix;
  /** smearing and geometry access **/
  void InitTransMat();
  void smear(TVector3 &pos);
  void smearLocal(TVector3 &pos);
  void CalcDetPlane(TVector3 &o, TVector3 &u, TVector3 &v);

  void Register();

  void Reset();

  void ProduceHits();

  ClassDef(PndMvdIdealRecoTask, 1);
};

#endif
