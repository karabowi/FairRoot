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

//-----------------------------------------------------------
//-----------------------------------------------------------

// Panda Headers ----------------------

#include "PndFtsCATracking.h"

#include <iostream>
#include <cmath>
#include <random>

#include "TClonesArray.h"
#include "TParticlePDG.h"
#include "PndMCTrack.h"

#include "FairRootManager.h"
//#include "FairGeanePro.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairField.h"
#include "FairLogger.h"
#include "PndTrackCand.h"
#include "PndTrack.h"

// fts
#include "PndFtsPoint.h"
#include "PndFtsHit.h"
#include "PndFtsTube.h"
#include "PndFtsMapCreator.h"
#include "PndGeoFtsPar.h"

#include "TGeoManager.h"
#include "PndCATrackFtsMCPointContainer.h"

#include <algorithm>
#include <vector>
using namespace std;

// -----------

#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <map>
using namespace std;

#include "PndFTSCATrackParam.h"
#include "PndFTSCAGBTracker.h"
#include "PndFTSCAPerformance.h"
#include "TFile.h"

ClassImp(PndFtsCATracking);

vector<int> allFwdTrackIds;

Int_t PndFtsCATracking::fVerbose = 0;

bool compareFtsPoints(PndFtsPoint *const a, PndFtsPoint *const b)
{
  return (a->GetTime() < b->GetTime());
}

PndFtsCATracking::PndFtsCATracking(const char *name, Int_t iVerbose)
  : PndPersistencyTask(name, iVerbose), fFtsTracksBranchName("FtsCaTracks"), fFtsTrackCandsBranchName("FtsCaTrackCands") /*, fMCTracks(0)*/, fTracks(nullptr), fDoPerformance(0),
    fTracker(nullptr), fPerfHistoFile(nullptr)
{
  fVerbose = iVerbose;

  fTracker = new PndFTSCAGBTracker;

  // string fP = "settings.data";
  string fts_geometry_str = "48\
  -10\
    0 294.895 0.00044 0.0117 0 1 6\
    1 295.77 0.00044 0.0117 0 1 6\
    2 299.39 0.00044 0.0117 0.0871557 1 6\
    3 300.265 0.00044 0.0117 0.0871557 1 6\
    4 304.39 0.00044 0.0117 -0.0871557 1 6\
    5 305.265 0.00044 0.0117 -0.0871557 1 6\
    6 309.39 0.00044 0.0117 0 1 6\
    7 310.265 0.00044 0.0117 0 1 6\
    8 326.895 0.00044 0.0117 0 1 6\
    9 327.77 0.00044 0.0117 0 1 6\
    10 331.39 0.00044 0.0117 0.0871557 1 6\
    11 332.265 0.00044 0.0117 0.0871557 1 6\
    12 336.39 0.00044 0.0117 -0.0871557 1 6\
    13 337.265 0.00044 0.0117 -0.0871557 1 6\
    14 341.39 0.00044 0.0117 0 1 6\
    15 342.265 0.00044 0.0117 0 1 6\
    16 393.995 0.00044 0.0117 0 1 6\
    17 394.87 0.00044 0.0117 0 1 6\
    18 400.965 0.00044 0.0117 0.0871557 1 6\
    19 401.84 0.00044 0.0117 0.0871557 1 6\
    20 415.49 0.00044 0.0117 -0.0871557 1 6\
    21 416.365 0.00044 0.0117 -0.0871557 1 6\
    22 422.965 0.00044 0.0117 0 1 6\
    23 423.84 0.00044 0.0117 0 1 6\
    24 437.49 0.00044 0.0117 0 1 6\
    25 438.365 0.00044 0.0117 0 1 6\
    26 444.965 0.00044 0.0117 0.0871557 1 6\
    27 445.84 0.00044 0.0117 0.0871557 1 6\
    28 459.49 0.00044 0.0117 -0.0871557 1 6\
    29 460.365 0.00044 0.0117 -0.0871557 1 6\
    30 466.965 0.00044 0.0117 0 1 6\
    31 467.84 0.00044 0.0117 0 1 6\
    32 606.995 0.00044 0.0117 0 1 6\
    33 607.87 0.00044 0.0117 0 1 6\
    34 611.49 0.00044 0.0117 0.0871557 1 6\
    35 612.365 0.00044 0.0117 0.0871557 1 6\
    36 616.49 0.00044 0.0117 -0.0871557 1 6\
    37 617.365 0.00044 0.0117 -0.0871557 1 6\
    38 621.49 0.00044 0.0117 0 1 6\
    39 622.365 0.00044 0.0117 0 1 6\
    40 638.995 0.00044 0.0117 0 1 6\
    41 639.87 0.00044 0.0117 0 1 6\
    42 643.49 0.00044 0.0117 0.0871557 1 6\
    43 644.365 0.00044 0.0117 0.0871557 1 6\
    44 648.49 0.00044 0.0117 -0.0871557 1 6\
    45 649.365 0.00044 0.0117 -0.0871557 1 6\
    46 653.49 0.00044 0.0117 0 1 6\
    47 654.365 0.00044 0.0117 0 1 6";

  std::istringstream settings(fts_geometry_str);
  fTracker->ReadSettings(settings);

  SetPersistency(kTRUE);

  // cout<<"READGEOM \n";
  // fTracker->ReadSettingsFromFile(fP);

  //#ifdef DO_TPCCATRACKER_EFF_PERFORMANCE
  // fDoPerformance = 1;

  // TFile* curFile = gFile;
  // TDirectory* curDirectory = gDirectory;

  ////static TFile* performanceHistoFile = 0; //[R.K. 9/2018] unused
  // string filePrefix = "./CATrackerData";
  // filePrefix += "/";

  // if( fDoPerformance ){
  // if( !fPerfHistoFile ){
  // fPerfHistoFile = new TFile( (filePrefix + "CATrackerPerformance.root").data(), "RECREATE" );
  // if( !fPerfHistoFile->IsOpen() ){
  // gSystem->Exec( "mkdir ./CATrackerData");
  // fPerfHistoFile = new TFile( (filePrefix + "CATrackerPerformance.root").data(), "RECREATE" );
  //}
  //}

  // fPerformance =  &PndFTSCAPerformance::Instance();
  // fPerformance->SetOutputFile(fPerfHistoFile);
  // fPerformance->CreateHistos();
  //}

  // gFile = curFile;
  // gDirectory = curDirectory;
  //#endif
}

