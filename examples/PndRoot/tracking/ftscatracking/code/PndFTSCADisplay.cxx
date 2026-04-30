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

// $Id: PndFTSCADisplay.cxx,v 1.8 2016/12/16 23:29:41 mpugach Exp $
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

// TODO now we use right CS, so z change sign, but it wasn't changed everywhere in the Display-code

#include "PndFTSCADisplay.h"

//#define DRAW_3D // infrequent use for drawing 3D pictures
#define CLEAR
//#define DRAW_ZR

#include "PndFTSCAParameters.h"

#include "PndFTSCAGBTracker.h"
#include "PndFTSCAGBTrack.h"
#include "PndFTSCAGBHit.h"
#include "PndFTSCAPerformance.h"
#include "PndFTSCAMCTrack.h"
#include "PndFTSCAParam.h"
#include "PndFTSCAClusterData.h"

#include "PndFTSCATrackPerformanceBase.h"
#include "PndFTSCATrackParam.h"

#include "FTSCAHits.h"
#include "FTSCAHitsV.h"
#include "FTSCATracks.h"

#include "TString.h"
#include "Riostream.h"
#include "TMath.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TApplication.h"
#include "TLatex.h"
#include "TVector3.h"
#include "TEllipse.h"

#ifdef DRAW_3D
#include "TVector3.h"
#endif // DRAW_3D

class PndFTSCADisplay::PndFTSCADisplayTmpHit {
  //* Displaying information on the screen*//

 public:
  int ID() const { return fHitID; }
  double S() const { return fS; }
  double Z() const { return fZ; }

  void SetID(int v) { fHitID = v; }
  void SetS(double v) { fS = v; }
  void SetZ(double v) { fZ = v; }

  static bool CompareHitDS(const PndFTSCADisplayTmpHit &a, const PndFTSCADisplayTmpHit &b) { return (a.fS < b.fS); }

  static bool CompareHitZ(const PndFTSCADisplayTmpHit &a, const PndFTSCADisplayTmpHit &b) { return (a.fZ < b.fZ); }

 private:
  int fHitID; // hit ID
  double fS;  // hit position on the XY track curve
  double fZ;  // hit Z position
};

PndFTSCADisplay &PndFTSCADisplay::Instance()
{
  // reference to static object
  static PndFTSCADisplay gPndFTSCADisplay;

  return gPndFTSCADisplay;
}

PndFTSCADisplay::PndFTSCADisplay()
  : fYX(nullptr), fZX(nullptr), fZR(nullptr), fAsk(1), fGB(nullptr), fPerf(nullptr), fZMin(-60), fZMax(60), fYMin(-210), fYMax(210),
#ifdef PANDA_FTS
    fXMin(0), fXMax(0),
#else
    fRInnerMin(50.), fRInnerMax(133.3), fROuterMin(50), fROuterMax(50), fTPCZMin(-60.), fTPCZMax(60),
#endif
    fArc(), fLine(), fPLine(), fMarker(), fBox(), fCrown(), fLatex(), fDrawOnlyRef(0) // iklm. This is just default. If they are not correct SetTPC(...) can and should be used!
{
  // fPerf = &( PndFTSCAPerformance::Instance() );
  // constructor
}

PndFTSCADisplay::~PndFTSCADisplay()
{
  // destructor
  //   if(fYX) delete fYX;
  //   if(fZX) delete fZX;
  //   if( fZR ) delete fZR;
}

void PndFTSCADisplay::Init()
{
  static bool firstCall = 1;
  if (firstCall) {
    if (!gApplication)
      new TApplication("myapp", 0, 0);

    // initialization
    gStyle->SetCanvasBorderMode(0);
    gStyle->SetCanvasBorderSize(1);
    gStyle->SetCanvasColor(0);
#ifdef PANDA_FTS
    fCanvas = new TCanvas("CA", "CA Display", 2 * 400, 2 * 300);
    fCanvas->Divide(2, 1);
    fZX = static_cast<TPad *>(fCanvas->GetPrimitive("CA_1"));
    fYX = static_cast<TPad *>(fCanvas->GetPrimitive("CA_2"));
#else
#ifdef DRAW_ZR
    fCanvas = new TCanvas("CA", "CA Display", 400 * 1.5, 300); // 1280*1.5, 645
    fCanvas->Divide(3, 1);
    fZX = static_cast<TPad *>(fCanvas->GetPrimitive("CA_2")); // ("ZX", "ZX window", -610, 0, 590, 600);
    fZR = static_cast<TPad *>(fCanvas->GetPrimitive("CA_3")); // ("ZX", "ZX window", -610, 0, 590, 600);
#else
    fCanvas = new TCanvas("CA", "CA Display", 400 * 2, 300 * 2);
    fCanvas->Divide(2, 1);
    fZX = static_cast<TPad *>(fCanvas->GetPrimitive("CA_2")); // ("ZX", "ZX window", -610, 0, 590, 600);
#endif
    fYX = static_cast<TPad *>(fCanvas->GetPrimitive("CA_1")); // ("YX", "YX window", -1, 0, 600, 600);
#endif // PANDA_FTS

    fYX->SetCanvas(fCanvas);
    fYX->SetTitle("XY");
    fZX->SetCanvas(fCanvas);
    fZX->SetTitle("ZX");
    if (fZR) {
      fZR->SetCanvas(fCanvas);
      fZR->SetTitle("ZR");
    }

    fMarker = TMarker(0.0, 0.0, 20); // 6);
    fDrawOnlyRef = 0;

    firstCall = 0;
  }
}

void PndFTSCADisplay::Update()
{
  // update windows
  if (!fAsk)
    return;
  fYX->Update();
  fZX->Update();
  // X   fYX->Print( "YX.pdf" );
  fZX->Print("ZX.pdf");
}

void PndFTSCADisplay::ClearView()
{
  // clear windows
#ifdef CLEAR
  fYX->Clear();
  fZX->Clear();
#endif
  DrawTPC();
}

void PndFTSCADisplay::Ask()
{
  // wait for the pressed key, when "r" pressed, don't ask anymore
  char symbol;
  if (fAsk) {
    Update();
    std::cout << "ask> ";
    do {
      std::cin.get(symbol);
      if (symbol == 'r')
        fAsk = false;
    } while (symbol != '\n');
  }
}

void PndFTSCADisplay::SetGB(const PndFTSCAGBTracker *GBTracker)
{
  fGB = GBTracker;
}

