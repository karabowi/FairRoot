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

//-------------------------------------------------------//
//     Class PndMvdCreateDefaultApvMap                   //
//                                                       //
// Creates a default Frontend number mapping file        //
//                                                       //
// Created 16.02.2010                                    //
// Author: Ralf Kliemt                                   //
// ralf.kliemt{at}hiskp.uni-bonn.de                      //
//-------------------------------------------------------//
#ifndef PNDMVDCREATEDEFAULTAPVMAP_H
#define PNDMVDCREATEDEFAULTAPVMAP_H

#include "TObject.h"
#include "TList.h"
#include "TGeoManager.h"
#include "TString.h"
#include "FairTask.h"
#include "PndSdsStripDigiPar.h"
#include "PndGeoHandling.h"
#include <utility>
#include <string>
#include <vector>
#include <map>

typedef std::pair<TString, std::pair<Int_t, Int_t>> Map_Entry;

class PndMvdCreateDefaultApvMap : public FairTask {

 public:
  PndMvdCreateDefaultApvMap();

  ~PndMvdCreateDefaultApvMap();
  PndMvdCreateDefaultApvMap(const PndMvdCreateDefaultApvMap &) = delete;
  PndMvdCreateDefaultApvMap &operator=(const PndMvdCreateDefaultApvMap &) = delete;

  // setup
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit() { return kSUCCESS; };

  /** Virtual method Exec **/
  virtual void Exec(Option_t *) { return; };

  /** Our main function here **/
  Bool_t CreateFile(TString outFile);

 private:
  // helper
  Bool_t SelectSensorParams(TString detname);
  void WriteExpandNode();
  std::map<Double_t, std::vector<Map_Entry>> fSortedMapping; //!
  // TString FindNodePath(TGeoNode* node);
  // void DiveDownToNode(TGeoNode* node);

  // helper members
  PndSdsStripDigiPar *fCurrentDigiPar; //!
  TList *fDigiParameterList;           //!
  Int_t fFeCount;                      // Running number of rw frontends
  Int_t fFakeCount;                    // Running number of fake frontends
  ClassDef(PndMvdCreateDefaultApvMap, 1);
};

#endif
