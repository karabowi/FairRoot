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
//
//  PndEmcHitProducer
//
//  Filler of PndEmcHit
//
//  Created 14/08/06  by S.Spataro
//
///////////////////////////////////////////////////////////////

#include "PndEmcHitProducer.h"

#include "PndEmcStructure.h"
#include "PndEmcHit.h"
#include "PndEmcPoint.h"
#include "PndEmcGeoPar.h"
#include "PndEmcDigiPar.h"
#include "PndEmcDigiNonuniformityPar.h"

#include "PndEmcXtal.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairDetector.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TROOT.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"
#include "TVector3.h"
#include "TSystem.h"
#include "TString.h"

static Int_t HowManyPoints = 0;
static Int_t HowManyHitsAll = 0;
static Int_t HowManyHitsAboveThreshold = 0;

// -----   Default constructor   -------------------------------------------
PndEmcHitProducer::PndEmcHitProducer()
  : PndPersistencyTask("Ideal EMC hit Producer"), fUse_nonuniformity(0), fNonuniformityFile(""), fPointArray(), fMCTrackArray(), fHitArray(), fVolumeArray(), fMapVersion(0),
    fEnergyThreshold(0), emcX(), emcY(), emcZ(), fEmcStr(), fMapper(), fDigiPar(), fGeoPar(), fNonuniformityPar(), fDayOne(false), fCutSameTrack(10), fCutMotherParticle(20),
    fCutMotherParticleShashlyk(60), fEntryNr(0), fStoreHits(false)
{
  fNonuniformityFile = gSystem->Getenv("VMCWORKDIR");
  fNonuniformityFile += "/input/EmcDigiNoniformityPars.root";
  fEmcVolumes.push_back(new PndEmcVolumeDisk(54.2, 94, -90, 170));          // Barrel
  fEmcVolumes.push_back(new PndEmcVolumeDisk(17, 101, 200, 230));           // Forward
  fEmcVolumes.push_back(new PndEmcVolumeDisk(17, 44, -98, -55));            // Backward
  fEmcVolumes.push_back(new PndEmcVolumeBox(-150, 150, -78, 78, 783, 853)); // Shashlyk
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

PndEmcHitProducer::PndEmcHitProducer(Bool_t val)
  : PndPersistencyTask("Ideal EMC hit Producer"), fUse_nonuniformity(0), fNonuniformityFile(""), fPointArray(), fMCTrackArray(), fHitArray(), fVolumeArray(), fMapVersion(0),
    fEnergyThreshold(0), emcX(), emcY(), emcZ(), fEmcStr(), fMapper(), fDigiPar(), fGeoPar(), fNonuniformityPar(), fDayOne(false), fCutSameTrack(10), fCutMotherParticle(20),
    fCutMotherParticleShashlyk(60), fEntryNr(0), fStoreHits(false)
{
  fNonuniformityFile = gSystem->Getenv("VMCWORKDIR");
  fNonuniformityFile += "/input/EmcDigiNoniformityPars.root";
  fEmcVolumes.push_back(new PndEmcVolumeDisk(54.2, 94, -90, 170));          // Barrel
  fEmcVolumes.push_back(new PndEmcVolumeDisk(17, 101, 200, 230));           // Forward
  fEmcVolumes.push_back(new PndEmcVolumeDisk(17, 44, -98, -55));            // Backward
  fEmcVolumes.push_back(new PndEmcVolumeBox(-150, 150, -78, 78, 783, 853)); // Shashlyk
  SetPersistency(val);
}

// -----   Destructor   ----------------------------------------------------
PndEmcHitProducer::~PndEmcHitProducer()
{
  delete fEmcStr;
}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndEmcHitProducer::Init()
{

  cout << " -I- PndEmcHitProducer INITIALIZATION *********************" << endl;

  // FairDetector::Initialize();
  // FairRun* sim = FairRun::Instance();
  // FairRuntimeDb* rtdb=sim->GetRuntimeDb();

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndEmcHitProducer::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray *)ioman->GetObject("EmcPoint");
  if (!fPointArray) {
    cout << "-W- PndEmcHitProducer::Init: "
         << "No EmcPoint array!" << endl;
    return kERROR;
  }

  // Get input array
  fMCTrackArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fMCTrackArray) {
    cout << "-W- PndEmcMakeCluster::Init: "
         << "No MCTrack array! Needed for MC Truth" << endl;
    // return kERROR;
  }

  // Create and register output array
  fHitArray = new TClonesArray("PndEmcHit");

  ioman->Register("EmcHit", "Emc", fHitArray, GetPersistency());

  fGeoPar->InitEmcMapper();
  fMapper = PndEmcMapper::Instance();
  fEmcStr = PndEmcStructure::Instance();

  emcX = fEmcStr->GetEmcX();
  emcY = fEmcStr->GetEmcY();
  emcZ = fEmcStr->GetEmcZ();
  ;

  fEnergyThreshold = fDigiPar->GetEnergyHitThreshold();
  fUse_nonuniformity = fDigiPar->GetUse_nonuniformity();

  if (fUse_nonuniformity) {
    LOG(info) << " PndEmcHitProducer: Using nonuniform light output";
  }
  if (fUse_nonuniformity && fNonuniformityFile.Length() > 0) {
    TFile *nonuniformityfile = new TFile(fNonuniformityFile);
    if (nonuniformityfile == nullptr) {
      LOG(error) << " PndEmcHitProducer: Could not open file " << fNonuniformityFile.Data() << " for Non-uniformity Information";
    } else {
      PndEmcDigiNonuniParObject *parObject;
      nonuniformityfile->GetObject("PndEmcDigiNonuniParObject", parObject);
      if (parObject == nullptr) {
        LOG(error) << " PndEmcHitProducer: Could not get Non-uniformity information from file " << fNonuniformityFile.Data();
      } else {
        fNonuniformityPar->SetNonuniParObject(parObject);
      }
    }
  }

  printf("HitProducer has EnergyHitThreshold of %f GeV and Use_nonuniformity %i\n", fEnergyThreshold, fUse_nonuniformity);
  LOG(info) << " PndEmcHitProducer: Initialization successful";

  return kSUCCESS;
}