void PndFTSCADisplay::DrawTPC()
{
  // schematically draw TPC detector
  const int detColor = kGray + 1; // color of the detector
#if defined(PANDA_STT)
                                  // just borders
  fYX->cd();
#ifdef CLEAR
  fYX->Clear();
#endif
  {
    fArc.SetLineColor(detColor);
    fArc.SetFillStyle(0);
    fArc.SetLineWidth(.1);
    fArc.DrawArc(0, 0, fROuterMax);
    fArc.DrawArc(0, 0, fRInnerMin);

    TLatex Tl;
    Tl.SetTextSize(0.03);
    Tl.SetTextAlign(22);
    Tl.DrawLatex(-fROuterMax, fROuterMax, "XY");
  }

  fZX->cd();
#ifdef CLEAR
  fZX->Clear();
#endif
  {
    TBox ZX;
    ZX.SetFillStyle(0);
    ZX.SetFillColor(0);
    ZX.SetLineWidth(.1);
    ZX.SetLineColor(detColor);
    ZX.DrawBox(fZMin, -fROuterMax, fZMax, fROuterMax);
    //  ZX.DrawBox(fZMin,fROuterMax,fZMax,fRInnerMin);

    TLatex Tl;
    Tl.SetTextSize(0.03);
    Tl.SetTextAlign(22);
    Tl.DrawLatex(fZMin * 0.93, fROuterMax * 0.95, "ZY");
  }
#elif defined(PANDA_FTS)
                                                              // just borders
  fYX->cd();
#ifdef CLEAR
  fYX->Clear();
#endif
  {
    TBox XY;
    XY.SetFillStyle(0);
    XY.SetFillColor(0);
    XY.SetLineWidth(.1);
    XY.SetLineColor(detColor);
    XY.DrawBox(fXMin, fYMin, fXMax, fYMax);

    TLatex Tl;
    Tl.SetTextSize(0.03);
    Tl.SetTextAlign(22);
    Tl.DrawLatex(fXMin + (fXMax - fXMin) * 0.07, fYMax - (fYMax - fYMin) * 0.05, "XY");
  }

  fZX->cd();
#ifdef CLEAR
  fZX->Clear();
#endif
  {
    TBox ZX;
    ZX.SetFillStyle(0);
    ZX.SetFillColor(0);
    ZX.SetLineWidth(.1);
    ZX.SetLineColor(detColor);
    ZX.DrawBox(fZMin, fXMin, fZMax, fXMax);

    const int NStations = 6;
    const float XLeft[NStations] = {-659.025, -659.025, -881.225, -1042.825, -1951.825, -1951.825}; // mm
    const float NTubes[NStations] = {132, 132, 176, 208, 388, 388};
    const float MinZ[NStations] = {2949.627, 3269.627, 3940.627, 4380.627, 6070.627, 6390.627};
    const float MaxZ[NStations] = {3108.373, 3428.373, 4244.123, 4684.123, 6229.373, 6549.373};
    const float Offset = 2;
    for (int iS = 0; iS < NStations; iS++)
      ZX.DrawBox(MinZ[iS] / 10 - Offset, XLeft[iS] / 10 - Offset, MaxZ[iS] / 10 + Offset, (XLeft[iS] + NTubes[iS] * 10.1) / 10 + Offset);

    TLatex Tl;
    Tl.SetTextSize(0.03);
    Tl.SetTextAlign(22);
    Tl.DrawLatex(fZMin + (fZMax - fZMin) * 0.07, fXMax - (fXMax - fXMin) * 0.05, "ZX");
  }
#else // FTS
  fYX->cd();
#ifdef CLEAR
  fYX->Clear();
#endif
  const int color = detColor;
  const float width = .1;
  // STT
  {
    const float RMax = 40.827;

    {
      TLine l;
      l.SetLineColor(color);
      l.SetLineWidth(width);
      l.SetLineStyle(9);

      const float x0L = 16.1; // 16.825;
      const float a = x0L / sqrt(3.f / 4.f);
      double x[7] = {0, x0L, x0L, 0, -x0L, -x0L, 0}, y[7] = {-a, -a / 2, a / 2, a, a / 2, -a / 2, -a};

      for (int i = 0; i < 6; i++) {
        const double c = RMax / sqrt(x[i] * x[i] + y[i] * y[i]);
        l.DrawLine(x[i], y[i], c * x[i], c * y[i]);
      }
    }

    TPolyLine pl;
    pl.SetLineColor(color);
    pl.SetLineWidth(width);

    {
      const float x0L = 16.1; // 16.825;
      const float a = x0L / sqrt(3.f / 4.f);
      double x[7] = {0, x0L, x0L, 0, -x0L, -x0L}, y[7] = {-a, -a / 2, a / 2, a, a / 2, -a / 2, -a};
      pl.DrawPolyLine(7, x, y);
    }

    pl.SetLineStyle(9);

    {
      const float x0L = 23.3; // 16.825;
      const float a = x0L / sqrt(3.f / 4.f);
      double x[7] = {0, x0L, x0L, 0, -x0L, -x0L}, y[7] = {-a, -a / 2, a / 2, a, a / 2, -a / 2, -a};
      pl.DrawPolyLine(7, x, y);
    }

    {
      const float x0L = 31.7; // 16.825;
      const float a = x0L / sqrt(3.f / 4.f);
      double x[7] = {0, x0L, x0L, 0, -x0L, -x0L}, y[7] = {-a, -a / 2, a / 2, a, a / 2, -a / 2, -a};
      pl.DrawPolyLine(7, x, y);
    }

    // MVD

    // {
    //   const float xMin = 2.126;
    //   TBox b;
    //   b.SetFillStyle(1);
    //   b.SetFillColor(kWhite);
    //   b.SetLineWidth(0);
    //   b.SetLineColor(kWhite);
    //   b.DrawBox(-xMin,-RMax-.1,xMin,RMax+.1);
    // }

    fArc.SetLineColor(color);
    fArc.SetFillStyle(0);
    fArc.SetNoEdges(0);
    fArc.SetLineWidth(width);
    // {
    //   const float R = 2.408;
    //   const float xmin = 1.143;
    //   const float phi = 90 - 180.f*asin(xmin/R)/TMath::Pi();
    //   fArc.DrawArc( 0, 0, R, -phi, phi,"only" ); // 1st MVD
    // }
    // {
    //   const float R = 2.408;
    //   const float xmin = -1.538;
    //   const float phi = 90 - 180.f*asin(xmin/R)/TMath::Pi();
    //   fArc.DrawArc( 0, 0, R, -phi+360, phi,"only" ); // 1st MVD
    // }
    fArc.DrawArc(0, 0, 2.408); // 1st MVD
    fArc.DrawArc(0, 0, 4.969);
    fArc.DrawArc(0, 0, 9.210);
    fArc.DrawArc(0, 0, 12.529);
    fArc.DrawArc(0, 0, RMax); // rMax STT

    TLatex Tl;
    Tl.SetTextSize(0.03);
    Tl.SetTextAlign(22);
    Tl.DrawLatex(-fROuterMax, fROuterMax, "XY");
  }

  fZX->cd();
#ifdef CLEAR
  fZX->Clear();
#endif
  {
    // barell MVD
    TBox ZX;
    ZX.SetFillStyle(0);
    ZX.SetFillColor(0);
    ZX.SetLineWidth(width);
    ZX.SetLineColor(color);
    // ZX.DrawBox(-1.601, -1.940, 0.976, 2.336); // from MCPoints
    // ZX.DrawBox(-6.066, -5.209, 5.773, 5.178);
    // ZX.DrawBox(-11.380, -9.567, 13.840, 9.616);
    // ZX.DrawBox(-7.292, -12.788, 13.713, 12.767);
    // ZX.DrawBox(-22.672, -40., 109.645, 40.);
    ZX.DrawBox(-39.8 / 10, -28.58 / 10, 9.8 / 10, 28.58 / 10); // from TDR
    ZX.DrawBox(-79.8 / 10, -52.82 / 10, 57.8 / 10, 52.82 / 10);
    ZX.DrawBox(-133.8 / 10, -96.86 / 10, 139.0 / 10, 96.86 / 10);
    ZX.DrawBox(-169.2 / 10, -129.24 / 10, 139.0 / 10, 129.24 / 10);
    ZX.DrawBox(fZMin, -fROuterMax, fZMax, fROuterMax);

    // forward MVD
    TLine l;
    l.SetLineWidth(width);
    l.SetLineColor(color);

    l.DrawLine(22 / 10, -36.56 / 10, 22 / 10, 36.56 / 10);
    l.DrawLine(42 / 10, -36.56 / 10, 42 / 10, 36.56 / 10);
    l.DrawLine(72 / 10, -73.96 / 10, 72 / 10, 73.96 / 10);
    l.DrawLine(102 / 10, -73.96 / 10, 102 / 10, 73.96 / 10);
    l.DrawLine(150 / 10, -73.96 / 10, 150 / 10, 73.96 / 10);       // 5.1
    l.DrawLine(162.5 / 10, -131.15 / 10, 162.5 / 10, 131.15 / 10); // 5.2
    l.DrawLine(220 / 10, -73.96 / 10, 220 / 10, 73.96 / 10);       // 6.1
    l.DrawLine(207.5 / 10, -131.15 / 10, 207.5 / 10, 131.15 / 10); // 6.2

    TLatex Tl;
    Tl.SetTextSize(0.03);
    Tl.SetTextAlign(22);
    Tl.DrawLatex(fZMin * 0.93, fROuterMax * 0.95, "ZY");
  }
  if (fZR) {
    fZR->cd();
#ifdef CLEAR
    fZR->Clear();
#endif
    {
      // STT
      {
        TLine l;
        l.SetLineWidth(width);
        l.SetLineColor(color);

        l.DrawLine(fZMin, 16.1, fZMax, 16.1);
        l.SetLineStyle(9);
        l.DrawLine(fZMin, 23.3, fZMax, 23.3);
        l.DrawLine(fZMin, 31.7, fZMax, 31.7);
      }

      // barrel MVD
      TBox ZX;
      ZX.SetFillStyle(0);
      ZX.SetFillColor(0);
      ZX.SetLineWidth(width);
      ZX.SetLineColor(color);
      // ZX.DrawBox(-1.601, -1.940, 0.976, 2.336); // from MCPoints
      // ZX.DrawBox(-6.066, -5.209, 5.773, 5.178);
      // ZX.DrawBox(-11.380, -9.567, 13.840, 9.616);
      // ZX.DrawBox(-7.292, -12.788, 13.713, 12.767);
      // ZX.DrawBox(-22.672, 0., 109.645, 40.);
      ZX.DrawBox(-39.8 / 10, 0, 9.8 / 10, 28.58 / 10); // from TDR
      ZX.DrawBox(-79.8 / 10, 0, 57.8 / 10, 52.82 / 10);
      ZX.DrawBox(-133.8 / 10, 0, 139.0 / 10, 96.86 / 10);
      ZX.DrawBox(-169.2 / 10, 0, 139.0 / 10, 129.24 / 10);
      ZX.DrawBox(fZMin, 0., fZMax, fROuterMax);

      // forward MVD
      {
        TLine l;
        l.SetLineWidth(width);
        l.SetLineColor(color);

        l.DrawLine(22 / 10, 11.70 / 10, 22 / 10, 36.56 / 10);
        l.DrawLine(42 / 10, 11.70 / 10, 42 / 10, 36.56 / 10);
        l.DrawLine(72 / 10, 11.70 / 10, 72 / 10, 73.96 / 10);
        l.DrawLine(102 / 10, 11.70 / 10, 102 / 10, 73.96 / 10);
        l.DrawLine(150 / 10, 11.70 / 10, 150 / 10, 73.96 / 10);      // 5.1
        l.DrawLine(162.5 / 10, 74.33 / 10, 162.5 / 10, 131.15 / 10); // 5.2
        l.DrawLine(220 / 10, 11.70 / 10, 220 / 10, 73.96 / 10);      // 6.1
        l.DrawLine(207.5 / 10, 74.33 / 10, 207.5 / 10, 131.15 / 10); // 6.2
      }

      TLatex Tl;
      Tl.SetTextSize(0.03);
      Tl.SetTextAlign(22);
      Tl.DrawLatex(fZMin * 0.93, fROuterMax * 0.975, "ZR");
    }
  }
#endif // PANDA // detailed view
}

void PndFTSCADisplay::DrawArc(float x, float y, float r, int Start, Size_t width)
{
  fArc.SetLineWidth(width);
  fArc.SetLineColor(2 + Start);

  fYX->cd();
  fArc.DrawArc(x, y, r);
}

void PndFTSCADisplay::DrawPoint(float x, float y, float z, int Start, Size_t width)
{
  fMarker.SetMarkerSize(width);
  fMarker.SetMarkerColor(2 + Start);

  fYX->cd();
  fMarker.DrawMarker(x, y);
  fZX->cd();
  fMarker.DrawMarker(z, y);
}

void PndFTSCADisplay::DrawGBPoint(float x, float y, float z, int Start, Size_t width)
{
  // std::cout<<"x y z "<<x<<" "<<y<<" "<<z<<std::endl;
  fMarker.SetMarkerSize(width);
  fMarker.SetMarkerColor(Start);

  fYX->cd();
  fMarker.DrawMarker(x, y);
  fZX->cd();
#ifdef PANDA_FTS
  fMarker.DrawMarker(z, x);
#else
  fMarker.DrawMarker(z, y);

  if (fZR) {
    fZR->cd();
    fMarker.DrawMarker(z, sqrt(y * y + x * x));
  }
#endif
}

