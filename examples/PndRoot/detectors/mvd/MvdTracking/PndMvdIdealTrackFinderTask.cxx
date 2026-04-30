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
// -----                PndMvdIdealTrackFinderTask source file             -----
// -------------------------------------------------------------------------

#include "TClonesArray.h"
#include "TArrayD.h"
#include "TGeoManager.h"

#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "PndMvdIdealTrackFinderTask.h"

#include "PndSdsDigiPixel.h"
#include "PndSdsDigiStrip.h"
// #include "PndMvdPixelCluster.h"

// -----   Default constructor   -------------------------------------------
PndMvdIdealTrackFinderTask::PndMvdIdealTrackFinderTask()
  : FairTask("MVD Ideal Track Finding Task"), fHitBranchStrip("MVDHitsStrip"), fHitBranchPixel("MVDHitsPixel"), fClusterBranchStrip("MVDStripClusterCand"),
    fClusterBranchPixel("MVDPixelClusterCand"), fDigiBranchStrip("MVDStripDigis"), fDigiBranchPixel("MVDPixelDigis"), fMcBranch("MVDPoint"), fTrackBranch("MCTrack"),
    fStripHitArray(nullptr), fPixelHitArray(nullptr), fStripClusterArray(nullptr), fPixelClusterArray(nullptr), fStripDigiArray(nullptr), fPixelDigiArray(nullptr),
    fMcArray(nullptr), fTrackArray(nullptr), fTrackCandArray(nullptr), fTrackCandMap()
{
}

// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMvdIdealTrackFinderTask::~PndMvdIdealTrackFinderTask() {}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndMvdIdealTrackFinderTask::SetParContainers()
{
  // Get Base Container
  /*
    FairRun* ana = FairRun::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();
    fGeoPar = (PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar"));
  */
}

