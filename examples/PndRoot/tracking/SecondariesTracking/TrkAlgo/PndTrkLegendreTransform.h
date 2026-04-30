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

#ifndef PNDTRKLEGENDRETRANSFORM_H
#define PNDTRKLEGENDRETRANSFORM_H

#include "TMath.h"
#include "TH2F.h"

class PndTrkLegendreTransform : public TObject {

 public:
  PndTrkLegendreTransform();
  ~PndTrkLegendreTransform();

  void SetUpLegendreHisto();
  void SetUpLegendreHisto(double thetaNofBin, double thetaMin, double thetaMax, double rNofBin, double rMin, double rMax);
  void SetUpZoomHisto(double theta, double r, double deltatheta, double deltar);
  void SetUpZoomHisto();

  void ResetLegendreHisto();
  void ResetZoomHisto();

  void FillHisto(TH2F *histo, double thetamin, double thetamax, double x, double y, double radius);
  void FillLegendreHisto(double x, double y, double radius);
  void FillZoomHisto(double x, double y, double radius);

  /*   void SetDeltaThetaSteppingRad(double deltathetarad) { fDeltaThetaRad = deltathetarad; } */
  /*   void SetDeltaThetaSteppingDeg(double deltathetadeg) { fDeltaThetaRad = TMath::DegToRad() * deltathetadeg;  } */
  void ApplyThresholdLegendreHisto(double threshold);

  int ExtractLegendreMaximum(double &theta_max, double &r_max);
  void ExtractLegendreMaxima(int nmaxima, std::vector<double> &theta_max, std::vector<double> &r_max, std::vector<int> &content_max);
  int ExtractZoomMaximum(double &theta_max, double &r_max);
  int ExtractMaximumFromHisto(TH2F *histo, double &theta_max, double &r_max);
  void ExtractMaximaFromHisto(int nmaxima, TH2F *histo, std::vector<double> &theta_max, std::vector<double> &r_max, std::vector<int> &content_max);

  void ExtractLineParameters(double theta, double r, double &slope, double &intercept);
  void ExtractLegendreSingleLineParameters(double &slope, double &intercept);
  void ExtractZoomSingleLineParameters(double &slope, double &intercept);

  void DeleteZoneAroundXYLegendre(double x, double y);
  void DeleteZoneAroundXYZoom(double x, double y);
  void DeleteZoneAroundXY(TH2F *histo, double x, double y);

  void Draw();
  void DrawZoom();

 private:
  Double_t fThetaNofBin, fThetaMin, fThetaMax, fRNofBin, fRMin, fRMax; //, fDeltaThetaRad;
  Double_t fThetaMinZoom, fThetaMaxZoom, fRMinZoom, fRMaxZoom;
  TH2F *fhLegendre, *fhLegendreZoom;
  ClassDef(PndTrkLegendreTransform, 1)
};

#endif