void PndFTSCADisplay::DrawGBPoint(float x, float y, float z, float angle, int Start, Size_t width)
{

  fMarker.SetMarkerSize(width);
  fMarker.SetMarkerColor(Start);

  //   fArrow.SetAngle(h.Angle());
  fArrow.SetFillColor(Start);
  fArrow.SetLineColor(Start);
  fArrow.SetLineWidth(1 * width);

  double ax, ay, az;
  double ax1 = 0;
  double ay1 = 3;
  double az1 = 0;
  PndFTSCAParameters::CALocalToGlobal(ax1, ay1, az1, double(angle), ax, ay, az);
  ax += x;
  ay += y;

  fYX->cd();
  fMarker.DrawMarker(x, y);
  //  fArrow.DrawArrow(x, y, ax, ay, 0.003, "|>"); // draw module direction

  fZX->cd();
#ifdef PANDA_FTS
  fMarker.DrawMarker(z, x);
#else
  fMarker.DrawMarker(z, y);

  if (fZR) {
    fZR->cd();
    fMarker.DrawMarker(z, sqrt(y * y + x * x));
  }
#endif
}

void PndFTSCADisplay::DrawGBLine(float x, float y, float z, float x2, float y2, float z2, int Start, Size_t /*width*/, int projection) //[R.K. 9/2018] unused
{
  fLine.SetLineWidth(2);
  fLine.SetLineColor(Start);

  if (projection == -1 || projection == 0) {
#ifdef PANDA_FTS
    fZX->cd();
    fLine.DrawLine(z, x, z2, x2);
#else
    fYX->cd();
    fLine.DrawLine(x, y, x2, y2);
#endif
  }
  if (projection == -1 || projection == 1) {
#ifdef PANDA_FTS
    fYX->cd();
    fLine.DrawLine(x, y, x2, y2);
#else
    fZX->cd();
    fLine.DrawLine(z, y, z2, y2);
    if (fZR) {
      fZR->cd();
      fLine.DrawLine(z, sqrt(x * x + y * y), z2, sqrt(x2 * x2 + y2 * y2));
    }
#endif
  }
}

int PndFTSCADisplay::GetColor(int i) const
{
  // Get color with respect to Z coordinate
  const Color_t kMyColor[9] = {kGreen, kBlue, kYellow, kCyan, kOrange, kSpring, kTeal, kAzure, kViolet};
  if (i < 0)
    i = 0;
  if (i == 0)
    return kBlack;
  return kMyColor[(i - 1) % 9];
}

int PndFTSCADisplay::GetColorZ(double z) const
{
  // Get color with respect to Z coordinate
  const Color_t kMyColor[11] = {kGreen, kBlue, kYellow, kMagenta, kCyan, kOrange, kSpring, kTeal, kAzure, kViolet, kPink};

  double zz = (z - fZMin) / (fZMax - fZMin);
  int iz = (int)(zz * 11);
  if (iz < 0)
    iz = 0;
  if (iz > 10)
    iz = 10;
  return kMyColor[iz];
}

int PndFTSCADisplay::GetColorY(double y) const
{
  // Get color with respect to Z coordinate
  const Color_t kMyColor[11] = {kGreen, kBlue, kYellow, kMagenta, kCyan, kOrange, kSpring, kTeal, kAzure, kViolet, kPink};

  double yy = (y - fYMin) / (fYMax - fYMin);
  int iy = (int)(yy * 11);
  if (iy < 0)
    iy = 0;
  if (iy > 10)
    iy = 10;
  return kMyColor[iy];
}

int PndFTSCADisplay::GetColorK(double k) const
{
  // Get color with respect to Z coordinate
  const Color_t kMyColor[11] = {kRed, kBlue, kYellow, kMagenta, kCyan, kOrange, kSpring, kTeal, kAzure, kViolet, kPink};
  const double kCLight = 0.000299792458;
  const double kBz = 5;
  double k2QPt = 100;
  if (TMath::Abs(kBz) > 1.e-4)
    k2QPt = 1. / (kBz * kCLight);
  double qPt = k * k2QPt;
  double pt = 100;
  if (TMath::Abs(qPt) > 1.e-4)
    pt = 1. / TMath::Abs(qPt);

  double yy = (pt - 0.1) / (1. - 0.1);
  int iy = (int)(yy * 11);
  if (iy < 0)
    iy = 0;
  if (iy > 10)
    iy = 10;
  return kMyColor[iy];
}

void PndFTSCADisplay::DrawGBHit(const PndFTSCAGBTracker &tracker, int iHit, int color, Size_t width)
{
  // draw hit
  const PndFTSCAGBHit &h = tracker.Hits()[iHit];

  if (color < 0) {
    if (fPerf) {
      int lab = fPerf->HitLabel(h.ID()).fLab[0];
      color = GetColor(lab + 1);
      if (lab >= 0) {
        const PndFTSCAMCTrack &mc = fPerf->MCTrack(lab);
        if (mc.P() >= 1.)
          color = kRed;
        else if (fDrawOnlyRef)
          return;
      }
    } else
      color = GetColorZ(h.Z());
  }
  if (width > 0)
    fMarker.SetMarkerSize(width);
  else
    fMarker.SetMarkerSize(.3);
  fMarker.SetMarkerColor(color);
  double vx = h.X(), vy = h.Y();

  fYX->cd();
  fMarker.DrawMarker(vx, vy);
  fZX->cd();
  fMarker.DrawMarker(h.Z(), vy);
}

void PndFTSCADisplay::DrawGBHits(const PndFTSCAGBTracker &tracker, int color, Size_t width, int hitsType)
{
  // draw hits

  if (!fPerf)
    return;
  if (width < 0)
    width = .6;

  for (int iHit = 0; iHit < tracker.NHits(); iHit++) {
    const PndFTSCAGBHit &h = tracker.Hits()[iHit];
    //     if ((hitsType == 1) && (h.ISlice() >= 12)) continue;
    //     if ((hitsType == 2) && (h.ISlice() < 12) ) continue;

    int imc = fPerf->HitLabel(h.ID()).fLab[0];
    const PndFTSCAMCTrack *mc = (imc >= 0) ? &(fPerf->MCTrack(imc)) : 0;
    if (fDrawOnlyRef && (!mc || (mc->P() < 1)))
      continue;
    int col = color;
    if (color < 0) {
      if (hitsType == 1) {
        if (h.Z() >= 0)
          col = kBlue;
        if (h.Z() < 0)
          col = 8;
      } else {
        col = GetColor(imc + 1);
        if (mc && (mc->P() >= PParameters::RefThreshold))
          col = kRed;
      }
    }

    fMarker.SetMarkerSize(width);
    fMarker.SetMarkerColor(col);
    double vx = h.X(), vy = h.Y();

    // //   fArrow.SetAngle(h.Angle());
    //    fArrow.SetFillColor( col );
    //    fArrow.SetLineColor( col );
    //    fArrow.SetLineWidth( 1*width );

    //    double ax, ay, az;
    //    double ax1 = 0;
    //    double ay1 = 1;
    //    double az1 = 0;
    //    PndFTSCAParameters::CALocalToGlobal(ax1,ay1,az1, double(h.Angle()), ax, ay, az );
    //    ax += vx;
    //    ay += vy;

    fYX->cd();
    fMarker.DrawMarker(vx, vy);
    //    fArrow.DrawArrow(vx, vy, ax, ay, 0.001, "|>"); // draw module direction

#if defined(DRIFT_TUBES)
    if (h.IRow() < PndFTSCAParameters::NMVDStations) // only MVD
#endif
    {
      fZX->cd();
      fMarker.DrawMarker(h.Z(), vy);

      if (fZR) {
        fZR->cd();
        fMarker.DrawMarker(h.Z(), sqrt(vx * vx + vy * vy));
      }
    }

#if defined(DRIFT_TUBES)
    TEllipse el;
    el.SetLineWidth(width);
    el.SetLineColor(col);
    el.SetFillStyle(0);
    if (h.IsPileduped()) {
      el.SetLineColor(kBlack);
    }
#ifdef PANDA_STT
    fYX->cd();

    if (fabs(h.ErrX12()) < 1e-7) {
      el.DrawEllipse(h.XW(), h.YW(), h.R(), h.R(), 0, 360, h.Angle() * 180 / TMath::Pi());
      fMarker.DrawMarker(h.XW(), h.YW());
    } else { // find the closes point of stereo tube to the track

      double b = h.Beta();
      double r2Min = 10e10, xSaved = 0, ySaved = 0, zSaved = 0;
      for (int itr = 0; itr < fGB->NTracks(); itr++) {
        PndFTSCAGBTrack &t = fGB->Tracks()[itr];
        PndFTSCATrackParam p = t.OuterParam();

        // transport to hit
        const bool rotated = p.Rotate(-p.Angle() + h.Angle());
        float x0, x1, x2;
        PndFTSCAParameters::GlobalToCALocal(h.XW(), h.YW(), h.ZW(), h.Angle(), x0, x1, x2);
        const bool transported = p.TransportToX0(x0, fGB->GetParameters().cBz());
        if (!rotated || !transported)
          continue;

        float p1 = p.Y(), p2 = p.Z();

        // find closest point on line
        // xx1 - x1 = b*(xx2 - x2)
        // perpendicular xx1 - p1 = 1/b*(xx2 - p2)

        const double beta = b;
        const double betaLast = M_PI_2 + b;
        const double zL = p2, yL = p1;
        const double z = x2, y = x1;

        const double ctbL = tan(-betaLast);
        const double ctb = tan(-beta);
        double xM = x0;
        double zM = ((zL * ctbL - yL) - (z * ctb - y)) / (ctbL - ctb);
        double yM = y + (zM - z) * ctb;

        double r2 = (yM - yL) * (yM - yL) + (zM - zL) * (zM - zL);

        if (r2 > r2Min)
          continue;
        r2Min = r2;
        PndFTSCAParameters::CALocalToGlobal(xM, yM, zM, static_cast<double>(h.Angle()), xSaved, ySaved, zSaved);
      }
      el.DrawEllipse(xSaved, ySaved, h.R(), h.R(), 0, 360, h.Angle() * 180 / TMath::Pi());
      fMarker.DrawMarker(xSaved, ySaved);
    }
#else // PANDA_FTS
    fZX->cd();

    if (fabs(h.ErrX12()) < 1e-7) {
      el.DrawEllipse(h.ZW(), h.XW(), h.R(), h.R(), 0, 360, h.Angle() * 180 / TMath::Pi());
      fMarker.DrawMarker(h.ZW(), h.XW());
    } else {
      fMarker.DrawMarker(h.ZW(), h.XW());
    }
#endif

    // fMarker.SetMarkerStyle(10);
    // fMarker.SetMarkerSize(width/4);

    // fMarker.SetMarkerStyle(9);
    // fMarker.SetMarkerSize(width);

    //     TLatex* Tl = new TLatex();
    //     Tl->SetTextSize(0.002);
    //     Tl->SetTextAlign(22);
    //     TString s = "";
    //     s += h.ID();
    //     s += ";";
    //     s += fPerf->HitLabel( h.ID() ).fLab[0];
    //     s += ";";
    //     // s += fPerf->HitLabel( h.ID() ).fLab[1];
    //     // s += ";";
    //     // s += fPerf->HitLabel( h.ID() ).fLab[2];
    //     // s += ";";
    //     s += h.IRow();

    // #ifdef PANDA_FTS
    //     s += ";";
    //     s+= int(h.Z()*100);
    //     s+= ";";
    //     s+= int(h.X()*100);
    //     Tl->DrawLatex( h.ZW(), h.XW(), s);
    // #else
    //       // s+= int(h.X()*100);
    //     // s+= ";";
    //     // s+= int(h.Y()*100);
    //     Tl->DrawLatex( vx, vy, s);
    // #endif

#ifdef PANDA_STT
    fZX->cd(); // TODO
#else
    fYX->cd(); // TODO
#endif

    // { // draw covariance ellipse
    //     // eigen values;
    //   const double C00 = h.C(2,2);
    //   const double C01 = h.C(2,1);
    //   const double C11 = h.C(1,1);
    //   const double b = - (C00+C11);
    //   const double det = sqrt(b*b - 4*(C00*C11 - C01*C01)); // l^2 - (C00+C11)l + C00*C11 - C01*C01 = 0
    //   const double e1 = (- b + det)*0.5, e2 = (- b - det)*0.5;
    //   double a = 0;
    //   if (abs(C00 - C11) > 1e-7) {
    //     a = 0.5*atan(2*C01/(C00-C11));
    //     if (C00 < C11)
    //       a = TMath::Pi()/2 - a;
    //   }

    //   el.DrawEllipse( h.ZW(), h.YW(), sqrt(e1)*kSS, sqrt(e2)*kSS*1e-10, 0, 360, a*180/TMath::Pi() );
    // }

    // if( h.IRow() >= 8 && h.IRow() <= 15 )
    //   el.DrawEllipse( h.Z(), vy, sqrt(h.Err2Z()), sqrt(h.Err2Y()), 0, 360, 0 );//h.Angle()*180/TMath::Pi() );
#endif // DRIFT_TUBES
  }
}