PndFtsCATracking::~PndFtsCATracking()
{
  if (fTracker)
    delete fTracker;
}

InitStatus PndFtsCATracking::Init()
{
  // Initialize magnetic field
  fTracker->GetParametersNonConst().InitMagneticField();

  // ---
  if (fVerbose > 3)
    Info("Init", "Start initialisation.");

  FairRootManager *fManager = FairRootManager::Instance();

  //// Get MC arrays
  // fMCTracks = dynamic_cast<TClonesArray *>(fManager->GetObject("MCTrack"));
  // if ( ! fMCTracks ) {
  // std::cout << "-W-  PndFtsTrackerIdeal::Init: No MCTrack array! Needed for MC Truth" << std::endl;
  // return kERROR;
  //}
  ////FTS
  // fMCPoints = dynamic_cast<TClonesArray *> (fManager->GetObject("FTSPoint"));
  // if ( !fMCPoints ) {
  // std::cout << "-W-  PndFtsTrackerIdeal::Init: No FTSPoint array!" << std::endl;
  // return kERROR;
  //}
  fHits = (TClonesArray *)(fManager->GetObject("FTSHit"));
  if (!fHits) {
    LOG(warn) << "  PndFtsTrackerIdeal::Init: No FTSHit array!";
    return kERROR;
  }
  // FTS tube geometry
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  PndGeoFtsPar *ftsParameters = (PndGeoFtsPar *)rtdb->getContainer("PndGeoFtsPar");

  if (ftsParameters->GetGeometryType() != -1) {
    PndFtsMapCreator *mapper = new PndFtsMapCreator(ftsParameters);
    fTubeArrayFts = mapper->FillTubeArray();
  }

  // fBranchID  =  FairRootManager::Instance()->GetBranchId("FTSHit");

  // output track array
  std::cout << "PndFtsCATracking::Init() register branches \"" << fFtsTracksBranchName << "\" and \"" << fFtsTrackCandsBranchName << "\"" << std::endl;
  fTrackCands = new TClonesArray("PndTrackCand");
  fManager->Register(fFtsTrackCandsBranchName, "FtsCA", fTrackCands, kTRUE);
  fTracks = new TClonesArray("PndTrack");
  fManager->Register(fFtsTracksBranchName, "FtsCA", fTracks, kTRUE);
  std::cout << "PndFtsCATracking::Init() registeing done" << std::endl;

  return kSUCCESS;
}

void PndFtsCATracking::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  rtdb->getContainer("PndGeoSttPar");
  rtdb->getContainer("PndGeoFtsPar");
}

vector<int> ftslabels;
// vector <PndFTSCALocalMCPoint> ftsmcpoints;
// vector <PndFTSCAMCTrack> ftsmctracks;

