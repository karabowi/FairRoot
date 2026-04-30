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
 * PndEmcReadProto192Data is a Fairtask to read
 * Data from the Proto60 and create PndEmcHits
 *
 * author: Christian Hammann <chammann@hiskp.uni-bonn.de>
 * Date: 8.1.2010
 *
 * ******************************************/

#ifndef PNDREADPROTO192DATA_H
#define PNDREADPROTO192DATA_H

#include "FairTask.h"
#include "PndEmcDigiPar.h"
#include "PndEmcGeoPar.h"
#include "PndGeoPassivePar.h"
#include "FairBaseParSet.h"
#include "TFile.h"
#include "TTree.h"
#include "TProtoUnpackEvent.h"
#include "TClonesArray.h"

class PndEmcReadProto192Data : public FairTask {
 public:
  virtual InitStatus Init();
  virtual void Exec(Option_t *opt);
  virtual InitStatus ReInit()
  {
    IsInit = kFALSE;
    return Init();
  };
  virtual void SetParContainers();
  void SetIndexMap(Long_t *map);
  void SetWaveformMap(Long_t *map);
  PndEmcReadProto192Data(TTree *prototree, TTree *taggertree = nullptr, Bool_t StoreHits = kTRUE, Int_t debuglevel = 0);
  ~PndEmcReadProto192Data(){};
  virtual void SetTaggerTreeOffset(Long_t offset = 0) { lTaggerTreeOffset = offset; };

 private:
  TTree *lProto192Tree;
  TTree *lTaggerTree;
  Long_t lProto192EventNum;
  UInt_t lTaggerEventNum;
  TProtoUnpackEvent *lProto192Event;
  TClonesArray *lWaveArray;
  TClonesArray *lTaggerArray;
  Long_t lIndexMap[64];
  Long_t lWaveformMap[64];

  PndEmcGeoPar *lGeoPar;
  PndEmcDigiPar *lDigiPar;
  PndGeoPassivePar *lGeoPassivePar;
  FairBaseParSet *lBaseParSet;
  Bool_t lStoreHits;
  Int_t lDebug;
  Bool_t IsInit;
  std::vector<Double_t> signal;
  Long_t lTaggerTreeOffset;
  Long_t lEventNumOverflow;
  ClassDef(PndEmcReadProto192Data, 1);
};

#endif