void PndFTSCADisplay::HitToGlobal(const FTSCAHit &h, float &x, float &y, float &z)
{
  PndFTSCAParameters::CALocalToGlobal(h.X0(), h.X1(), h.X2(), h.Angle(), x, y, z);
}

void PndFTSCADisplay::HitToGlobal(const FTSCAHitV &h, int iV, float &x, float &y, float &z)
{
  PndFTSCAParameters::CALocalToGlobal(float(h.X0()[iV]), float(h.X1()[iV]), float(h.X2()[iV]), float(h.Angle()[iV]), x, y, z);
}

#if defined(DRIFT_TUBES)
#define COUNT_SAME_HITS // count same wires positions
#endif
#ifdef COUNT_SAME_HITS
bool operator<(const TVector3 &a, const TVector3 &b)
{
  if (a.X() != b.X())
    return a.X() < b.X();
  else if (a.Y() != b.Y())
    return a.Y() < b.Y();
  else
    return a.Z() < b.Z();
}
#endif

void PndFTSCADisplay::DrawGBHits(const FTSCAHitsV &all)
{

  for (int iS = 0, iColor = 0; iS < all.NStations(); ++iS, iColor++) {
    if (iColor == kYellow)
      iColor++;
    if (iColor == kWhite)
      iColor++;

#ifdef COUNT_SAME_HITS
    map<TVector3, int> nSameHits;
#endif
    const FTSCAElementsOnStation<FTSCAHitV> &s = all.OnStation(iS);
    for (unsigned int i = 0; i < s.size(); ++i) {

      const FTSCAHitV &h = s[i];
      foreach_bit(int iV, h.IsValid())
      {
        float gx, gy, gz;
        HitToGlobal(h, iV, gx, gy, gz);
        // if ( abs(atan(gy/gx)) < 3.1415/180*5 )
        DrawGBPoint(gx, gy, gz, iColor, 0.1);

#ifdef COUNT_SAME_HITS
        TVector3 v(gx, gy, gz);
        if (nSameHits.count(v) > 0)
          nSameHits[v]++;
        else
          nSameHits[v] = 1;
#endif

        TLatex *Tl = new TLatex();
        Tl->SetTextSize(0.002);
        Tl->SetTextAlign(22);
        // TString ss = "";
        // ss+= h.Angle()[iV];
        // ss+= " ";
        // ss+= h.X0()[iV];
        // ss+= " ";
        // ss+= h.X1()[iV];
        TString ss = "";
        ss += fPerf->HitLabel(fGB->Hits()[h.Id()[iV]].ID()).fLab[0];
        ss += ";";
        ss += fPerf->HitLabel(fGB->Hits()[h.Id()[iV]].ID()).fLab[1];
        ss += ";";
        ss += fPerf->HitLabel(fGB->Hits()[h.Id()[iV]].ID()).fLab[2];
        fYX->cd();
        Tl->DrawLatex(gx, gy, ss);
        // std::cout << iS << " " << i << cd -" " << iV << " " << h.X1()[iV] << " " << h.X2()[iV] << " " << h.X0()[iV] << std::endl; // dbg
      }
    }
#ifdef COUNT_SAME_HITS
    for (std::map<TVector3, int>::iterator it = nSameHits.begin(); it != nSameHits.end(); ++it) {
      TLatex *Tl = new TLatex();
      Tl->SetTextSize(0.002);
      Tl->SetTextAlign(22);
      TString text = "";
      text += it->second;
      fYX->cd();
      Tl->DrawLatex(it->first.X(), it->first.Y(), text);
    }
#endif
  }
}

void PndFTSCADisplay::DrawGBHits(const FTSCAHits &all)
{

  for (int iS = 0, iColor = 0; iS < all.NStations(); ++iS, iColor++) {
    if (iColor == kYellow)
      iColor++;
    if (iColor == kWhite)
      iColor++;

#ifdef COUNT_SAME_HITS
    map<TVector3, int> nSameHits;
#endif
    const FTSCAElementsOnStation<FTSCAHit> &s = all.OnStation(iS);
    for (unsigned int i = 0; i < s.size(); ++i) {

      const FTSCAHit &h = s[i];
      // if  (h.IsUsed()) continue;
      if (h.IStation() < 0)
        continue;
      float gx, gy, gz;
      HitToGlobal(h, gx, gy, gz);
      // if ( abs(atan(gy/gx)) < 3.1415/180*5 )
      DrawGBPoint(gx, gy, gz, iColor, 0.8);

#ifdef COUNT_SAME_HITS
      TVector3 v(gx, gy, gz);
      if (nSameHits.count(v) > 0)
        nSameHits[v]++;
      else
        nSameHits[v] = 1;
#endif

      TLatex *Tl = new TLatex();
      Tl->SetTextSize(0.002);
      Tl->SetTextAlign(22);
      // TString ss = "";
      // ss+= h.Angle()[iV];
      // ss+= " ";
      // ss+= h.X0()[iV];
      // ss+= " ";
      // ss+= h.X1()[iV];
      TString ss = "";
      ss += fPerf->HitLabel(fGB->Hits()[h.Id()].ID()).fLab[0];
      ss += ";";
      ss += fPerf->HitLabel(fGB->Hits()[h.Id()].ID()).fLab[1];
      ss += ";";
      ss += fPerf->HitLabel(fGB->Hits()[h.Id()].ID()).fLab[2];
      fYX->cd();
      Tl->DrawLatex(gx, gy, ss);
      // std::cout << iS << " " << i << cd -" " << " " << h.X1() << " " << h.X2() << " " << h.X0() << std::endl; // dbg
    }
#ifdef COUNT_SAME_HITS
    for (std::map<TVector3, int>::iterator it = nSameHits.begin(); it != nSameHits.end(); ++it) {
      TLatex *Tl = new TLatex();
      Tl->SetTextSize(0.002);
      Tl->SetTextAlign(22);
      TString text = "";
      text += it->second;
      fYX->cd();
      Tl->DrawLatex(it->first.X(), it->first.Y(), text);
    }
#endif
  }
}

