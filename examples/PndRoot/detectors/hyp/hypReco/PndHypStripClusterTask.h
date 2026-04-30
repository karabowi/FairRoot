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
// -----                 PndMvdStripClusterTask header file             -----
// -----                  Created 10/01/06  by V. Friese               -----
// -------------------------------------------------------------------------

#ifndef PNDHYPSTRIPCLUSTERTASK_H
#define PNDHYPSTRIPCLUSTERTASK_H

#include "FairTask.h"
#include "PndGeoHypPar.h"
#include "PndHypStripDigiPar.h"
#include "PndHypHit.h"
#include "PndHypPoint.h"
//#include "PndHypPixel.h"
#include "PndHypCalcStrip.h"
#include "FairGeoVector.h"
#include "FairGeoTransform.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
//#include "PndHypHybridHitProducer.h"
#include "PndHypStripHitProducer.h"

#include <string>
#include <vector>

class TClonesArray;

class PndHypStripClusterTask : public FairTask {
 public:
  /** Default constructor **/
  PndHypStripClusterTask();

  PndHypStripClusterTask(Double_t chargecut, TString geofile);

  /** Destructor **/
  virtual ~PndHypStripClusterTask();

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

 private:
  TVector2 CalcLineCross(TVector2 point1, TVector2 dir1, TVector2 point2, TVector2 dir2);

  TClonesArray *fDigiArray;    // Input array of PndHypDigis
  TClonesArray *fClusterArray; // Output array of PndHypClusters
  TClonesArray *fHitArray;     // Output array of PndHypHits

  void Register();
  void Reset();
  void ProduceHits();

  Double_t fRadius;
  Int_t fFEcolumns;
  Int_t fFErows;
  TString fGeoFile;
  Double_t fChargeCut;

  PndHypStripDigiPar *fDigiPar; //! Digitization Parameters
  PndHypCalcStrip *fstripcalcTOP;
  PndHypCalcStrip *fstripcalcBOT;
  PndHypGeoHandling *fGeoH; //! Geometry name handling
  PndGeoHypPar *fGeoPar;
  ClassDef(PndHypStripClusterTask, 3);
};

#endif /* HYPCLUSTERTASK_H */
