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
//***************************************************************************
// List of most recent changes:                                             *
// 29-05-17 Adjustment of initialization for PANDA data (Irina Rostovtseva) *
//          Correction of booking and saving of histograms (IR)             *
//***************************************************************************
//
// Panda Headers ----------------------

#include "PndCATracking.h"

#include <iostream>
#include <cmath>

#include "TClonesArray.h"
#include "TParticlePDG.h"
#include "PndMCTrack.h"
#include "FairRootManager.h"
//#include "FairGeanePro.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "PndGeoSttPar.h"
//#include "PndSttMapCreator.h"
#include "PndStt2GeoHandler.h"

#include "PndSttTube.h"
#include "PndSttHit.h"

#include "PndSdsHit.h"
#include "PndSdsClusterPixel.h"
#include "PndSdsClusterStrip.h"
#include "PndSdsDigiPixel.h"
#include "PndSdsDigiStrip.h"

#include "TGeoManager.h"
#include "PndGeoHandling.h"

#include "PndSttPoint.h"
#include "PndCATrackMCPointContainer.h"

#include "FairField.h"

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

#include "PndCATrackParam.h"
#include "PndCAGBTracker.h"
#include "PndCAPerformance.h"
#include "TFile.h"

ClassImp(PndCATracking);

bool compareSdsPoints(PndSdsMCPoint *const a, PndSdsMCPoint *const b)
{
  return (a->GetTime() < b->GetTime());
}
bool compareSttPoints(PndSttPoint *const a, PndSttPoint *const b)
{
  return (a->GetTime() < b->GetTime());
}

PndCATracking::PndCATracking(const char *name, Int_t iVerbose)
  : PndPersistencyTask(name, iVerbose), fMvdPixelHitsBranchName("MVDHitsPixel"), fMvdStripHitsBranchName("MVDHitsStrip"), fMvdPixelClusterBranchName("MVDPixelClusterCand"),
    fMvdStripClusterBranchName("MVDStripClusterCand"), fMvdPixelDigiBranchName("MVDPixelDigis"), fMvdStripDigiBranchName("MVDStripDigis"), fSttHitsBranchName("STTHit"),
    fMvdPointsBranchName("MVDPoint"), fSttPointsBranchName("STTPoint"), fMCTracksBranchName("MCTrack"), fTubeArray(nullptr), fSttMvdPndTrackArray(nullptr), fDoPerformance(0)
{
#ifdef DO_TPCCATRACKER_EFF_PERFORMANCE
  fDoPerformance = 1;
#endif
  SetPersistency(kTRUE);
}

PndCATracking::~PndCATracking() {}

InitStatus PndCATracking::Init()
{
  perf = &PndCAPerformance::Instance();

  // Get ROOT Manager
  FairRootManager *ioman = FairRootManager::Instance();

  if (ioman == 0) {
    Error("PndCATracking::Init", "RootManager not instantiated!");
    return kERROR;
  }

  // Get Hits
  // MVD Pixel Hits
  fMvdPixelHitsArray = (TClonesArray *)ioman->GetObject(fMvdPixelHitsBranchName);
  if (fMvdPixelHitsArray == nullptr) {
    Error("PndCATracking::Init", "mvd pixel hit array not found!");
    return kERROR;
  }
  // MVD Strip Hits
  fMvdStripHitsArray = (TClonesArray *)ioman->GetObject(fMvdStripHitsBranchName);
  if (fMvdStripHitsArray == nullptr) {
    Error("PndCATracking::Init", "mvd strip hit array not found!");
    return kERROR;
  }
  // STT hits
  fSttHitsArray = (TClonesArray *)ioman->GetObject(fSttHitsBranchName);
  if (fSttHitsArray == nullptr) {
    Error("PndCATracking::Init", "stt hit array not found!");
    return kERROR;
  }

  // MVD Pixel Clusters
  fMvdPixelClusterArray = (TClonesArray *)ioman->GetObject(fMvdPixelClusterBranchName);
  if (fMvdPixelClusterArray == nullptr) {
    Error("PndCATracking::Init", "mvd pixel cluster array not found!");
    return kERROR;
  }
  // MVD Strip Clusters
  fMvdStripClusterArray = (TClonesArray *)ioman->GetObject(fMvdStripClusterBranchName);
  if (fMvdStripClusterArray == nullptr) {
    Error("PndCATracking::Init", "mvd strip cluster array not found!");
    return kERROR;
  }

  // MVD Pixel Digis
  fMvdPixelDigiArray = (TClonesArray *)ioman->GetObject(fMvdPixelDigiBranchName);
  if (fMvdPixelDigiArray == nullptr) {
    Error("PndCATracking::Init", "mvd pixel digi array not found!");
    return kERROR;
  }
  // MVD Strip Digis
  fMvdStripDigiArray = (TClonesArray *)ioman->GetObject(fMvdStripDigiBranchName);
  if (fMvdStripDigiArray == nullptr) {
    Error("PndCATracking::Init", "mvd strip digi array not found!");
    return kERROR;
  }

  // STT points
  fSttPointsArray = (TClonesArray *)ioman->GetObject(fSttPointsBranchName);
  if (fSttPointsArray == nullptr) {
    Error("PndCATracking::Init", "sts points array not found!");
    return kERROR;
  }

  // MVD points
  fMvdPointsArray = (TClonesArray *)ioman->GetObject(fMvdPointsBranchName);
  if (fMvdPointsArray == nullptr) {
    Error("PndCATracking::Init", "mvd points array not found!");
    return kERROR;
  }

  // MC Tracks
  fMCTrackArray = (TClonesArray *)ioman->GetObject(fMCTracksBranchName);
  if (fMCTrackArray == nullptr) {
    Error("PndCATracking::Init", "mc tracks array not found!");
    return kERROR;
  }

  // STT tube geometry
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  PndGeoSttPar *sttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
  fTubeArray = PndStt2GeoHandler::Instance(sttParameters)->GetTubeArray();

  // handles both old and new geometry
  //PndStt2GeoHandler geoH(sttParameters);
  //fTubeArray = geoH.GetTubeArray();
  
  //if (sttParameters->GetGeometryType() != -1) {
    //PndSttMapCreator *mapper = new PndSttMapCreator(sttParameters);
    //fTubeArray = mapper->FillTubeArray();
  //}

  //--------------------------------  output TClonesArrays ------------------------

  // Create and register output array for PndTrack of Stt+Mvd combined

  fSttMvdPndTrackArray = new TClonesArray("PndTrack");
  ioman->Register("SttMvdTrack", "SttMvd", fSttMvdPndTrackArray, kTRUE);

  //-----------------------

  return kSUCCESS;
}