bool PndFtsCATracking::NonReconstructableEvent()
{
  int NHits = fTracker->GetHitsSize();
  // non-reconstructable due to very few hits
  if (NHits < PndFTSCAParameters::MinimumHitsForRecoTrack)
    return true;
  // explanation for this upper-limit see in perStationCounts-cycle
  /*if (fHits.Size()>240)
    return true;*/

  vector<int> perStationCounts(6);
  for (int i = 0; i < NHits; i++) {
    // we've got 6 blocks (each block has 8 sub-stations)
    for (int j = 0; j < 6; j++) {
      if (((((fTracker->Hit(i)).IRow() + 1) / 8) < (1 + j)) && ((((fTracker->Hit(i)).IRow() + 1) / 8) > (-1 + j))) {
        perStationCounts[j]++;
        break;
      }
    }
  }

  for (unsigned int j = 0; j < perStationCounts.size(); j++) {
    // cout<<"perStationCounts[j] "<<perStationCounts[j]<<endl;

    // NHits/Station-limit; 40 assumes 40*6=240 hits/event limit
    if (perStationCounts[j] > 40)
      //       if (perStationCounts[j]>20) // l_r
      return true;
    // don't allow sequences of empty station-blocks due
    // to non-reconstructable circumstances of such events
    if (j < 3) {
      // first and last block have hits but 2 between them are empty
      /*if (((perStationCounts[j+1]+perStationCounts[j+2])<4) && ( (perStationCounts[j]>0) && (perStationCounts[j+3]>0)))
      return true;
      //3 empty blocks sequences ("0+1+2", "1+2+3" or "2+3+4") and 1 following block with few hits
      if (((perStationCounts[j]+perStationCounts[j+1]+perStationCounts[j+2])<4)
      && (perStationCounts[j+3]<4))
      return true;*/
      // 4 empty blocks sequences("0+1+2+3","1+2+3+4" or "2+3+4+5")
      if ((perStationCounts[j] + perStationCounts[j + 1] + perStationCounts[j + 2] + perStationCounts[j + 3]) < 4)
        return true;
    }
    // 5 empty block sequences("0+1+2+3+4" or "1+2+3+4+5")
    if (j < 2) {
      if ((perStationCounts[j] + perStationCounts[j + 1] + perStationCounts[j + 2] + perStationCounts[j + 3] + perStationCounts[j + 4]) < 4)
        return true;
    }
  }

  // case for cylindrically curved tracks (when mc-track has more than 1 hit/station; or if z_hit_AnywereBefore_last_hit > z_last_hit)
  // PndFTSCAPerformance* perf = &PndFTSCAPerformance::Instance();
  // const int NMCTracks = perf->GetMCTracks()->Size();
  /*int NMCTracks_reconstructable=0;
  for(int iT=0; iT<NMCTracks; iT++)
  {
    int nMCPoints = (*perf->GetMCTracks())[iT].NMCPoints();
    if (nMCPoints>PndFTSCAParameters::MinimumHitsForRecoTrack)
      NMCTracks_reconstructable++;
  }*/
  // for(int iT=0; iT<NMCTracks; iT++)
  //{
  // int nFirstMC = (*perf->GetMCTracks())[iT].FirstMCPointID();
  // PndFTSCALocalMCPoint *points = &((*perf->GetMCPoints()).Data()[nFirstMC]);
  // int nMCPoints = (*perf->GetMCTracks())[iT].NMCPoints();
  ///*dbg
  // for (int iP1=0; iP1<nMCPoints-1; iP1++)
  //{
  // cout<<"points[iP1].Z() "<<points[iP1].Z()<<endl;
  //}*/
  // if (nMCPoints<PndFTSCAParameters::MinimumHitsForRecoTrack) continue;
  // for (int iP1=0; iP1<nMCPoints-1; iP1++)
  //{
  // for (int iP2=iP1+1; iP2<nMCPoints; iP2++)
  //{
  ///*
  ////skip events which have only a single cylindrical track
  // if ((points[iP1].Z()>points[iP2].Z()) && (NMCTracks_reconstructable==1))
  // return true;
  ////if NMCTracks>1 - mark cylindrical tracks as non-reconstructable and process event further
  //*/
  // if (points[iP1].Z()>points[iP2].Z())
  //(*perf->GetMCTracks())[iT].SetIsForwardTrack(false);
  // break;
  //}
  //}
  //}
  return false;
}

