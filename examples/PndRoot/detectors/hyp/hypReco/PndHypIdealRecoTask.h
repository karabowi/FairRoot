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
// -----                 MvdIdealRecoTask header file             -----
// -----                  Created 20/03/07  by R.Kliemt           -----
// -----     Modified to HypIdealRecoTask 12.10.07 by A.Sanchez
// -------------------------------------------------------------------------

/** MvdIdealRecoTask.h
 *@author R.Kliemt <r.kliemt@physik.tu-dresden.de>
 **
 ** The ideal tracking produces tracks of type MvdCluster by using MC information
 ** directly, but with the correct data interface.
 **/

#ifndef PNDHYPIDEALRECOTASK_H
#define PNDHYPIDEALRECOTASK_H

// Root includes
#include "TVector3.h"
#include "TRandom.h"
#include "TString.h"
#include "TGeoManager.h"

// framework includes
#include "FairTask.h"

// Hyp includes
#include "PndHypPoint.h"
#include "PndHypGeoHandling.h"
class TClonesArray;

class PndHypIdealRecoTask : public FairTask {
 public:
  /** Default constructor **/
  PndHypIdealRecoTask();

  PndHypIdealRecoTask(Double_t sx, Double_t sy, Double_t sz);

  /** Destructor **/
  ~PndHypIdealRecoTask();

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

 private:
  TString fBranchName;
  PndHypGeoHandling *fGeoH;
  /** Input array of Points **/
  TClonesArray *fPointArray;
  TClonesArray *fMctruthArray;

  /** Output array of Hits **/
  TClonesArray *fHitOutputArray;
  /** Properties **/
  Double_t fSigmaX; // Variance vector of sensor smearing
  Double_t fSigmaY; // Variance vector of sensor smearing
  Double_t fSigmaZ; // Variance vector of sensor smearing
  PndHypPoint *fCurrentHypPoint;
  TGeoHMatrix *fCurrentTransMat;
  TMatrixT<double> fHitCovMatrix;
  /** smearing and geometry access **/
  void InitTransMat();
  void smear(TVector3 &pos);
  void smearLocal(TVector3 &pos);
  void CalcDetPlane(TVector3 &o, TVector3 &u, TVector3 &v);

  void Register();

  void Reset();

  void ProduceHits();

  ClassDef(PndHypIdealRecoTask, 2);
};

#endif
