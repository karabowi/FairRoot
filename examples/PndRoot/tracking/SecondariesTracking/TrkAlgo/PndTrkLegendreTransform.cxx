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
// PndTrkLegendreTransform
//
// Class to perform the Legendre Transformation
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//
////////////////////////////////////////////////////////////

#include "PndTrkLegendreTransform.h"

#include <iostream>

using namespace std;

// -----   Default constructor   -------------------------------------------
PndTrkLegendreTransform::PndTrkLegendreTransform()
{
  fThetaNofBin = 180;
  fThetaMin = 0;
  fThetaMax = 180;
  fRNofBin = 1000;
  fRMin = -0.07;
  fRMax = 0.07;

  fThetaMinZoom = 0;
  fThetaMaxZoom = 180;
  fRMinZoom = -0.07;
  fRMaxZoom = 0.07;

  fhLegendre = nullptr;
  fhLegendreZoom = nullptr;
}

// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndTrkLegendreTransform::~PndTrkLegendreTransform() {}
// -------------------------------------------------------------------------

// ------ HISTOGRAM --------------------------------------------------------
void PndTrkLegendreTransform::SetUpLegendreHisto()
{
  fhLegendre = new TH2F("fhLegendre", "", fThetaNofBin, fThetaMin, fThetaMax, fRNofBin, fRMin, fRMax);
}

void PndTrkLegendreTransform::SetUpLegendreHisto(double thetaNofBin, double thetaMin, double thetaMax, double rNofBin, double rMin, double rMax)
{
  fThetaNofBin = thetaNofBin;
  fThetaMin = thetaMin;
  fThetaMax = thetaMax;
  fRNofBin = rNofBin;
  fRMin = rMin;
  fRMax = rMax;
  if (fhLegendre) {
    fhLegendre->SetBins(fThetaNofBin, fThetaMin, fThetaMax, fRNofBin, fRMin, fRMax);
    ResetLegendreHisto();
  } else
    SetUpLegendreHisto();
}

void PndTrkLegendreTransform::ResetLegendreHisto()
{
  fhLegendre->Reset();
  fhLegendre->SetStats(kFALSE);
}

void PndTrkLegendreTransform::FillHisto(TH2F *histo, double thetamin, double thetamax, double x, double y, double radius)
{
  Double_t thetaRad = thetamin * TMath::DegToRad();
  double deltaThetaRad = TMath::DegToRad() * (thetamax - thetamin) / fThetaNofBin;

  // computation
  // double deltar = deltaThetaRad * (y * TMath::Cos(thetaRad) - x * TMath::Sin(thetaRad)); //[R.K. 01/2017] unused variable
  //  cout << "cfr delta r " << x << " " << y << " " << radius << " " << deltar << " " << (fRMax - fRMin)/fRNofBin << " " << 2 * radius << endl;

  while (thetaRad < thetamax * TMath::DegToRad()) {
    double thetaDeg = thetaRad * TMath::RadToDeg();

    // STT
    if (radius > 0.) {
      double r1 = x * TMath::Cos(thetaRad) + y * TMath::Sin(thetaRad) + radius;
      double r2 = x * TMath::Cos(thetaRad) + y * TMath::Sin(thetaRad) - radius;
      //   histo->Fill(thetaDeg, r1, radius);
      //   histo->Fill(thetaDeg, r2, radius);
      histo->Fill(thetaDeg, r1);
      histo->Fill(thetaDeg, r2);
    } else { // MVD, SCITIL, GEM
      double r = x * TMath::Cos(thetaRad) + y * TMath::Sin(thetaRad);
      histo->Fill(thetaDeg, r);
    }

    thetaRad += deltaThetaRad;
  }
}

void PndTrkLegendreTransform::FillLegendreHisto(double x, double y, double radius)
{
  FillHisto(fhLegendre, fThetaMin, fThetaMax, x, y, radius);
}