bool PndFtsCATracking::CATrackParToFairTrackParP(FairTrackParP *fairParam, const PndFTSCATrackParam *kfParam)
{
  const double cA = TMath::Cos(kfParam->Angle());
  const double sA = -TMath::Sin(kfParam->Angle());

  Double_t x = kfParam->X();
  if (x != x)
    return false;
  Double_t y = kfParam->Y();
  if (y != y)
    return false;
  Double_t z = kfParam->Z();
  if (z != z)
    return false;
  Double_t tx = kfParam->Tx();
  if (tx != tx)
    return false;
  Double_t ty = kfParam->Ty();
  if (ty != ty)
    return false;
  Double_t qp = kfParam->QP();
  if (qp != qp)
    return false;

  Double_t q = kfParam->QP() > 0 ? 1 : -1;

  // Double_t covCA[15];

  // for(int i=0; i<15; i++)
  // covCA[i] = kfParam->Cov(i);

  /*
  double mB = 1/TMath::Sqrt(1 + kfParam->DzDs()*kfParam->DzDs());
  double mA = -kfParam->QPt() * kfParam->DzDs() *mB*mB*mB;
  double mC = 1/kfParam->GetCosPhi();
  double mE = mC*mC*mC;
  double mD = kfParam->DzDs() * kfParam->SinPhi() * mC*mC*mC;

  cov1[14] = cov[2];
  cov1[13] = cov[1];
  cov1[12] = cov[0];
  cov1[11] = mE *cov[7] + cov[4]* mD;
  cov1[10] = mE *cov[6] + cov[3]* mD;
  cov1[ 9] = mE*mE* cov[9] + 2* mE *cov[8]* mD + cov[5] *mD*mD;
  cov1[ 8] = mC *cov[4];
  cov1[ 7] = mC *cov[3];
  cov1[ 6] = mC *(mE* cov[8] + cov[5]* mD);
  cov1[ 5] = mC*mC* cov[5];
  cov1[ 4] = mB *cov[11] + mA *cov[7];
  cov1[ 3] = mB *cov[10] + mA *cov[6];
  cov1[ 2] = mB *mE *cov[13] + mA* mE* cov[9] + mB* cov[12] *mD + mA* cov[8]* mD;
  cov1[ 1] = mC *(mB *cov[12] + mA* cov[8]);
  cov1[ 0] = 2*mA *mB *cov[13] + mB*mB* cov[14] + mA*mA* cov[9];
  */

  // TODO not clear whether we should invert the covariance mtx
  // fairParam->SetTrackPar(x, y, tx, ty, qp, cov, TVector3(0,0,0), TVector3(-sA,cA,0), TVector3(cA,sA,0), TVector3(0,0,-1), q);
  // last parameter is SPU, a direction information for propagation later on.
  //      SPU       SIGN OF U-COMPONENT OF PARTICLE MOMENTUM
  //                SPU = sign[p.(DJ x DK)]  --> I think this is tx, then (Ralf)
  // fairParam->SetTrackPar(x, y, tx, ty, qp, cov, TVector3(0,0,z), TVector3(-sA,cA,0), TVector3(cA,sA,0), TVector3(0,0,-1), (tx < 0) ? -1 : 1);
  // fairParam->SetTrackPar(x, y, tx, ty, qp, cov, TVector3(0,0,z), TVector3(cA,sA,0), TVector3(-sA,cA,0), TVector3(0,0,1), (tx < 0) ? -1 : 1);

  Double_t covSD[15];
  // Transform from CA system (x,y,tx,ty,q/p) to GEANE SD system (q/p,tv,tw,v,w)
  covSD[0] = kfParam->Cov(14);  // 00 cov( q/p- q/p ) = covCA44 = covCA(14)
  covSD[1] = kfParam->Cov(12);  // 10 cov( tv - q/p ) = covCA42 = covCA(12)
  covSD[2] = kfParam->Cov(5);   // 11 cov( tv - tv  ) = covCA22 = covCA( 5)
  covSD[3] = kfParam->Cov(13);  // 20 cov( tw - q/p ) = covCA43 = covCA(13)
  covSD[4] = kfParam->Cov(8);   // 21 cov( tw - tv  ) = covCA32 = covCA( 8)
  covSD[5] = kfParam->Cov(9);   // 22 cov( tw - tw  ) = covCA33 = covCA( 9)
  covSD[6] = kfParam->Cov(10);  // 30 cov( v  - q/p ) = covCA40 = covCA(10)
  covSD[7] = kfParam->Cov(3);   // 31 cov( v  - tv  ) = covCA20 = covCA( 3)
  covSD[8] = kfParam->Cov(6);   // 32 cov( v  - tw  ) = covCA30 = covCA( 6)
  covSD[9] = kfParam->Cov(0);   // 33 cov( v  - v   ) = covCA00 = covCA( 0)
  covSD[10] = kfParam->Cov(11); // 40 cov( w  - q/p ) = covCA41 = covCA(11)
  covSD[11] = kfParam->Cov(4);  // 41 cov( w  - tv  ) = covCA21 = covCA( 4)
  covSD[12] = kfParam->Cov(7);  // 42 cov( w  - tw  ) = covCA31 = covCA( 7)
  covSD[13] = kfParam->Cov(1);  // 43 cov( w  - v   ) = covCA10 = covCA( 1)
  covSD[14] = kfParam->Cov(2);  // 44 cov( w  - w   ) = covCA11 = covCA( 2)
  // Caution we move from a plane,normal in ((x,y),z) to a plane/normal (u,(v,w))
  fairParam->SetTrackPar(x, y, tx, ty, qp, covSD, TVector3(0, 0, z), TVector3(0, 0, 1), TVector3(cA, sA, 0), TVector3(-sA, cA, 0), (tx < 0) ? -1 : 1);

  // Double_t v=sqrt(x*x+y*y);
  // Double_t w=z;
  // Double_t tv=...
  // Double_t tW=...
  // fairParam->SetTrackPar(v, w, tv, tw, qp, covVW, TVector3(0,0,z), TVector3(cA,sA,0), TVector3(-sA,cA,0), TVector3(0,0,1), (tx < 0) ? -1 : 1);

  if (fVerbose > 0)
    std::cout << "PndFtsCATracking::CATrackParToFairTrackParP: q=" << q << "  q/p=(" << qp << ")"
              << "  (x,y,z)=(" << x << "," << y << "," << z << ")"
              << "  (tx,ty)=(" << tx << "," << ty << ")"
              << " Angle: " << kfParam->Angle() << " cA=" << cA << " sA=" << sA << std::endl;

  fairParam->Print();
  return true;
}