//#define CALC_GEO
void PndFTSCADisplay::DrawGBPoints()
{
#ifdef CALC_GEO
  const int NSta = 30;
  static int N[NSta];
  // for MVD
  static double r[NSta];
  static double zRange[NSta][2];
  static double yRange[NSta][2];
  static double xRange[NSta][2][2]; // 0 - negative, 1 - positive
                                    // for STT
  static double rMax[NSta];
  static double x0LRange[NSta][2];

  static bool first_call = true;
  if (first_call) {
    for (int i = 0; i < NSta; i++) {
      N[i] = 0;
      r[i] = 0;
      zRange[i][0] = 1e10;
      zRange[i][1] = -1e10;
      yRange[i][0] = 1e10;
      yRange[i][1] = -1e10;
      xRange[i][0][0] = 1e10;
      xRange[i][0][1] = -1e10;
      xRange[i][1][0] = 1e10;
      xRange[i][1][1] = -1e10;
      rMax[i] = -1e10;
      x0LRange[i][0] = 1e10;
      x0LRange[i][1] = -1e10;
    }
    first_call = false;
  }
#endif // CALC_GEO

  const PndFTSResizableArray<PndFTSCALocalMCPoint> &mcPs = *(fPerf->GetMCPoints());
  for (int i = 0; i < mcPs.Size(); ++i) {
    PndFTSCALocalMCPoint mcPoint = mcPs[i];
    double mcX0 = mcPoint.X();
    double mcY0 = mcPoint.Y();
    double mcZ = mcPoint.Z();

    PndFTSCADisplay::Instance().DrawGBPoint((float)mcX0, (float)mcY0, (float)mcZ, kRed, (Size_t).8);
    // switch ( mcPoint.IRow() ) {
    //   case 0: PndFTSCADisplay::Instance().DrawGBPoint((float)mcX0, (float)mcY0, (float)mcZ, kBlack, (Size_t).2); break;
    //   case 1: PndFTSCADisplay::Instance().DrawGBPoint((float)mcX0, (float)mcY0, (float)mcZ, kBlue, (Size_t).2); break;
    //   case 2: PndFTSCADisplay::Instance().DrawGBPoint((float)mcX0, (float)mcY0, (float)mcZ, kGreen, (Size_t).2); break;
    //   case 3: PndFTSCADisplay::Instance().DrawGBPoint((float)mcX0, (float)mcY0, (float)mcZ, kRed, (Size_t).2); break;
    //   default: PndFTSCADisplay::Instance().DrawGBPoint((float)mcX0, (float)mcY0, (float)mcZ, kGray, (Size_t).2);
    // }

#ifdef CALC_GEO
    const int iS = mcPoint.IRow();
    if (iS >= NSta || iS < 0)
      continue;
    N[iS]++;
    const double rCurr = sqrt(mcX0 * mcX0 + mcY0 * mcY0);
    r[iS] += sqrt(mcX0 * mcX0 + mcY0 * mcY0);
    zRange[iS][0] = std::min(zRange[iS][0], mcZ);
    zRange[iS][1] = std::max(zRange[iS][1], mcZ);
    yRange[iS][0] = std::min(yRange[iS][0], mcY0);
    yRange[iS][1] = std::max(yRange[iS][1], mcY0);
    if (mcX0 < 0) {
      xRange[iS][0][0] = std::min(xRange[iS][0][0], mcX0);
      xRange[iS][0][1] = std::max(xRange[iS][0][1], mcX0);
    } else {
      xRange[iS][1][0] = std::min(xRange[iS][1][0], mcX0);
      xRange[iS][1][1] = std::max(xRange[iS][1][1], mcX0);
    }
    rMax[iS] = std::max(rMax[iS], rCurr);
    double x0, x1;

    double A = atan(abs(mcY0 / mcX0)); // angle of slice
    const double pi2 = TMath::Pi() / 2;
    if (mcY0 >= 0 && mcX0 >= 0)
      A = pi2 - A;
    if (mcY0 < 0 && mcX0 >= 0)
      A = pi2 + A;
    if (mcY0 < 0 && mcX0 < 0)
      A = 3 * pi2 - A;
    if (mcY0 >= 0 && mcX0 < 0)
      A = 3 * pi2 + A;
    A = floor(A / pi2 * 6) * pi2 * 2 / 3.f;
    // turn by -(A+3.1415/6)
    A = A + pi2 / 3.f;
    PndFTSCAParameters::GlobalToCALocal(mcX0, mcY0, -A, x0, x1);

    cout << iS << " " << mcX0 << " " << x0 << " " << A << " " << mcPoint.Angle() << endl;
    x0LRange[iS][0] = std::min(x0LRange[iS][0], abs(x0));
    x0LRange[iS][1] = std::max(x0LRange[iS][1], abs(x0));
#endif // CALC_GEO
  }

#ifdef CALC_GEO
  for (int i = 0; i < NSta; i++)
    cout << i << " station: "
         << " x- = [" << xRange[i][0][0] << "," << xRange[i][0][1] << "]"
         << " x+ = [" << xRange[i][1][0] << "," << xRange[i][1][1] << "]"
         << " y = [" << yRange[i][0] << "," << yRange[i][1] << "]"
         << " z = [" << zRange[i][0] << "," << zRange[i][1] << "]" << endl
         << " r = " << r[i] / N[i] << " rMax = " << rMax[i] << " x0Local = [" << x0LRange[i][0] << "," << x0LRange[i][1] << "]" << endl;
#endif // CALC_GEO
}

void PndFTSCADisplay::DrawPVHisto(const vector<float> &pvHist, const PndFTSCAParam &param)
{
  const unsigned int N = pvHist.size();
  const float maxZ = param.MaxZ();
  float max = -1;
  for (unsigned int i = 0; i < N; ++i) {
    max = (max < pvHist[i]) ? pvHist[i] : max;
  }

  for (unsigned int i = 0; i < N; ++i) {
    float z = (2.f * i / N - 1) * maxZ;
    float dr = pvHist[i] / max * maxZ;

    fZX->cd();
    fLine.SetLineColor(kBlue);
    fLine.SetLineWidth(0);
    fLine.DrawLine(z, 0, z, -dr);
  }
}

void PndFTSCADisplay::DrawGBNPlets(const FTSCANPletsV &all)
{
  for (int iS = 0; iS < all.NStations(); ++iS) {
    const FTSCAElementsOnStation<FTSCANPletV> &s = all.OnStation(iS);
    if (s.size() <= 0)
      continue;
    const int N = s[0].N();

    int color = 2;
    color = kRed;
    /*switch( N )
    {
      case 1: color = kYellow; break;
      case 2: color = kMagenta; break;
      case 3: color = kBlue; break;
      case 4: color = kOrange; break;
      case 5: color = kGreen; break;
      case 6: color = kRed; break;
    }*/
    /*switch( int(iS/10) )
    {
        case 0: color = kViolet+iS; break;
        case 1: color = kOrange+((iS%2)*10); break;
        case 2: color = kAzure+((iS%2)*10); break;
        case 3: color = kGreen+((iS%2)*4); break;
        case 4: color = kMagenta+((iS%2)*4); break;
    }*/
    for (unsigned int i = 0; i < s.size(); ++i) {
      foreach_bit(int iV, s[i].IsValid())
      {
        vector<float> gx(N), gy(N), gz(N);
        for (int ih = 0; ih < N; ih++) {
          HitToGlobal(s.GetHit(iV, ih, i), gx[ih], gy[ih], gz[ih]);
        }

        for (int ih = 1; ih < N; ih++) {
          DrawGBLine(gx[ih - 1], gy[ih - 1], gz[ih - 1], gx[ih], gy[ih], gz[ih], color, 0);
        }

        if (N == 1) // singlets
          DrawGBLine(0, 0, 0, gx[0], gy[0], gz[0], color, 0);

        /*if (gz[N-1]>606){
              cout<<"N "<<s[i].N()<<" QP "<<s[i].Param().QP()[0]<<" Tx "<<s[i].Param().Tx()[0]<<" Ty "<<s[i].Param().Ty()[0]<<" X "<<s[i].Param().X()[0]<<" Y
        "<<s[i].Param().Y()[0]<<" Chi2 "<<s[i].Param().Chi2()[0]<<endl; Ask();
        }*/
        cout << "N " << s[i].N() << " QP " << s[i].Param().QP()[0] << " Tx " << s[i].Param().Tx()[0] << " Ty " << s[i].Param().Ty()[0] << " X " << s[i].Param().X()[0] << " Y "
             << s[i].Param().Y()[0] << " Chi2 " << s[i].Param().Chi2()[0] << endl;
      }
    }
    // Update();
    // Ask();
  }
}

void PndFTSCADisplay::DrawGBNPlets(const FTSCAElementsOnStation<FTSCANPletV> &s)
{
  if (s.size() <= 0)
    return;
  const int N = s[0].N();

  int color = 2;
  switch (N) {
  case 1: color = kYellow; break;
  case 2: color = kMagenta; break;
  case 3: color = kBlue; break;
  case 4: color = kOrange; break;
  case 5: color = kGreen; break;
  case 6: color = kRed; break;
  }

  for (unsigned int i = 0; i < s.size(); ++i) {
    // cout<<"s[i].IsValid() "<<s[i].IsValid()<<endl;
    // cout<<"N "<<N<<endl;
    foreach_bit(int iV, s[i].IsValid())
    {
      vector<float> gx(N), gy(N), gz(N);
      for (int ih = 0; ih < N; ih++) {
        HitToGlobal(s.GetHit(iV, ih, i), gx[ih], gy[ih], gz[ih]);
      }
      if (N == 1) // singlets
      {
        // cout<<"gx[0], gy[0], gz[0] "<<gx[0]<<" "<<gy[0]<<" "<<gz[0]<<endl;
        DrawGBLine(0, 0, 0, gx[0], gy[0], gz[0], color, 0);
      }
      for (int ih = 1; ih < N; ih++) {
        // cout<<"gx[ih], gy[ih], gz[ih] "<<gx[ih]<<" "<<gy[ih]<<" "<<gz[ih]<<endl;
        DrawGBLine(gx[ih - 1], gy[ih - 1], gz[ih - 1], gx[ih], gy[ih], gz[ih], color + 2 * (ih == 1), 0);
      }
    }
  }
}

