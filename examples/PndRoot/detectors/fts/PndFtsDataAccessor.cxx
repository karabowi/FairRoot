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

#include "PndFtsDataAccessor.h"

//#include "PndDetectorList.h"
#include "PndMCTrack.h"
#include "PndTrackCand.h"
#include "PndTrack.h"

#include "FairTrackParP.h"
#include "FairMCPoint.h"
#include "FairHit.h"
//#include "FairMCApplication.h"
#include "FairTask.h"
//#include "FairRunAna.h"
//#include "FairGeoNode.h"
//#include "FairGeoVector.h"
//#include "FairGeoMedium.h"
#include "FairRootManager.h"
#include "FairLogger.h"

#include "TObjectTable.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"
#include "TRandom.h"
#include <iostream>

// fts
#include "PndFtsPoint.h"
#include "PndFtsHit.h"
#include "PndFtsTube.h"
#include "PndFtsMapCreator.h"
// fairroot
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "FairField.h"
#include "TMatrixD.h"
#include "TVectorD.h"

using namespace std;
//________________________________________________________________
PndFtsDataAccessor::PndFtsDataAccessor() : FairTask("FTSDataAccessor"), fMCTracks(0), By(0), fPersistence(kTRUE), pdg(0)
{
  //---
  fVerbose = 0;
  for (int i = 0; i < 4; i++)
    fBranchActive[i] = kTRUE;
}

//_________________________________________________________________
PndFtsDataAccessor::~PndFtsDataAccessor()
{
  FairRootManager *fManager = FairRootManager::Instance();
  fManager->Write();
}

//_________________________________________________________________
void PndFtsDataAccessor::Register()
{
  //---
  if (fVerbose > 3)
    LOG(info) << "Register Done.";
}

void PndFtsDataAccessor::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fFtsParameters = (PndGeoFtsPar *)rtdb->getContainer("PndGeoFtsPar");
}

//________________________________________________________________
InitStatus PndFtsDataAccessor::Init()
{
  // ---
  if (fVerbose > 3)
    LOG(info) << "Init Start initialisation.";

  FairRootManager *fManager = FairRootManager::Instance();

  // Get MC arrays
  fMCTracks = dynamic_cast<TClonesArray *>(fManager->GetObject("MCTrack"));
  if (!fMCTracks) {
    LOG(warn) << "  PndFtsDataAccessor::Init: No MCTrack array! Needed for MC Truth";
    return kERROR;
  }

  // FTS
  fMCPoints[0] = dynamic_cast<TClonesArray *>(fManager->GetObject("FTSPoint"));
  if (!fMCPoints[0]) {
    LOG(warn) << "  PndFtsDataAccessor::Init: No FTSPoint array!";
    return kERROR;
  }
  fHits[0] = dynamic_cast<TClonesArray *>(fManager->GetObject("FTSHit"));
  if (!fHits[0]) {
    LOG(warn) << "  PndFtsDataAccessor::Init: No FTSHit array!";
    return kERROR;
  }
  fBranchIDs[0] = FairRootManager::Instance()->GetBranchId("FTSHit");

  // GEM
  fMCPoints[1] = dynamic_cast<TClonesArray *>(fManager->GetObject("GEMPoint"));
  if (!fMCPoints[1]) {
    LOG(warn) << "  PndFtsDataAccessor::Init: No GEMPoint array!";
    fMCPoints[1] = new TClonesArray("FairMCPoint");
  }
  fHits[1] = dynamic_cast<TClonesArray *>(fManager->GetObject("GEMHit"));
  if (!fHits[1]) {
    LOG(warn) << "  PndFtsDataAccessor::Init: No GEMHit array!";
    fHits[1] = new TClonesArray("FairHit");
  }
  fBranchIDs[1] = FairRootManager::Instance()->GetBranchId("GEMHit");

  // MVD Pixel
  fMCPoints[2] = dynamic_cast<TClonesArray *>(fManager->GetObject("MVDPoint"));
  if (!fMCPoints[2]) {
    LOG(warn) << "  PndFtsDataAccessor::Init: No MVDPoint array!";
    fMCPoints[1] = new TClonesArray("FairMCPoint");
  }
  fHits[2] = dynamic_cast<TClonesArray *>(fManager->GetObject("MVDHitsPixel"));
  if (!fHits[2]) {
    LOG(warn) << "  PndFtsDataAccessor::Init: No MVDHitsPixel array!";
    fHits[2] = new TClonesArray("FairHit");
  }
  fBranchIDs[2] = FairRootManager::Instance()->GetBranchId("MVDHitsPixel");

  fMCPoints[3] = fMCPoints[2];
  fHits[3] = dynamic_cast<TClonesArray *>(fManager->GetObject("MVDHitsStrip"));
  if (!fHits[3]) {
    LOG(warn) << "  PndFtsDataAccessor::Init: No MVDHitsStrip array!";
    fMCPoints[3] = new TClonesArray("FairHit");
  }
  fBranchIDs[3] = FairRootManager::Instance()->GetBranchId("MVDHitsStrip");

  if (fVerbose > 3)
    LOG(info) << "Init Fetched all arrays.";

  Register();

  pdg = new TDatabasePDG();
  if (fVerbose > 3)
    LOG(info) << "Init End initialisation.";

  PndFtsMapCreator *mapperFts = new PndFtsMapCreator(fFtsParameters);
  fTubeArrayFts = mapperFts->FillTubeArray();

  if (fVerbose > 3)
    Info("Init", "Try to get B field.");
  fField = FairRunAna::Instance()->GetField();

  By = 0.;
  po[0] = 0;
  po[1] = 0;
  po[2] = 0;
  fField->GetFieldValue(po, BB); // return value in KG (G3)
  By = BB[1] / 10.;              // By is y-component of magnetic field in Tesla

  return kSUCCESS;
}

