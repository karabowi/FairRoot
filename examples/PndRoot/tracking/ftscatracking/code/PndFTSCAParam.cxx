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

// @(#) $Id: PndCAParam.cxx,v 1.5 2016/10/11 02:33:47 mpugach Exp $
// **************************************************************************
// This file is property of and copyright by the ALICE HLT Project          *
// ALICE Experiment at CERN, All rights reserved.                           *
//                                                                          *
// Primary Authors: Sergey Gorbunov <sergey.gorbunov@kip.uni-heidelberg.de> *
//                  Ivan Kisel <kisel@kip.uni-heidelberg.de>                *
//                  for The ALICE HLT Project.                              *
//                                                                          *
// Developed by:   Igor Kulakov <I.Kulakov@gsi.de>                          *
//                 Mykhailo Pugach <M.Pugach@gsi.de>                        *
//                 Maksym Zyzak <M.Zyzak@gsi.de>                            *
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

#include "PndFTSCAParam.h"
#include "PndFTSCAMath.h"

#include <iostream>
#include "FairRunAna.h"
#include "FairField.h"
#include "TFile.h"
#include "debug.h"

#include "TVector3.h"
#include "TVectorD.h"
#include "TMatrixD.h"
#include "TH2F.h"

PndFTSCAParam::PndFTSCAParam() : fNStations(0), fStations(nullptr) {}

void PndFTSCAParam::InitMagneticField()
{
  //   const int M=5; // polinom order
  //   const int N=21;//(M+1)*(M+2)/2;

  float xMax[48] = {66,  66,  66,  66,  66,  66,  66,  66,  66,  66,  66,  66,  66,  66,  66,  66,  88,  88,  88,  88,  88,  88,  88,  88,
                    104, 104, 104, 104, 104, 104, 104, 104, 160, 160, 160, 160, 160, 160, 160, 160, 160, 160, 160, 160, 160, 160, 160, 160};

  float yMax[48] = {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 37, 37, 37, 37, 37, 37, 37, 37,
                    41, 41, 41, 41, 41, 41, 41, 41, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60};

  for (Int_t ist = 0; ist < fNStations; ist++) {
    // std::cout << " Z " << fStations[ist].x0 << std::endl;
    L1FieldSlice &fieldSlice = fStations[ist].fieldSlice;
    if (ist == 32 || ist == 16) {
      vector<L1FieldSlice> &virtualSlices = fStations[ist].fieldVirtualSlice;
      virtualSlices.resize(10);
      float x0Virtual = fStations[ist].x0;
      float xMaxVirtual = xMax[ist];
      float yMaxVirtual = yMax[ist];
      float dx = (xMax[ist] - xMax[ist - 1]) / 11.f;
      float dy = (yMax[ist] - yMax[ist - 1]) / 11.f;
      float dz = (fStations[ist].x0 - fStations[ist - 1].x0) / 11.f;
      for (unsigned int iV = 0; iV < virtualSlices.size(); iV++) {
        x0Virtual -= dz;
        xMaxVirtual -= dx;
        yMaxVirtual -= dy;
        // std::cout << "Virt " << xMaxVirtual << " " << yMaxVirtual << " " << x0Virtual << std::endl;
        CalculateFieldSlice(virtualSlices[iV], xMaxVirtual, yMaxVirtual, x0Virtual);
      }
    }
    CalculateFieldSlice(fieldSlice, xMax[ist], yMax[ist], fStations[ist].x0);
  }

  CheckFieldApproximation();
}

void PndFTSCAParam::CalculateFieldSlice(L1FieldSlice &fieldSlice, const float xMax, const float yMax, const float z)
{
  const int M = 5;  // polinom order
  const int N = 21; //(M+1)*(M+2)/2;

  double dx = 1.; // step for the field approximation
  double dy = 1.;

  if (dx > xMax / N / 2)
    dx = xMax / N / 4.;
  if (dy > yMax / N / 2)
    dy = yMax / N / 4.;

  TMatrixD A(N, N);
  TVectorD b0(N), b1(N), b2(N);
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++)
      A(i, j) = 0.;
    b0(i) = b1(i) = b2(i) = 0.;
  }

  if (FairRunAna::Instance()->GetField()) {
    for (double x = -xMax; x <= xMax; x += dx)
      for (double y = -yMax; y <= yMax; y += dy) {
        double r = sqrt(fabs(x * x / xMax / xMax + y / yMax * y / yMax));
        //       if( r>1. ) continue;
        Double_t w = 1. / (r * r + 1);
        Double_t p[3] = {x, y, z};
        Double_t B[3] = {0., 0., 0.};
        FairRunAna::Instance()->GetField()->Field(p, B);
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
    double det;
    A.Invert(&det);
    TVectorD c0 = A * b0, c1 = A * b1, c2 = A * b2;
    for (int i = 0; i < N; i++) {
      fieldSlice.cx[i] = c0(i);
      fieldSlice.cy[i] = c1(i);
      fieldSlice.cz[i] = c2(i);
    }
  }
}

