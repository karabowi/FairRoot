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

////////////////////////////////////////////////////////////
//
// PndTrkCombiLegendreTransform
//
// Class to perform the Combinatorial Legendre Transformation:
// for each couple ij of hits, the 4 intersections of the r = r(theta)
// sinusoidal curves are computed and the accumulation histo is
// filled with these points only.
// 1. the nof entries in the histo and its binning are not dependent anymore
// 2. the nof entries in the histo is 2 * N * (N - 1), with N  = nof hits
// 3. if the couple ij was considered, then the couple ji is skipped
// 4. this method is equivalent to fill the accumulation plot with the 4 tangents
//    of each couple of hits (4 lines are tangent to both two circles)
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//
////////////////////////////////////////////////////////////

#include "PndTrkCombiLegendreTransform.h"

#include <iostream>

using namespace std;

// -----   Default constructor   -------------------------------------------
PndTrkCombiLegendreTransform::PndTrkCombiLegendreTransform()
{
  fThetaNofBin = 180;
  fThetaMin = 0;
  fThetaMax = 180;
  fRNofBin = 1000;
  fRMin = -0.07;
  fRMax = 0.07;
  fhLegendre = nullptr;
}

// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndTrkCombiLegendreTransform::~PndTrkCombiLegendreTransform() {}
// -------------------------------------------------------------------------

// ------ HISTOGRAM --------------------------------------------------------
void PndTrkCombiLegendreTransform::SetUpLegendreHisto()
{
  if (fhLegendre == nullptr)
    fhLegendre = new TH2F("fhCombiLegendre", "", fThetaNofBin, fThetaMin, fThetaMax, fRNofBin, fRMin, fRMax);
  else
    fhLegendre->SetBins(fThetaNofBin, fThetaMin, fThetaMax, fRNofBin, fRMin, fRMax);
}

void PndTrkCombiLegendreTransform::SetUpLegendreHisto(double thetaNofBin, double thetaMin, double thetaMax, double rNofBin, double rMin, double rMax)
{
  fThetaNofBin = thetaNofBin;
  fThetaMin = thetaMin;
  fThetaMax = thetaMax;
  fRNofBin = rNofBin;
  fRMin = rMin;
  fRMax = rMax;
  SetUpLegendreHisto();
}

void PndTrkCombiLegendreTransform::ResetLegendreHisto()
{
  fhLegendre->Reset();
  fhLegendre->SetStats(kFALSE);
}

void PndTrkCombiLegendreTransform::FillHisto(TH2F *histo, double x1, double y1, double r1, double x2, double y2, double r2)
{
  double theta, r;
  ComputeThetaR(x1, y1, r1, x2, y2, r2, theta, r);
  histo->Fill(theta * TMath::RadToDeg(), r);
}

