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

#ifndef PNDMDTCLUSTERTASK_H
#define PNDMDTCLUSTERTASK_H 1

#include <PndPersistencyTask.h>
#include "PndMdtCluster.h"

class TClonesArray;

using std::map;
using std::vector;

class PndMdtClusterTask : public PndPersistencyTask {

 public:
  /** Default constructor **/
  PndMdtClusterTask();

  /** Destructor **/
  ~PndMdtClusterTask();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  void SetPersistence(Bool_t pers) { SetPersistency(pers); };

 private:
  Bool_t MdtMapping(); // Creates maps of MDT hits
  void Reset();        // reset maps

  PndMdtCluster *AddClusterBox(std::vector<Int_t> digiList);
  PndMdtCluster *AddClusterStrip(std::vector<Int_t> digiList);

  /** Input array of PndMdtDigi **/
  TClonesArray *fBoxIArray;
  TClonesArray *fStripIArray;

  /** Output array of PndMdtHit **/
  TClonesArray *fBoxOArray;
  TClonesArray *fStripOArray;

  /** Maps layer - digiIndex **/
  map<Int_t, Int_t> mapBox;
  map<Int_t, Int_t> mapStrip;

  ClassDef(PndMdtClusterTask, 1);
};

#endif
