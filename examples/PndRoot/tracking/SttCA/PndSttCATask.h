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

#ifndef PndSttCATask_H_
#define PndSttCATask_H_

#include "FairTask.h"
#include "PndGeoSttPar.h"
#include "PndSttGeometryMap.h"
//#include "PndSttStrawMap.h"
#include "PndSttCA.h"
class PndStt2GeoHandler;

#ifdef RUNCUDA
extern "C" int *AllocateStaticData(int *, int);
extern "C" void FreeStaticData(int *);
#endif

#include <vector>

class PndSttCATask : public FairTask {
 public:
  PndSttCATask() : FairTask("Stt CA"), fPersistence(kTRUE), fTrackFinder(nullptr)
  {
    fOutBranchNamePrefix = "";
    fInBranchNamePrefix = "";
  };

  virtual ~PndSttCATask(){};

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  // virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  virtual void FinishEvent();

  virtual void FinishTask();

  void SetPersistence(Bool_t val) { fPersistence = val; };
  void SetUseGPU(Bool_t val) { fUseGPU = val; };

  // define a Prefix for the Output in case you run more Instances of the CellTrackFinderTask
  void SetOutBranchNamePrefix(TString prefix) { fOutBranchNamePrefix = prefix + "_"; };
  // set a prefix for the defoult inbut Branch names
  void SetInBranchNamePrefix(TString prefix) { fInBranchNamePrefix = prefix + "_"; };
  // if nothing is defined, the standard STTHit branches are used.
  // If InputBranches are defined, so far only Branches holding data of type STTHit are further processed (See "initHitArray" method)
  void AddHitBranch(TString branchName) { fHitBranch.push_back(branchName); }; // Has to be called before Init() is used otherwise the default names are taken!
  // initialise all STTHit type data,  If someone wants to change this also the "Addhit" method of the Trackfinder must be adapted!
  void InitHitArray(TString branchName);

 private:
  TString fOutBranchNamePrefix;
  TString fInBranchNamePrefix;

  // for GPU computing
  Bool_t fUseGPU;
  int *fDev_tubeNeighborings = nullptr;

  std::vector<TString> fHitBranch;
  std::vector<TString> fSTTHitBranch;

  std::vector<TClonesArray *> fSTTHitArray;

  TClonesArray *fFirstTrackCandArray = nullptr;

  Bool_t fPersistence; // safe data?

  PndGeoSttPar *fSttParameters = nullptr; // needed for SttStrawMap
  TClonesArray *fTubeArray = nullptr;     // needed for SttStrawMap

  //PndSttGeometryMap *fSttGeometryMap = nullptr;
  //PndSttStrawMap *fSttStrawMap = nullptr;
  PndStt2GeoHandler *fSttGeoH = nullptr;       //!
  PndSttCA *fTrackFinder = nullptr;

  TClonesArray *fEventHeader = nullptr;

  std::vector<std::vector<int>> fNumHitsPerEvent;

  ClassDef(PndSttCATask, 1);
};

#endif /*PndSttCATask_H_*/