void PndTrkCombiLegendreTransform::ComputeThetaR(double x1, double y1, double r1, double x2, double y2, double r2, double &theta, double &r)
{

  double dx = x1 - x2;
  double dy = y1 - y2;

  if (r1 == 0 && r2 == 0) { // two points
    theta = TMath::ATan2(-dx, dy);
    r = x1 * TMath::Cos(theta) + y1 * TMath::Sin(theta);
    //    cout << "2points "  << theta * TMath::RadToDeg() << "/ r "  << r << endl;
  } else if (r1 > 0 && r2 > 0) { // two circles
    double dr[4] = {r1 - r2, r1 + r2, -r1 - r2, -r1 + r2};
    for (int ir = 0; ir < 4; ir++) {

      double alpha = TMath::ATan2(dy, dx);
      if (dy < 0)
        alpha += (2 * TMath::Pi());

      double distance = TMath::Sqrt(dx * dx + dy * dy);
      theta = alpha + TMath::ACos(-dr[ir] / distance);

      while (theta < 0)
        theta += TMath::Pi();
      while (theta > TMath::Pi())
        theta -= TMath::Pi();

      // try all the combinations (++ +- -+ --) and...
      double rA[2], rB[2];
      rA[0] = x1 * TMath::Cos(theta) + y1 * TMath::Sin(theta) - r1;
      rA[1] = x1 * TMath::Cos(theta) + y1 * TMath::Sin(theta) + r1;
      rB[0] = x2 * TMath::Cos(theta) + y2 * TMath::Sin(theta) - r2;
      rB[1] = x2 * TMath::Cos(theta) + y2 * TMath::Sin(theta) + r2;

      // ...get the one where curve A and curve B have the same r --> they cross
      if (fabs(rA[0] - rB[0]) < 1e-10 || fabs(rA[0] - rB[1]) < 1e-10)
        r = rA[0];
      else if (fabs(rA[1] - rB[0]) < 1e-10 || fabs(rA[1] - rB[1]) < 1e-10)
        r = rA[1];

      //   cout << "2circles "  << theta * TMath::RadToDeg() << "/ r "  << r << endl;
      //   cout << "ir " << ir << " theta " << theta * TMath::RadToDeg() << "/ r "  << r << endl;
    }
  } else { // one point/one circle
    double rj;
    if (r1 > 0)
      rj = r1;
    else
      rj = r2;

    double dr[2] = {-rj, rj};
    for (int ir = 0; ir < 2; ir++) {

      double alpha = TMath::ATan2(dy, dx);
      if (dy < 0)
        alpha += (2 * TMath::Pi());

      double distance = TMath::Sqrt(dx * dx + dy * dy);
      theta = alpha + TMath::ACos(dr[ir] / distance);

      while (theta < 0)
        theta += TMath::Pi();
      while (theta > TMath::Pi())
        theta -= TMath::Pi();

      if (r1 > 0)
        r = x2 * TMath::Cos(theta) + y2 * TMath::Sin(theta);
      else
        r = x1 * TMath::Cos(theta) + y1 * TMath::Sin(theta);
      //   cout << "1pt/1cir "  << theta * TMath::RadToDeg() << "/ r "  << r << endl;

      //   cout << "ir " << ir << " theta " << theta * TMath::RadToDeg() << "/ r "  << r << endl;
    }
  }
}

void PndTrkCombiLegendreTransform::FillLegendreHisto(double x1, double y1, double radius1, double x2, double y2, double radius2)
{
  FillHisto(fhLegendre, x1, y1, radius1, x2, y2, radius2);
}

void PndTrkCombiLegendreTransform::Draw()
{
  fhLegendre->Draw("colz");
}

int PndTrkCombiLegendreTransform::ExtractLegendreMaximum(double &theta_max, double &r_max)
{
  ExtractMaximumFromHisto(fhLegendre, theta_max, r_max);
}

int PndTrkCombiLegendreTransform::ExtractMaximumFromHisto(TH2F *histo, double &theta_max, double &r_max)
{
  int bin = histo->GetMaximumBin();
  int binx, biny, binz;
  histo->GetBinXYZ(bin, binx, biny, binz);
  theta_max = histo->GetXaxis()->GetBinCenter(binx);
  r_max = histo->GetYaxis()->GetBinCenter(biny);
  return histo->GetMaximum();
}

void PndTrkCombiLegendreTransform::ExtractLegendreSingleLineParameters(double &slope, double &intercept)
{
  double theta, r;
  ExtractLegendreMaximum(theta, r);
  ExtractLineParameters(theta, r, slope, intercept);
}

void PndTrkCombiLegendreTransform::ExtractLineParameters(double theta, double r, double &slope, double &intercept)
{
  // r = x cost + y sint --> y = r/sint - x/tant

  if (theta == 0.)
    theta = 1.e-6; // CHECK

  slope = -1. / TMath::Tan(TMath::DegToRad() * theta);
  intercept = r / TMath::Sin(TMath::DegToRad() * theta);
}
ClassImp(PndTrkCombiLegendreTransform)