void PndEmcHitProducer::SetParContainers()
{

  // Get run and runtime database
  FairRun *run = FairRun::Instance();
  if (!run)
    LOG(fatal) << "SetParContainers: No analysis run";

  FairRuntimeDb *db = run->GetRuntimeDb();
  if (!db)
    LOG(fatal) << "SetParContainers: No runtime database";

  // Get Emc geometry parameter container
  fGeoPar = (PndEmcGeoPar *)db->getContainer("PndEmcGeoPar");

  // Get Emc digitisation parameter container
  fDigiPar = (PndEmcDigiPar *)db->getContainer("PndEmcDigiPar");

  fNonuniformityPar = (PndEmcDigiNonuniformityPar *)db->getContainer("PndEmcDigiNonuniformityPar");

  fDigiPar->setChanged();
  fDigiPar->setInputVersion(run->GetRunId(), 1);

  fNonuniformityPar->setChanged();
  fNonuniformityPar->setInputVersion(run->GetRunId(), 1);
}

// -------------------------------------------------------------------------

// Helper function, does not depend on class, identical to the one in PndEmcMakeCluster
void PndEmcHitProducer::cleansortmclist(std::vector<Int_t> &newlist, TClonesArray *mcTrackArray)
{
  std::vector<Int_t> tmplist;
  std::vector<Int_t> tmplist2;
  // Sort list...
  std::sort(newlist.begin(), newlist.end());
  // and copy every id only once (even so it might be in the list several times)
  std::unique_copy(newlist.begin(), newlist.end(), std::back_inserter(tmplist));

  // Now check if mother or (grand)^x-mother are already in the list
  // (which means i am a secondary)... if so, remove myself
  for (Int_t j = tmplist.size() - 1; j >= 0; j--) {
    bool flag = false;
    PndMCTrack *pt;
    Int_t id = tmplist[j];
    // if -1 index put it in the list and continue
    if (id < 0) {
      tmplist2.push_back(id);
      continue;
    }
    while (!flag) {
      pt = ((PndMCTrack *)mcTrackArray->At(id));
      // If the particle is primary store it and stop
      if (pt->GetMotherID() < 0) {
        tmplist2.push_back(id);
        break;
      }
      // Stop when it finds the first MCTrack not produced in emc
      TString node = gGeoManager->FindNode(pt->GetStartVertex().X(), pt->GetStartVertex().Y(), pt->GetStartVertex().Z())->GetName();
      if (!(node.BeginsWith("emc") || node.BeginsWith("CrystalVol") || node.BeginsWith("Fsc"))) {
        tmplist2.push_back(id);
        break;
      }
      // not exactly clear this part of the code, it needs to be checked (SS, 18/03/2015)
      id = pt->GetMotherID();
      for (Int_t k = j - 1; k >= 0; k--) {
        if (tmplist[k] == id) {
          tmplist.erase(tmplist.begin() + j);
          flag = true;
          break;
        }
      }
    }
  }
  newlist.clear();
  std::unique_copy(tmplist2.begin(), tmplist2.end(), std::back_inserter(newlist));
}