void PndCATracking::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  rtdb->getContainer("PndGeoSttPar");
  rtdb->getContainer("PndGeoFtsPar");

  // for MVD geometry extraction
  fGeoH = PndGeoHandling::Instance();
  fGeoH->SetParContainers();
}

void PndCATracking::CATrackParToFairTrackParP(FairTrackParP *fairParam, const PndCATrackParam *kfParam)
{

  const double cA = TMath::Cos(kfParam->Angle());
  const double sA = -TMath::Sin(kfParam->Angle());

  double x0 = kfParam->X();
  double y0 = kfParam->Y();
  double z0 = kfParam->Z();

  double pz0 = -kfParam->DzDs() / TMath::Abs(kfParam->QPt());
  double py0 = -kfParam->SinPhi() / TMath::Abs(kfParam->QPt());
  double px0 = -kfParam->GetCosPhi() / TMath::Abs(kfParam->QPt());

  double x = -sA * x0 + cA * y0;
  double y = cA * x0 + sA * y0;
  double z = -z0;

  double px = -sA * px0 + cA * py0;
  double py = cA * px0 + sA * py0;
  double pz = -pz0;

  int q = kfParam->QPt() > 0 ? 1 : -1;

  Double_t cov[15], cov1[15];

  for (int i = 0; i < 15; i++)
    cov[i] = kfParam->Cov()[i];

  double mB = 1 / TMath::Sqrt(1 + kfParam->DzDs() * kfParam->DzDs());
  double mA = -kfParam->QPt() * kfParam->DzDs() * mB * mB * mB;
  double mC = 1 / kfParam->GetCosPhi();
  double mE = mC * mC * mC;
  double mD = kfParam->DzDs() * kfParam->SinPhi() * mC * mC * mC;

  cov1[14] = cov[2];
  cov1[13] = cov[1];
  cov1[12] = cov[0];
  cov1[11] = mE * cov[7] + cov[4] * mD;
  cov1[10] = mE * cov[6] + cov[3] * mD;
  cov1[9] = mE * mE * cov[9] + 2 * mE * cov[8] * mD + cov[5] * mD * mD;
  cov1[8] = mC * cov[4];
  cov1[7] = mC * cov[3];
  cov1[6] = mC * (mE * cov[8] + cov[5] * mD);
  cov1[5] = mC * mC * cov[5];
  cov1[4] = mB * cov[11] + mA * cov[7];
  cov1[3] = mB * cov[10] + mA * cov[6];
  cov1[2] = mB * mE * cov[13] + mA * mE * cov[9] + mB * cov[12] * mD + mA * cov[8] * mD;
  cov1[1] = mC * (mB * cov[12] + mA * cov[8]);
  cov1[0] = 2 * mA * mB * cov[13] + mB * mB * cov[14] + mA * mA * cov[9];

  fairParam->SetTrackPar(x, y, z, px, py, pz, q, cov1, TVector3(0, 0, 0), TVector3(-sA, cA, 0), TVector3(cA, sA, 0), TVector3(0, 0, -1));
}

