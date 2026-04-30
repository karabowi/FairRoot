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

/*******************************************
 * PndEmcReadProtoData is a Fairtask to read
 * Data from the Proto60 and create PndEmcHits
 *
 * author: Christian Hammann <chammann@hiskp.uni-bonn.de>
 * Date: 8.1.2010
 *
 * ******************************************/

#ifndef PNDREADPROTODATA_H
#define PNDREADPROTODATA_H

#include "FairTask.h"
#include "PndEmcHit.h"
#include "PndEmcDigi.h"
#include "ReadMainzProto60.h"
#include "PndEmcDigiPar.h"
#include "PndEmcGeoPar.h"
#include "PndGeoPassivePar.h"
#include "FairBaseParSet.h"

#include "TClonesArray.h"

class PndEmcReadProtoData : public FairTask {
 public:
  virtual InitStatus Init();
  virtual void Exec(Option_t *opt);
  virtual InitStatus ReInit()
  {
    IsInit = kFALSE;
    return Init();
  };

  Bool_t OnlyPrompt(Bool_t onlyPrompt = kTRUE);
  Bool_t OnlyTagged(Bool_t onlyTagged = kTRUE);

  PndEmcReadProtoData(ReadMainzProto60 *datareader, Bool_t StoreHits = kTRUE, Int_t debuglevel = 0);
  ~PndEmcReadProtoData(){};

 private:
  virtual void SetParContainers();

  ReadMainzProto60 *lDataReader;
  Bool_t lStoreHits;
  Bool_t lOnlyPrompt;
  Bool_t lOnlyTagged;
  Int_t lDebug;
  TClonesArray *lHitArray;
  TClonesArray *lDigiArray;
  TClonesArray *lGammaArray;

  Double_t lEnergies[60];
  Double_t lADCs[60];
  Double_t lTimes[60];
  Double_t lTaggerTimes[16];
  Double_t lTaggerEnergies[16];

  Double_t lEnergyHitThreshold;
  Double_t lEnergyDigiThreshold;

  Bool_t IsInit;

  PndEmcDigiPar *lDigiPar;
  PndEmcGeoPar *lGeoPar;
  PndGeoPassivePar *lGeoPassivePar;
  FairBaseParSet *lBaseParSet;

  ClassDef(PndEmcReadProtoData, 1);
};

#endif
