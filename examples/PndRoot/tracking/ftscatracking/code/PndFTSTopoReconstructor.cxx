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

// **************************************************************************
//                                                                          *
// Primary Authors: Ivan Kisel <kisel@kip.uni-heidelberg.de                 *
//                  Igor Kulakov <I.Kulakov@gsi.de>                         *
//                  Maksym Zyzak <M.Zyzak@gsi.de>                           *
//                                                                          *
//                                                                          *
// Permission to use, copy, modify and distribute this software and its     *
// documentation strictly for non-commercial purposes is hereby granted     *
// without fee, provided that the above copyright notice appears in all     *
// copies and that both the copyright notice and this permission notice     *
// appear in the supporting documentation. The authors make no claims       *
// about the suitability of this software for any purpose. It is            *
// provided "as is" without express or implied warranty.                    *
//                                                                          *
//***************************************************************************
#include "PndFTSTopoReconstructor.h"

#include "KFPTopoReconstructor.h"
#include "PndFTSCAGBTracker.h"
#include "KFPTrack.h"

#include "KFParticleSIMD.h"
#include "KFParticleFinder.h"
#include "PndFTSCADef.h"

#ifdef DO_TPCCATRACKER_EFF_PERFORMANCE
#include "PndFTSCAPerformance.h"       // dbg
#include "PndFTSCAGlobalPerformance.h" // dbg
#endif

PndFTSTopoReconstructor::~PndFTSTopoReconstructor()
{
  if (fKFPTopoReconstructor)
    delete fKFPTopoReconstructor;
}