void PndCATracking::Exec(Option_t *)
{
  if (fVerbose > 0)
    std::cout << "PndCATracking::Exec" << std::endl;

  fSttMvdPndTrackArray->Delete();

  FairField *MF = FairRunAna::Instance()->GetField();

  Double_t xyz[3] = {0, 0, 0};
  Double_t fieldValue[3];
  MF->Field(xyz, fieldValue);
  double Bz = fieldValue[2];

  static int iEvent = -1;
  iEvent++;

  string filePrefix = "./CATrackerData";
  TString fadata_name = "CATrackerData/event";
  filePrefix += "/";
  static TFile *perfHistoFile = 0;

  fadata_name += iEvent;
  std::fstream outH;
  std::fstream outHL;
  std::fstream outMCT;
  std::fstream outMCP;

  if (fDoPerformance) {
    if (!perfHistoFile) {
      perfHistoFile = new TFile((filePrefix + "CATrackerPerformance.root").data(), "RECREATE");
      if (!perfHistoFile->IsOpen()) {
        gSystem->Exec("mkdir ./CATrackerData");
        perfHistoFile = new TFile((filePrefix + "CATrackerPerformance.root").data(), "RECREATE");
      }
    }
    outH.open(fadata_name + "_hits.data", std::fstream::out);
    outHL.open(fadata_name + "_hitLabels.data", std::fstream::out);
    outMCT.open(fadata_name + "_MCTracks.data", std::fstream::out);
    outMCP.open(fadata_name + "_MCPoints.data", std::fstream::out);
  }

  int nMvdhits = 0;
  // Calculate a number of MVD hits in the barrel part
  for (int iHMvdP = 0; iHMvdP < fMvdPixelHitsArray->GetEntriesFast(); iHMvdP++) {
    // PndSdsHit* currenthit = (PndSdsHit*) fMvdPixelHitsArray->At(iHMvdP);
    // Int_t sensorID = currenthit->GetSensorID();
    // gGeoManager->cd(PndGeoHandling::Instance()->GetPath(sensorID));
    // TGeoHMatrix* transMat = gGeoManager->GetCurrentMatrix();
    // Double_t *mmm = transMat->GetRotationMatrix();
    //    if(fabs(mmm[6]) < 0.999 && fabs(mmm[7]) < 0.999 ) continue;
    nMvdhits++;
  }

  /*
  cout<<"\n\nCA pixel detector: "<< FairRootManager::Instance()->GetBranchId(fMvdPixelHitsBranchName)<<endl;
  cout<<"CA strip detector: "<< FairRootManager::Instance()->GetBranchId(fMvdStripHitsBranchName)<<endl;
  cout<<"N of MVD Pixels: "<<nMvdhits<<endl;
  */
  for (int iHMvdS = 0; iHMvdS < fMvdStripHitsArray->GetEntriesFast(); iHMvdS++) {
    // PndSdsHit* currenthit = (PndSdsHit*) fMvdStripHitsArray->At(iHMvdS);
    // Int_t sensorID = currenthit->GetSensorID();
    // gGeoManager->cd(PndGeoHandling::Instance()->GetPath(sensorID));
    // TGeoHMatrix* transMat = gGeoManager->GetCurrentMatrix();
    // Double_t *mmm = transMat->GetRotationMatrix();
    //    if( fabs(mmm[6]) < 0.999 && fabs(mmm[7]) < 0.999 ) continue;
    nMvdhits++;
  }
  // cout<<"nTotal N of MVD hits: "<<nMvdhits<<endl;

  const Int_t nHits = fSttHitsArray->GetEntriesFast() + nMvdhits;

  Int_t nPoints = 0; // fSttPointsArray->GetEntriesFast() + fMvdPointsArray->GetEntriesFast();
  const Int_t nMCTracks = fMCTrackArray->GetEntriesFast();

  map<int, unsigned int> nHitsInMCTrack, nMCPointsInMCTrack, FirstMCPointIDInMCTrack;
  PndCATrackMCPointContainer *MCTrackSortedArray = new PndCATrackMCPointContainer[fMCTrackArray->GetEntriesFast()];

  for (Int_t iMvd = 0; iMvd < fMvdPointsArray->GetEntriesFast(); iMvd++) {
    PndSdsMCPoint *MvdPoint = (PndSdsMCPoint *)fMvdPointsArray->At(iMvd);
    int sensorID = MvdPoint->GetSensorID();
    gGeoManager->cd(PndGeoHandling::Instance()->GetPath(sensorID));
    TGeoHMatrix *transMat = gGeoManager->GetCurrentMatrix();
    Double_t *mmm = transMat->GetRotationMatrix();
    if (fabs(mmm[6]) < 0.999 && fabs(mmm[7]) < 0.999) {
      continue;
    } // save only points from the barrel part
    // if(!(fabs(mmm[6]) < 0.999 && fabs(mmm[7]) < 0.999)) { continue; }// save only points from the forward part

    MCTrackSortedArray[MvdPoint->GetTrackID()].MvdArray.push_back(MvdPoint);
    nPoints++;

    // Double_t x = MvdPoint->GetX();
    // Double_t y = MvdPoint->GetY();
    // Double_t z = MvdPoint->GetZ();
    // Double_t r = TMath::Sqrt(x*x + y*y);
    // int iSta = -1;
    //     if( (r>0.) && (r<3.5) ) iSta = 2;
    //     if( (r>3.5) && (r<7.5) ) iSta = 3;
    //     if( (r>7.5) && (r<11.0) ) iSta = 4;
    //     if( (r>11.0) && (r<15.0) ) iSta = 5;

    // if( (z>0.) && (z<3.) ) iSta = 0;
    // if( (z>3.) && (z<5.5) ) iSta = 1;
    // if( (z>5.5) && (z<8.0) ) iSta = 2;
    // if( (z>8.0) && (z<12.0) ) iSta = 3;
    // if( (z>12.0) && (z<18.5) ) iSta = 4;
    // if( (z>18.5) && (z<24.5) ) iSta = 5;
  }

  for (Int_t iSts = 0; iSts < fSttPointsArray->GetEntriesFast(); iSts++) {
    PndSttPoint *SttPoint = (PndSttPoint *)fSttPointsArray->At(iSts);
    MCTrackSortedArray[SttPoint->GetTrackID()].SttArray.push_back(SttPoint);
    nPoints++;
  }

  if (fDoPerformance) {
    outH << nHits << endl;
    outHL << nHits << endl;
    outMCP << nPoints << endl;
    outMCT << nMCTracks << endl;
  }

  std::vector<PndCAGBHit> vHits;

  int iHit = 0;

  // Save MVD Pixel hits
  WriteMVDHits(vHits, outH, outHL, outMCT, outMCP, iHit, nHitsInMCTrack, 1);

  // Save MVD Strip hits
  WriteMVDHits(vHits, outH, outHL, outMCT, outMCP, iHit, nHitsInMCTrack, 0);

  // Save STT hits

  Int_t sttLinkType = FairRootManager::Instance()->GetBranchId("STTPoint");

  std::map<Int_t, Int_t> tubeMap;
  std::map<Int_t, Int_t>::iterator mapIt;

  for (int iHStt = 0; iHStt < fSttHitsArray->GetEntriesFast(); iHStt++) {
    PndSttHit *currenthit = (PndSttHit *)fSttHitsArray->At(iHStt);
    Int_t tubeID = currenthit->GetTubeID();
    mapIt = tubeMap.find('b');
    if (mapIt == tubeMap.end()) {
      tubeMap.insert(std::pair<Int_t, Int_t>(tubeID, 1));
    } else {
      mapIt->second++;
      if (mapIt->second > 3)
        continue; // SG!!!
    }

    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);
    TVector3 wire_direction = tube->GetWireDirection();

    const Double_t kSS = 1.5; // coefficient between size and sigma
    const Double_t dXY = tube->GetRadIn();
    const Double_t errXY = dXY / kSS;
    const Double_t errXY2 = errXY * errXY;
    const Double_t errZ = tube->GetHalfLength() / kSS * sqrt(26); // 26 rows with basicaly same information

    TMatrixT<Double_t> RM = tube->GetRotationMatrix();
    TMatrixT<Double_t> C(3, 3); // CovMatrix
    C[0][0] = errXY2;
    C[1][1] = errXY2;
    C[2][2] = errZ * errZ;
    C[0][1] = C[0][2] = C[1][0] = C[1][2] = C[2][0] = C[2][1] = 0;

    TMatrixT<Double_t> CR = C; // rotated CovMatrix

    CR = RM * C;
    CR = CR * RM.Transpose(RM);

    Double_t radius = currenthit->GetIsochrone();

    // error calculation according to the curve chosen by flag
    Double_t closestDistanceError = currenthit->GetIsochroneError();
    TVector3 position = tube->GetPosition(); // CHECK added

    Double_t x = position.X();
    Double_t y = position.Y();
    Double_t z = position.Z();

    Int_t iSta = 0;
    Double_t A = 0;
    {
      A = atan(fabs(y / x));
      const Double_t pi = TMath::Pi(); // 3.14159265359;
      const Double_t pi2 = pi / 2;
      if (y >= 0 && x >= 0)
        A = pi2 - A;
      if (y < 0 && x >= 0)
        A = pi2 + A;
      if (y < 0 && x < 0)
        A = 3 * pi2 - A;
      if (y >= 0 && x < 0)
        A = 3 * pi2 + A;

      A = floor(A / pi / 2 * 6) * pi / 3;
      // turn by -(A+3.1415/6)
      A = -(A + pi / 6);
      // const Double_t xx = x*cos(A)+y*sin(A);
      const Double_t yy = -x * sin(A) + y * cos(A);
      Double_t iS = (yy - 16.619027) / sqrt(3.f / 4) / 1.01;
      if (iS < 7.5)
        iSta = floor(iS + 0.5);
      else if (iS - .1547 < 9.5)
        iSta = floor(iS - .1547 + 0.5);
      else if (iS - .5883 < 11.5)
        iSta = floor(iS - .5883 + 0.5);
      else if (iS - 1.022 < 13.5)
        iSta = floor(iS - 1.022 + 0.5);
      else if (iS - 1.4556 < 15.5)
        iSta = floor(iS - 1.4556 + 0.5);
      else
        iSta = floor(iS - 2 + 0.5);
    }
    iSta += 4; // 4 MVD stations

    int trackID = -1;
    FairMultiLinkedData links = currenthit->GetLinksWithType(sttLinkType);
    if (links.GetNLinks() > 0) {
      int iPoint = links.GetLink(0).GetIndex();
      PndSttPoint *point = (PndSttPoint *)fSttPointsArray->At(iPoint);
      if (!point) {
        cout << "CA tracker: wrong index of Stt point: " << iPoint << " of " << fSttPointsArray->GetEntriesFast() << endl;
        exit(0); // SG!!
      } else {
        trackID = point->GetTrackID();
      }
    } else {
      cout << "CA tracker: stt hit has no link to stt point" << endl;
      exit(0); // SG!!
    }
    // cout<<"hit, sta "<<iSta<<" trackid "<<trackID<<" tubeID "<<tubeID<<" radius "<<radius<<endl;
    PndCAGBHit h;
    h.SetGlobalX(x);
    h.SetGlobalY(y);
    h.SetZ(z);
    h.SetXW(x);
    h.SetYW(y);
    h.SetZW(z);

    h.SetR(radius);
    h.SetC(CR);
    h.SetErr2R(closestDistanceError * closestDistanceError);
    h.SetIRow(iSta);
    h.SetID(iHit);
    //   h.SetPndDetID(currenthit->GetDetectorID());
    h.SetPndDetID(FairRootManager::Instance()->GetBranchId(fSttHitsBranchName));
    h.SetPndHitID(iHStt);
    h.SetAngle(-A);

    h.SetTubeR(tube->GetRadIn());
    h.SetTubeHalfLength(tube->GetHalfLength());

    vHits.push_back(h);

    if (fDoPerformance) {
      outH << h;
      outHL << trackID << " " << -1 << " " << -1 << endl;
    }
    /*
    outH << x << " " << y << " " << z << " " << radius << endl;
    outH << CR[0][0] << " " << CR[0][1] << " " << CR[0][2] << endl;
    outH << CR[1][0] << " " << CR[1][1] << " " << CR[1][2] << endl;
    outH << CR[2][0] << " " << CR[2][1] << " " << CR[2][2] << endl;
    outH << closestDistanceError*closestDistanceError << endl;
    outH << tube->GetRadIn() << " " << tube->GetHalfLength() << endl;
    outH << wire_direction.X() << " "  << wire_direction.Y() << " "  << wire_direction.Z() << endl;
    outH << iSta << " " << iHit << " " << A << endl;
    */
    iHit++;

    if (nHitsInMCTrack.find(trackID) != nHitsInMCTrack.end()) {
      nHitsInMCTrack[trackID]++;
    } else {
      nHitsInMCTrack[trackID] = 1;
    }
  }

  int nMCPoints = 0;
  for (int iTr = 0; iTr < nMCTracks; iTr++) {

    // save MVD points
    std::sort(MCTrackSortedArray[iTr].MvdArray.begin(), MCTrackSortedArray[iTr].MvdArray.end(), compareSdsPoints);
    std::sort(MCTrackSortedArray[iTr].SttArray.begin(), MCTrackSortedArray[iTr].SttArray.end(), compareSttPoints);

    for (int iPStt = 0; iPStt < MCTrackSortedArray[iTr].GetNMvdPoints(); iPStt++) {
      PndSdsMCPoint *point = MCTrackSortedArray[iTr].MvdArray[iPStt];
      int trackID = point->GetTrackID();

      Double_t q = 1;
      { // get charge
        if (trackID < fMCTrackArray->GetEntriesFast()) {
          const PndMCTrack *mcTr = (PndMCTrack *)fMCTrackArray->At(trackID);
          if (mcTr) {
            TParticlePDG *part = TDatabasePDG::Instance()->GetParticle(mcTr->GetPdgCode());
            if (part)
              q = part->Charge() / 3.f;
          } else {
            cout << "CA tracker: Bad MCTracks2" << endl;
          }
        } else {
          cout << "CA traker: Bad MCTracks" << endl;
        }
      }

      Double_t x = point->GetX(), y = point->GetY();
      Double_t r = TMath::Sqrt(x * x + y * y);
      int iSta = -1;
      if ((r > 0.) && (r < 3.5))
        iSta = 0;
      if ((r > 3.5) && (r < 7.5))
        iSta = 1;
      if ((r > 7.5) && (r < 11.0))
        iSta = 2;
      if ((r > 11.0) && (r < 15.0))
        iSta = 3;

      Double_t px = point->GetPx();
      Double_t py = point->GetPy();
      Double_t pz = point->GetPz();
      Double_t p = sqrt(px * px + py * py + pz * pz);
      if (TMath::Abs(p) < 1.e-6) {
        px = 1.e-6;
        py = 1.e-6;
        pz = 1.e-6;
        p = 1.e-6;
      }

      if (fDoPerformance) {
        outMCP << point->GetX() << " " << point->GetY() << " " << point->GetZ() << endl;
        outMCP << px << " " << py << " " << pz << " " << q / p << endl;
        outMCP << 0 << " " << iSta << " " << trackID << " " << trackID << endl;
      }

      if (nMCPointsInMCTrack.find(trackID) != nMCPointsInMCTrack.end()) {
        nMCPointsInMCTrack[trackID]++;
      } else {
        nMCPointsInMCTrack[trackID] = 1;
        FirstMCPointIDInMCTrack[trackID] = nMCPoints;
      }
      nMCPoints++;
    }

    // save STT points
    for (int iPStt = 0; iPStt < MCTrackSortedArray[iTr].GetNSttPoints(); iPStt++) {
      PndSttPoint *point = MCTrackSortedArray[iTr].SttArray[iPStt];
      int trackID = point->GetTrackID();

      Double_t q = 1;
      { // get charge
        if (trackID < fMCTrackArray->GetEntriesFast()) {
          const PndMCTrack *mcTr = (PndMCTrack *)fMCTrackArray->At(trackID);
          if (mcTr) {
            TParticlePDG *part = TDatabasePDG::Instance()->GetParticle(mcTr->GetPdgCode());
            if (part)
              q = part->Charge() / 3.f;
          } else {
            cout << "CA tracker: Bad MCTracks2" << endl;
          }
        } else {
          cout << "CA tracker: Bad MCTracks" << endl;
        }
      }

      int tubeID = point->GetTubeID();
      PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);
      TVector3 position = tube->GetPosition(); // CHECK added
      Double_t x = position.X();
      Double_t y = position.Y();

      Int_t iSta = 0;
      Double_t A = 0;
      {
        A = atan(fabs(y / x));
        const Double_t pi = TMath::Pi(); // 3.14159265359;
        const Double_t pi2 = pi / 2;
        if (y >= 0 && x >= 0)
          A = pi2 - A;
        if (y < 0 && x >= 0)
          A = pi2 + A;
        if (y < 0 && x < 0)
          A = 3 * pi2 - A;
        if (y >= 0 && x < 0)
          A = 3 * pi2 + A;

        A = floor(A / pi / 2 * 6) * pi / 3;
        // turn by -(A+3.1415/6)
        A = -(A + pi / 6);
        // const Double_t xx = x*cos(A)+y*sin(A);
        const Double_t yy = -x * sin(A) + y * cos(A);
        Double_t iS = (yy - 16.619027) / sqrt(3.f / 4) / 1.01;
        if (iS < 7.5)
          iSta = floor(iS + 0.5);
        else if (iS - .1547 < 9.5)
          iSta = floor(iS - .1547 + 0.5);
        else if (iS - .5883 < 11.5)
          iSta = floor(iS - .5883 + 0.5);
        else if (iS - 1.022 < 13.5)
          iSta = floor(iS - 1.022 + 0.5);
        else if (iS - 1.4556 < 15.5)
          iSta = floor(iS - 1.4556 + 0.5);
        else
          iSta = floor(iS - 2 + 0.5);
      }
      //      iSta += 6; // 6 MVD stations
      iSta += 4; // 4 MVD barrel stations
      Double_t px = point->GetPx();
      Double_t py = point->GetPy();
      Double_t pz = point->GetPz();
      Double_t p = sqrt(px * px + py * py + pz * pz);
      if (TMath::Abs(p) < 1.e-6) {
        px = 1.e-6;
        py = 1.e-6;
        pz = 1.e-6;
        p = 1.e-6;
      }

      if (fDoPerformance) {
        outMCP << point->GetX() << " " << point->GetY() << " " << point->GetZ() << endl;
        outMCP << px << " " << py << " " << pz << " " << q / p << endl;
        outMCP << 0 << " " << iSta << " " << trackID << " " << trackID << endl;
      }

      if (nMCPointsInMCTrack.find(trackID) != nMCPointsInMCTrack.end()) {
        nMCPointsInMCTrack[trackID]++;
      } else {
        nMCPointsInMCTrack[trackID] = 1;
        FirstMCPointIDInMCTrack[trackID] = nMCPoints;
      }
      nMCPoints++;
    }

    const PndMCTrack *mcTr = (PndMCTrack *)fMCTrackArray->At(iTr);

    if (fDoPerformance) {
      if (!mcTr) {
        outMCT << -1 << " " << -1 << endl;
        outMCT << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << endl;
        outMCT << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << endl;
        outMCT << 0 << " " << 0 << endl;
        outMCT << 0 << " " << 0 << " " << 0 << endl;
        outMCT << 0 << " " << 0 << " " << 1 << endl;
      } else {
        Int_t pdg = mcTr->GetPdgCode();
        Double_t px = mcTr->GetMomentum().X();
        Double_t py = mcTr->GetMomentum().Y();
        Double_t pz = mcTr->GetMomentum().Z();
        Double_t p = sqrt(px * px + py * py + pz * pz);
        if (TMath::Abs(p) < 1.e-6) {
          px = 1.e-6;
          py = 1.e-6;
          pz = 1.e-6;
          p = 1.e-6;
        }
        Double_t q = 1;
        { // get charge
          TParticlePDG *part = TDatabasePDG::Instance()->GetParticle(pdg);
          if (part)
            q = part->Charge() / 3.f;
        }
        // Double_t ex,ey,ez,qp;
        outMCT << mcTr->GetMotherID() << " " << pdg << endl;
        outMCT << mcTr->GetStartVertex().X() << " " << mcTr->GetStartVertex().Y() << " " << mcTr->GetStartVertex().Z() << " " << px / fabs(p) << " " << py / fabs(p) << " "
               << pz / fabs(p) << " " << q / p << endl;
        outMCT << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << endl;
        outMCT << p << " " << sqrt(px * px + py * py) << endl;
        outMCT << nHitsInMCTrack[iTr] << " " << nMCPointsInMCTrack[iTr] << " " << FirstMCPointIDInMCTrack[iTr] << endl;
        outMCT << 0 << " " << 0 << " " << 1 << endl;
      }
    }
  }

  if (MCTrackSortedArray)
    delete[] MCTrackSortedArray;

  if (fDoPerformance) {
    outH.close();
    outHL.close();
    outMCT.close();
    outMCP.close();
  }

  // tracking

