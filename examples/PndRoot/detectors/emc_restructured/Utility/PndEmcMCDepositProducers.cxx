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


#include "PndEmcMCDepositProducers.h"

#include "PndPersistencyTask.h"
#include "PndProcessTask.h"

#include "BSEmcBarrelZDependentMCDepositProductionProcess.h"
#include "BSEmcCounter.h"
#include "BSEmcDataBranchNames.h"
#include "BSEmcMCDepositProducerTask.h"

#include "BSEmcMCDepositProductionProcess.h"
#include "BSEmcParameterLoadingProcess.h"

//---------------------------------------------------------------

BSEmcBarrelMCDepositProducer::BSEmcBarrelMCDepositProducer()
    : PndPersistencyTask("BSEmcBarrelMCDepositProducer") {
  PndProcessTask *barrelDepositProducer = new BSEmcMCDepositProducerTask("Barrel");
  BSEmcMCDepositProductionProcess *barrelDepositProducerProcess = new BSEmcBarrelZDependentMCDepositProductionProcess();
  barrelDepositProducerProcess->SetVolume(new BSEmcVolumeDisk(54.2, 94, -90, 170));
  barrelDepositProducer->AddProcess(barrelDepositProducerProcess);
  this->Add(barrelDepositProducer);
}

//---------------------------------------------------------------

BSEmcBwEndcapMCDepositProducer::BSEmcBwEndcapMCDepositProducer()
    : PndPersistencyTask("BSEmcBwEndcapMCDepositProducer") {
  PndProcessTask *bwDepositProducer = new BSEmcMCDepositProducerTask("BwEndcap");
  BSEmcMCDepositProductionProcess *bwDepositProducerProcess = new BSEmcMCDepositProductionProcess();
  bwDepositProducerProcess->SetVolume(new BSEmcVolumeDisk(17, 44, -98, -55));
  bwDepositProducer->AddProcess(bwDepositProducerProcess);
  this->Add(bwDepositProducer);
}

//---------------------------------------------------------------

BSEmcFwEndcapMCDepositProducer::BSEmcFwEndcapMCDepositProducer()
    : PndPersistencyTask("BSEmcFwEndcapMCDepositProducer") {
  PndProcessTask *fwDepositProducer = new BSEmcMCDepositProducerTask("FwEndcap");
  BSEmcMCDepositProductionProcess *fwDepositProducerProcess = new BSEmcMCDepositProductionProcess();
  fwDepositProducerProcess->SetVolume(new BSEmcVolumeDisk(17, 101, 200, 230));
  fwDepositProducer->AddProcess(fwDepositProducerProcess);
  this->Add(fwDepositProducer);
}

//---------------------------------------------------------------

BSEmcShashlikMCDepositProducer::BSEmcShashlikMCDepositProducer()
    : PndPersistencyTask("BSEmcShashlikMCDepositProducer") {
  PndProcessTask *shashlikDepositProducer = new BSEmcMCDepositProducerTask("Shashlik");

  BSEmcMCDepositProductionProcess *shashlikDepositProducerProcess = new BSEmcMCDepositProductionProcess();
  shashlikDepositProducerProcess->SetVolume(new BSEmcVolumeBox(-150, 150, -78, 78, 783, 853));
  shashlikDepositProducer->AddProcess(shashlikDepositProducerProcess);
  this->Add(shashlikDepositProducer);
}

//---------------------------------------------------------------

BSEmcMCDepositProducer::BSEmcMCDepositProducer()
    : PndPersistencyTask("BSEmcMCDepositProducer") {
  TString vmcdir = gSystem->Getenv("VMCWORKDIR");
  TString qadir = vmcdir + "/macro/params/";
  PndProcessTask *general = new BSEmcMCDepositProducerTask("General");
  general->AddProcess(new BSEmcParameterLoadingProcess(TString{qadir + "EmcCrystalsNeighbouringRelations.txt"}.Data()));
  general->AddProcess(new BSEmcCounter());

  this->Add(general);
  this->Add(new BSEmcBarrelMCDepositProducer);
  this->Add(new BSEmcBwEndcapMCDepositProducer);
  this->Add(new BSEmcFwEndcapMCDepositProducer);
  this->Add(new BSEmcShashlikMCDepositProducer);
}
