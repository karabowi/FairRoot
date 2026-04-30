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

#include "PndEmcReadProto192Data.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "TParticle.h"
#include "PndEmcMapper.h"
#include "PndEmcMultiWaveform.h"

PndEmcReadProto192Data::PndEmcReadProto192Data(TTree *prototree, TTree *taggertree, Bool_t StoreHits, Int_t debuglevel)
  : lProto192Tree(prototree), lTaggerTree(taggertree), lStoreHits(StoreHits), lDebug(debuglevel), IsInit(kFALSE), signal(256), lTaggerTreeOffset(0), lEventNumOverflow(0)
{
  for (Int_t i = 0; i < 64; i++) {
    lIndexMap[i] = 0;
    lWaveformMap[i] = 1;
  }
};

InitStatus PndEmcReadProto192Data::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    printf("-E- PndEmcHitProducer::Init: RootManager not instantiated!\n");
    return kFATAL;
  }

  lWaveArray = new TClonesArray("PndEmcMultiWaveform");
  ioman->Register("EmcMultiWaveform", "Emc", lWaveArray, lStoreHits);

  lGeoPar->SetMapperVersion(1);
  lGeoPar->InitEmcMapper();

  printf("PndEmcReadProto192Data: Init successfull\n");

  lProto192Event = new TProtoUnpackEvent;
  lProto192Tree->SetBranchAddress("UnpackEvent.", &lProto192Event);
  lProto192EventNum = 0;

  if (lTaggerTree != nullptr) {
    lTaggerArray = new TClonesArray("TParticle");
    ioman->Register("BeamPhotons", "Emc", lTaggerArray, lStoreHits);
    lTaggerTree->SetBranchAddress("BeamPhotons", &lTaggerArray);
    lTaggerTree->SetBranchAddress("EventNumber", &lTaggerEventNum);
  }

  IsInit = kTRUE;
  return kSUCCESS;
}

void PndEmcReadProto192Data::SetParContainers()
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

void PndEmcReadProto192Data::SetIndexMap(Long_t *map)
{
  for (Int_t i = 0; i < 64; i++) {
    lIndexMap[i] = map[i];
  }
}

void PndEmcReadProto192Data::SetWaveformMap(Long_t *map)
{
  for (Int_t i = 0; i < 64; i++) {
    lWaveformMap[i] = map[i];
  }
}

void PndEmcReadProto192Data::Exec(Option_t *)
{
  if (!IsInit) {
    printf("Not Initialised! You have to call Init() first.\n");
    return;
  }

  TClonesArray &WaveArrayRef = *lWaveArray;
  //    printf("Size of Wavearray: %i ",WaveArrayRef.GetEntries());
  WaveArrayRef.Delete();
  lProto192Tree->GetEntry(lProto192EventNum);
  PndEmcMultiWaveform *waveform;
  std::map<Long_t, PndEmcMultiWaveform *> wavemap;
  std::map<Long_t, PndEmcMultiWaveform *>::iterator it;
  Long_t detid;
  for (Int_t channel = 0; channel < 64; channel++) {
    detid = lIndexMap[channel];
    if (detid != 0) {
      it = wavemap.find(detid);
      if (it != wavemap.end()) {
        waveform = (*it).second;
      } else {
        waveform = new (WaveArrayRef[WaveArrayRef.GetEntriesFast()]) PndEmcMultiWaveform(1, detid, 256, channel);
        wavemap.insert(std::pair<Long_t, PndEmcMultiWaveform *>(detid, waveform));
      }
      for (Int_t i = 0; i < 256; i++) {
        signal[i] = lProto192Event->waveform[channel][i];
      }
      waveform->SetWaveform(signal, 256, lWaveformMap[channel]);
    }
  }
  if (lTaggerTree != nullptr) {
    Long_t cbevent = lProto192Event->rawData[0] + lTaggerTreeOffset + lEventNumOverflow;
    if (cbevent > 0) {
      lTaggerTree->GetEntry(cbevent - 1);
      if (lTaggerEventNum != cbevent) {
        printf("-E- PndEmcReadProto192Data: No Matching Tagger Event found for event %li\n", cbevent);
      }
    }
    if (lProto192Event->rawData[0] == 65535) {
      lEventNumOverflow += 65536;
    }
  }
  //    delete lProto192Event;
  //    lProto192Event= nullptr;
  lProto192EventNum++;
}