// void PndTrkLegendreTransform::ApplyThresholdLegendreHisto(double threshold) {
//   int max = fhLegendre->GetMaximum();
//   fhLegendre->GetZaxis()->SetRangeUser(max * threshold, max);
// }

int PndTrkLegendreTransform::ExtractLegendreMaximum(double &theta_max, double &r_max)
{
  return ExtractMaximumFromHisto(fhLegendre, theta_max, r_max);
}

void PndTrkLegendreTransform::ExtractLegendreMaxima(int nmaxima, std::vector<double> &theta_max, std::vector<double> &r_max, std::vector<int> &content_max)
{
  ExtractMaximaFromHisto(nmaxima, fhLegendre, theta_max, r_max, content_max);
}

void PndTrkLegendreTransform::ExtractLegendreSingleLineParameters(double &slope, double &intercept)
{
  double theta, r;
  ExtractLegendreMaximum(theta, r);
  ExtractLineParameters(theta, r, slope, intercept);
}

// ------ ZOOM -----------------------------------------------------------------
void PndTrkLegendreTransform::SetUpZoomHisto()
{
  fhLegendreZoom = new TH2F("fhLegendreZoom", "", fThetaNofBin, fThetaMinZoom, fThetaMaxZoom, fRNofBin, fRMin, fRMax);
}

void PndTrkLegendreTransform::SetUpZoomHisto(double theta, double r, double deltatheta, double deltar)
{
  if (fhLegendreZoom) {
    ResetZoomHisto();
  }

  fThetaMinZoom = theta - deltatheta;
  fThetaMaxZoom = theta + deltatheta;
  fRMinZoom = r - deltar;
  fRMaxZoom = r + deltar;

  fhLegendreZoom->GetXaxis()->SetLimits(fThetaMinZoom, fThetaMaxZoom);
  fhLegendreZoom->GetYaxis()->SetLimits(fRMinZoom, fRMaxZoom);
}

void PndTrkLegendreTransform::ResetZoomHisto()
{
  fhLegendreZoom->Reset();
  fhLegendreZoom->SetStats(kFALSE);
}

void PndTrkLegendreTransform::FillZoomHisto(double x, double y, double radius)
{
  FillHisto(fhLegendreZoom, fThetaMinZoom, fThetaMaxZoom, x, y, radius);
}

int PndTrkLegendreTransform::ExtractZoomMaximum(double &theta_max, double &r_max)
{
  return ExtractMaximumFromHisto(fhLegendreZoom, theta_max, r_max);
}

void PndTrkLegendreTransform::ExtractZoomSingleLineParameters(double &slope, double &intercept)
{
  double theta, r;
  ExtractZoomMaximum(theta, r);
  ExtractLineParameters(theta, r, slope, intercept);
}

// GET A SINGLE LINE --> THE MAXIMUM -------------------------------------------
int PndTrkLegendreTransform::ExtractMaximumFromHisto(TH2F *histo, double &theta_max, double &r_max)
{
  int bin = histo->GetMaximumBin();
  int binx, biny, binz;
  histo->GetBinXYZ(bin, binx, biny, binz);
  theta_max = histo->GetXaxis()->GetBinCenter(binx);
  r_max = histo->GetYaxis()->GetBinCenter(biny);
  return histo->GetMaximum();
  //    cout << "== THETA, R max ======== " << theta_max << " " << r_max << endl;
}