int PndEmcHitProducer::FindMCIndex(PndEmcPoint *point)
{
  int result = FindMCIndexIterative(point, point->GetTrackID());
  return result;
}

int PndEmcHitProducer::FindMCIndexIterative(PndEmcPoint *point, int mcIndex)
{
  PndMCTrack *track = (PndMCTrack *)fMCTrackArray->At(mcIndex);
  TVector3 pointPos(point->GetX(), point->GetY(), point->GetZ());
  TVector3 trackPos;
  double cutMotherParticle = fCutMotherParticle;
  if (point->GetZ() > 700) { // point is in the Shashlyk Calorimeter. There the distances are larger. todo: Improve implementation!
    cutMotherParticle = fCutMotherParticleShashlyk;
  }

  trackPos = (track->GetStartVertex());

  // std::cout << "FindMCIndexIterative: mcIndex " << mcIndex << " MotherID: " << track->GetMotherID() << " Distance: " << (pointPos - trackPos).Mag() << std::endl;
  bool inEmc = false;
  std::for_each(fEmcVolumes.begin(), fEmcVolumes.end(), [&inEmc, trackPos](PndEmcVolume *vol) { inEmc += vol->InsideVolume(trackPos); });

  if (track->GetMotherID() == -1 || (pointPos - trackPos).Mag() > cutMotherParticle || inEmc == false) {
    // std::cout << "FindMCIndexIterative index found " << mcIndex << std::endl;
    return mcIndex;
  } else {

    if (fLastPointForTrack.count(track->GetMotherID()) > 0) {
      double trackTime = track->GetStartTime();
      double pointTime = fLastPointForTrack[track->GetMotherID()]->GetTime() * 1E9;
      if (trackTime > pointTime)
        return fLastPointForTrack[track->GetMotherID()]->GetClusterID();
    }
    return FindMCIndexIterative(point, track->GetMotherID());
  }
}