void PndFTSCADisplay::DrawGBTracks(const FTSCATracks &all)
{
  for (unsigned int i = 0; i < all.size(); ++i) {
    const FTSCATrack &t = all[i];
    const unsigned int NTHits = t.NHits();
    FTSCAHit hitP = all.Hit(0, i);
    for (unsigned int iH = 1; iH < NTHits; iH++) {
      const FTSCAHit &hit = all.Hit(iH, i);

      float gx0, gy0, gz0, gx1, gy1, gz1;
      HitToGlobal(hitP, gx0, gy0, gz0);
      HitToGlobal(hit, gx1, gy1, gz1);
      DrawGBLine(gx0, gy0, gz0, gx1, gy1, gz1, kRed, 0.1);

      hitP = hit;
    }
  }
}

int PndFTSCADisplay::GetTrackMC(const PndFTSCADisplayTmpHit *vHits, int NHits)
{
  // get MC label for the track

  const PndFTSCAGBTracker &tracker = *fGB;

  int label = -1;
  double purity = 0;
  int *lb = new int[NHits * 3];
  int nla = 0;
  // std::cout<<"\n\nTrack hits mc: "<<std::endl;
  for (int ihit = 0; ihit < NHits; ihit++) {
    const PndFTSCAGBHit &h = tracker.Hits()[vHits[ihit].ID()];
    const PndFTSCAPerformance::PndFTSCAHitLabel &l = fPerf->HitLabel(h.ID());
    if (l.fLab[0] >= 0)
      lb[nla++] = l.fLab[0];
    if (l.fLab[1] >= 0)
      lb[nla++] = l.fLab[1];
    if (l.fLab[2] >= 0)
      lb[nla++] = l.fLab[2];
    // std::cout<<ihit<<":  "<<l.fLab[0]<<" "<<l.fLab[1]<<" "<<l.fLab[2]<<std::endl;
  }
  std::sort(lb, lb + nla);
  int labmax = -1, labcur = -1, lmax = 0, lcurr = 0, nh = 0;
  // std::cout<<"MC track IDs :"<<std::endl;
  for (int i = 0; i < nla; i++) {
    if (lb[i] != labcur) {
      if (0 && i > 0 && lb[i - 1] >= 0) {
        const PndFTSCAMCTrack &mc = fPerf->MCTrack(lb[i - 1]);
        std::cout << lb[i - 1] << ": nhits=" << nh << ", pdg=" << mc.PDG() << ", Pt=" << mc.Pt() << ", P=" << mc.P() << ", par=" << mc.Par()[0] << " " << mc.Par()[1] << " "
                  << mc.Par()[2] << " " << mc.Par()[3] << " " << mc.Par()[4] << " " << mc.Par()[5] << " " << mc.Par()[6] << std::endl;
      }
      nh = 0;
      if (labcur >= 0 && lmax < lcurr) {
        lmax = lcurr;
        labmax = labcur;
      }
      labcur = lb[i];
      lcurr = 0;
    }
    lcurr++;
    nh++;
  }
  if (0 && nla - 1 > 0 && lb[nla - 1] >= 0) {
    const PndFTSCAMCTrack &mc = fPerf->MCTrack(lb[nla - 1]);
    std::cout << lb[nla - 1] << ": nhits=" << nh << ", pdg=" << mc.PDG() << ", Pt=" << mc.Pt() << ", P=" << mc.P() << ", par=" << mc.Par()[0] << " " << mc.Par()[1] << " "
              << mc.Par()[2] << " " << mc.Par()[3] << " " << mc.Par()[4] << " " << mc.Par()[5] << " " << mc.Par()[6] << std::endl;
  }
  if (labcur >= 0 && lmax < lcurr) {
    lmax = lcurr;
    labmax = labcur;
  }
  lmax = 0;
  for (int ihit = 0; ihit < NHits; ihit++) {
    const PndFTSCAGBHit &h = tracker.Hits()[vHits[ihit].ID()];
    const PndFTSCAPerformance::PndFTSCAHitLabel &l = fPerf->HitLabel(h.ID());
    if (l.fLab[0] == labmax || l.fLab[1] == labmax || l.fLab[2] == labmax)
      lmax++;
  }
  label = labmax;
  purity = ((NHits > 0) ? double(lmax) / double(NHits) : 0);
  if (lb)
    delete[] lb;
  if (purity < .9)
    label = -1;
  return label;
}

bool PndFTSCADisplay::DrawTrack(PndFTSCATrackParam t, double Alpha, const PndFTSCADisplayTmpHit *vHits, int NHits, int color, Size_t width, bool pPoint)
{
  //   // draw track
  // #ifndef DRAW_3D
  //   bool drawEndPoints = 1;
  // #else
  const bool drawEndPoints = 0;
  // #endif // DRAW_3D

  if (NHits < 2)
    return 0;

  const PndFTSCAGBTracker &tracker = *fGB;
  if (width < 0)
    width = 2;

  if (fDrawOnlyRef) {
    int lab = GetTrackMC(vHits, NHits);
    if (lab < 0)
      return 0;
    const PndFTSCAMCTrack &mc = fPerf->MCTrack(lab);
    if (mc.P() < 1)
      return 0;
  }

  if (color < 0) {
    // color = GetColorZ( (vz[0]+vz[mHits-1])/2. );
    // color = GetColorK(t.Kappa());
    int lab = GetTrackMC(vHits, NHits);
    color = GetColor(lab + 1);
    if (lab >= 0) {
      const PndFTSCAMCTrack &mc = fPerf->MCTrack(lab);
      if (mc.P() >= PParameters::RefThreshold)
        color = kRed;
    }
  }

#if !defined(PANDA_FTS)
  if (t.SinPhi() > .999)
    t.SetSinPhi(.999);
  else if (t.SinPhi() < -.999)
    t.SetSinPhi(-.999);
#endif
  //  int iSlice = fSlice->Param().ISlice();

  // sort(vHits, vHits + NHits, PndFTSCADisplayTmpHit::CompareHitZ );

  vector<double> vx(NHits), vy(NHits), vz(NHits);
  int mHits = 0;

  // int oldSlice = -1;
  double alpha = Alpha;
  PndFTSCATrackParam tt = t;

  for (int iHit = 0; iHit < NHits; iHit++) {

    const PndFTSCAGBHit &h = tracker.Hits()[vHits[iHit].ID()];

    double hCos = TMath::Cos(alpha);
    double hSin = TMath::Sin(alpha);
    double x0 = h.X(), y0 = h.Y(), z1 = h.Z();
    double x1 = x0 * hCos + y0 * hSin;
    double y1 = y0 * hCos - x0 * hSin;

    {
      double dx = x1 - tt.X();
      double dy = y1 - tt.Y();
      if (dx * dx + dy * dy > 1.) {
        double dalpha = TMath::ATan2(dy, dx);
        if (tt.Rotate(dalpha)) {
          alpha += dalpha;
          hCos = TMath::Cos(alpha);
          hSin = TMath::Sin(alpha);
          x1 = x0 * hCos + y0 * hSin;
          y1 = y0 * hCos - x0 * hSin;
        }
      }
    }

    vx[mHits] = x1;
    vy[mHits] = y1;
    vz[mHits] = z1;
    mHits++;
  }
  if (pPoint) {
    double x1 = t.X(), y1 = t.Y(), z1 = t.Z();

    double dx = x1 - vx[0];
    double dy = y1 - vy[0];
    // std::cout<<x1<<" "<<y1<<" "<<vx[0]<<" "<<vy[0]<<" "<<dx<<" "<<dy<<std::endl;
    double d0 = dx * dx + dy * dy;
    dx = x1 - vx[mHits - 1];
    dy = y1 - vy[mHits - 1];
    // std::cout<<x1<<" "<<y1<<" "<<vx[mHits-1]<<" "<<vy[mHits-1]<<" "<<dx<<" "<<dy<<std::endl;
    double d1 = dx * dx + dy * dy;
    // std::cout<<"d0, d1="<<d0<<" "<<d1<<std::endl;
    if (d1 < d0) {
      vx[mHits] = x1;
      vy[mHits] = y1;
      vz[mHits] = z1;
      mHits++;
    } else {
      for (int i = mHits; i > 0; i--) {
        vx[i] = vx[i - 1];
        vy[i] = vy[i - 1];
        vz[i] = vz[i - 1];
      }
      vx[0] = x1;
      vy[0] = y1;
      vz[0] = z1;
      mHits++;
    }
  }

#ifdef DRAW_3D
  const float zoom = 0.6;
  const float z_zoom = 1.2;
  const float z0_zoom = 1000.;
  for (int i = mHits - 1; i >= 0; i--) {
    TVector3 v(vx[i], -vz[i], vy[i]);

    v.RotateX(TMath::Pi() / 70);
    v.RotateZ(TMath::Pi() / 12);
    vx[i] = v.X() * zoom;
    vy[i] = v.Z() * zoom;
    vz[i] = -v.Y() * zoom;

    vx[i] *= (z0_zoom - vz[i]) / z0_zoom * z_zoom;
    vy[i] *= (z0_zoom - vz[i]) / z0_zoom * z_zoom;
  }
#endif // DRAW_3D

  fLine.SetLineColor(color);
  fLine.SetLineWidth(width);
  fArc.SetFillStyle(0);
  fArc.SetLineColor(color);
  fArc.SetLineWidth(width);
  TPolyLine pl;
  pl.SetLineColor(color);
  pl.SetLineWidth(width);
  TPolyLine plZ;
  plZ.SetLineColor(color);
  plZ.SetLineWidth(width);

  fMarker.SetMarkerSize(width / 2.);
  fMarker.SetMarkerColor(color);

  fYX->cd();
  pl.DrawPolyLine(mHits, &vx[0], &vy[0]);
  if (drawEndPoints) {
    fMarker.DrawMarker(vx[0], vy[0]);
    fMarker.DrawMarker(vx[mHits - 1], vy[mHits - 1]);
  }
  fZX->cd();
  plZ.DrawPolyLine(mHits, &vz[0], &vy[0]);
  if (drawEndPoints) {
    fMarker.DrawMarker(vz[0], vy[0]);
    fMarker.DrawMarker(vz[mHits - 1], vy[mHits - 1]);
  }

  fLine.SetLineWidth(1);
  return 1;
}