void PndFTSTopoReconstructor::Init(PndFTSCAGBTracker *tracker)
{
#ifdef PANDA_STT
  KFParticle::SetField(-tracker->GetParameters().Bz());     // to understand -1 see SetField
  KFParticleSIMD::SetField(-tracker->GetParameters().Bz()); // to understand -1 see SetField
#else
  KFParticle::SetField(-tracker->GetParameters().Bz());     // to understand -1 see SetField
  KFParticleSIMD::SetField(-tracker->GetParameters().Bz()); // to understand -1 see SetField
#endif
                                                            // create and fill array of tracks to init KFParticleTopoReconstructor
  const int nTracks = tracker->NTracks();
  tmpTracks.resize(nTracks);
  fChiToPrimVtx.resize(nTracks, 10);
  tmpGbTracks.resize(nTracks);

  int iOTr = 0; // index in out array
  for (int iTr = 0; iTr < nTracks; iTr++) {
    // get track params in local CS
    PndFTSCATrackParam trParam = tracker->Track(iTr).InnerParam();

    trParam.TransportToX0(0, tracker->GetParameters().cBz());

    KFPTrack &tmpTr = tmpTracks[iOTr];

    // -- convert parameters
    tmpTr.SetX(trParam.X());
    tmpTr.SetY(trParam.Y());
    tmpTr.SetZ(trParam.Z());

    const float pt = CAMath::Abs(1.f / trParam.QPt());
    const int q = (trParam.QPt() >= 0 ? 1 : -1);
    //    if ( pt < 1 ) continue; // dbg
    //    if ( trParam.NDF() < 20-5 ) continue; // at least 15 hits in track
    //    if ( trParam.Chi2() > 10*trParam.NDF() ) continue; // dbg
    //    if ( iOTr >= 4 ) continue; // dbg

    const float cosL = trParam.DzDs();
    tmpTr.SetPx(pt * trParam.CosPhi());
    tmpTr.SetPy(pt * trParam.SinPhi());
    tmpTr.SetPz(pt * cosL);

    // -- convert cov matrix
    // get jacobian
    float J[6][6];
    for (int i = 0; i < 6; i++)
      for (int j = 0; j < 6; j++)
        J[i][j] = 0;
    J[0][0] = 1; // x -> x
    J[1][1] = 1; // y -> y
    J[2][2] = 1; // z -> z
    J[3][3] = -pt * trParam.SinPhi() / trParam.CosPhi();
    J[3][5] = q * pt * pt * trParam.CosPhi(); // q/pt -> px
    J[4][3] = pt;                             // sinPhi -> py
    J[4][5] = q * pt * pt * trParam.SinPhi(); // q/pt -> py
    J[5][4] = pt;                             // dz/ds -> pz
    J[5][5] = q * pt * pt * cosL;             // q/pt -> pz

    float CovIn[6][6]; // triangular -> symmetric matrix
    {
      CovIn[0][0] = .0001f * .0001f; // dx. From nowhere. TODO
      for (int i = 1; i < 6; i++) {
        CovIn[i][0] = 0;
        CovIn[0][i] = 0;
      }
      int k = 0;
      for (int i = 1; i < 6; i++) {
        for (int j = 1; j <= i; j++, k++) {
          CovIn[i][j] = trParam.Cov()[k];
          CovIn[j][i] = trParam.Cov()[k];
        }
      }
    }

    float CovInJ[6][6]; // CovInJ = CovIn * J^t
    for (int i = 0; i < 6; i++)
      for (int j = 0; j < 6; j++) {
        CovInJ[i][j] = 0;
        for (int k = 0; k < 6; k++) {
          CovInJ[i][j] += CovIn[i][k] * J[j][k];
        }
      }

    float CovOut[6][6]; // CovOut = J * CovInJ
    for (int i = 0; i < 6; i++)
      for (int j = 0; j < 6; j++) {
        CovOut[i][j] = 0;
        for (int k = 0; k < 6; k++) {
          CovOut[i][j] += J[i][k] * CovInJ[k][j];
        }
      }

    float KFPCov[21]; // symmetric matrix -> triangular
    {
      int k = 0;
      for (int i = 0; i < 6; i++) {
        for (int j = 0; j <= i; j++, k++) {
          KFPCov[k] = CovOut[i][j];
          ASSERT(!finite(CovOut[i][j]) || CovOut[i][j] == 0 || fabs(1. - CovOut[j][i] / CovOut[i][j]) <= 0.05,
                 "CovOut[" << i << "][" << j << "] == CovOut[" << j << "][" << i << "] : " << CovOut[i][j] << " == " << CovOut[j][i]);
        }
      }
    }

    static int nInfCov = 0;
    static int nNegCov = 0;
    { // check cov matrix
      bool ok = true;
      bool finiteCov = true;
      bool negCov = true;
      int k = 0;
      for (int i = 0; i < 6; i++) {
        for (int j = 0; j <= i; j++, k++) {
          ok &= finite(KFPCov[k]);
          finiteCov &= finite(KFPCov[k]);
        }
        ok &= (KFPCov[k - 1] > 0);
        negCov &= (KFPCov[k - 1] > 0);
      }
      if (!finiteCov)
        nInfCov++;
      if (!negCov)
        nNegCov++;
      // if(!negCov || !finiteCov)
      // {
      // std::cout <<
      //       for (int i = 0; i < 6; i++) {
      //         for (int j = 0; j <= i; j++, k++) {
      //           std::cout <<  KFPCov[k] <<" ";
      //         }
      //         std::cout << std::endl;
      //       }
      // std::cout << "  track "<< std::endl;
      //       for (int i = 0; i < 5; i++) {
      //         for (int j = 0; j <= i; j++, k++) {
      //           std::cout <<  trParam.Cov()[k] <<" ";
      //         }
      //         std::cout << std::endl;
      //       }
      // }
      // std::cout << "Neg " << nNegCov << " infinite " << nInfCov << std::endl;

      if (!ok)
        continue;
    }
    // { // dbg output
    //   float *outM = &(CovIn[0][0]);
    //   std::cout << "CovIn = " << std::endl;
    //   for (int i = 0; i < 6; i++) {
    //     for (int j = 0; j <= i; j++) {
    //       std::cout << outM[6*i+j] << " ";
    //     }
    //     std::cout << std::endl;
    //   }
    // }
    // { // dbg output
    //   float *outM = &(CovOut[0][0]);
    //   std::cout << "CovOut = " << std::endl;
    //   for (int i = 0; i < 6; i++) {
    //     for (int j = 0; j <= i; j++) {
    //       std::cout << outM[6*i+j] << " ";
    //     }
    //     std::cout << std::endl;
    //   }
    // }

    tmpTr.SetCovarianceMatrix(KFPCov);
    tmpTr.SetCharge((trParam.QPt()) / CAMath::Abs(trParam.QPt()));

    tmpTr.RotateXY(tracker->Track(iTr).InnerParam().Angle()); // convert into Global CS. Can't be done erlier because in tracker X hasn't correspondent covMatrix elements.

    tmpTr.SetChi2(trParam.Chi2());
    tmpTr.SetNDF(trParam.NDF());
    tmpTr.SetId(iTr);

    fRTrackIds.push_back(iTr);

    //     trParam.TransportToX0WithMaterial( 0, tracker->GetParameters().cBz( ) );
    //
    //     fChiToPrimVtx[iOTr] = (trParam.Cov()[2]* trParam.Y()*trParam.Y() + trParam.Z() *(-2 *trParam.Cov()[1]*trParam.Y() + trParam.Cov()[0]
    //     *trParam.Z()))/(-trParam.Cov()[1]*trParam.Cov()[1] + trParam.Cov()[0]* trParam.Cov()[2]);

    tmpGbTracks[iOTr] = &tracker->Track(iTr).InnerParam();
    iOTr++;
  }
  tmpTracks.resize(iOTr);
  fChiToPrimVtx.resize(iOTr);
  tmpGbTracks.resize(iOTr);

  fKFPTopoReconstructor->Init(tmpTracks, iOTr);
} // void PndFTSTopoReconstructor::Init(PndFTSCAGBTracker* tracker)