// -----   Public method Exec   --------------------------------------------
void PndEmcHitProducer::Exec(Option_t *)
{
  if (fVerbose > 1)
    cout << " -I- PndEmcHitProducer POINT EXECUTION *********************" << endl;
  // Reset output array
  if (!fHitArray)
    Fatal("Exec", "No DigiArray");

  fHitArray->Delete();

  // Declare some variables
  // PndEmcPoint* point  = nullptr;
  Int_t DetId;

  fTrackEnergy.clear();
  fTrackTime.clear();
  fTrackMcTruth.clear();
  fPointMatch.clear();
  fTrackEntering.clear();
  fTrackExiting.clear();
  fLastPointForTrack.clear();
  fClusterIDs.clear();
  fShower.clear();

  map<Int_t, Float_t>::const_iterator p;

  std::vector<PndEmcPoint *> fPointList; // to pass to EmcHit
  const PndEmcTciXtalMap &XtalMap = fEmcStr->GetTciXtalMap();
  TVector3 frontvec;
  TVector3 normvec;
  TVector3 pointvec;
  TVector3 distvec;
  Double_t zpos;
  Double_t energyscalefactor = 1.0;
  Double_t c[3];
  PndEmcXtal *tmpXtal;
  PndEmcTwoCoordIndex *tmpTCI;
  // Loop over EmcPoints
  Int_t nPoints = fPointArray->GetEntriesFast();

  Double_t point_time = 0.00;
  //------- init containers ---

  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    PndEmcPoint *point = (PndEmcPoint *)fPointArray->At(iPoint);
    if (!AcceptDayOne(point))
      continue;
    fTrackEnergy[point->GetDetectorID()] = 0.00;
    fTrackTime[point->GetDetectorID()] = std::numeric_limits<float>::max();
  }

  //----------------------------

  // Int_t Counter[3] = {0};
  HowManyPoints += nPoints;

  //  std::cout << "PndEmcHitProducer" << std::endl;
  FairRootManager *ioman = FairRootManager::Instance();

  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    //    std::cout << "iPoint: " << iPoint << " ";
    PndEmcPoint *point = (PndEmcPoint *)fPointArray->At(iPoint);
    //    point->Print("");
    PndMCTrack *mcTrack = (PndMCTrack *)fMCTrackArray->At(point->GetTrackID());
    if (mcTrack == nullptr) {
      std::cout << "PndEmcHitProducer::Exec() mcTrack nullptr for point " << iPoint << " trackID " << point->GetTrackID() << std::endl;
      continue;
    }
    TVector3 pos(point->GetX(), point->GetY(), point->GetZ());
    TVector3 mcStartPos(mcTrack->GetStartVertex());

    if (fLastPointForTrack.count(point->GetTrackID()) == 0) { // there is no entry for this track
      bool decayTimeBeforePoint = false;
      if (fLastPointForTrack.count(mcTrack->GetMotherID()) > 0) {
        double pointTime = fLastPointForTrack[mcTrack->GetMotherID()]->GetTime() * 1E9;
        double mcTrackTime = mcTrack->GetStartTime();
        //  std::cout << "PointTime: " << pointTime << " MCTime: " << mcTrackTime << std::endl;
        if (mcTrackTime < pointTime)
          decayTimeBeforePoint = true;
      }
      if (mcTrack->GetMotherID() == -1 || fLastPointForTrack.count(mcTrack->GetMotherID()) == 0 || decayTimeBeforePoint) { // there is no entry for the mother of the track
        int trackIndex = FindMCIndex(point);
        point->SetClusterID(trackIndex);
      } else { // there is an entry for the mother
        // std::cout << "There is a mother" << std::endl;
        PndEmcPoint *motherPoint = fLastPointForTrack[mcTrack->GetMotherID()];
        TVector3 motherPos(motherPoint->GetX(), motherPoint->GetY(), motherPoint->GetZ());
        // std::cout << "Distance: " << (pos - motherPos).Mag() << " cut: " << fCutMotherParticle << std::endl;
        double cutMotherParticle = fCutMotherParticle;
        if (pos.Z() > 700) {
          cutMotherParticle = fCutMotherParticleShashlyk;
        }
        if ((pos - motherPos).Mag() < cutMotherParticle) { // check if mother point is close by
          point->SetClusterID(motherPoint->GetClusterID());
        } else {
          point->SetClusterID(point->GetTrackID());
        }
      }
    } else { // there is already an entry for the track
             // std::cout << "There is a previous point" << std::endl;
      PndEmcPoint *previousPoint = fLastPointForTrack[point->GetTrackID()];
      TVector3 previousPos(previousPoint->GetX(), previousPoint->GetY(), previousPoint->GetZ());
      // std::cout << "Distance: " << (pos - previousPos).Mag() << " cut: " << fCutSameTrack << std::endl;
      if ((pos - previousPos).Mag() < fCutSameTrack) { // check if mother point is close by
        point->SetClusterID(previousPoint->GetClusterID());
      } else {
        point->SetClusterID(point->GetTrackID());
      }
    }
    fLastPointForTrack[point->GetTrackID()] = point;

    // if (point->GetEnergyLoss() > 0) {
    //    std::cout << iPoint << " : ";
    //    point->Print("");
    TVector3 startMC = mcTrack->GetStartVertex();
    TVector3 pointVertex(point->GetX(), point->GetY(), point->GetZ());
    //   std::cout << *mcTrack << std::endl;
    //  std::cout << "Distance MC Vertex to point: " << (startMC - pointVertex).Mag() << std::endl;
    //    }

    if (!AcceptDayOne(point))
      continue;
    DetId = point->GetDetectorID();

    if (point->GetEntering()) {
      fTrackEntering[DetId].AddLinks(point->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack")));
    }
    if (point->GetExiting()) {
      fTrackExiting[DetId].AddLinks(point->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack")));
    }
    if (point->GetEnergyLoss() == 0)
      continue;
    if (point->GetModule() == 10) {
      cout << " -I-  PndEmcHitProducer::Exec"
           << "\t"
           << "Skipping Module 10 (FscFiber)" << endl;
      continue;
    }
    // if(point->GetTrackID() < 0)
    //	std::cout<<"negative track id #"<<point->GetTrackID()<<std::endl;

    // std::cout<<"point belongs to track #"<<point->GetTrackID()<<std::endl;
    // if(point->GetTrackID() == 0) ++Counter[0];
    // if(point->GetTrackID() == 1) ++Counter[1];
    // if(point->GetTrackID() == 2) ++Counter[2];

    if (fUse_nonuniformity != 0) {
      // light output is z-dependent, so calculate z

      tmpTCI = fMapper->GetTCI(DetId);
      if (tmpTCI == nullptr) {
        printf("no TCI found for DetectorID %d\n", DetId);
        continue;
      }
      tmpXtal = XtalMap.find(tmpTCI)->second;
      point->Position(pointvec);
      frontvec = tmpXtal->frontCentre();
      normvec = tmpXtal->normalToFrontFace();
      distvec = pointvec - frontvec;
      zpos = distvec.Dot(normvec);
      fNonuniformityPar->GetNonuniformityParameters(DetId, c);
      energyscalefactor = c[0] + zpos * (c[1] + zpos * c[2]);
      fShower[DetId][point->GetClusterID()] += point->GetEnergyLoss() * energyscalefactor;
      fTrackEnergy[DetId] += point->GetEnergyLoss() * energyscalefactor;
      fPointMatch[DetId].push_back(iPoint);
      //        printf("point with detID %d has z Position %f and energyloss %f scaled with %f\n",DetId,zpos, point->GetEnergyLoss(),energyscalefactor);
      //        printf("front is at x: %f y: %f z: %f\n", frontvec.X(),frontvec.Y(),frontvec.Z());
    } else {
      fShower[DetId][point->GetClusterID()] += point->GetEnergyLoss();
      fTrackEnergy[DetId] += point->GetEnergyLoss();
      fPointMatch[DetId].push_back(iPoint);
      //        printf("point with detID %d has z Position %f and energyloss %f not scaled\n",DetId,zpos, point->GetEnergyLoss());
    }
    point_time = point->GetTime();

    if (point_time < fTrackTime[point->GetDetectorID()]) {
      fTrackTime[point->GetDetectorID()] = point_time;
    }

    // Check and save MC truth information
    // Eloss==0 tracks are only stored in point, if track is entering detector from outside
    // and thats what we are interested in...
    // std::cout<<"track id #"<<point->GetTrackID()<<", Energyloss #"<<point->GetEnergyLoss()<<endl;
    if (point->GetEnergyLoss() > 0) {
      (fTrackMcTruth[point->GetDetectorID()]).push_back(point->GetTrackID());
      (fClusterIDs[point->GetDetectorID()]).insert(point->GetClusterID());
    }
    // if(point->GetEnergyLoss() == 0 ){
    //	cout << "ELoss== 0 : " <<point->GetEnergyLoss()<<", ID "<<
    //		point->GetTrackID()<<","<<point->GetDetectorID()<<","<<point->GetXPad()<<","<<point->GetYPad()<<endl;
    //}else{
    //	cout << "ELoss>0 : " <<point->GetEnergyLoss()<<", ID "<<
    //		point->GetTrackID()<<","<<point->GetDetectorID()<<","<<point->GetXPad()<<","<<point->GetYPad()<<endl;
    //}
  }
  // Loop over EmcPoint

  // Loop to register EmcHit
  Int_t idx = 0;
  for (p = fTrackEnergy.begin(); p != fTrackEnergy.end(); ++p) {
    ++HowManyHitsAll;
    ++idx;
    if ((*p).second > fEnergyThreshold) {
      ++HowManyHitsAboveThreshold;
      // Check and save MC truth information B.S.
      // remove MC Truth particles which are not needed (eg grand^x-daugherts)
      if (fMCTrackArray) {
        std::vector<Int_t> &plist = fTrackMcTruth[(*p).first];
        cleansortmclist(fTrackMcTruth[(*p).first], fMCTrackArray);
        // std::cout<<"The "<<(idx)<<" hit produced by track #";
        // for(Int_t ip=0;ip<plist.size();++ip){
        //	cout<<plist[ip]<<", ";
        //}
        // cout<<endl;
      }
      // std::vector<Int_t>& track = fTrackMcTruth[(*p).first];
      // std::cout<<track.size()<<" tracks contributes energy to this hit, ";
      // for(size_t i=0;i<track.size();++i){
      //	if(i < track.size() - 1){
      //		std::cout<<track[i]<<", ";
      //	}else{
      //		std::cout<<track[i];
      //	}
      //}
      // std::cout<<std::endl;
      AddHit(1, (*p).first, (*p).second, fTrackTime[(*p).first], fTrackMcTruth[(*p).first], fTrackEntering[(*p).first], fTrackExiting[(*p).first], fClusterIDs[(*p).first]);
      // myHit->AddLinks(FairMultiLinkedData("EmcPoint", fPointMatch[p->first]));
      // myHit->AddLinks(FairMultiLinkedData("MCTrack", fTrackMcTruth[(*p).first));
    }
  }
  // check
  // Int_t nTrack = fMCTrackArray->GetEntriesFast();
  // for(Int_t itrack = 0; itrack < nTrack; ++itrack){
  //	PndMCTrack* pt1 =((PndMCTrack*)fMCTrackArray->At(itrack));
  //	if(pt1->IsGeneratorCreated()){
  //		pt1->Print(itrack);
  //	}
  //}
}