InitStatus PndMvdIdealTrackFinderTask::ReInit()
{

  InitStatus stat = kSUCCESS;
  return stat;

  /*
  FairRun* ana = FairRun::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  fGeoPar=(PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar"));

  return kSUCCESS;
  */
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdIdealTrackFinderTask::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    std::cout << "-E- PndMvdIdealTrackFinderTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input array
  fStripHitArray = (TClonesArray *)ioman->GetObject(fHitBranchStrip);
  if (!fStripHitArray) {
    std::cout << "-W- PndMvdIdealTrackFinderTask::Init: "
              << "No fStripHitArray!" << std::endl;
    return kERROR;
  }

  fPixelHitArray = (TClonesArray *)ioman->GetObject(fHitBranchPixel);
  if (!fPixelHitArray) {
    std::cout << "-W- PndMvdIdealTrackFinderTask::Init: "
              << "No fPixelHitArray!" << std::endl;
    return kERROR;
  }

  fStripClusterArray = (TClonesArray *)ioman->GetObject(fClusterBranchStrip);
  if (!fStripClusterArray) {
    std::cout << "-W- PndMvdIdealTrackFinderTask::Init: "
              << "No StripclusterArray!" << std::endl;
    return kERROR;
  }
  fPixelClusterArray = (TClonesArray *)ioman->GetObject(fClusterBranchPixel);
  if (!fPixelClusterArray) {
    std::cout << "-W- PndMvdIdealTrackFinderTask::Init: "
              << "No PixelclusterArray!" << std::endl;
    return kERROR;
  }

  fStripDigiArray = (TClonesArray *)ioman->GetObject(fDigiBranchStrip);
  if (!fStripDigiArray) {
    std::cout << "-W- PndMvdIdealTrackFinderTask::Init: "
              << "No StripdigiArray!" << std::endl;
    return kERROR;
  }

  fPixelDigiArray = (TClonesArray *)ioman->GetObject(fDigiBranchPixel);
  if (!fPixelDigiArray) {
    std::cout << "-W- PndMvdIdealTrackFinderTask::Init: "
              << "No PixeldigiArray!" << std::endl;
    return kERROR;
  }

  fMcArray = (TClonesArray *)ioman->GetObject(fMcBranch);
  if (!fMcArray) {
    std::cout << "-W- PndMvdIdealTrackFinderTask::Init: "
              << "No mcArray!" << std::endl;
    return kERROR;
  }

  fTrackArray = (TClonesArray *)ioman->GetObject(fTrackBranch);
  if (!fTrackArray) {
    std::cout << "-W- PndMvdIdealTrackFinderTask::Init: "
              << "No trackArray!" << std::endl;
    return kERROR;
  }

  fTrackCandArray = new TClonesArray("PndTrackCand");
  ioman->Register("MVDIdealTrackCand", "MVD", fTrackCandArray, kTRUE);

  LOG(info) << " PndMvdIdealTrackFinderTask: Initialisation successfull";
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndMvdIdealTrackFinderTask::Exec(Option_t *)
{

  // Reset output array
  if (!fTrackCandArray)
    Fatal("Exec", "No trackCandArray");
  fTrackCandArray->Delete();

  Int_t nStripHits = fStripHitArray->GetEntriesFast();
  Int_t nPixelHits = fPixelHitArray->GetEntriesFast();
  // TODO: Thi becomes easier now, with the FairHit::GetRefIndex()
  // pixel part
  for (Int_t iHit = 0; iHit < nPixelHits; iHit++) {
    PndSdsHit *myHit = (PndSdsHit *)(fPixelHitArray->At(iHit));
    PndSdsCluster *myCluster = (PndSdsCluster *)(fPixelClusterArray->At(myHit->GetClusterIndex()));
    PndSdsDigiPixel *apixeldigi = (PndSdsDigiPixel *)fPixelDigiArray->At(myCluster->GetDigiIndex(0));
    if (apixeldigi->GetIndex(0) == -1)
      continue; // sort out noise
    PndSdsMCPoint *myPoint = (PndSdsMCPoint *)(fMcArray->At(apixeldigi->GetIndex(0)));

    AddAndExpand(myPoint->GetTrackID(), apixeldigi->GetDetID(), iHit, myHit);
  }
  // strip part
  for (Int_t iHit = 0; iHit < nStripHits; iHit++) {
    PndSdsHit *myHit = (PndSdsHit *)(fStripHitArray->At(iHit));
    PndSdsCluster *myCluster = (PndSdsCluster *)(fStripClusterArray->At(myHit->GetClusterIndex()));
    PndSdsDigiStrip *astripdigi = (PndSdsDigiStrip *)fStripDigiArray->At(myCluster->GetDigiIndex(0));
    if (astripdigi->GetIndex(0) == -1)
      continue; // sort out noise
    PndSdsMCPoint *myPoint = (PndSdsMCPoint *)(fMcArray->At(astripdigi->GetIndex(0)));

    AddAndExpand(myPoint->GetTrackID(), astripdigi->GetDetID(), iHit, myHit);
  }

  if (fVerbose > 0)
    PrintResult();

  Int_t i = 0;
  for (std::map<Int_t, PndTrackCand *>::const_iterator kIt = fTrackCandMap.begin(); kIt != fTrackCandMap.end(); kIt++) {
    Double_t eventTime = FairRootManager::Instance()->GetEventTime();
    kIt->second->SetTimeStamp(eventTime);
    new ((*fTrackCandArray)[i]) PndTrackCand(*(kIt->second));
  }
  ClearTrackCandMap();
}

void PndMvdIdealTrackFinderTask::AddAndExpand(Int_t trackID, Int_t detnum, Int_t iHit, PndSdsHit *theHit)
{
  if (fTrackCandMap[trackID] == 0) {
    PndTrackCand *myTCand = new PndTrackCand();
    // PndMCTrack* myMCTrack = (PndMCTrack*)fTrackArray->At(trackID); //[R.K.03/2017] unused variable
    //    myTCand->setCurv(GetTrackCurvature(myMCTrack));
    //    myTCand->setDip(GetTrackDip(myMCTrack));
    //    myTCand->setInverted(false);
    // int pdg = myMCTrack->GetPdgCode(); //[R.K.02/2017] Unused variable?
    // double charge; //[R.K.02/2017] Unused variable?
    // if(pdg<100000000){ //[R.K.02/2017] Unused variable?
    // charge =  TDatabasePDG::Instance()->GetParticle(pdg)->Charge()/3.; //[R.K.02/2017] Unused variable?
    //} //[R.K.02/2017] Unused variable?
    // else{ //[R.K.02/2017] Unused variable?
    // charge = 0.; //[R.K.02/2017] Unused variable?
    //} //[R.K.02/2017] Unused variable?
    //    myTCand->setTrackSeed(
    //			  myMCTrack->GetStartVertex(),
    //			  myMCTrack->GetMomentum(),
    //			  charge/myMCTrack->GetMomentum().Mag()
    //			  );
    myTCand->setMcTrackId(trackID);
    fTrackCandMap[trackID] = myTCand;
  }

  fTrackCandMap[trackID]->AddHit(detnum, iHit, theHit->GetPosition().Mag());
}

Double_t PndMvdIdealTrackFinderTask::GetTrackCurvature(PndMCTrack *myTrack)
{
  TVector3 p = myTrack->GetMomentum();
  return (2 / TMath::Sqrt(p.Px() * p.Px() + p.Py() * p.Py()));
}

Double_t PndMvdIdealTrackFinderTask::GetTrackDip(PndMCTrack *myTrack)
{
  TVector3 p = myTrack->GetMomentum();
  return (p.Mag() / TMath::Sqrt(p.Px() * p.Px() + p.Py() * p.Py()));
}

void PndMvdIdealTrackFinderTask::PrintResult()
{
  std::cout << "**** TrackFinding *****" << std::endl;
  Int_t nStripHits = fStripHitArray->GetEntriesFast();
  for (std::map<Int_t, PndTrackCand *>::const_iterator kIt = fTrackCandMap.begin(); kIt != fTrackCandMap.end(); kIt++) {
    std::cout << "TrackID: " << kIt->first << std::endl;
    PndTrackCand *trackCand = kIt->second;
    unsigned int detId, hitId;
    for (unsigned int i = 0; i < trackCand->GetNHits(); i++) {
      detId = trackCand->GetSortedHit(i).GetDetId();
      hitId = trackCand->GetSortedHit(i).GetHitId();
      PndSdsHit *myHit;
      if (hitId < (UInt_t)nStripHits)
        myHit = (PndSdsHit *)(fStripHitArray->At(hitId));
      else
        myHit = (PndSdsHit *)(fPixelHitArray->At(hitId - nStripHits));
      std::cout << "Detector no. " << detId << ": " << *myHit;
    }
  }
  std::cout << std::endl;
}

void PndMvdIdealTrackFinderTask::ClearTrackCandMap()
{
  for (std::map<Int_t, PndTrackCand *>::const_iterator kIt = fTrackCandMap.begin(); kIt != fTrackCandMap.end(); kIt++) {
    delete (kIt->second);
  }
  fTrackCandMap.clear();
}
ClassImp(PndMvdIdealTrackFinderTask);