void PndFTSCAParam::GetStripInfo(FTSCAStripInfoVector &stripInfo, const int_v iStation, const float_m &mask) const
{
  for (int iV = 0; iV < int_v::Size; iV++) {
    if (!(mask[iV]))
      continue;

    stripInfo.sin[iV] = Station(iStation[iV]).f.sin;
    stripInfo.cos[iV] = Station(iStation[iV]).f.cos;
  }
}

std::istream &operator>>(std::istream &in, PndFTSCAParam &p)
{
  // Read settings from the file
  in >> p.fNStations;
  if (p.fStations)
    delete[] p.fStations;
  p.fStations = new FTSCAStation[p.fNStations];
  in >> p.fBz;

  for (int i = 0; i < p.fNStations; i++) {
    float inttmp;
    in >> inttmp;
    if (inttmp != i) {
      cout << "ERROR: Settings.data format is wrong! Station: " << i - 1 << endl;
      exit(0);
    }
    in >> p.fStations[i].x0;
#ifdef PANDA_FTS
    in >> p.fStations[i].materialInfo.RadThick;
    p.fStations[i].materialInfo.RadThick *= 10.f; // TODO !!!!!!! put correct material
    p.fStations[i].materialInfo.logRadThick = log(p.fStations[i].materialInfo.RadThick);
    float xTimesRho;
    in >> xTimesRho;
    p.fStations[i].materialInfo.thick = xTimesRho / 1.39; // ~54 mum mular
    p.fStations[i].materialInfo.RL = p.fStations[i].materialInfo.thick / p.fStations[i].materialInfo.RadThick;
#else
    in >> p.fStations[i].xOverX0;
    in >> p.fStations[i].xTimesRho;
#endif

    float beta;
    in >> beta;
    p.fStations[i].f.sin = sin(beta);
    p.fStations[i].f.cos = cos(beta);
    // std::cout<<"p.fStations[i].f.sin "<<p.fStations[i].f.sin<<" p.fStations[i].f.cos "<<p.fStations[i].f.cos<<std::endl;
    in >> inttmp;
    p.fStations[i].NDF = inttmp;
    in >> inttmp;
    p.fStations[i].CellLength = inttmp;
  }

#ifdef STAR_HFT
  p.fMaxZ = 35;
  p.fMaxR = 25;
  p.fMinR = 0;
  p.fMinZ = -p.fMaxZ;
#elif ALICE_ITS
  p.fMaxZ = 60;
  p.fMaxR = 60;
  p.fMinR = 0;
  p.fMinZ = -p.fMaxZ;
#elif PANDA_STT
  p.fMaxZ = 75 + 20;
  p.fMaxR = 41;
  p.fMinR = 0;
  p.fMinZ = -75 + 20;
#elif PANDA_FTS
  p.fMinX = -200; // more precisely 196
  p.fMaxX = 200;
  p.fMinY = -60;
  p.fMaxY = 60;
  p.fMinZ = 290;
  p.fMaxZ = 660;
#endif

#ifdef PANDA_FTS
  // p.fVtxFieldValue[0].y = p.Bz();
  // p.fVtxFieldValue[1].y = p.Bz();
  // p.fZVtxFieldValue[0] = 0.f;
  // p.fZVtxFieldValue[1] = 1.f;
  // for(int i=0; i<p.fNStations; i++)
  //   p.fStations[i].fieldSlice.cy[0] = p.Bz();
//   if(1){ // read field
//     float tmp;
//     for( int i = 0; i < 2; i++ ) {
//       in >> tmp;
//       p.fZVtxFieldValue[i] = tmp;
//       in >> tmp;
//       p.fVtxFieldValue[i].x = tmp;
//       in >> tmp;
//       p.fVtxFieldValue[i].y = tmp;
//       in >> tmp;
//       p.fVtxFieldValue[i].z = tmp;
//     }
//
//     in >> tmp >> tmp >> tmp >> tmp; // skip one point
//
//     for(int iSta = 0; iSta < p.fNStations; iSta++) {
// #if 0
//       int N;
//       in >> N;
//       for( int i = 0; i < N; i++ ) {
//         in >> tmp;
//         p.fStations[i].fieldSlice.cx[i] = tmp;
//       }
//       for( int i = 0; i < N; i++ ) {
//         in >> tmp;
//         p.fStations[i].fieldSlice.cy[i] = tmp;
//       }
//       for( int i = 0; i < N; i++ ) {
//         in >> tmp;
//         p.fStations[i].fieldSlice.cz[i] = tmp;
//       }
// #else // 0
//       L1FieldSlice &sl = p.fStations[iSta].fieldSlice;
//       vector<L1FieldSlice> &vSls = p.fStations[iSta].fieldVirtualSlice;
//       int itmp;
//       in >> itmp; // station
//       if ( itmp != iSta ) { cout << "Wrong field" << endl; exit(0); }
//       ASSERT( itmp == iSta, itmp << " = " << iSta );
//       in >> itmp >> p.fStations[iSta].dZVirtualStation; // n virtual stations & dz
//       vSls.resize(itmp);
//       in >> sl.xMin >> sl.dx >> sl.nXBins;
//       in >> sl.yMin >> sl.dy >> sl.nYBins;
//       const int NBins = sl.nXBins*sl.nYBins;
//       for( unsigned int iV = 0; iV < vSls.size(); iV++ ) {
//         L1FieldSlice &vSl = vSls[iV];
//         vSl.xMin = sl.xMin;
//         vSl.dx = sl.dx;
//         vSl.nXBins = sl.nXBins;
//         vSl.yMin = sl.yMin;
//         vSl.dy = sl.dy;
//         vSl.nYBins = sl.nYBins;
//         vSl.AlocateMemory();
//         for( int iB = 0; iB < NBins; iB++ ) {
//           in >> vSl.bX[iB] >> vSl.bY[iB] >> vSl.bZ[iB];
//         }
//       }
//
//       sl.AlocateMemory();
//       for( int iB = 0; iB < NBins; iB++ ) {
//         in >> sl.bX[iB] >> sl.bY[iB] >> sl.bZ[iB];
//       }
// #endif // 0
//     }
//   }
#else //  PANDA_FTS
  p.fVtxFieldValue[0] = p.cBz();
#endif

  return in;
}