void PndFTSTopoReconstructor::ReconstructPrimVertex()
{
  fKFPTopoReconstructor->ReconstructPrimVertex();
} // void PndFTSTopoReconstructor::ReconstructPrimVertex

void PndFTSTopoReconstructor::GetChiToPrimVertex(vector<KFPTrack> &tracks, KFParticleSIMD &pv, vector<float> &chi)
{
  UNUSED_PARAM1(tracks);
  fvec b[3], vertex[3] = {0, 0, 0};
  pv.GetFieldValue(vertex, b);
  float cBz = b[2][0] * 0.000299792458;

  unsigned short NTr = tmpGbTracks.size();
  for (unsigned short iTr = 0; iTr < NTr; iTr++) {
    PndFTSCATrackParam trParam = *(tmpGbTracks[iTr]);

    const float cA = cos(-trParam.Angle());
    const float sA = sin(-trParam.Angle());

    const fvec *cov = pv.CovarianceMatrix();

    // parameters and covariance matrix of the primary vertex in YZ of the track
    float pvP[2] = {pv.X()[0] * sA + pv.Y()[0] * cA, pv.Z()[0]};
    float pvC[3] = {cA * cA * cov[2][0] + 2 * cA * cov[1][0] * sA + cov[0][0] * sA * sA, cA * cov[4][0] + cov[3][0] * sA, cov[5][0]};
    // Transport track to PV
    float pvX = pv.X()[0] * cA - pv.Y()[0] * sA;
    trParam.TransportToX0(pvX, -cBz);

    float dy = trParam.Y() - pvP[0];
    float dz = trParam.Z() - pvP[1];
    float c[3] = {trParam.Cov()[0], trParam.Cov()[1], trParam.Cov()[2]};
    c[0] += pvC[0];
    c[1] += pvC[1];
    c[2] += pvC[2];
    float d = c[0] * c[2] - c[1] * c[1];
    float chiTrack = sqrt(fabs(0.5 * (dy * dy * c[0] - 2 * dy * dz * c[1] + dz * dz * c[2]) / d));
    chi[iTr] = chiTrack;
  }
}

void PndFTSTopoReconstructor::ReconstructParticles()
{
  // find short-lived particles

  KFParticleSIMD pVtx(GetPrimVertex());
  vector<int> vTrackPDG(tmpTracks.size(), -1);

  float fCuts[3][3];

  fCuts[0][0] = 3.;
  fCuts[0][1] = 3.;
  fCuts[0][2] = -3;
  fCuts[1][0] = 3.;
  fCuts[1][1] = 3.;
  fCuts[1][2] = -3;

  for (unsigned int iTr = 0; iTr < tmpTracks.size(); iTr++) {
    int iMC = PndFTSCAPerformance::Instance().GetSubPerformance("Global Performance")->GetRecoData()[tmpTracks[iTr].Id()].GetMCTrackId();
    int PDG = (*PndFTSCAPerformance::Instance().GetSubPerformance("Global Performance")->fMCTracks)[iMC].PDG();
    vTrackPDG[iTr] = PDG;
  }
  // calculate chi to primary vertex, chi = sqrt(dr C-1 dr)
  GetChiToPrimVertex(tmpTracks, pVtx, fChiToPrimVtx);
  KFParticleFinder::FindParticles(tmpTracks, fChiToPrimVtx, fParticles, pVtx, vTrackPDG, fCuts);
} // void PndFTSTopoReconstructor::ReconstructPrimVertex
