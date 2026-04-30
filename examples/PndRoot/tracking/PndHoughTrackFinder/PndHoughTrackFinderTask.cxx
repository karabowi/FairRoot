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

/////////////////////////////////////////////////////////////
//  PndHoughTrackFinderTask
//  Finds Track for one event
/////////////////////////////////////////////////////////////////

/** PndHoughTrackFinderTask
 *@author Anna Alicke <a.alicke@fz-juelich.de>
 *@since 01.06.2020
 *@updated 23.11.2021
 *@version 2.1
 **
 ** PANDA task class for finding tracks based on the HoughTrackFinder
 ** Task level RECO
 **/

// Includes from base
#include "FairRuntimeDb.h"
#include "FairRunAna.h"
#include <FairField.h>
#include "FairLogger.h"
// Pnd includes
//#include "PndSttMapCreator.h"
#include "PndStt2GeoHandler.h"
#include "PndTrack.h"
#include "PndTrackCand.h"
#include "PndHoughTrackFinderTask.h"
// general
using std::cout;
using std::endl;

// -----   Constructor   -------------------------------------------
PndHoughTrackFinderTask::PndHoughTrackFinderTask()
  : FairTask("HoughTrackFinder"), fNbins1(450), fNbins2(450), fNBinsSeg(90.), fCutMergeByHoughSpace(7.), fWithCuda(kFALSE), fWithGhostReduction(kTRUE),
    fPreselectedTrackCandName("")
{
}