void PndFTSCADisplay::DrawHelix(float p0, float c, float z, float zStart, float z0, float xc, float yc, float r, float b, int color, Size_t width)
{
  fLine.SetLineColor(color);
  fLine.SetLineWidth(width);
  // draw slice track
  float x, y, p;
  p = p0 + c * (zStart - z0) / b;
  y = yc + r * sin(p);
  x = xc + c * r * cos(p);

  float zPrev = zStart;
  float xPrev = x;
  float yPrev = y;
  float zEnd = z;

  for (int i = 1; i < 100; i++) {
    z = zStart + (zEnd - zStart) / 100 * i;
    p = p0 + c * (z - z0) / b;
    y = yc + r * sin(p);
    x = xc + c * r * cos(p);

    fYX->cd();
    fLine.DrawLine(x, y, xPrev, yPrev);
    fZX->cd();
    fLine.DrawLine(z, y, zPrev, yPrev);

    xPrev = x;
    yPrev = y;
    zPrev = z;
  }
}

void PndFTSCADisplay::DrawParticleGlobal(float *param, float q, float tStart, float tEnd, float b, int color, Size_t width)
{
  fLine.SetLineColor(color);
  fLine.SetLineWidth(width);
  fArrow.SetFillColor(color);
  fArrow.SetLineColor(color);
  fArrow.SetLineWidth(width);

  float p[8];
  for (int iP = 0; iP < 8; iP++)
    p[iP] = param[iP];

  float t = tStart;

  float xPrev = 0, yPrev = 0, zPrev = 0;

  const float kCLight = 0.000299792458;
  b = b * q * kCLight;

  for (int i = 0; i <= 100; i++) {
    t = tEnd / 100 * i;
    float bs = b * t;
    float s = sin(bs), c = cos(bs);
    float sB, cB;

    const float kOvSqr6 = 1. / sqrt(6.);

    sB = (1.e-8 < fabs(bs)) ? (s / b) : ((1 - bs * kOvSqr6) * (1 + bs * kOvSqr6) * t);
    cB = (1.e-8 < fabs(bs)) ? ((1 - c) / b) : (.5 * sB * bs);

    float px = param[3];
    float py = param[4];
    float pz = param[5];

    p[0] = param[0] + sB * px + cB * py;
    p[1] = param[1] - cB * px + sB * py;
    p[2] = param[2] + t * pz;
    p[3] = c * px + s * py;
    p[4] = -s * px + c * py;
    p[5] = param[5];
    p[6] = param[6];
    p[7] = param[7];

    if (i > 0) {
      fYX->cd();
      fLine.DrawLine(p[0], p[1], xPrev, yPrev);
      fZX->cd();
      fLine.DrawLine(p[2], p[1], zPrev, yPrev);
    }

    xPrev = p[0];
    yPrev = p[1];
    zPrev = p[2];
  }
}

void PndFTSCADisplay::DrawParticleGlobal(float *param, float q, float n[4], float b, int color, Size_t width)
{
  fLine.SetLineColor(color);
  fLine.SetLineWidth(width);
  fArrow.SetFillColor(color);
  fArrow.SetLineColor(color);
  fArrow.SetLineWidth(width);

  float p[8];
  for (int iP = 0; iP < 8; iP++)
    p[iP] = param[iP];

  float t = 0;

  float xPrev = p[0], yPrev = p[1], zPrev = p[2];

  const float kCLight = 0.000299792458;
  b = b * q * kCLight;

  float step = -0.05;
  double dist = p[0] * n[0] + p[1] * n[1] + p[2] * n[2] + n[3];
  double dist_last = dist;
  bool step_changed = false;
  for (int i = 1; dist > 0 && i < 10000; i++) {
    //    cout << p[0]*n[0]+p[1]*n[1]+p[2]*n[2]+n[3] << endl;
    t = step * i;
    float bs = b * t;
    float s = sin(bs), c = cos(bs);
    float sB, cB;

    const float kOvSqr6 = 1. / sqrt(6.);

    sB = (1.e-8 < fabs(bs)) ? (s / b) : ((1 - bs * kOvSqr6) * (1 + bs * kOvSqr6) * t);
    cB = (1.e-8 < fabs(bs)) ? ((1 - c) / b) : (.5 * sB * bs);

    float px = param[3];
    float py = param[4];
    float pz = param[5];

    p[0] = param[0] + sB * px + cB * py;
    p[1] = param[1] - cB * px + sB * py;
    p[2] = param[2] + t * pz;
    p[3] = c * px + s * py;
    p[4] = -s * px + c * py;
    p[5] = param[5];
    p[6] = param[6];
    p[7] = param[7];

    dist = p[0] * n[0] + p[1] * n[1] + p[2] * n[2] + n[3];

    if (dist < dist_last) {
      fYX->cd();
      fLine.DrawLine(p[0], p[1], xPrev, yPrev);
      fZX->cd();
      fLine.DrawLine(p[2], p[1], zPrev, yPrev);
    } else if (abs(dist - dist_last) < 1e-8) {
      step *= 2;
      i--;
      continue;
    } else if (!step_changed) { // wrong direction
      step *= -1;
      step_changed = true;
      i--;
      continue;
    } else { // too curved track
      break;
    }

    xPrev = p[0];
    yPrev = p[1];
    zPrev = p[2];

    dist_last = dist;
  }

  DrawGBPoint(xPrev, yPrev, zPrev, color, 0.2);
}

void PndFTSCADisplay::DrawGBTrack(int itr, int color, int width)
{
  // draw global track

  const PndFTSCAGBTracker &tracker = *fGB;

  const PndFTSCAGBTrack &track = tracker.Track(itr);
  if (track.NHits() < 2)
    return;

  vector<PndFTSCADisplayTmpHit> vHits(track.NHits());

  for (int ih = 0; ih < track.NHits(); ih++) {
    const int i = tracker.TrackHit(track.FirstHitRef() + ih);
    const PndFTSCAGBHit &h = tracker.Hit(i);
    vHits[ih].SetID(i);
    vHits[ih].SetS(0);
    vHits[ih].SetZ(h.Z());
  }

  DrawTrack(track.Param(), track.Param().Angle(), &(vHits[0]), track.NHits(), color, width);
}

void PndFTSCADisplay::DrawRecoTrack(int itr, int color, int width)
{
  const PndFTSCAGBTracker &tracker = *fGB;

  const PndFTSCAGBTrack &track = tracker.Track(itr);
  if (track.NHits() < 2)
    return;

#if 1 // draw by hits or by parameters
  PndFTSCAGBHit hLast = tracker.Hit(tracker.TrackHit(track.FirstHitRef()));
#if defined(DRIFT_TUBES)
  double hLx = hLast.X(), hLy = hLast.Y(), hLz = hLast.Z();
  int skipped = 0;
#endif
  for (int ih = 1; ih < track.NHits(); ih++) {
    const int i = tracker.TrackHit(track.FirstHitRef() + ih);
    const PndFTSCAGBHit &h = tracker.Hit(i);
#if defined(DRIFT_TUBES)

    // draw cross point (this point is drawn in assupmtion that track is perpendicular to layers, if it is not, the point will be shifted a little)
    {
      const double betaLast = M_PI_2 + hLast.Beta(); // strip angle
      const double beta = M_PI_2 + h.Beta();         // strip angle
      if (fabs(betaLast - beta) > 5e-7 && h.IRow() > PndFTSCAParameters::NMVDStations) {

        const double ctbL = tan(M_PI_2 - betaLast);
        const double ctb = tan(M_PI_2 - beta);
        float xL, yL, zL, x, y, z;
        hLast.GetLocalX0X1X2(xL, yL, zL);
        h.GetLocalX0X1X2(x, y, z);

        double xM = (xL + x) * 0.5;
        double zM = ((zL * ctbL - yL) - (z * ctb - y)) / (ctbL - ctb);
        double yM = y + (zM - z) * ctb;
        double xMg, yMg, zMg;
        PndFTSCAParameters::CALocalToGlobal(xM, yM, zM, static_cast<double>(h.Angle()), xMg, yMg, zMg);

        if (ih - skipped != 1 || (fabs(hLast.ErrX12()) < 1e-7)) {
          if (fabs(hLast.ErrX12()) < 1e-7)
            DrawGBLine(hLx, hLy, hLz, xMg, yMg, zMg, color, width, 0);
          else
            DrawGBLine(hLx, hLy, hLz, xMg, yMg, zMg, color, width);
        }
        DrawGBPoint(xMg, yMg, zMg, kBlack, 0.1);

        hLx = xMg;
        hLy = yMg;
        hLz = zMg;
        skipped = 0;
      } else
        skipped++;
    }

    if (fabs(h.ErrX12()) < 1e-7) {
      if (h.IRow() >= PndFTSCAParameters::NMVDStations)
        DrawGBLine(hLx, hLy, hLz, h.X(), h.Y(), h.Z(), color, width, 0);
      else
        DrawGBLine(hLx, hLy, hLz, h.X(), h.Y(), h.Z(), color, width);
      hLx = h.X(), hLy = h.Y(), hLz = h.Z();
      skipped = 0;
    }

#else
    DrawGBLine(hLast.X(), hLast.Y(), hLast.Z(), h.X(), h.Y(), h.Z(), color, width);
#endif
    hLast = h;
  }
#else  // draw by hits or by parameters

  PndFTSCATrackParam p = track.InnerParam();

  float x, y, z, px, py, pz;
  PndFTSCAParameters::CALocalToGlobal(p.X(), p.Y(), p.Z(), p.Angle(), x, y, z);
  const float px0 = p.SignCosPhi() * sqrt(1 - p.SinPhi() * p.SinPhi()) / p.QPt();
  const float py0 = p.SinPhi() / p.QPt();
  const float pz0 = p.DzDs() / p.QPt();
  PndFTSCAParameters::CALocalToGlobal(px0, py0, pz0, p.Angle(), px, py, pz);

  float param[8] = {x, y, z, px, py, pz, 0, 0};
  const float l = fGB->GetParameters().MaxR() - sqrt(x * x + y * y);
  DrawParticleGlobal(param, 1.f, 0, l / sqrt(px0 * px0 + py0 * py0), tracker.GetParameters().Bz(), color, 0);
#endif // draw by hits or by parameters
}