void PndTrkLegendreTransform::ExtractMaximaFromHisto(int nmaxima, TH2F *histo, std::vector<double> &theta_max, std::vector<double> &r_max, std::vector<int> &content_max)
{

  // PEAK Finder 2D
  int maxbins[nmaxima];
  for (int ibin = 0; ibin < nmaxima; ibin++)
    maxbins[ibin] = 0;

  for (int ibinx = 1; ibinx <= histo->GetNbinsX(); ibinx++) {
    for (int ibiny = 1; ibiny <= histo->GetNbinsY(); ibiny++) {

      int bincontent = histo->GetBinContent(ibinx, ibiny);
      if (bincontent == 0)
        continue;

      int ibintotal = histo->GetBin(ibinx, ibiny);

      //      cout << "current bin: " << ibintotal << " contains " << bincontent << " entries" << endl;

      for (int ibinm = nmaxima - 1; ibinm >= 0; ibinm--) {
        // cout << "ibinm " << ibinm << " " << bincontent << " " << maxbins[ibinm] << " " << histo->GetBinContent(maxbins[ibinm]) << endl;
        if (bincontent >= histo->GetBinContent(maxbins[ibinm])) {
          for (int jbinm = 0; jbinm < ibinm; jbinm++)
            maxbins[jbinm] = maxbins[jbinm + 1];
          maxbins[ibinm] = ibintotal;
          break;
        }
      }
    }
  }

  for (int ibin = 0; ibin < nmaxima; ibin++) {
    int bin = maxbins[ibin];
    int binx, biny, binz;
    histo->GetBinXYZ(bin, binx, biny, binz);
    theta_max.push_back(histo->GetXaxis()->GetBinCenter(binx));
    r_max.push_back(histo->GetYaxis()->GetBinCenter(biny));
    content_max.push_back(histo->GetBinContent(bin));
  }
}

void PndTrkLegendreTransform::ExtractLineParameters(double theta, double r, double &slope, double &intercept)
{
  // r = x cost + y sint --> y = r/sint - x/tant

  if (theta == 0.)
    theta = 1.e-6; // CHECK

  slope = -1. / TMath::Tan(TMath::DegToRad() * theta);
  intercept = r / TMath::Sin(TMath::DegToRad() * theta);
}

// -----------------------------------------------------------------------------
// DELETE A PEAK
void PndTrkLegendreTransform::DeleteZoneAroundXYLegendre(double x, double y)
{
  DeleteZoneAroundXY(fhLegendre, x, y);
}

void PndTrkLegendreTransform::DeleteZoneAroundXYZoom(double x, double y)
{
  DeleteZoneAroundXY(fhLegendreZoom, x, y);
}

void PndTrkLegendreTransform::DeleteZoneAroundXY(TH2F *histo, double x, double y)
{

  //  int bin = histo->GetMaximumBin();
  //  int bin1, bin2, bin3;
  //  histo->GetBinXYZ(bin, bin1, bin2, bin3);
  // cout << "MAX BIN BEFORE " << bin << " " << bin1 << " " << bin2 << " " << bin3 << endl;

  int binx = (int)((x - histo->GetXaxis()->GetXmin()) / histo->GetXaxis()->GetBinWidth(1)) + 1;
  int biny = (int)((y - histo->GetYaxis()->GetXmin()) / histo->GetYaxis()->GetBinWidth(1)) + 1;

  // delete the bin and a line and a row around it
  histo->SetBinContent(binx, biny, 0);
  histo->SetBinContent(binx + 1, biny, 0);
  histo->SetBinContent(binx - 1, biny, 0);
  histo->SetBinContent(binx + 1, biny + 1, 0);
  histo->SetBinContent(binx, biny + 1, 0);
  histo->SetBinContent(binx - 1, biny + 1, 0);
  histo->SetBinContent(binx + 1, biny - 1, 0);
  histo->SetBinContent(binx, biny - 1, 0);
  histo->SetBinContent(binx - 1, biny - 1, 0);

  // cout << "deleting binx " << binx << " " << biny << endl;
  //  bin = histo->GetMaximumBin();
  //  histo->GetBinXYZ(bin, bin1, bin2, bin3);
  //  cout << "MAX BIN AFTER " << bin << " " << bin1 << " " << bin2 << " " << bin3 << endl;
}

// -----------------------------------------------------------------------------

void PndTrkLegendreTransform::Draw()
{
  fhLegendre->Draw("colz");
}

void PndTrkLegendreTransform::DrawZoom()
{
  fhLegendreZoom->Draw("colz");
}

ClassImp(PndTrkLegendreTransform)