#ifdef DO_TPCCATRACKER_EFF_PERFORMANCE
  /*PndCAPerformance *  */ perf = &PndCAPerformance::Instance(); //[R.K. 9/2018] unshadow
#endif

  PndCAGBTracker *tracker = 0;
  const PndCAGBTracker *trackerConst = 0;

  tracker = new PndCAGBTracker;

  /*if ( !tracker->ReadSettingsFromFile(filePrefix) ) {
    cout << "ERROR: Settings file can't be opened!" << endl;
    }*/

  std::string str = "30 \
-20 \
0 2 0.0048 1.6 0 2 2 \
1 4 0.0048 1.6 0 2 2 \
2 9 0.0048 1.6 0 2 2 \
3 12 0.0048 1.6 0 2 2 \
4 16.619 0.0006 0.016 0 1 4 \
5 17.4937 0.0006 0.016 0 1 4 \
6 18.3684 0.0006 0.016 0 1 4 \
7 19.2431 0.0006 0.016 0 1 4 \
8 20.1178 0.0006 0.016 0 1 4 \
9 20.9924 0.0006 0.016 0 1 6 \
10 21.8671 0.0006 0.016 0 1 6 \
11 22.7418 0.0006 0.016 0 1 6 \
12 23.7518 0.0006 0.016 0.0523596 1 6 \
13 24.6265 0.0006 0.016 0.0523596 1 6 \
14 25.8805 0.0006 0.016 -0.0523596 1 6 \
15 26.7552 0.0006 0.016 -0.0523596 1 6 \
16 28.0092 0.0006 0.016 0.0523596 1 6 \
17 28.8839 0.0006 0.016 0.0523596 1 6 \
18 30.1378 0.0006 0.016 -0.0523596 1 6 \
19 31.0126 0.0006 0.016 -0.0523596 1 6 \
20 32.3634 0.0006 0.016 0 1 5 \
21 33.2381 0.0006 0.016 0 1 4 \
22 34.1128 0.0006 0.016 0 1 4 \
23 34.9874 0.0006 0.016 0 1 4 \
24 35.8621 0.0006 0.016 0 1 4 \
25 36.7368 0.0006 0.016 0 1 4 \
26 37.6115 0.0006 0.016 0 1 4 \
27 38.4862 0.0006 0.016 0 1 4 \
28 39.3609 0.0006 0.016 0 1 4 \
29 40.2356 0.0006 0.016 0 1 4";

  std::istringstream settings(str);
  tracker->ReadSettings(settings);

  // set settings

  tracker->GetParametersNonConst().SetBz(Bz);

  trackerConst = tracker;

  do {
    int kEvents = iEvent;
    char buf[6];
    sprintf(buf, "%d", kEvents);
    const string fileName = filePrefix + "event" + string(buf) + "_";

    // std::cout << "CA tracker: Loading Event " << kEvents << "..." << std::endl;
    tracker->SetHits(vHits);
    /*
    if (!tracker->ReadHitsFromFile(fileName)) {
      cout << "Hits Data for Event " << kEvents << " can't be read." << std::endl;
      break;
    }
    */
    // std::cout << "Event " << kEvents << " CPU reconstruction..." << std::endl;

#ifdef DO_TPCCATRACKER_EFF_PERFORMANCE
    // cout<<"Filename "<<fileName<<endl;
    if (fDoPerformance && perf) {
      perf->SetOutputFile(perfHistoFile);
      perf->SetTracker(tracker);
      std::cout << "Loading Monte-Carlo Data for Event " << kEvents << "..." << std::endl;
      if (!perf->ReadDataFromFiles(fileName)) {
        cout << "Monte-Carlo Data for Event " << kEvents << " can't be read." << std::endl;
        break;
      }

      perf->CombineHits();
    }
#endif

    // cout<<"Run trackfinder .. "<<endl;

    tracker->FindTracks();

    // Write output
    {
      int nOutTracks = 0;
      for (int itr = 0; itr < tracker->NTracks(); itr++) {
        const PndCAGBTrack &tr = tracker->Track(itr);
        // cout<<"Output track:"<<endl;
        PndTrackCand outCand;
        for (int ih = 0; ih < tr.NHits(); ih++) {
          int hitIndex = tracker->TrackHit(tr.FirstHitRef() + ih);
          const PndCAGBHit &hit = tracker->Hit(hitIndex);
          if (hit.PndDetID() == 24)
            cout << hit.PndDetID() << " " << hit.PndHitID() << endl;
          outCand.AddHit(hit.PndDetID(), hit.PndHitID(), hit.IRow());
        }
        outCand.setMcTrackId(-1);
        // float x,y,z,px,py,pz,cov[21];
        // tr.Param().GetXYZPxPyPz( x,y,z,px,py,pz,cov);

        FairTrackParP paramFirst;
        FairTrackParP paramLast;

        CATrackParToFairTrackParP(&paramFirst, &tr.InnerParam());
        CATrackParToFairTrackParP(&paramLast, &tr.OuterParam());

        // PndTrack tmp(paramFirst,paramLast,outCand);
        // tmp.Print();
        PndTrack *outTrack = new ((*fSttMvdPndTrackArray)[nOutTracks]) PndTrack(paramFirst, paramLast, outCand);
        outTrack->SetRefIndex(nOutTracks);
        outTrack->SetFlag(0);
        nOutTracks++;
      }
    }

#ifdef DO_TPCCATRACKER_EFF_PERFORMANCE
    if (fDoPerformance && perf) {
      cout << "Run performance.. " << endl;
      // SG!!! tracker->SaveTracksInFile(fileName);
      if (trackerConst->NHits() > 0) {
        perf->InitSubPerformances();
        perf->ExecPerformance();
      } else {
        cout << "Event " << kEvents << " contains 0 hits." << std::endl;
      }
    }
#endif

    if (fVerbose > 0) {

      const bool ifAvarageTime = 1;
      if (!ifAvarageTime) {
        std::cout << "Reconstruction Time"
                  << " Real = " << std::setw(10) << (trackerConst->SliceTrackerTime() + trackerConst->StatTime(9)) * 1.e3 << " ms,"
                  << " CPU = " << std::setw(10) << (trackerConst->SliceTrackerCpuTime() + trackerConst->StatTime(10)) * 1.e3 << " ms" << std::endl;
      } else {
        const int NTimers = trackerConst->NTimers();
        static int statIEvent = 0;
        static double *statTime = new double[NTimers];
        static double statTime_SliceTrackerTime = 0;
        static double statTime_SliceTrackerCpuTime = 0;

        if (!statIEvent) {
          for (int i = 0; i < NTimers; i++) {
            statTime[i] = 0;
          }
        }

        statIEvent++;
        for (int i = 0; i < NTimers; i++) {
          statTime[i] += trackerConst->StatTime(i);
        }
        statTime_SliceTrackerTime += trackerConst->SliceTrackerTime();
        statTime_SliceTrackerCpuTime += trackerConst->SliceTrackerCpuTime();

        std::cout << "Reconstruction Time"
                  << " Real = " << std::setw(10) << 1. / statIEvent * (statTime_SliceTrackerTime + statTime[9]) * 1.e3 << " ms,"
                  << " CPU = " << std::setw(10) << 1. / statIEvent * (statTime_SliceTrackerCpuTime + statTime[10]) * 1.e3 << " ms," << std::endl;
      }
    } // fVerbose>0

  } while (0);

  delete tracker;
}

