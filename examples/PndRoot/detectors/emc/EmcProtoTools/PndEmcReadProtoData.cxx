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

#include "PndEmcReadProtoData.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "TParticle.h"
#include "PndEmcMapper.h"

PndEmcReadProtoData::PndEmcReadProtoData(ReadMainzProto60 *datareader, Bool_t StoreHits, Int_t debuglevel)
  : lDataReader(datareader), lStoreHits(StoreHits), lOnlyPrompt(kFALSE), lOnlyTagged(kFALSE), lDebug(debuglevel), IsInit(kFALSE){};

InitStatus PndEmcReadProtoData::Init()
{
  lDataReader->Reset();

  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    printf("-E- PndEmcHitProducer::Init: RootManager not instantiated!\n");
    return kFATAL;
  }

  lHitArray = new TClonesArray("PndEmcHit");
  ioman->Register("EmcHit", "Emc", lHitArray, lStoreHits);

  lDigiArray = new TClonesArray("PndEmcDigi");
  ioman->Register("EmcDigi", "Emc", lDigiArray, lStoreHits);

  lGammaArray = new TClonesArray("TParticle");
  ioman->Register("TaggedGamma", "Emc", lGammaArray, lStoreHits);

  printf("Reading info from PndEmcDigiPar\n");

  //    FairRun* run = FairRun::Instance();
  //    run->GetRuntimeDb()->initContainers(0);

  lEnergyHitThreshold = lDigiPar->GetEnergyHitThreshold();
  lEnergyDigiThreshold = lDigiPar->GetEnergyDigiThreshold();
  lGeoPar->SetMapperVersion(10);
  lGeoPar->InitEmcMapper();

  printf("PndEmcReadProtoData: Init successfull\n");

  IsInit = kTRUE;
  return kSUCCESS;
}

void PndEmcReadProtoData::SetParContainers()
{

  // Get run and runtime database
  FairRun *run = FairRun::Instance();
  if (!run)
    LOG(fatal) << "SetParContainers: No analysis run";

  FairRuntimeDb *db = run->GetRuntimeDb();
  if (!db)
    LOG(fatal) << "SetParContainers: No runtime database";

  // Get Emc digitisation parameter container
  lDigiPar = (PndEmcDigiPar *)db->getContainer("PndEmcDigiPar");

  // Get Emc geometry parameter container
  lGeoPar = (PndEmcGeoPar *)db->getContainer("PndEmcGeoPar");

  lGeoPassivePar = (PndGeoPassivePar *)db->getContainer("PndGeoPassivePar");
  lBaseParSet = (FairBaseParSet *)db->getContainer("FairBaseParSet");

  if (lDigiPar != nullptr) {
    printf("Got PndEmcDigiPar from database\n");
    lDigiPar->Print();
  } else {
    printf("could not get PndEmcDigiPar from database\n");
    abort();
  }
}

Bool_t PndEmcReadProtoData::OnlyTagged(Bool_t onlyTagged)
{
  lOnlyTagged = onlyTagged;
  return lOnlyTagged;
}

Bool_t PndEmcReadProtoData::OnlyPrompt(Bool_t onlyPrompt)
{
  lOnlyPrompt = onlyPrompt;
  return lOnlyPrompt;
}

void PndEmcReadProtoData::Exec(Option_t *)
{
  if (!IsInit) {
    printf("Not Initialised! You have to call Init() first.\n");
    return;
  }

  TClonesArray &HitArrayRef = *lHitArray;
  TClonesArray &DigiArrayRef = *lDigiArray;
  TClonesArray &GammaArrayRef = *lGammaArray;
  GammaArrayRef.Clear();
  HitArrayRef.Clear();
  DigiArrayRef.Clear();

  lDataReader->ReadNextEvent();
  lDataReader->GetEnergies(lEnergies);
  lDataReader->GetADCValues(lADCs);
  lDataReader->GetTimes(lTimes);
  lDataReader->GetTaggerTimes(lTaggerTimes);
  lDataReader->GetTaggerEnergies(lTaggerEnergies);
  // lDataReader->PrintEvent();

  Int_t NumberOfPhotons = 0;
  Int_t NumberOfHits = 0;
  Int_t NumberOfDigis = 0;
  Int_t DetectorId = 0;
  Int_t Row = 0;
  Int_t Col = 0;
  for (Int_t i = 0; i < 16; i++) {
    if (lTaggerTimes[i] > 10 && lTaggerEnergies[i] > 0) {
      NumberOfPhotons = GammaArrayRef.GetEntriesFast();
      new (GammaArrayRef[NumberOfPhotons]) TParticle(22, 0, 0, 0, 0, 0, 0, 0, lTaggerEnergies[i] / 1000, lTaggerEnergies[i] / 1000, 0, 0, 0, lTaggerTimes[i]);
    }
  }
  if (!lOnlyTagged || GammaArrayRef.GetEntriesFast() > 0) {
    for (Int_t n = 0; n < 60; n++) {
      if ((!lOnlyPrompt || lTimes[n] > 10) && lEnergies[n] > lEnergyHitThreshold * 1000) {
        NumberOfHits = HitArrayRef.GetEntriesFast();
        if (NumberOfHits > 60) {
          printf("more than 60 Hits in this event, aborting");
          return;
        }
        Row = n / 10 + 1;
        Col = n % 10 + 1;
        DetectorId = 700010000 + Col + Row * 1000000;
        new (HitArrayRef[NumberOfHits]) PndEmcHit(1, DetectorId, lADCs[n], lTimes[n], 0, 0, 0);

        if (lEnergies[n] > lEnergyDigiThreshold * 1000) {
          NumberOfDigis = DigiArrayRef.GetEntriesFast();
          new (DigiArrayRef[NumberOfDigis]) PndEmcDigi(1, DetectorId, lEnergies[n] / 1000, lTimes[n], NumberOfHits);
        }
      }
    }
  }
}
