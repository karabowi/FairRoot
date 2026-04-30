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
// -----                PndMdtClusterTask source file                  -----
// -----                  Created 04/04/11  by  S.Spataro              -----
// -------------------------------------------------------------------------

#include "PndMdtClusterTask.h"

#include "PndMdtCluster.h"
#include "PndMdtDigi.h"

#include "FairRootManager.h"
#include "FairDetector.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndMdtClusterTask::PndMdtClusterTask() : PndPersistencyTask("MDT Cluster Task")
{
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMdtClusterTask::~PndMdtClusterTask() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndMdtClusterTask::Init()
{

  cout << "-I- PndMdtClusterTask::Init: "
       << "INITIALIZATION *********************" << endl;

  // FairRun* sim = FairRun::Instance(); //[R.K. 01/2017] unused variable?
  // FairRuntimeDb* rtdb=sim->GetRuntimeDb(); //[R.K. 01/2017] unused variable?

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndMdtClusterTask::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fBoxIArray = (TClonesArray *)ioman->GetObject("MdtDigiBox");
  if (!fBoxIArray) {
    cout << "-W- PndMdtClusterTask::Init: "
         << "No MdtDigiBox array!" << endl;
    return kERROR;
  }

  fStripIArray = (TClonesArray *)ioman->GetObject("MdtDigiStrip");
  if (!fStripIArray) {
    cout << "-W- PndMdtClusterTask::Init: "
         << "No MdtDigiStrip array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fBoxOArray = new TClonesArray("PndMdtCluster");
  fStripOArray = new TClonesArray("PndMdtCluster");
  ioman->Register("MdtBoxCluster", "Mdt", fBoxOArray, GetPersistency());
  ioman->Register("MdtStripCluster", "Mdt", fStripOArray, GetPersistency());

  LOG(info) << " PndMdtClusterTask: Intialization successfull";

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndMdtClusterTask::Exec(Option_t *)
{

  // Reset output array
  fBoxOArray->Delete();
  fStripOArray->Delete();

  if (!MdtMapping())
    return;

  if (mapBox.size() > 0) {
    map<Int_t, Int_t>::const_iterator box_iter;
    map<Int_t, Int_t>::const_iterator find_iter;
    for (box_iter = mapBox.begin(); box_iter != mapBox.end(); ++box_iter) // layer loop
    {
      std::vector<Int_t> boxList; // list of digis
      boxList.push_back((*box_iter).first);

      Int_t nn = 1;
      while (1) {
        find_iter = mapBox.find((*box_iter).first + nn);
        if (find_iter == mapBox.end())
          break; // if no close box
        boxList.push_back((*box_iter).first + nn);
        mapBox.erase((*box_iter).first + nn); // delete already used cells
        nn++;
      }
      PndMdtCluster *boxClu = AddClusterBox(boxList);
      boxClu->SetLinks(FairMultiLinkedData("MdtDigiBox", boxList));
    }

  } // end of box loop

  if (mapStrip.size() > 0) {
    map<Int_t, Int_t>::const_iterator strip_iter;
    map<Int_t, Int_t>::const_iterator find_iter;
    for (strip_iter = mapStrip.begin(); strip_iter != mapStrip.end(); ++strip_iter) // layer loop
    {
      std::vector<Int_t> stripList; // list of digis
      stripList.push_back((*strip_iter).first);

      Int_t nn = 1;
      while (1) {
        find_iter = mapStrip.find((*strip_iter).first + nn);
        if (find_iter == mapStrip.end())
          break; // if no close strip
        stripList.push_back((*strip_iter).first + nn);
        mapStrip.erase((*strip_iter).first + nn); // delete already used cells
        nn++;
      }
      PndMdtCluster *stripClu = AddClusterStrip(stripList);
      stripClu->SetLinks(FairMultiLinkedData("MdtDigiStrip", stripList));
    }

  } // end of strip loop
}

// -----   Private method MdtMapping   --------------------------------------------
Bool_t PndMdtClusterTask::MdtMapping()
{
  // Reset output array
  Int_t nBox = fBoxIArray->GetEntriesFast();
  Int_t nStrip = fStripIArray->GetEntriesFast();
  if (nBox == 0 || nStrip == 0)
    return kFALSE;

  Reset();
  PndMdtDigi *mdtDigi = nullptr;
  // Mapping BOX digis per layer
  for (Int_t iDigi = 0; iDigi < nBox; iDigi++) {
    mdtDigi = (PndMdtDigi *)fBoxIArray->At(iDigi);
    // each box has 8 cells. let's change the detId to have continuity
    Int_t newDetectorID = mdtDigi->GetWire() + 8 * mdtDigi->GetBox() + 1000 * mdtDigi->GetLayerID() + 100000 * mdtDigi->GetSector() + 1000000 * mdtDigi->GetModule();
    mapBox[newDetectorID] = iDigi;
  }

  // Mapping STRIP digis per layer
  for (Int_t iDigi = 0; iDigi < nStrip; iDigi++) {
    mdtDigi = (PndMdtDigi *)fStripIArray->At(iDigi);
    // strips are continuous in detId
    mapStrip[mdtDigi->GetDetectorID()] = iDigi;
  }

  return kTRUE;
}

// -----   Private method Reset   --------------------------------------------
void PndMdtClusterTask::Reset()
{
  // reset maps
  mapBox.clear();
  mapStrip.clear();
}

// -------------------------------------------------------------------------

// -----   Private method AddCluster   --------------------------------------------
PndMdtCluster *PndMdtClusterTask::AddClusterBox(std::vector<Int_t> digiList)
{
  // It fills the PndMdtCluster category
  TClonesArray &clref = *fBoxOArray;
  Int_t size = clref.GetEntriesFast();
  return new (clref[size]) PndMdtCluster(digiList);
}
// ----

// -----   Private method AddCluster   --------------------------------------------
PndMdtCluster *PndMdtClusterTask::AddClusterStrip(std::vector<Int_t> digiList)
{
  // It fills the PndMdtCluster category

  TClonesArray &clref = *fStripOArray;
  Int_t size = clref.GetEntriesFast();
  return new (clref[size]) PndMdtCluster(digiList);
}
// ----

ClassImp(PndMdtClusterTask)