#include "BinaryStoreHelper.h"

void PndFTSCAParam::StoreToFile(FILE *f) const
{
  BinaryStoreWrite(*this, f);
}

void PndFTSCAParam::RestoreFromFile(FILE *f)
{
  BinaryStoreRead(*this, f);
}

void PndFTSCAParam::CheckFieldApproximation()
{
  TDirectory *curr = gDirectory;
  TFile *currentFile = gFile;
  TFile *fout = new TFile("FieldApprox.root", "RECREATE");
  fout->cd();

  float xMax[48] = {66,  66,  66,  66,  66,  66,  66,  66,  66,  66,  66,  66,  66,  66,  66,  66,  88,  88,  88,  88,  88,  88,  88,  88,
                    104, 104, 104, 104, 104, 104, 104, 104, 160, 160, 160, 160, 160, 160, 160, 160, 160, 160, 160, 160, 160, 160, 160, 160};

  float yMax[48] = {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 37, 37, 37, 37, 37, 37, 37, 37,
                    41, 41, 41, 41, 41, 41, 41, 41, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60};

  for (int ist = 0; ist < fNStations; ist++) {
    double z = fStations[ist].x0;
    double Xmax = xMax[ist], Ymax = yMax[ist];

    Double_t r[3], B[3];
    Double_t bbb, bbb_L1;

    L1FieldSlice &fieldSlice = fStations[ist].fieldSlice;
    CAFieldValue B_L1;

    int NbinsX = 100; // static_cast<int>(2*Xmax/step);
    int NbinsY = 100; // static_cast<int>(2*Ymax/step);
    float ddx = 2 * Xmax / NbinsX;
    float ddy = 2 * Ymax / NbinsY;

    TH2F *stB = new TH2F(Form("station %i, dB", ist + 1), Form("station %i, dB, z = %0.f cm", ist + 1, z), static_cast<int>(NbinsX + 1), -(Xmax + ddx / 2.), (Xmax + ddx / 2.),
                         static_cast<int>(NbinsY + 1), -(Ymax + ddy / 2.), (Ymax + ddy / 2.));
    TH2F *stBx = new TH2F(Form("station %i, dBx", ist + 1), Form("station %i, dBx, z = %0.f cm", ist + 1, z), static_cast<int>(NbinsX + 1), -(Xmax + ddx / 2.), (Xmax + ddx / 2.),
                          static_cast<int>(NbinsY + 1), -(Ymax + ddy / 2.), (Ymax + ddy / 2.));
    TH2F *stBy = new TH2F(Form("station %i, dBy", ist + 1), Form("station %i, dBy, z = %0.f cm", ist + 1, z), static_cast<int>(NbinsX + 1), -(Xmax + ddx / 2.), (Xmax + ddx / 2.),
                          static_cast<int>(NbinsY + 1), -(Ymax + ddy / 2.), (Ymax + ddy / 2.));
    TH2F *stBz = new TH2F(Form("station %i, dBz", ist + 1), Form("station %i, dBz, z = %0.f cm", ist + 1, z), static_cast<int>(NbinsX + 1), -(Xmax + ddx / 2.), (Xmax + ddx / 2.),
                          static_cast<int>(NbinsY + 1), -(Ymax + ddy / 2.), (Ymax + ddy / 2.));

    Int_t i = 1, j = 1;

    float x, y;
    for (int ii = 1; ii <= NbinsX + 1; ii++) {
      j = 1;
      x = -Xmax + (ii - 1) * ddx;
      for (int jj = 1; jj <= NbinsY + 1; jj++) {
        y = -Ymax + (jj - 1) * ddy;
        // double rrr = sqrt(fabs(x*x/Xmax/Xmax+y/Ymax*y/Ymax));
        //         if(rrr>1. )
        //         {
        //           j++;
        //           continue;
        //         }
        r[2] = z;
        r[0] = x;
        r[1] = y;
        FairRunAna::Instance()->GetField()->Field(r, B);
        bbb = sqrt(B[0] * B[0] + B[1] * B[1] + B[2] * B[2]);

        fieldSlice.GetFieldValue(x, y, B_L1);
        bbb_L1 = sqrt(B_L1.x[0] * B_L1.x[0] + B_L1.y[0] * B_L1.y[0] + B_L1.z[0] * B_L1.z[0]);

        stB->SetBinContent(ii, jj, fabs(bbb) > 1.e-2 ? (bbb - bbb_L1) / bbb * 100 : 0);
        stBx->SetBinContent(ii, jj, fabs(B[0]) > 1.e-2 ? (B[0] - B_L1.x[0]) / B[0] * 100 : 0);
        stBy->SetBinContent(ii, jj, fabs(B[1]) > 1.e-2 ? (B[1] - B_L1.y[0]) / B[1] * 100 : 0);
        stBz->SetBinContent(ii, jj, fabs(B[2]) > 1.e-2 ? (B[2] - B_L1.z[0]) / B[2] * 100 : 0);

        //         stB  -> SetBinContent(ii,jj, bbb_L1);
        //         stBx -> SetBinContent(ii,jj, B_L1.x[0]);
        //         stBy -> SetBinContent(ii,jj, B_L1.y[0]);
        //         stBz -> SetBinContent(ii,jj, B_L1.z[0]);

        //         stB  -> SetBinContent(ii,jj, (bbb - bbb_L1) );
        //         stBx -> SetBinContent(ii,jj, (B[0] - B_L1.x[0]));
        //         stBy -> SetBinContent(ii,jj, (B[1] - B_L1.y[0]));
        //         stBz -> SetBinContent(ii,jj, (B[2] - B_L1.z[0]));
        j++;
      }
      i++;
    }

    stB->GetXaxis()->SetTitle("X, cm");
    stB->GetYaxis()->SetTitle("Y, cm");
    stB->GetXaxis()->SetTitleOffset(1);
    stB->GetYaxis()->SetTitleOffset(1);
    stB->GetZaxis()->SetTitle("B_map - B_L1, kGauss");
    stB->GetZaxis()->SetTitleOffset(1.3);

    stBx->GetXaxis()->SetTitle("X, cm");
    stBx->GetYaxis()->SetTitle("Y, cm");
    stBx->GetXaxis()->SetTitleOffset(1);
    stBx->GetYaxis()->SetTitleOffset(1);
    stBx->GetZaxis()->SetTitle("Bx_map - Bx_L1, kGauss");
    stBx->GetZaxis()->SetTitleOffset(1.3);

    stBy->GetXaxis()->SetTitle("X, cm");
    stBy->GetYaxis()->SetTitle("Y, cm");
    stBy->GetXaxis()->SetTitleOffset(1);
    stBy->GetYaxis()->SetTitleOffset(1);
    stBy->GetZaxis()->SetTitle("By_map - By_L1, kGauss");
    stBy->GetZaxis()->SetTitleOffset(1.3);

    stBz->GetXaxis()->SetTitle("X, cm");
    stBz->GetYaxis()->SetTitle("Y, cm");
    stBz->GetXaxis()->SetTitleOffset(1);
    stBz->GetYaxis()->SetTitleOffset(1);
    stBz->GetZaxis()->SetTitle("Bz_map - Bz_L1, kGauss");
    stBz->GetZaxis()->SetTitleOffset(1.3);

    stB->Write();
    stBx->Write();
    stBy->Write();
    stBz->Write();

  } // for ista

  fout->Close();
  fout->Delete();
  gFile = currentFile;
  gDirectory = curr;
} // void CbmL1::FieldApproxCheck()
