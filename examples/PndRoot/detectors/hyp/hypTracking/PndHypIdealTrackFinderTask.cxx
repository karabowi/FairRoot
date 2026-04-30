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
// -----                PndHypIdealTrackFinderTask source file         -----
// -------------------------------------------------------------------------

#include "TClonesArray.h"
#include "TArrayD.h"
#include "TGeoManager.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "PndHypIdealTrackFinderTask.h"
#include "PndHypDigiStrip.h"
//#include "PndHypDigiPixel.h"
// nclude "PndHypPixelCluster.h"

// -----   Default constructor   -------------------------------------------
PndHypIdealTrackFinderTask::PndHypIdealTrackFinderTask() : FairTask("HYP Ideal Track Finding Task")
{
  fHitBranchStrip = "HypHit";
  // fHitBranchPixel = "MVDHitsPixel";
  /* fClusterBranchStrip = "MVDStripClusterCand";
  //fClusterBranchPixel = "MVDClusterCand";
  fDigiBranchStrip = "MVDStripDigis";
  //fDigiBranchPixel = "MVDPixelDigis";*/

  fMcBranch = "HypPoint";
  fTrackBranch = "MCTrack";
}

// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndHypIdealTrackFinderTask::~PndHypIdealTrackFinderTask() {}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndHypIdealTrackFinderTask::SetParContainers()
{
  // Get Base Container
  /*
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();
    fGeoPar = (PndHypGeoPar*)(rtdb->getContainer("PndHypGeoPar"));
  */
}

InitStatus PndHypIdealTrackFinderTask::ReInit()
{

  InitStatus stat = kERROR;
  return stat;

  /*
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  fGeoPar=(PndHypGeoPar*)(rtdb->getContainer("PndHypGeoPar"));

  return kSUCCESS;
  */
}

// -----   Public method Init   --------------------------------------------
InitStatus PndHypIdealTrackFinderTask::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    std::cout << "-E- PndHypIdealTrackFinderTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input array
  fStripHitArray = (TClonesArray *)ioman->GetObject(fHitBranchStrip);
  if (!fStripHitArray) {
    std::cout << "-W- PndHypIdealTrackFinderTask::Init: "
              << "No hitArray!" << std::endl;
    return kERROR;
  }

  /* fStripClusterArray = (TClonesArray*) ioman->GetObject(fClusterBranchStrip);
   if ( !fStripClusterArray){
     LOG(warn) << " PndHypIdealTrackFinderTask::Init: " << "No clusterArray!" ;
     return kERROR;
   }

   fStripDigiArray = (TClonesArray*) ioman->GetObject(fDigiBranchStrip);
   if ( !fStripDigiArray){
     LOG(warn) << " PndHypIdealTrackFinderTask::Init: " << "No digiArray!" ;
     return kERROR;
   }

 */
  fMcArray = (TClonesArray *)ioman->GetObject(fMcBranch);
  if (!fMcArray) {
    std::cout << "-W- PndHypIdealTrackFinderTask::Init: "
              << "No mcArray!" << std::endl;
    return kERROR;
  }

  fTrackArray = (TClonesArray *)ioman->GetObject(fTrackBranch);
  if (!fTrackArray) {
    std::cout << "-W- PndHypIdealTrackFinderTask::Init: "
              << "No trackArray!" << std::endl;
    return kERROR;
  }

  fTrackCandArray = new TClonesArray("GFTrackCand");
  ioman->Register("HypTrackCand", "HYP", fTrackCandArray, kTRUE);

  LOG(info) << " PndHypIdealTrackFinderTask: Initialisation successfull";
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndHypIdealTrackFinderTask::Exec(Option_t *)
{
  // Reset output array
  if (!fTrackCandArray)
    Fatal("Exec", "No trackCandArray");
  fTrackCandArray->Delete();

  Int_t nStripHits = fStripHitArray->GetEntriesFast();

  for (Int_t iHit = 0; iHit < nStripHits; iHit++) {
    PndHypHit *myHit = (PndHypHit *)(fStripHitArray->At(iHit));

    /*PndHypCluster* myCluster =(PndHypCluster*)(fStripClusterArray->At(myHit->GetRefIndex()));
    PndHypDigiStrip* astripdigi = (PndHypDigiStrip*)fStripDigiArray->At(myCluster->GetDigiIndex(0));*/

    PndHypPoint *myPoint = (PndHypPoint *)(fMcArray->At(myHit->GetRefIndex()));

    AddAndExpand(myPoint->GetTrackID(), 2, iHit);
  }

  if (fVerbose > 0)
    PrintResult();

  Int_t i = 0;
  for (std::map<Int_t, GFTrackCand *>::const_iterator ci = fTrackCandMap.begin(); ci != fTrackCandMap.end(); ci++) {
    new ((*fTrackCandArray)[i]) GFTrackCand(*(ci->second));
    i++;
  }
  ClearTrackCandMap();
  fMcArray->Delete();
  fTrackArray->Delete();
}

void PndHypIdealTrackFinderTask::AddAndExpand(Int_t trackID, Int_t detnum, Int_t iHit)
{
  if (fTrackCandMap[trackID] == 0) {
    GFTrackCand *myTCand = new GFTrackCand();
    PndMCTrack *myMCTrack = (PndMCTrack *)fTrackArray->At(trackID);
    myTCand->setCurv(GetTrackCurvature(myMCTrack));
    myTCand->setDip(GetTrackDip(myMCTrack));
    myTCand->setInverted(false);
    // myTCand->setMCID(trackID);
    fTrackCandMap[trackID] = myTCand;
  }
  fTrackCandMap[trackID]->addHit(detnum, iHit);
}

Double_t PndHypIdealTrackFinderTask::GetTrackCurvature(PndMCTrack *myTrack)
{
  TVector3 p = myTrack->GetMomentum();
  return (2 / TMath::Sqrt(p.Px() * p.Px() + p.Py() * p.Py()));
}

Double_t PndHypIdealTrackFinderTask::GetTrackDip(PndMCTrack *myTrack)
{
  TVector3 p = myTrack->GetMomentum();
  return (p.Mag() / TMath::Sqrt(p.Px() * p.Px() + p.Py() * p.Py()));
}

void PndHypIdealTrackFinderTask::PrintResult()
{
  std::cout << "**** TrackFinding *****" << std::endl;
  Int_t nStripHits = fStripHitArray->GetEntriesFast();
  for (std::map<Int_t, GFTrackCand *>::const_iterator ci = fTrackCandMap.begin(); ci != fTrackCandMap.end(); ci++) {
    std::cout << "TrackID: " << ci->first << std::endl;
    GFTrackCand *trackCand = ci->second;
    for (unsigned int i = 0; i < trackCand->getNHits(); i++) {
      unsigned int detId, hitId;
      trackCand->getHit(i, detId, hitId);
      PndHypHit *myHit;
      myHit = (PndHypHit *)fStripHitArray->At(hitId);
      std::cout << "Detector no. " << detId << ": " << *myHit;
    }
  }
  std::cout << std::endl;
}

void PndHypIdealTrackFinderTask::ClearTrackCandMap()
{
  for (std::map<Int_t, GFTrackCand *>::const_iterator ci = fTrackCandMap.begin(); ci != fTrackCandMap.end(); ci++) {
    delete (ci->second);
  }
  fTrackCandMap.clear();
}
ClassImp(PndHypIdealTrackFinderTask)
