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

// using timestamp for the clock counts
// and timestamperr to store the spill resets (just a temporary solution)

// libc includes
#include <iostream>
#include <map>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"

// framework includes
#include "FairRootManager.h"
// PndMvd includes
#include "PndMvdConvertApvTask.h"
#include "PndMvdConvertApv.h"
#include "PndMvdBoxMap.h"

#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "PndMvdContFact.h"
#include "PndSdsStripDigiPar.h"

#include "PndSdsDigiStrip.h"
#include <vector>

using namespace std;

// -----   Default constructor   -------------------------------------------
PndMvdConvertApvTask::PndMvdConvertApvTask(PndMvdConvertApv *Apvconvert, PndMvdBoxMap *Apvmapper)
  : FairTask(), fPersistance(kTRUE), fApvConvert(Apvconvert), fApvMapper(Apvmapper), fStripArray(nullptr), fGeoH(PndGeoHandling::Instance()), iStrip(0),
    fDigiParameterList(new TList()), fBotSides()
{
}
// -----   Destructor   ----------------------------------------------------
PndMvdConvertApvTask::~PndMvdConvertApvTask() {}

void PndMvdConvertApvTask::SetParContainers()
{ // in this task we even don't need the digitization info

  cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
  cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
  cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
  cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
  cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

  FairRun *ana = FairRun::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  PndMvdContFact *themvdcontfact = (PndMvdContFact *)rtdb->getContFactory("PndMvdContFact");
  TList *theContNames = themvdcontfact->GetDigiParNames();
  Info("SetParContainers()", "The container names list contains %i entries", theContNames->GetEntries());
  TIter cfIter(theContNames);
  while (TObjString *contname = (TObjString *)cfIter()) {
    TString parsetname = contname->String();
    if (parsetname.BeginsWith("MVDStripDigiPar")) {
      PndSdsStripDigiPar *digipar = (PndSdsStripDigiPar *)(rtdb->getContainer(parsetname.Data()));
      Info("SetParcontiners()", "check some values. fDigiParameterList: %p  digipar: %p", fDigiParameterList, digipar);
      fDigiParameterList->Add(digipar);

      Info("SetParContainers()", "Loaded container %s", parsetname.Data());
    }
  }

  return;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdConvertApvTask::Init()
{

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndMvdStripHitProducer::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Create and register output array
  fStripArray = new TClonesArray("PndSdsDigiStrip");
  ioman->Register("MVDStripDigis", "MVD", fStripArray, fPersistance);
  fApvConvert->Init();
  fApvMapper->Init();
  cout << "Init of Task finished" << endl;

  // set up the geohandler
  std::vector<std::string> listOfSensitives;
  listOfSensitives.push_back("StripActive");
  fGeoH->CreateUniqueSensorId("", listOfSensitives);

  if (fVerbose > 1)
    fGeoH->PrintSensorNames();

  TIter parsetiter(fDigiParameterList);
  while (PndSdsStripDigiPar *digipar = (PndSdsStripDigiPar *)parsetiter()) {
    cout << "Debugging, sens type: " << (TString)digipar->GetSensType() << ", num of bot FE: " << digipar->GetNrBotFE() << endl;
    if (digipar->GetNrBotFE() == 1) { // we count top side first from 0; Bot side strarts at #top fe's
      fBotSides[(TString)digipar->GetSensType()] = digipar->GetNrTopFE();
    }
  }

  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndMvdConvertApvTask::Exec(Option_t *)
{

  //	cout << "--*--" << endl;

  // Reset output array
  fStripArray->Delete();

  std::map<Int_t, Double_t> singleSidedBacksideMap;
  std::map<Int_t, std::vector<Int_t>> buffIndex;

  Int_t nbox, ch;
  // Int_t rw=-1;
  Int_t sw = -1, botfe = -1;
  TString detpath = "";
  Int_t detnameid;
  Int_t stripnum;
  Int_t buffCh = -1;
  Double_t buffClock;
  Double_t buffSpill;

  std::vector<PndSdsDigiStrip> strips = fApvConvert->ReadNext();
  for (std::vector<PndSdsDigiStrip>::iterator strip = strips.begin(); strip != strips.end(); ++strip) {
    // rw=strip->GetFE();
    // sw=strip->GetFE();

    nbox = strip->GetSensorID();
    // ch = strip -> GetChannel();
    buffCh = strip->GetChannel();
    fApvMapper->DoMapping(nbox, buffCh, detpath);
    detnameid = fGeoH->GetShortID(detpath);

    buffClock = strip->GetTimeStamp();
    buffSpill = strip->GetTimeStampError();

    if (fVerbose > 1)
      Info("Exec", "Write a Digi from detector %s %i", detpath.Data(), detnameid);
    stripnum = fStripArray->GetEntriesFast();
    // cout << "stripnum: " << stripnum << endl;

    sw = (Int_t)buffCh / 128.;
    ch = (Int_t)buffCh % 128;

    if (IsSingleSided(detpath)) {
      // cout << "Detected as single sided: " << detpath.Data() << endl;
      if (sw > 2) {
        sw = sw - 3;
      }
    }

    // cout << "Box: " << nbox << ", channel: " << buffCh << ", nome: " << detpath.Data() << ", iD: " << detnameid << ", FE: " << sw << ", corrCh: " << ch << endl;

    PndSdsDigiStrip *buffStr = new PndSdsDigiStrip(strip->GetIndices(), strip->GetDetID(), detnameid, sw, ch, strip->GetCharge(), strip->GetTimeStamp());
    buffStr->SetTimeStampError(strip->GetTimeStampError());

    new ((*fStripArray)[stripnum]) PndSdsDigiStrip(*buffStr);

    // new ((*fStripArray)[stripnum]) PndSdsDigiStrip(strip->GetIndices(), strip->GetDetID(),detnameid, sw, ch, strip->GetCharge() ,0);
    // collect information of fake bottom sides if singlesided
    if (IsSingleSided(detpath)) { // collect information of fake bottom sides if singlesided
      singleSidedBacksideMap[detnameid] += strip->GetCharge();
      (buffIndex[detnameid]).push_back(strip->GetIndex());
    }
  }

  // writing the single sided fake backside
  for (std::map<Int_t, Double_t>::iterator it = singleSidedBacksideMap.begin(); it != singleSidedBacksideMap.end(); ++it) {
    stripnum = fStripArray->GetEntriesFast();
    botfe = CalcBotFakeFE(fGeoH->GetPath(it->first));

    PndSdsDigiStrip *buffStr2 = new PndSdsDigiStrip(buffIndex[it->first], static_cast<int>(DetectorType::kMVDHitsStrip), it->first, botfe, 0, it->second, buffClock);
    buffStr2->SetTimeStampError(buffSpill);

    new ((*fStripArray)[stripnum]) PndSdsDigiStrip(*buffStr2);

    // new ((*fStripArray)[stripnum])PndSdsDigiStrip(buffIndex[it->first], DetectorType::kMVDHitsStrip,it->first, botfe, 0, it->second, 0);
  }
}

Bool_t PndMvdConvertApvTask::IsSingleSided(TString &detpath)
{

  if (!(detpath.Contains("Strip")))
    return kFALSE;

  for (std::map<TString, Int_t>::iterator it = fBotSides.begin(); it != fBotSides.end(); it++) {
    if (detpath.Contains(it->first)) {
      // cout << "Sensor type: " << it->first << endl;
      // cout << "Path: " << detpath << endl;
      return kTRUE;
    }
  }

  return kFALSE;
}

Int_t PndMvdConvertApvTask::CalcBotFakeFE(TString detpath)
{

  for (std::map<TString, Int_t>::iterator it = fBotSides.begin(); it != fBotSides.end(); it++) {
    if (detpath.Contains(it->first)) {
      return it->second;
    }
  }
  return -99999; // return something silly
}

void PndMvdConvertApvTask::Finish() {}

ClassImp(PndMvdConvertApvTask);