// -------------------------------------------------------------------------
// -----   Private method AddDigi   --------------------------------------------
PndEmcHit *PndEmcHitProducer::AddHit(Int_t trackID, Int_t detID, Float_t energy, Float_t time, std::vector<Int_t> &mctruth, FairMultiLinkedData entering,
                                     FairMultiLinkedData exiting, std::set<Int_t> &clusterIDs)
{
  // It fills the PndEmcHit category

  // cout << "PndEmcHitProducer: track " << trackID << " evt " << eventID
  //<< " sec " << sec << " plane " << pla << " strip " << strip << "box
  //" << box << " tube " << tub << endl;
  TClonesArray &clref = *fHitArray;
  Int_t size = clref.GetEntriesFast();
  PndEmcHit *hit = new (clref[size]) PndEmcHit(trackID, detID, energy, time, emcX[detID], emcY[detID], emcZ[detID], mctruth, entering, exiting, clusterIDs);

  hit->SetDepositedEnergyMap(fShower[detID]);
  // hit->Print();
  return hit;
}
// ----

void PndEmcHitProducer::SetStorageOfData(Bool_t val)
{
  fStoreHits = val;
  return;
}

void PndEmcHitProducer::FinishTask()
{
  std::cout << "=========================================================" << std::endl;
  std::cout << "PndEmcHitProducer::FinishTask" << std::endl;
  std::cout << "*********************************************************" << std::endl;
  if (fDayOne)
    std::cout << " DAY 1 Setup active, only 12/16 Slices available " << std::endl;
  std::cout << "Read points # " << HowManyPoints << std::endl;
  std::cout << "Produc hits# " << HowManyHitsAll << ", threshold# " << fEnergyThreshold << std::endl;
  std::cout << "Hits above threshhod#" << HowManyHitsAboveThreshold << std::endl;
  std::cout << "*********************************************************" << std::endl;
}

bool PndEmcHitProducer::AcceptDayOne(PndEmcPoint *p)
{
  if (!fDayOne)
    return true;
  if (p->GetDetectorID() > 250e6)
    return true; // afaik this is convention for the barrel
  float phi = p->GetPhi();
  if (abs(phi - 90) < 22.5)
    return false;
  if (abs(phi - 270) < 22.5)
    return false;
  if (abs(phi + 90) < 22.5)
    return false;
  return true;
}

ClassImp(PndEmcHitProducer)