// -----   Destructor   ----------------------------------------------------
PndHoughTrackFinderTask::~PndHoughTrackFinderTask()
{
  delete fPndHoughTrackFinder;
  delete fPreselectedTrackArrayCand;
  delete fApolloniusTrackArrayMerged;
  delete fApolloniusTrackArrayMergedCand;
  delete fSttParameters;
  //delete fSttTubeArray;
}
// -----   Private method SetParContainers   -------------------------------
void PndHoughTrackFinderTask::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
// This is the initialization method for the HoughTrackFinder task.
// It is called once before analyzing all events and loads the main data from the simulation and digitization.
//
// @return     The initialize status.
//
InitStatus PndHoughTrackFinderTask::Init()
{

  // Get RootManager
  ioman = FairRootManager::Instance();

  if (!ioman) {
    std::cout << "-E- PndHoughTrackFinderTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  if (fBranchNames.size() == 0) {
    // Use hits of all tracking subsystems if nothing is given
    AddBranchName("MVDHitsPixel");
    AddBranchName("MVDHitsStrip");
    AddBranchName("STTHit");
    AddBranchName("GEMHit");
  }

  for (size_t i = 0; i < fBranchNames.size(); i++) {
    if (ioman->GetObject(fBranchNames[i]) != 0) {
      fBranchMap[fBranchNames[i]] = (TClonesArray *)ioman->GetObject(fBranchNames[i]);
    }
  }

  // ----------------------------------------   maps of STT tubes
  //PndSttMapCreator *mapperStt = new PndSttMapCreator(fSttParameters);
  //fSttTubeArray = mapperStt->FillTubeArray();
  //delete mapperStt;
  FairField *Field = FairRunAna::Instance()->GetField();
  Double_t po[3], BB[3];
  po[0] = 0.;
  po[1] = 0.;
  po[2] = 0.;
  Field->GetFieldValue(po, BB);

  //PndStt2GeoHandler *geoH = new PndStt2GeoHandler(fSttParameters);
  PndStt2GeoHandler *geoH = PndStt2GeoHandler::Instance(fSttParameters);
  fPndHoughTrackFinder = new PndHoughTrackFinder(geoH);
  //fPndHoughTrackFinder = new PndHoughTrackFinder(fSttTubeArray);
  fPndHoughTrackFinder->SetBz(BB[2] / 10.);
  fPndHoughTrackFinder->SetBinningX(fNbins1);
  fPndHoughTrackFinder->SetBinningY(fNbins2);
  fPndHoughTrackFinder->SetNBinsSeg(fNBinsSeg);
  fPndHoughTrackFinder->SetCutMergeByHoughSpace(fCutMergeByHoughSpace);
  fPndHoughTrackFinder->SetWithGPU(fWithCuda);
  fPndHoughTrackFinder->SetWithGhostReduction(fWithGhostReduction);

  fPndHoughTrackFinder->Init();

  if (fPreselectedTrackCandName == "")
    fPreselectedTrackArrayCand = ioman->Register("fPreselectedTrackArrayCand", "PndTrackCand", "tracking", kTRUE);
  else
    fPreselectedTrackArrayCand = (TClonesArray *)ioman->GetObject(fPreselectedTrackCandName);

  fCombinedSkewed = (TClonesArray *)ioman->GetObject("STTCombinedSkewedHits");
  if (fCombinedSkewed == nullptr)
    std::cerr << "-E- PndHoughTrackFinderTask::Init: cannot find \"STTCombinedSkewedHits\" array!" << std::endl;
  fApolloniusTrackArrayMerged = ioman->Register("fApolloniusTrackArrayMerged", "PndTrack", "tracking", kTRUE);
  fApolloniusTrackArrayMergedCand = ioman->Register("fApolloniusTrackArrayMergedCand", "PndTrackCand", "tracking", kTRUE);
  // fPreselectedTrackArrayCand = (TClonesArray *)ioman->GetObject("fPreselectedTrackArrayCand");

  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
// This is the execution method for the HoughTrackFinder task.
// It is the method that evaluates each event.
//
//
void PndHoughTrackFinderTask::Exec(Option_t *)
{
  LOG(debug1) << "event nr.: " << ioman->GetEntryNr() << endl;

  fPndHoughTrackFinder->Reset();
  fApolloniusTrackArrayMerged->Delete();
  fApolloniusTrackArrayMergedCand->Delete();
  // fPreselectedTrackArrayCand->Delete();
  std::vector<PndTrackCand> cands;
  cands.clear();

  if (fPreselectedTrackCandName == "")
    fPreselectedTrackArrayCand->Delete();
  else {
    fPndHoughTrackFinder->UseDefaultPreselection(kFALSE);
    for (int i = 0; i < fPreselectedTrackArrayCand->GetEntriesFast(); i++) {
      PndTrackCand *cand = (PndTrackCand *)fPreselectedTrackArrayCand->At(i);
      cands.push_back(*(cand));
    }
    fPndHoughTrackFinder->SetPreselectedTrackCands(cands);
  }
  fPndHoughTrackFinder->SetCombinedSkewed(fCombinedSkewed);
  for (std::map<TString, TClonesArray *>::iterator iter = fBranchMap.begin(); iter != fBranchMap.end(); iter++) {
    fPndHoughTrackFinder->AddHits(iter->second, iter->first);
  }

  fPndHoughTrackFinder->FindTracks();
  LOG(debug1) << "fPndHoughTrackFinder->GetNumMergedTracks(): " << fPndHoughTrackFinder->GetNumMergedTracks() << endl;

  for (int i = 0; i < fPndHoughTrackFinder->GetNumMergedTracks(); i++) {
    PndTrackCand cand = fPndHoughTrackFinder->GetMergedTrack(i).GetTrackCand();
    if (!CheckZInfo(cand))
      continue;
    PndTrackCand *myCand =
      new ((*fApolloniusTrackArrayMergedCand)[fApolloniusTrackArrayMergedCand->GetEntriesFast()]) PndTrackCand(fPndHoughTrackFinder->GetMergedTrack(i).GetTrackCand());
    PndTrack *myTrack = new ((*fApolloniusTrackArrayMerged)[fApolloniusTrackArrayMerged->GetEntriesFast()]) PndTrack(fPndHoughTrackFinder->GetMergedTrack(i));
  }
  LOG(debug1) << "fApolloniusTrackArrayMerged: " << fApolloniusTrackArrayMerged->GetEntriesFast() << std::endl;
}

bool PndHoughTrackFinderTask::CheckZInfo(PndTrackCand &cand)
{

  int MvdGemCounter = 0;
  int SkewedCounter = 0;
  for (int i = 0; i < cand.GetNHits(); i++) {
    FairLink link = cand.GetSortedHit(i);
    if (ioman->GetBranchName(link.GetType()) == "MVDHitsPixel" || ioman->GetBranchName(link.GetType()) == "MVDHitsStrip" || ioman->GetBranchName(link.GetType()) == "GEMHit")
      MvdGemCounter++;
    else {
      Int_t tubeID = (fPndHoughTrackFinder->GetData()->GetMapFairLinktoTubeId())[link];
      LOG(debug1) << "tubeID: " << tubeID << " is skewed? " << fPndHoughTrackFinder->GetData()->GetSttGeoH()->IsSkewedStraw(tubeID) << std::endl;
      //LOG(debug1) << "tubeID: " << tubeID << " is skewed? " << fPndHoughTrackFinder->GetData()->GetStrawMap()->IsSkewedStraw(tubeID) << std::endl;
      if (fPndHoughTrackFinder->GetData()->GetSttGeoH()->IsSkewedStraw(tubeID))
      //if (fPndHoughTrackFinder->GetData()->GetStrawMap()->IsSkewedStraw(tubeID))
        SkewedCounter++;
    }
  }
  LOG(debug1) << "event: " << ioman->GetEntryNr() << " MVD+GEM: " << MvdGemCounter << ", skewed: " << SkewedCounter << std::endl;
  if (MvdGemCounter >= 2 || MvdGemCounter + SkewedCounter / 2 >= 2)
    return true;

  return false;
}

void PndHoughTrackFinderTask::Finish() {}

ClassImp(PndHoughTrackFinderTask)