void PndFtsCATracking::Exec(Option_t * /*opt*/) //[R.K. 9/2018] unused
{
  if (fVerbose > 1)
    std::cout << "PndFtsCATracking::Exec" << std::endl;

  fTrackCands->Delete();
  fTracks->Delete();

  fTracker->StartEvent();

  static int iEvent = -1;
  iEvent++;

  if (fVerbose > 1)
    cout << "iEvent " << iEvent << endl;

  PndFtsHit *ghit = nullptr;
  // PndFtsPoint* myPoint=nullptr;
  // std::map<Int_t, PndMCTrack*> mctracklist;
  Int_t nFtsHits = 0;
  // Int_t nPoints = 0;
  ////const Int_t nMCTracks = fMCTracks->GetEntriesFast();
  ////Int_t nMCTracks=0;
  // unsigned int nMCPoints=0;
  ////Int_t prevTrackID=-1;

  ////vector <Int_t> TrackIds;
  // map<int, unsigned int> nHitsInMCTrack, nMCPointsInMCTrack, FirstMCPointIDInMCTrack;
  ////PndCATrackFtsMCPointContainer* MCTrackSortedArray = new PndCATrackFtsMCPointContainer[fMCTracks->GetEntriesFast()];
  // vector < vector <PndFtsPoint*> > MCTrackSortedArray(fMCTracks->GetEntriesFast());

  for (Int_t ih = 0; ih < fHits->GetEntriesFast(); ih++) {
    ghit = (PndFtsHit *)fHits->At(ih);
    if (!ghit)
      continue;
    nFtsHits++;
    // Int_t mchitid=ghit->GetRefIndex();
    // if(mchitid<0) continue;
    // myPoint = (PndFtsPoint*)(fMCPoints->At(mchitid));
    // if(!myPoint) continue;
    // Int_t trackID = myPoint->GetTrackID();
    // if(trackID<0) continue;
    // nPoints++;
    ////PndMCTrack* mctr = mctracklist[trackID];
    ////if(nullptr==mctr)
    ////{
    ////    mctr=new PndMCTrack();
    //////mctr->setMcTrackId(trackID);
    ////}
    ////mctr->AddHit(fBranchID,ih,myPoint->GetTime());
    ////MCTrackSortedArray[myPoint->GetTrackID()].FtsArray.push_back(myPoint);
    // MCTrackSortedArray[myPoint->GetTrackID()].push_back(myPoint);
    ////prevTrackID=myPoint->GetTrackID();
    ////TrackIds.push_back(prevTrackID);
    ////mctracklist[trackID] = mctr;
  }

  // 31.01 ftsmctracks.resize(fMCTracks->GetEntriesFast());
  // 31.01 ftsmcpoints.resize(fMCPoints->GetEntriesFast());

  /// outMCT<<fMCTracks->GetEntriesFast()<<endl;
  /// outMCP<<fMCPoints->GetEntriesFast()<<endl;

  ////cout<<"NMCTRACKS: "<<fMCTracks->GetEntriesFast()<<endl;
  ////cout<<"NMCPOINTS: "<<fMCPoints->GetEntriesFast()<<endl;
  // int xNMCTracks=0;
  // for ( int iTr = 0; iTr < fMCTracks->GetEntriesFast(); iTr++ )
  //{
  ////std::sort(MCTrackSortedArray[iTr].FtsArray.begin(), MCTrackSortedArray[iTr].FtsArray.end(), compareFtsPoints);
  // std::sort(MCTrackSortedArray[iTr].begin(), MCTrackSortedArray[iTr].end(), compareFtsPoints);
  ////int curTrID=-1;
  ////bool checker=true;
  ////cout<<"NFTSPOINTS: "<<MCTrackSortedArray[iTr].size()<<endl;
  // for(unsigned int iPFts=0; iPFts < MCTrackSortedArray[iTr].size(); iPFts++)
  //{
  // PndFtsPoint* point = MCTrackSortedArray[iTr][iPFts]; //MCTrackSortedArray[iTr].FtsArray[iPFts];
  // int trackID = point->GetTrackID();
  // Double_t q = 1;
  //{  // get charge
  // if ( trackID < fMCTracks->GetEntriesFast() ) {
  // const PndMCTrack* mcTr = (PndMCTrack*) fMCTracks->At(trackID);

  // if ( mcTr ) {
  // TParticlePDG * part = TDatabasePDG::Instance()->GetParticle(mcTr->GetPdgCode());
  // if ( part )
  //{
  // q = part->Charge()/3.f;
  ////cout<<"q/p "<<q/sqrt( point->GetPx()*point->GetPx() + point->GetPy()*point->GetPy() + point->GetPz()*point->GetPz() )<<" tx "<<point->GetPx()/point->GetPz()<<" ty
  ///"<<point->GetPy()/point->GetPz()<<" x "<<point->GetX()<<" y "<<point->GetY()<<" z "<<point->GetZ()<<endl;
  //}
  //}
  //}
  //}
  ///*int iSta = point->GetLayerID();
  // Double_t px = point->GetPx();
  // Double_t py = point->GetPy();
  // Double_t pz = point->GetPz();
  // Double_t p = sqrt( px*px + py*py + pz*pz );*/

  // if ( fDoPerformance ){
  // PndFTSCALocalMCPoint xxx;
  // xxx.SetPoint(point, q);
  // ftsmcpoints.push_back(xxx);
  ///*outMCP << point->GetX() << " " << point->GetY() << " " << point->GetZ() << endl;
  // outMCP << px << " " << py << " " << pz << " "
  //<< q/p << endl;
  // outMCP << 0 << " " << iSta << " " << trackID << " " << trackID << endl;*/
  //}

  // if ( nMCPointsInMCTrack.find(trackID) != nMCPointsInMCTrack.end() ) {
  // nMCPointsInMCTrack[trackID]++;
  //} else {
  // nMCPointsInMCTrack[trackID] = 1;
  // FirstMCPointIDInMCTrack[trackID] = nMCPoints;
  //}
  ////cout<<"nMCPointsInMCTrack[trackID] "<<nMCPointsInMCTrack[trackID]<<endl;
  ////cout<<"trackID "<<trackID<<endl;
  // nMCPoints++;
  //}
  // const PndMCTrack* mcTr = (PndMCTrack*) fMCTracks->At(iTr);

  // if( fDoPerformance ){
  // if ( !mcTr ) {
  // PndFTSCAMCTrack yyy;
  // PndMCTrack* mcTrempty = new PndMCTrack();
  // yyy.SetMCTrack(mcTrempty, 0,0,0);
  // ftsmctracks.push_back(yyy);
  //}
  // else {
  // Int_t pdg = mcTr->GetPdgCode();
  // Double_t px = mcTr->GetMomentum().X();
  // Double_t py = mcTr->GetMomentum().Y();
  // Double_t pz = mcTr->GetMomentum().Z();
  // Double_t p = sqrt( px*px + py*py + pz*pz );
  // if(TMath::Abs(p)<1.e-6){
  // px = 1.e-6;
  // py = 1.e-6;
  // pz = 1.e-6;
  // p  = 1.e-6;
  //}
  // Double_t q = 1;
  //{  // get charge
  // TParticlePDG * part = TDatabasePDG::Instance()->GetParticle(pdg);
  // if ( part )
  // q = part->Charge()/3.f;
  //}
  ////Double_t ex,ey,ez,qp;
  ////cout<<"pdg px py pz "<<pdg<<" "<<px<<" "<<py<<" "<<pz<<" ";
  // PndFTSCAMCTrack yyy;
  // unsigned int nmcpimct = nMCPointsInMCTrack[iTr];
  // unsigned int fmcpidimct = FirstMCPointIDInMCTrack[iTr];
  // yyy.SetMCTrack(mcTr, q, nmcpimct, fmcpidimct);
  ///* we do it in the function
  ////check for curved tracks
  // for (int iP1=0; iP1<nmcpimct-1; iP1++)
  //{
  // for (int iP2=iP1+1; iP2<nmcpimct; iP2++)
  //{
  // if (points[iP1].Z()>points[iP2].Z())
  //{
  // yyy.SetIsForwardTrack(false);
  // break;
  //}
  //}
  //}
  //*/

  // if (nmcpimct>6)
  ////         if (nmcpimct>22) // dbg
  // xNMCTracks++;
  // ftsmctracks.push_back(yyy);
  ///*outMCT << mcTr->GetMotherID() << " " << pdg << endl;
  // outMCT << mcTr->GetStartVertex().X() << " " << mcTr->GetStartVertex().Y() << " " << mcTr->GetStartVertex().Z() << " "
  //<< px/fabs(p) << " " << py/fabs(p) << " " << pz/fabs(p) << " " << q/p << endl;
  // outMCT << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << endl;
  // outMCT << p << " " << sqrt( px*px + py*py ) << endl;
  // outMCT << nMCPointsInMCTrack[iTr]  << " " << nMCPointsInMCTrack[iTr] << " " << FirstMCPointIDInMCTrack[iTr] << endl; //nHitsInMCTrack[iTr]
  // outMCT << 0 << " " << 0 << " " << 1 << endl;*/
  //}
  //}
  //}

  ////  if (xNMCTracks!=1)
  ////    return;

  ////  if (xNMCTracks==1)
  ////    return;

  // cout<<"ftsmcpoints.size() "<<ftsmcpoints.size()<<endl;
  // cout<<"ftsmctracks.size() "<<ftsmctracks.size()<<endl;
  std::vector<PndFTSCAGBHit> vHits;

  int iHit = 0;

  // Save FTS hits
  WriteFTSHits(vHits, /*outH, outHL, outMCT, outMCP,*/ iHit /*, nHitsInMCTrack*/);

  // if(MCTrackSortedArray) delete[] MCTrackSortedArray;

  /*  if( fDoPerformance ){
      outH.close();
      outHL.close();
      outMCT.close();
      outMCP.close();
    }
    */
  //////////////////

  // const PndFTSCAGBTracker *fTrackerConst = fTracker;

  // SETHITS AS IN MVD+STT
  do {
    int kEvents = iEvent;
    char buf[6];
    sprintf(buf, "%d", kEvents);
    /// const string fileName = filePrefix + "event" + string(buf) + "_";
    // std::cout << "CA fTracker: Loading Event " << kEvents << "..." << std::endl;
    fTracker->SetHits(vHits);

    if (fVerbose > 1)
      cout << "NHits " << fTracker->fHits.Size() << endl;

    //  if (fTracker->fHits.Size() > 200) return;

    /* dbg-cout
    if (fTracker->fHits.Size() >=8 )
    {
      for(unsigned int iH=0; iH<8; iH++)
        {
          int iStation = fTracker->fHits[iH].IRow();
         float X_Hit = fTracker->fHits[iH].X();
         float Z_Hit = fTracker->fHits[iH].Z();
         float R_Hit = fTracker->fHits[iH].R();
         float RSigned   = fTracker->fHits[iH].IsLeft();


    cout << " iStation " << iStation << " X_Hit " << X_Hit << " Z_Hit " << Z_Hit << endl;
    cout << " R_Hit " << R_Hit << " RSigned " << RSigned << endl;

    }
    }*/

    // std::cout << "Event " << kEvents << " CPU reconstruction..." << std::endl;
    ////#ifdef DO_TPCCATRACKER_EFF_PERFORMANCE
    ////// cout<<"Filename "<<fileName<<endl;
    ////if ( fDoPerformance && fPerformance ) {
    ////fPerformance->SetTracker(fTracker);
    ////std::cout << "Loading Monte-Carlo Data for Event " << kEvents << "..." << std::endl;
    ////if (!fPerformance->ReadData(ftslabels,ftsmcpoints, ftsmctracks)) {
    ////cout << "Monte-Carlo Data for Event " << kEvents << " can't be read." << std::endl;
    ////break;
    ////}
    //////cout<<"here!!!\n";
    ////fPerformance->CombineHits();
    //////      fPerformance->DivideHitsOnLR();
    ////}
    ////#endif

    if (NonReconstructableEvent()) {
      ftslabels.clear();
      // ftsmcpoints.clear();
      // ftsmctracks.clear();
      return;
    }

    if (fVerbose > 1)
      cout << "Run CA trackfinder... " << endl;

    fTracker->FindTracks();

    // rewrite reco tracks into global PndTrack-format
    {
      int nOutTracks = 0;
      for (int itr = 0; itr < fTracker->NTracks(); itr++) {
        const PndFTSCAGBTrack &tr = fTracker->Track(itr);
        // filter bad tracks
        if (tr.InnerParam().QP() < 1e-6)
          continue;
        if (tr.OuterParam().QP() < 1e-6)
          continue;
        // cout<<"Output track:"<<endl;
        PndTrackCand *outCand = new ((*fTrackCands)[nOutTracks]) PndTrackCand();
        for (int ih = 0; ih < tr.NHits(); ih++) {
          int hitIndex = fTracker->TrackHit(tr.FirstHitRef() + ih);
          const PndFTSCAGBHit &hit = fTracker->Hit(hitIndex);
          outCand->AddHit(hit.PndDetID(), hit.PndHitID(), hit.IRow());
        }
        outCand->setMcTrackId(-1);

        FairTrackParP paramFirst;
        FairTrackParP paramLast;

        bool check1 = CATrackParToFairTrackParP(&paramFirst, &tr.InnerParam());
        bool check2 = CATrackParToFairTrackParP(&paramLast, &tr.OuterParam());

        if (!check1 || !check2) {
          if (fVerbose > 0)
            std::cout << "PndFtsCATracking::Exec: Error in track finding, skipping track. check1=" << check1 << " check2=" << check2 << std::endl;
          continue;
        }

        PndTrack *outTrack = new ((*fTracks)[nOutTracks]) PndTrack(paramFirst, paramLast, *outCand);
        outTrack->SetChi2(tr.InnerParam().Chi2());
        outTrack->SetNDF(tr.InnerParam().NDF());
        outTrack->SetRefIndex(nOutTracks);
        outTrack->SetFlag(0);
        nOutTracks++;
      }
    }

    ////#ifdef DO_TPCCATRACKER_EFF_PERFORMANCE
    ////if ( fDoPerformance && fPerformance ) {
    //////SG!!! fTracker->SaveTracksInFile(fileName);
    ////if (fTrackerConst->NHits() > 0) {
    ////fPerformance->InitSubPerformances();
    ////fPerformance->ExecPerformance();
    ////}
    ////else {
    ////cout << "Event " << kEvents << " contains 0 hits." << std::endl;
    ////}
    ////}
    ////#endif

    // if (fVerbose>2) {

    // const bool ifAvarageTime = 1;
    // if (!ifAvarageTime) {
    // std::cout << "Reconstruction Time"
    //<< " Real = " << std::setw( 10 ) << (fTrackerConst->SliceTrackerTime() + fTrackerConst->StatTime( 9 )) * 1.e3 << " ms,"
    //<< " CPU = " << std::setw( 10 ) << (fTrackerConst->SliceTrackerCpuTime() + fTrackerConst->StatTime( 10 )) * 1.e3 << " ms"
    //<< std::endl;
    //}
    // else {
    // const int NTimers = fTrackerConst->NTimers();
    // static int statIEvent = 0;
    // static double *statTime = new double[NTimers];
    // static double statTime_SliceTrackerTime = 0;
    // static double statTime_SliceTrackerCpuTime = 0;

    // if (!statIEvent) {
    // for (int i = 0; i < NTimers; i++) {
    // statTime[i] = 0;
    //}
    //}

    // statIEvent++;
    // for (int i = 0; i < NTimers; i++) {
    // statTime[i] += fTrackerConst->StatTime( i );
    //}
    // statTime_SliceTrackerTime += fTrackerConst->SliceTrackerTime();
    // statTime_SliceTrackerCpuTime += fTrackerConst->SliceTrackerCpuTime();

    // std::cout << "Reconstruction Time"
    //<< " Real = " << std::setw( 10 ) << 1./statIEvent*(statTime_SliceTrackerTime+statTime[ 9 ]) * 1.e3 << " ms,"
    //<< " CPU = " << std::setw( 10 ) << 1./statIEvent*(statTime_SliceTrackerCpuTime+statTime[ 10 ]) * 1.e3 << " ms,"
    //<< std::endl;
    //}
    //} // fVerbose>2

  } while (0);

  ftslabels.clear();
  // ftsmcpoints.clear();
  // ftsmctracks.clear();
}