void PndCATracking::Finish()
{
#ifdef DO_TPCCATRACKER_EFF_PERFORMANCE
  if (fDoPerformance && perf) {
    perf->WriteHistos();
  }
  // perfHistoFile->Close();
#endif
}

void PndCATracking::WriteMVDHits(std::vector<PndCAGBHit> &vHits, std::fstream &outH, std::fstream &outHL, std::fstream & /*outMCT*/, std::fstream & /*outMCP*/, int &iHit,
                                 map<int, unsigned int> &nHitsInMCTrack, bool isPixel) //[R.K. 9/2018] unused
{
  TClonesArray *mvdHitsArray;
  if (isPixel)
    mvdHitsArray = fMvdPixelHitsArray;
  else
    mvdHitsArray = fMvdStripHitsArray;
  for (int iHMvd = 0; iHMvd < mvdHitsArray->GetEntriesFast(); iHMvd++) {
    PndSdsHit *currenthit = (PndSdsHit *)mvdHitsArray->At(iHMvd);

    Int_t sensorID = currenthit->GetSensorID();
    gGeoManager->cd(PndGeoHandling::Instance()->GetPath(sensorID));
    TGeoHMatrix *transMat = gGeoManager->GetCurrentMatrix();
    Double_t *mmm = transMat->GetRotationMatrix();
    /*
    if( isPixel ){
      cout<<"sensor "<<sensorID<<endl;
      cout<<"     "<<mmm[0]<<" "<<mmm[1]<<" "<<mmm[2]<<endl;
      cout<<"     "<<mmm[3]<<" "<<mmm[4]<<" "<<mmm[5]<<endl;
      cout<<"     "<<mmm[6]<<" "<<mmm[7]<<" "<<mmm[8]<<endl;
    }
    */
    Double_t A = 0;

    TVector3 position = currenthit->GetPosition();
    Double_t x = position.X();
    Double_t y = position.Y();
    Double_t z = position.Z();
    // cout<<x<<" "<<y<<" "<<z<<endl;
    Double_t r = TMath::Sqrt(x * x + y * y);
    int iSta = -1;
    /*
     Double_t *mTrans = transMat->GetTranslation();
     cout<<"sensor "<<sensorID<<", pixel "<<isPixel<<endl;
     cout<<"trans "<<mTrans[0]<<" "<<mTrans[1]<<" "<<mTrans[2]<<endl;
     for( int i=0; i<3; i++){
     for( int j=0; j<3; j++){
     cout<<mmm[i*3+j]<<" ";
     }
      cout<<endl;
    }
    cout<<"xyz "<<x<<" "<<y<<" "<<z<<endl;
    */

    // get station angle A and station index iSta

    if (fabs(mmm[6]) < 0.999 && fabs(mmm[7]) < 0.999)
      continue;
    if (fabs(mmm[6]) < 0.999 && fabs(mmm[7]) < 0.999) { // forward detector, perpendicular to beam axis Z
      // cout<<"forward detector!!??"<<endl;
      // Double_t sinA = mmm[2];
      // cout << "angle "<<TMath::ASin(sinA)<<endl;
      A = 1234;
      if ((z > 0.) && (z < 3.))
        iSta = 0;
      if ((z > 3.) && (z < 5.5))
        iSta = 1;
      if ((z > 5.5) && (z < 8.0))
        iSta = 2;
      if ((z > 8.0) && (z < 12.0))
        iSta = 3;
      if ((z > 12.0) && (z < 18.5))
        iSta = 4;
      if ((z > 18.5) && (z < 24.5))
        iSta = 5;
    } else {
      // cout<<" phi detector"<<endl;
      if (fabs(mmm[6]) > 0.999) {
        if (currenthit->GetPosition().Y() >= 0) {
          Double_t sinA = mmm[2];
          A = TMath::ASin(sinA);
        } else {
          Double_t sinA = -mmm[2];
          A = TMath::ASin(sinA) - TMath::Pi();
        }
      }
      if (fabs(mmm[7]) > 0.999) {
        if (currenthit->GetPosition().Y() >= 0) {
          Double_t sinA = -mmm[2];
          A = TMath::ASin(sinA);
        } else {
          Double_t sinA = mmm[2];
          A = TMath::ASin(sinA) + TMath::Pi();
        }
      }
      if ((r > 0.) && (r < 3.5))
        iSta = 0; // 2;
      if ((r > 3.5) && (r < 7.5))
        iSta = 1; // 3;
      if ((r > 7.5) && (r < 11.0))
        iSta = 2; // 4;
      if ((r > 11.0) && (r < 15.0))
        iSta = 3; // 5;
    }

    TMatrixT<Double_t> RM(3, 3, mmm);
    TMatrixT<Double_t> CR = currenthit->GetCov(); // rotated CovMatrix

    //     CR = RM*CR;
    //     CR = CR*RM.Transpose(RM);

    Double_t radius = 0;

    // error calculation according to the curve chosen by flag
    Double_t closestDistanceError = 0;

    int clusterID = currenthit->GetClusterIndex();
    PndSdsCluster *cluster;
    if (isPixel)
      cluster = (PndSdsClusterPixel *)fMvdPixelClusterArray->At(clusterID);
    else
      cluster = (PndSdsClusterStrip *)fMvdStripClusterArray->At(clusterID);
    int digiID = -1;
    vector<int> trackID;

    for (int iD = 0; iD < cluster->GetClusterSize(); iD++) {
      digiID = cluster->GetDigiIndex(iD);
      if (digiID < 0)
        continue;
      PndSdsDigi *digi;
      if (isPixel)
        digi = (PndSdsDigiPixel *)fMvdPixelDigiArray->At(digiID);
      else
        digi = (PndSdsDigiStrip *)fMvdStripDigiArray->At(digiID);

      int pointID = -1;
      for (unsigned int iP = 0; iP < digi->GetIndices().size(); iP++) {
        pointID = digi->GetIndices()[iP];
        if (pointID < 0)
          continue;
        PndSdsMCPoint *point = (PndSdsMCPoint *)fMvdPointsArray->At(pointID);
        int curTrID = point->GetTrackID();
        bool newID = 1;
        for (unsigned int iTr = 0; iTr < trackID.size(); iTr++)
          if (trackID[iTr] == curTrID) {
            newID = 0;
            break;
          }
        if (newID)
          trackID.push_back(curTrID);
      }
    }
    // cout<<" is pixel "<<isPixel<<" sta "<<iSta<<" r "<<r<<endl;

    // bool fwd = ( fabs(mmm[6]) < 0.999 && fabs(mmm[7]) < 0.999 );
    // if( isPixel ) cout<<"bla "<< fwd<<", "<<x<<", "<<y<<", "<<z<<", "<<r<<","<<endl;
    PndCAGBHit h;
    h.SetGlobalX(x);
    h.SetGlobalY(y);
    h.SetZ(z);
    h.SetXW(x);
    h.SetYW(y);
    h.SetZW(z);

    h.SetR(radius);
    h.SetC(CR);
    h.SetErr2R(closestDistanceError * closestDistanceError);
    h.SetIRow(iSta);
    h.SetID(iHit);
    if (isPixel)
      h.SetPndDetID(FairRootManager::Instance()->GetBranchId(fMvdPixelHitsBranchName));
    else
      h.SetPndDetID(FairRootManager::Instance()->GetBranchId(fMvdStripHitsBranchName));
    h.SetPndHitID(iHMvd);
    h.SetAngle(-A);
    h.SetTubeR(0.);
    h.SetTubeHalfLength(0.);

    vHits.push_back(h);

    int trackIDs[3] = {-1, -1, -1};
    if (trackID.size() > 0)
      trackIDs[0] = trackID[0];
    if (trackID.size() > 1)
      trackIDs[1] = trackID[1];
    if (trackID.size() > 2)
      trackIDs[2] = trackID[2];

    if (fDoPerformance) {
      outH << h;
      outHL << trackIDs[0] << " " << trackIDs[1] << " " << trackIDs[2] << endl;
    }

    /*
     outH << x << " " << y << " " << z << " " << radius << endl;
     outH << CR[0][0] << " " << CR[0][1] << " " << CR[0][2] << endl;
     outH << CR[1][0] << " " << CR[1][1] << " " << CR[1][2] << endl;
     outH << CR[2][0] << " " << CR[2][1] << " " << CR[2][2] << endl;
     outH << closestDistanceError*closestDistanceError << endl;
     outH << 0 << " " << 0 << endl;
     outH << 0 << " "  << 0 << " "  << 0 << endl;
     outH << iSta << " " << iHit << " " << A << endl;
     */
    iHit++;

    if (nHitsInMCTrack.find(trackIDs[0]) != nHitsInMCTrack.end()) {
      nHitsInMCTrack[trackIDs[0]]++;
    } else {
      nHitsInMCTrack[trackIDs[0]] = 1;
    }
  }
}