void PndFTSCADisplay::DrawMCTrack(int itr, int color, int width)
{
  PndFTSCAPerformance &perf = PndFTSCAPerformance::Instance();

  const PndFTSResizableArray<PndFTSCAMCTrack> &mcTs = *perf.GetMCTracks();
  const PndFTSResizableArray<PndFTSCALocalMCPoint> &mcPs = *perf.GetMCPoints();

  const PndFTSCAMCTrack &track = mcTs[itr];
  if (track.NMCPoints() < 2)
    return;

  fLine.SetLineStyle(3);
  PndFTSCALocalMCPoint mcPLast = mcPs[track.FirstMCPointID()];
  for (int ih = 1; ih < track.NMCPoints(); ih++) {
    const PndFTSCALocalMCPoint &mcP = mcPs[track.FirstMCPointID() + ih];
    DrawGBLine(mcPLast.X(), mcPLast.Y(), mcPLast.Z(), mcP.X(), mcP.Y(), mcP.Z(), color, width);
    mcPLast = mcP;
  }

  fLine.SetLineStyle(1);
}

#if !defined(PANDA_FTS)
void PndFTSCADisplay::DrawGBTrackFast(const PndFTSCAGBTracker &tracker, int itr, int color)
{
  // draw global track

  PndFTSCAGBTrack &track = tracker.Tracks()[itr];
  if (track.NHits() < 2)
    return;
  int width = 1;

  PndFTSCADisplayTmpHit *vHits = new PndFTSCADisplayTmpHit[track.NHits()];
  PndFTSCATrackParam t = track.Param();

  for (int ih = 0; ih < track.NHits(); ih++) {
    int i = tracker.TrackHits()[track.FirstHitRef() + ih];
    const PndFTSCAGBHit *h = &(tracker.Hits()[i]);
    vHits[ih].SetID(i);
    vHits[ih].SetS(0);
    vHits[ih].SetZ(h->Z());
  }

  sort(vHits, vHits + track.NHits(), PndFTSCADisplayTmpHit::CompareHitZ);
  int colorY = color;
  {
    const PndFTSCAGBHit &h1 = tracker.Hits()[vHits[0].ID()];
    const PndFTSCAGBHit &h2 = tracker.Hits()[vHits[track.NHits() - 1].ID()];
    if (color < 0)
      color = GetColorZ((h1.Z() + h2.Z()) / 2.);
    double gy1 = h1.Y(), gy2 = h2.Y();
    if (colorY < 0)
      colorY = GetColorY((gy1 + gy2) / 2.);
    color = colorY = GetColorK(t.QPt());
  }

  fMarker.SetMarkerColor(color); // kBlue);
  fMarker.SetMarkerSize(1.);
  fLine.SetLineColor(color);
  fLine.SetLineWidth(width);
  fArc.SetFillStyle(0);
  fArc.SetLineColor(color);
  fArc.SetLineWidth(width);
  TPolyLine pl;
  pl.SetLineColor(colorY);
  pl.SetLineWidth(width);

  // YX
  {

    const PndFTSCAGBHit &h1 = tracker.Hits()[vHits[0].ID()];
    const PndFTSCAGBHit &h2 = tracker.Hits()[vHits[track.NHits() - 1].ID()];
    float x1, y1, z1, x2, y2, z2;
    double vx1, vy1, vx2, vy2;

    t.GetDCAPoint(h1.X(), h1.Y(), h1.Z(), x1, y1, z1, tracker.GetParameters().Bz());
    vx1 = x1;
    vy1 = y1;

    t.GetDCAPoint(h2.X(), h2.Y(), h2.Z(), x2, y2, z2, tracker.GetParameters().Bz());
    vx2 = x2;
    vy2 = y2;

    double x0 = t.X();
    double y0 = t.Y();
    double sinPhi = t.SinPhi();
    double k = t.Kappa(tracker.GetParameters().Bz());
    double ex = t.CosPhi();
    double ey = sinPhi;

    if (TMath::Abs(k) > 1.e-4) {

      fYX->cd();

      double r = 1 / TMath::Abs(k);
      double xc = x0 - ey * (1 / k);
      double yc = y0 + ex * (1 / k);

      double vx = xc, vy = yc;

      double a1 = TMath::ATan2(vy1 - vy, vx1 - vx) / TMath::Pi() * 180.;
      double a2 = TMath::ATan2(vy2 - vy, vx2 - vx) / TMath::Pi() * 180.;
      if (a1 < 0)
        a1 += 360;
      if (a2 < 0)
        a2 += 360;
      if (a2 < a1)
        a2 += 360;
      double da = TMath::Abs(a2 - a1);
      if (da > 360)
        da -= 360;
      if (da > 180) {
        da = a1;
        a1 = a2;
        a2 = da;
        if (a2 < a1)
          a2 += 360;
      }
      fArc.DrawArc(vx, vy, r, a1, a2, "only");
      // fArc.DrawArc(vx,vy,r, 0,360,"only");
    } else {
      fYX->cd();
      fLine.DrawLine(vx1, vy1, vx2, vy2);
    }
  }

  // ZX
  PndFTSResizableArray<double> py(track.NHits()), pz(track.NHits());

  for (int iHit = 0; iHit < track.NHits(); iHit++) {

    const PndFTSCAGBHit &h1 = tracker.Hits()[vHits[iHit].ID()];
    float x1, y1, z1;
    double vx1, vy1;
    t.GetDCAPoint(h1.X(), h1.Y(), h1.Z(), x1, y1, z1, tracker.GetParameters().Bz());
    vx1 = x1;
    vy1 = y1;
    py[iHit] = vy1;
    pz[iHit] = z1;
  }

  fZX->cd();
  pl.DrawPolyLine(track.NHits(), pz.Data(), py.Data());

  fLine.SetLineWidth(1);
  delete[] vHits;
}

void PndFTSCADisplay::DrawTrackParam(TrackParam t, int color)
{
  const PndFTSCAGBTracker &tracker = *fGB;

  for (int i = 0; i < 100; ++i) {
    double x = t.X();
    double y = t.Y();
    double sinPhi = t.SinPhi();
    double z = t.Z();
    double dzds = t.DzDs();
    double ex = t.CosPhi();
    double ey = sinPhi;

    fLine.SetLineWidth(1);
    fLine.SetLineColor(color);

    double vx = x, vy = y, vex = ex, vey = ey;
    double d = CAMath::RSqrt(vex * vex + vey * vey);
    vex *= d;
    vey *= d;

    fYX->cd();
    fLine.DrawLine(vx, vy, vx + vex * 4, vy + vey * 4);
    fZX->cd();
    fLine.DrawLine(z, vy, z + dzds * 4, vy + vey * 4);

    t.TransportToX0(x + ex * CAMath::RSqrt(ex * ex + ey * ey), tracker.GetParameters().cBz());
  }
}
#endif
void PndFTSCADisplay::SaveCanvasToFile(TString fileName)
{
  fCanvas->SaveAs(fileName);
}

void PndFTSCADisplay::SetTPC(const PndFTSCAParam &tpcParam)
{
#ifdef PANDA_FTS
  fXMin = tpcParam.MinX();
  fXMax = tpcParam.MaxX();
  fYMin = tpcParam.MinY();
  fYMax = tpcParam.MaxY();
  fZMin = tpcParam.MinZ();
  fZMax = tpcParam.MaxZ();

  const double dx = fXMax - fXMin;
  const double dy = fYMax - fYMin;
  const double dz = fZMax - fZMin;
  fYX->Range(fXMin - dx * 0.01, fYMin - dy * 0.01, fXMax + dx * 0.01, fYMax + dy * 0.01);
  fZX->Range(fZMin - dz * 0.01, fXMin - dx * 0.01, fZMax + dz * 0.01, fXMax + dx * 0.01);
  // fZX->Range( fZMin - dz*0.01, fXMin - -53*dx*0.01, fZMax + -90*dz*0.01, fXMax + -44*dx*0.01 ); zoom
#else
  fZMin = tpcParam.MinZ();
  fZMax = tpcParam.MaxZ();
  fYMin = -60;
  fYMax = 60;
  fRInnerMin = tpcParam.MinR();
  fRInnerMax = 123.; // approximate TODO: from file!
  fROuterMin = 123.;
  fROuterMax = tpcParam.MaxR(); // tpcParam.RMax();

  fYX->Range(-fROuterMax - 2, -fROuterMax - 2, fROuterMax + 2, fROuterMax + 2);
  fZX->Range(fZMin * 1.01, -fROuterMax * 1.01, fZMax * 1.01, fROuterMax * 1.01);
  if (fZR) {
    fZR->Range(fZMin * 1.01, -fROuterMax * 0.01, fZMax * 1.01, fROuterMax * 1.01);
  }
#endif
}