//_________________________________________________________________
void PndFtsDataAccessor::Exec(Option_t *)
{
  if (fVerbose > 3)
    LOG(info) << "Exec Start eventloop.";
  if (fVerbose > 4) {
    LOG(info) << "Exec Print some array properties";
    for (int iii = 0; iii < 4; iii++) {
      std::cout << "fHits[" << iii << "] is branchID " << fBranchIDs[iii] << " with the name " << fHits[iii]->GetName() << " and contains " << fHits[iii]->GetEntriesFast()
                << " entries." << std::endl;
      std::cout << "fMCPoints[" << iii << "] with the name " << fMCPoints[iii]->GetName() << " and contains " << fMCPoints[iii]->GetEntriesFast() << " entries." << std::endl;
    }
  }

  // create output files
  static int iEvent = -1;
  iEvent++;
  TString folder_name = "data/";
  TString fadata_name = "data/event";
  fadata_name += iEvent;
  TString fadataH_name = fadata_name + "_hits.data";
  TString fadataHL_name = fadata_name + "_hitLabels.data";
  TString fadataMCT_name = fadata_name + "_MCTracks.data";
  TString fadataMCP_name = fadata_name + "_MCPoints.data";
  fstream outH(fadataH_name, fstream::out);
  fstream outHL(fadataHL_name, fstream::out);
  fstream outMCT(fadataMCT_name, fstream::out);
  fstream outMCP(fadataMCP_name, fstream::out);

  // FairHit* ghit = nullptr; //[R.K. 01/2017] unused variable?

  // Loop over all hits from FTS first (later other detectors, too)

  // Detector loop
  for (Int_t iDet = 0; iDet < 1; iDet++) { // only FTS hits at the moment

    if (kFALSE == fBranchActive[iDet])
      continue; // skip manually switched off detector
    if (fVerbose > 4)
      LOG(info) << "Exec Use detector " << iDet;

    std::map<Int_t, FairHit *> firstHit;
    std::map<Int_t, FairHit *> lastHit;
    std::map<Int_t, FairMCPoint *> firstPoint;
    std::map<Int_t, FairMCPoint *> lastPoint;
    std::map<Int_t, PndTrackCand *> candlist;

    const int NHits = fHits[iDet]->GetEntriesFast();
    // cout << " ----------------------- Store data ------------------------- " << endl;
    // cout << " NFTSHits = " << NHits << endl;
    outH << NHits << endl;
    outHL << NHits << endl;
    outMCP << NHits << endl; // WARNING: Currently use only MCPoints, which produces hits!!!!
    map<int, unsigned int> nHitsInMCTrack, nMCPointsInMCTrack, FirstMCPointIDInMCTrack;
    const int NStations = 48;
    vector<float> zOfStation(NStations, -1.f);
    int iWrittenHit = -1;
    for (Int_t ih = 0; ih < NHits; ih++) {

      // RecoHits
      PndFtsHit *hit = (PndFtsHit *)fHits[iDet]->At(ih);
      if (!hit) {
        if (fVerbose > 3)
          Error("Exec", "Have no ghit %i, array size: %i", ih, fHits[iDet]->GetEntriesFast());
        continue;
      }

      iWrittenHit++;
      Int_t tubeID = ((PndFtsHit *)hit)->GetTubeID();
      PndFtsTube *tube = (PndFtsTube *)fTubeArrayFts->At(tubeID);
      TVector3 position = tube->GetPosition();
      const Double_t x = position.X();
      const Double_t y = position.Y();
      const Double_t z = position.Z();
      outH << x << " " << y << " " << z << " " << hit->GetIsochrone() << endl;

      const int iSta = hit->GetLayerID() - 1;
      if (zOfStation[iSta] == -1.f)
        zOfStation[iSta] = z;
      else if (std::abs(zOfStation[iSta] - z) > 1e-4f) // 1mum precision
        cout << " WARNING: different z positions within one layer. Settings inforation can be corrupted. " << zOfStation[iSta] << " " << z << endl;

      const Double_t dR = hit->GetIsochroneError();
      const Double_t dR2 = dR * dR;
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
      outH << CR[0][0] << " " << CR[0][1] << " " << CR[0][2] << endl;
      outH << CR[1][0] << " " << CR[1][1] << " " << CR[1][2] << endl;
      outH << CR[2][0] << " " << CR[2][1] << " " << CR[2][2] << endl;
      outH << dR2 << endl;
      outH << tube->GetRadIn() << " " << tube->GetHalfLength() << endl;

      TVector3 wire_direction = tube->GetWireDirection();                                           // shows the wire direction, ( notSkewed layers - wire_direction.X() == 1 ).
      outH << wire_direction.X() << " " << wire_direction.Y() << " " << wire_direction.Z() << endl; // TODO rid of covMatrix
      outH << iSta << " " << iWrittenHit << " " << -1234 << endl;                                   // station position is normal to z

      // MC Points
      Int_t mchitid = hit->GetRefIndex();
      if (mchitid < 0) {
        if (fVerbose > 3)
          LOG(error) << "Exec Have a negative mcHit " << mchitid;
        continue;
      }
      const FairMCPoint *point = (FairMCPoint *)(fMCPoints[iDet]->At(mchitid));
      if (!point)
        continue;
      Int_t trackID = point->GetTrackID();
      if (trackID < 0)
        continue;

      Double_t q = 1;
      { // get charge
        const TClonesArray *fMCTrackArray = fMCTracks;
        if (trackID < fMCTrackArray->GetEntriesFast()) {
          const PndMCTrack *mcTr = (PndMCTrack *)fMCTrackArray->At(trackID);
          if (mcTr) {
            TParticlePDG *part = TDatabasePDG::Instance()->GetParticle(mcTr->GetPdgCode());
            if (part)
              q = part->Charge() / 3.f;
          } else {
            cout << " Bad MCTracks2" << endl;
          }
        } else {
          cout << " Bad MCTracks" << endl;
        }
      }

      outHL << trackID << " " << -1 << " " << -1 << endl;
      outMCP << point->GetX() << " " << point->GetY() << " " << point->GetZ() << endl;
      outMCP << point->GetPx() << " " << point->GetPy() << " " << point->GetPz() << " "
             << q / sqrt(point->GetPx() * point->GetPx() + point->GetPy() * point->GetPy() + point->GetPz() * point->GetPz()) << endl;
      outMCP << 0 << " " << iSta << " " << trackID << " " << trackID << endl;

      if (nHitsInMCTrack.find(trackID) != nHitsInMCTrack.end()) {
        nHitsInMCTrack[trackID]++;
        nMCPointsInMCTrack[trackID]++;
      } else {
        nHitsInMCTrack[trackID] = 1;
        nMCPointsInMCTrack[trackID] = 1;
        FirstMCPointIDInMCTrack[trackID] = iWrittenHit;
      }

    } // end loop over hits

    { // MC Tracks
      const TClonesArray *fMCTrackArray = fMCTracks;
      const int nMCTracks = fMCTrackArray->GetEntriesFast();

      outMCT << nMCTracks << endl;
      for (int i = 0; i < nMCTracks; i++) {
        const PndMCTrack *mcTr = (PndMCTrack *)fMCTrackArray->At(i);

        if (!mcTr) {
          outMCT << -1 << " " << -1 << endl;
          outMCT << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << endl;
          outMCT << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << endl;
          outMCT << 0 << " " << 0 << endl;
          outMCT << 0 << " " << 0 << " " << 0 << endl;
          outMCT << 0 << " " << 0 << " " << 1 << endl;

        } else {

          Int_t pdgCode = mcTr->GetPdgCode();
          Double_t px = mcTr->GetMomentum().X();
          Double_t py = mcTr->GetMomentum().Y();
          Double_t pz = mcTr->GetMomentum().Z();
          Double_t p = sqrt(px * px + py * py + pz * pz);
          Double_t q = 1;
          { // get charge
            TParticlePDG *part = TDatabasePDG::Instance()->GetParticle(pdgCode);
            if (part)
              q = part->Charge() / 3.f;
          }
          // Double_t ex,ey,ez,qp; //[R.K. 01/2017] unused variable?
          outMCT << mcTr->GetMotherID() << " " << pdgCode << endl;
          outMCT << mcTr->GetStartVertex().X() << " " << mcTr->GetStartVertex().Y() << " " << mcTr->GetStartVertex().Z() << " " << px / fabs(p) << " " << py / fabs(p) << " "
                 << pz / fabs(p) << " " << q / p << endl;
          outMCT << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << endl;
          outMCT << p << " " << sqrt(px * px + py * py) << endl;
          outMCT << nHitsInMCTrack[i] << " " << nMCPointsInMCTrack[i] << " " << FirstMCPointIDInMCTrack[i] << endl;
          outMCT << 0 << " " << 0 << " " << 1 << endl;
        }
      }
    }

    // Settings
    static bool isFirstEvent = true;
    if (isFirstEvent) {
      isFirstEvent = false;

      TString fadataGeo_name = folder_name + "settings.data";
      fstream outGeo(fadataGeo_name, fstream::out);

      outGeo << NStations << endl;
      outGeo << -10 << endl; // field
      for (int iS = 0; iS < NStations; iS++) {
        float slope = 5.f / 180.f * 3.14159265359f;
        switch ((iS / 2) % 4) {
        case 0: slope = 0; break;
        case 1: slope = slope; break;
        case 2: slope = -slope; break;
        case 3: slope = 0; break;
        }
        outGeo << iS << " " << zOfStation[iS] << " " << 0.00044 << " " << 0.0117 << " " << slope << " " << 1 << " " << 6 << endl;
      }

      const int NSubStations = 8;
      const float Height[NStations / NSubStations] = {640, 640, 741.9, 818.9, 1200.0, 1200.0};                // mm
      float YMin[NStations / NSubStations];                                                                   // filled later
      float YMax[NStations / NSubStations];                                                                   // filled later
      float XMin[NStations / NSubStations] = {-659.025, -659.025, -881.225, -1042.825, -1951.825, -1951.825}; // mm
      float XMax[NStations / NSubStations];                                                                   // filled later
      const float NTubes[NStations / NSubStations] = {132, 132, 176, 208, 388, 388};
      float Z[NStations] = {2949.627, 2958.373, 2999.627, 3008.373, 3049.627, 3058.373, 3099.627, 3108.373, // mm
                            3269.627, 3278.373, 3319.627, 3328.373, 3369.627, 3378.373, 3419.627, 3428.373, 3940.627, 3949.373, 4015.377, 4024.123, 4160.627, 4169.373,
                            4235.377, 4244.123, 4380.627, 4389.373, 4455.377, 4464.123, 4600.627, 4609.373, 4675.377, 4684.123, 6070.627, 6079.373, 6120.627, 6129.373,
                            6170.627, 6179.373, 6220.627, 6229.373, 6390.627, 6399.373, 6440.627, 6449.373, 6490.627, 6499.373, 6540.627, 6549.373};

      for (int i = 0; i < NStations / NSubStations; i++) {
        XMax[i] = XMin[i] + NTubes[i] * 10.1;
        YMin[i] = -Height[i] / 2;
        YMax[i] = Height[i] / 2;
        // convert into cm
        XMin[i] /= 10;
        XMax[i] /= 10;
        YMin[i] /= 10;
        YMax[i] /= 10;
      }
      for (int i = 0; i < NStations; i++) {
        Z[i] /= 10;
      }

      // Field
      // int ind = 0; //[R.K. 01/2017] unused variable?
      for (int i = 0; i < 3; i++) {
        Double_t z = i * (Z[0] - 1.f) / 2; // last point is taken in 1 cm from first station
        Double_t point[3] = {0, 0, z};
        Double_t B[3] = {0, 0, 0};
        fField->GetFieldValue(point, B);
        outGeo << z << " " << B[0] << " " << B[1] << " " << B[2] << endl;
      }

      const int M = 5; // polinom order
      const int N = (M + 1) * (M + 2) / 2;
      const int MaxN = 100;

      for (Int_t ist = 0; ist < NStations; ist++) {
        const float XMinS = XMin[ist / 8] / 2;
        const float XMaxS = XMax[ist / 8] / 2;
        const float YMinS = YMin[ist / 8] / 2;
        const float YMaxS = YMax[ist / 8] / 2;
        const float ZS = Z[ist];
        const float DX = XMaxS - XMinS;
        const float DY = YMaxS - YMinS;
        const float XAverage = XMaxS - XMinS;
        const float YAverage = YMaxS - YMinS;

        double dx = 1.; // step for the field approximation
        double dy = 1.;

        if (dx > DX / N / 4)
          dx = DX / N / 4.;
        if (dy > DY / N / 2)
          dy = DY / N / 4.;

        double C[3][MaxN] = {0};
        for (int i = 0; i < 3; i++)
          for (int k = 0; k < MaxN; k++)
            C[i][k] = 0;

        TMatrixD A(N, N);
        TVectorD b0(N), b1(N), b2(N);
        for (int i = 0; i < N; i++) {
          for (int j = 0; j < N; j++)
            A(i, j) = 0.;
          b0(i) = b1(i) = b2(i) = 0.;
        }

        const int NXBins = (XMaxS - XMinS) / dx;
        const int NYBins = (YMaxS - YMinS) / dy;
        const float dzVirtualStations = 5.f;
        int nVirtualStations = (ist > 0 && ist % NSubStations == 0) ? static_cast<int>((Z[ist] - Z[ist - 1]) / dzVirtualStations) : 0;
        outGeo << ist << endl;
        outGeo << nVirtualStations << " " << dzVirtualStations << endl;
        outGeo << XMinS << " " << dx << " " << NXBins << endl;
        outGeo << YMinS << " " << dy << " " << NYBins << endl;

        for (int iV = nVirtualStations; iV >= 0; iV--)
          for (double ixb = 0; ixb < NXBins; ixb++) {
            for (double iyb = 0; iyb < NYBins; iyb++) {
              const double x = XMinS + ixb * dx;
              const double y = YMinS + iyb * dy;

              Double_t p[3] = {x, y, ZS - iV * dzVirtualStations};
              Double_t B[3] = {0., 0., 0.};
              fField->GetFieldValue(p, B);
              //            outGeo << B[0] << " " << B[1] << " " << B[2] << endl;
              double r = sqrt((x - XAverage) * (x - XAverage) / DX / DX + (y - YAverage) * (y - YAverage) / DY / DY);
              //            if( r>1. ) continue;
              Double_t w = 1. / (r * r + 1);

              TVectorD m(N);
              m(0) = 1;
              for (int i = 1; i <= M; i++) {
                int k = (i - 1) * (i) / 2;
                int l = i * (i + 1) / 2;
                for (int j = 0; j < i; j++)
                  m(l + j) = x * m(k + j);
                m(l + i) = y * m(k + i - 1);
              }

              TVectorD mt = m;
              for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++)
                  A(i, j) += w * m(i) * m(j);
                b0(i) += w * B[0] * m(i);
                b1(i) += w * B[1] * m(i);
                b2(i) += w * B[2] * m(i);
              }
            }
          }
        double det;
        A.Invert(&det);
        TVectorD c0 = A * b0, c1 = A * b1, c2 = A * b2;
        for (int i = 0; i < N; i++) {
          C[0][i] = c0(i);
          C[1][i] = c1(i);
          C[2][i] = c2(i);
        }

        outGeo << N << endl;
        for (int k = 0; k < 3; k++) {
          for (int j = 0; j < N; j++)
            outGeo << C[k][j] << " ";
          outGeo << endl;
        }

        // check
#if 0
        Int_t N = 0;
        Double_t sumR = 0;
        for( double x = XMinS; x <= XMaxS; x += dx/10 )
          for( double y = YMinS; y <= YMaxS; y += dy/10 ) {
            float x2 = x*x;
            float y2 = y*y;
            float xy = x*y;

            float x3 = x2*x;
            float y3 = y2*y;
            float xy2 = x*y2;
            float x2y = x2*y;

            float x4 = x3*x;
            float y4 = y3*y;
            float xy3 = x*y3;
            float x2y2 = x2*y2;
            float x3y = x3*y;

            float x5 = x4*x;
            float y5 = y4*y;
            float xy4 = x*y4;
            float x2y3 = x2*y3;
            float x3y2 = x3*y2;
            float x4y = x4*y;

            float x6 = x5*x;
            float y6 = y5*y;
            float xy5 = x*y5;
            float x2y4 = x2*y4;
            float x3y3 = x3*y3;
            float x4y2 = x4*y2;
            float x5y = x5*y;

            float y7 = y6*y;
            float xy6 = x*y6;
            float x2y5 = x2*y5;
            float x3y4 = x3*y4;
            float x4y3 = x4*y3;
            float x5y2 = x5*y2;
            float x6y = x6*y;
            float x7 = x5*x;

            Double_t BA[3] = {0.,0.,0.};
            BA[0] = C[0][0] +C[0][1]*x +C[0][2]*y +C[0][3]*x2 +C[0][4]*xy +C[0][5]*y2 +C[0][6]*x3 +C[0][7]*x2y +C[0][8]*xy2 +C[0][9]*y3
              +C[0][10]*x4 +C[0][11]*x3y +C[0][12]*x2y2 +C[0][13]*xy3 +C[0][14]*y4
              +C[0][15]*x5 +C[0][16]*x4y +C[0][17]*x3y2 +C[0][18]*x2y3 +C[0][19]*xy4 +C[0][20]*y5
              +C[0][21]*x6 +C[0][22]*x5y +C[0][23]*x4y2 +C[0][24]*x3y3 +C[0][25]*x2y4 +C[0][26]*xy5 +C[0][27]*y6
              +C[0][28]*x7 +C[0][29]*x6y +C[0][30]*x5y2 +C[0][31]*x4y3 +C[0][32]*x3y4 +C[0][33]*x2y5 +C[0][34]*xy6 + C[0][35]*y7;

            BA[1] = C[1][0] +C[1][1]*x +C[1][2]*y +C[1][3]*x2 +C[1][4]*xy +C[1][5]*y2 +C[1][6]*x3 +C[1][7]*x2y +C[1][8]*xy2 +C[1][9]*y3
              +C[1][10]*x4 +C[1][11]*x3y +C[1][12]*x2y2 +C[1][13]*xy3 +C[1][14]*y4
              +C[1][15]*x5 +C[1][16]*x4y +C[1][17]*x3y2 +C[1][18]*x2y3 +C[1][19]*xy4 +C[1][20]*y5
              +C[1][21]*x6 +C[1][22]*x5y +C[1][23]*x4y2 +C[1][24]*x3y3 +C[1][25]*x2y4 +C[1][26]*xy5 +C[1][27]*y6
              +C[1][28]*x7 +C[1][29]*x6y +C[1][30]*x5y2 +C[1][31]*x4y3 +C[1][32]*x3y4 +C[1][33]*x2y5 +C[1][34]*xy6 + C[1][35]*y7;

            BA[2] = C[2][0] +C[2][1]*x +C[2][2]*y +C[2][3]*x2 +C[2][4]*xy +C[2][5]*y2 +C[2][6]*x3 +C[2][7]*x2y +C[2][8]*xy2 +C[2][9]*y3
              +C[2][10]*x4 +C[2][11]*x3y +C[2][12]*x2y2 +C[2][13]*xy3 +C[2][14]*y4
              +C[2][15]*x5 +C[2][16]*x4y +C[2][17]*x3y2 +C[2][18]*x2y3 +C[2][19]*xy4 +C[2][20]*y5
              +C[2][21]*x6 +C[2][22]*x5y +C[2][23]*x4y2 +C[2][24]*x3y3 +C[2][25]*x2y4 +C[2][26]*xy5 +C[2][27]*y6
              +C[2][28]*x7 +C[2][29]*x6y +C[2][30]*x5y2 +C[2][31]*x4y3 +C[2][32]*x3y4 +C[2][33]*x2y5 +C[2][34]*xy6 + C[2][35]*y7;

            Double_t p[3] = {x, y, ZS};
            Double_t B[3] = {0.,0.,0.};
            fField->GetFieldValue(p, B);
            Double_t r = (B[0] - BA[0])*(B[0] - BA[0]) +
              (B[1] - BA[1])*(B[1] - BA[1]) +
              (B[2] - BA[2])*(B[2] - BA[2]);
            r = sqrt(r);
            N++;
            sumR += r;
              //            if( r < 0.005 ) { cout << r << " " << x << " " << y << " " << B[0] << " " << B[1] << " " << B[2] <<  " " << BA[0] << " " << BA[1] << " " << BA[2] << endl; exit(0); }
          }
          //        cout << ist << " AVR R = " << sumR/N << endl;
#endif
      }

      outGeo.close();
    } // settings

  } // iDet

  outH.close();
  outHL.close();
  outMCT.close();
  outMCP.close();

  if (fVerbose > 3)
    LOG(info) << "Exec End eventloop.";
}

ClassImp(PndFtsDataAccessor);
