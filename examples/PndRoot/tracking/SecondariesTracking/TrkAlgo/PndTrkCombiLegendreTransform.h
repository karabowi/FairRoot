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

#ifndef PNDTRKCOMBILEGENDRETRANSFORM_H
#define PNDTRKCOMBILEGENDRETRANSFORM_H

#include "TMath.h"
#include "TH2F.h"

class PndTrkCombiLegendreTransform : public TObject {

 public:
  PndTrkCombiLegendreTransform();
  ~PndTrkCombiLegendreTransform();

  void SetUpLegendreHisto();
  void SetUpLegendreHisto(double thetaNofBin, double thetaMin, double thetaMax, double rNofBin, double rMin, double rMax);
  void ResetLegendreHisto();
  TH2F *GetLegendreHisto() { return fhLegendre; }
  void FillLegendreHisto(double x1, double y1, double radius1, double x2, double y2, double radius2);
  void FillHisto(TH2F *histo, double x1, double y1, double r1, double x2, double y2, double r2);
  void ComputeThetaR(double x1, double y1, double r1, double x2, double y2, double r2, double &theta, double &r);

  int ExtractLegendreMaximum(double &theta_max, double &r_max);
  int ExtractMaximumFromHisto(TH2F *histo, double &theta_max, double &r_max);
  void ExtractLegendreSingleLineParameters(double &slope, double &intercept);
  void ExtractLineParameters(double theta, double r, double &slope, double &intercept);

  void Draw();

 private:
  Double_t fThetaNofBin, fThetaMin, fThetaMax, fRNofBin, fRMin, fRMax; //, fDeltaThetaRad;
  TH2F *fhLegendre;
  ClassDef(PndTrkCombiLegendreTransform, 1)
};

#endif