void PndFtsCATracking::Finish()
{
  //#ifdef DO_TPCCATRACKER_EFF_PERFORMANCE
  // if ( fDoPerformance && fPerformance ) {
  // fPerformance->WriteHistos();
  //}
  ////fPerformanceHistoFile->Close();
  //#endif
}

void PndFtsCATracking::WriteFTSHits(std::vector<PndFTSCAGBHit> &vHits,
                                    /*std::fstream &outH, std::fstream &outHL, std::fstream &outMCT, std::fstream &outMCP,*/ int &iHit /*, map<int, unsigned int> &nHitsInMCTrack*/)
{
  TClonesArray *hitsArray;
  hitsArray = fHits;
  // Int_t ftsLinkType = FairRootManager::Instance()->GetBranchId("FTSPoint");
  // 31.01 ftslabels.resize(hitsArray->GetEntriesFast());

  // outHL << hitsArray->GetEntriesFast() << endl;
  // outH << hitsArray->GetEntriesFast() << endl;

  std::map<Int_t, Int_t> tubeMap;
  std::map<Int_t, Int_t>::iterator mapIt;

  for (int iH = 0; iH < hitsArray->GetEntriesFast(); iH++) {
    PndFtsHit *currenthit = (PndFtsHit *)hitsArray->At(iH);
    //    cout << " currenthit->GetTimeStamp() " << currenthit->GetTimeStamp() << endl;
    Int_t tubeID = currenthit->GetTubeID();
    mapIt = tubeMap.find('b');
    if (mapIt == tubeMap.end()) {
      tubeMap.insert(std::pair<Int_t, Int_t>(tubeID, 1));
    } else {
      mapIt->second++;
      if (mapIt->second > 3)
        continue; // SG!!!
    }

    PndFtsTube *tube = (PndFtsTube *)fTubeArrayFts->At(tubeID);
    TVector3 wire_direction = tube->GetWireDirection();

    const Double_t kSS = 1; //.5; // coefficient between size and sigma
    const Double_t dXY = tube->GetRadIn();
    const Double_t errXY = dXY / kSS;
    const Double_t errXY2 = errXY * errXY;
    const Double_t errZ = tube->GetHalfLength() / kSS;

    TMatrixT<Double_t> RM = tube->GetRotationMatrix();
    //     cout<<"mtx \n";
    //     for (int row = 0; row<3; row++)
    //     {
    //         for (int col = 0; col<3; col++)
    //         {
    //                 cout<<RM[row][col]<<" ";
    //         }
    //         cout<<endl;
    //     }
    //     cout<<endl;
    TMatrixT<Double_t> C(3, 3); // CovMatrix
    C[0][0] = errXY2;
    C[1][1] = errXY2;
    C[2][2] = errZ * errZ;
    C[0][1] = C[0][2] = C[1][0] = C[1][2] = C[2][0] = C[2][1] = 0;

    TMatrixT<Double_t> CR = C; // rotated CovMatrix

    CR = RM * C;
    CR = CR * RM.Transpose(RM);

    Double_t A = 0;

    TVector3 position = tube->GetPosition();
    // Double_t xx = position.X();
    // Double_t yy = position.Y();
    // Double_t zz = position.Z();
    // cout<<"tube coords "<<xx<<" "<<yy<<" "<<zz<<endl;

    Double_t x = currenthit->GetX();
    Double_t y = currenthit->GetY();
    Double_t z = currenthit->GetZ();
    // cout<<"hit coords "<<x<<" "<<y<<" "<<z<<endl;
    // cout<<"NEXT HIT \n";
    //     cout<<"direct coords "<<x<<" "<<y<<" "<<z<<endl;
    //     cout<<"mtx \n";
    //     for (int row = 0; row<3; row++)
    //     {
    //         for (int col = 0; col<3; col++)
    //         {
    //                 cout<<RM[row][col]<<" ";
    //         }
    //         cout<<endl;
    //     }
    /// Double_t r = TMath::Sqrt(x*x + y*y);
    int iSta = -1;

    // get station angle A and station index iSta

    //     A=1234;
    iSta = currenthit->GetLayerID() - 1;

    Double_t radius = currenthit->GetIsochrone(); // 0;
    // cout<<"radius "<<radius<<endl;
    // error calculation according to the curve chosen by flag
    Double_t closestDistanceError = currenthit->GetIsochroneError(); // 0;

    // int pointID = -1; //[R.K. 9/2018] unused

    // int trackID;
    // PndFtsPoint* point=nullptr;

    // FairMultiLinkedData links = currenthit->GetLinksWithType(ftsLinkType);
    // if( links.GetNLinks() >0 )
    //{
    // int iPoint = links.GetLink(0).GetIndex();
    // point = (PndFtsPoint*) fMCPoints->At(iPoint);
    // if( !point )
    //{
    ////       cout<<"CA tracker: wrong index of Fts point: "<<iPoint<<" of "<<fFtsPointsArray->GetEntriesFast()<<endl;
    // return;
    //}
    // else
    //{
    // trackID = point->GetTrackID();
    //}
    //}

    PndFTSCAGBHit h;

    ////const PndMCTrack* mcTr = (PndMCTrack*) fMCTracks->At(trackID);
    ////TParticlePDG * part = TDatabasePDG::Instance()->GetParticle(mcTr->GetPdgCode());
    // float q = part->Charge()/3.f;
    // float qp = q/sqrt( point->GetPx()*point->GetPx() + point->GetPy()*point->GetPy() + point->GetPz()*point->GetPz() );
    ////mc-links:begin
    // h.point_X = (float) point->GetX();
    // h.point_Y = (float) point->GetY();
    // h.point_Z = (float) point->GetZ();
    // h.point_Px = (float) point->GetPx();
    // h.point_Py = (float) point->GetPy();
    // h.point_Pz = (float) point->GetPz();
    // h.point_Qp = qp;
    // h.Track_ID = trackID;
    ////mc-links:end

    h.SetX(x);
    h.SetY(y);
    h.SetZ(z);
    h.SetErr2X(currenthit->GetDx() * currenthit->GetDx());
    h.SetErr2Y(currenthit->GetDy() * currenthit->GetDy());

    h.SetXW(x);
    h.SetYW(y);
    h.SetZW(z);

    h.SetEX(tube->GetWireDirection().X());
    h.SetEY(tube->GetWireDirection().Y());
    h.SetEZ(tube->GetWireDirection().Z());
    // cout<<"ex "<<tube->GetWireDirection().X()<<" ey "<<tube->GetWireDirection().Y()<<" ez "<<tube->GetWireDirection().Z()<<endl;
    h.SetR(radius);
    h.SetC(CR);
    h.SetErr2R(closestDistanceError * closestDistanceError);
    h.SetIRow(iSta);

    h.SetID(iHit);

    // h.SetPndHitID( iH );
    h.SetAngle(-A);
    h.SetTubeR(tube->GetRadIn());
    h.SetTubeHalfLength(tube->GetHalfLength());

    h.SetPndDetID(FairRootManager::Instance()->GetBranchId("FTSHit"));
    h.SetPndHitID(iH);

    vHits.push_back(h);

    // int trackIDs[3] = {-1, -1, -1};
    // trackIDs[0] = trackID;

    // if( fDoPerformance ) {
    ////outH << h;
    ////outHL << trackIDs[0] << " " << trackIDs[1] << " " << trackIDs[2] << endl;
    // ftslabels.push_back(trackIDs[0]);
    ////cout<<"fPerformance trackIDs[0] "<<trackIDs[0]<<endl;
    //}

    iHit++;

    // if ( nHitsInMCTrack.find(trackIDs[0]) != nHitsInMCTrack.end() ) {
    // nHitsInMCTrack[trackIDs[0]]++;
    //} else {
    // nHitsInMCTrack[trackIDs[0]] = 1;
    //}
  }
}
