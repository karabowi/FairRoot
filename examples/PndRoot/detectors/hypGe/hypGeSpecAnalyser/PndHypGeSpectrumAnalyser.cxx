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

/*
 * PndHypGeSpectrumAnalyser.cxx
 *
 * Copyright 2013 Marcell Steinen <steinen@kph.uni-mainz.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#include "PndHypGeSpectrumAnalyser.h"
#include "PndHypGePeakFitFunction.h"

#include "TMath.h"
#include "TROOT.h"
#include "TArrow.h"
#include "TText.h"
#include "TNtuple.h"
#include "TVirtualFitter.h"
#include "TMatrixD.h"

PndHypGeSpectrumAnalyser::PndHypGeSpectrumAnalyser(TH1D *hEnergyspec_ext, Int_t nPeaks_ext, Int_t FuncWidthExt) : PeaksPosX(0), PeaksPosY(0)
{
  fhEnergySpectrum = hEnergyspec_ext;
  nPeaks = nPeaks_ext;
  FuncWidth = FuncWidthExt;
  fSpectrum = new TSpectrum(nPeaks);
  if (CompareNuclei("eu152") == -1)
    BreakProgram = -1;
  fFittingMethod = 1;
  NoDrawingMode = false;
}

PndHypGeSpectrumAnalyser::PndHypGeSpectrumAnalyser(TH1D *hEnergySpec_ext, vector<double> *Energies_ext, Int_t FuncWidthExt) : PeaksPosX(0), PeaksPosY(0)
// constructor with energies from external array
{
  fhEnergySpectrum = hEnergySpec_ext;
  nPeaks = Energies.size();
  FuncWidth = FuncWidthExt;
  fSpectrum = new TSpectrum(nPeaks);
  fFittingMethod = 1;
  NoDrawingMode = false;
}

PndHypGeSpectrumAnalyser::PndHypGeSpectrumAnalyser(TH1D *hEnergySpec_ext, TString Nuclei, Int_t FuncWidthExt) : PeaksPosX(0), PeaksPosY(0)
// constructor with energies from internal database
{
  fhEnergySpectrum = hEnergySpec_ext;
  FuncWidth = FuncWidthExt;
  nPeaks = CompareNuclei(Nuclei);

  fSpectrum = new TSpectrum(nPeaks);
  fFittingMethod = 1;
  NoDrawingMode = false;
}

// deconstructor
PndHypGeSpectrumAnalyser::~PndHypGeSpectrumAnalyser()
{
  delete fSpectrum;
  // delete fEnergySpecCanvas;
  // delete fhEnergySpectrum;
  // delete fCalSpecCanvas;
  // delete fhCalibratedSpectrum;
  // delete fSubstractCanvas;
  // delete fhSubstractSpectrum;
  // delete PeaksPosX;
  // delete PeaksPosY;
}

void PndHypGeSpectrumAnalyser::SetEnergySpectrum(TH1D *hEnergySpec_ext)
{
  fhEnergySpectrum = hEnergySpec_ext;
}

Int_t PndHypGeSpectrumAnalyser::AnalyseSpectrum()
{
  if (nPeaks == -1 || BreakProgram == -1) {
    std::cerr << "*******Nuclei not found!" << endl;
    return -1;
  }
  FindPeaks();
  FitPeaks();
  DoEnergyCalibration();

  CalculateFWHM();
  CalculateFWTM();
  DrawCalibratedSpectrum(); // after FWHM and FWTM for TArrow s
  ExportToTextFile();
  ExportToRootFile();
  return 0;
}

Int_t PndHypGeSpectrumAnalyser::AnalyseSimulationSpectrum()
{
  if (nPeaks == -1 || BreakProgram == -1) {
    std::cerr << "*******Nuclei not found!" << endl;
    return -1;
  }
  FitPeaks();

  CalculateFWHM();
  CalculateFWTM();
  CalculateEfficiency();
  DrawSimulationSpectrum(); // after FWHM and FWTM for TArrow s
  // ExportToTextFile();
  ExportToRootFile();
  return 0;
}

Int_t PndHypGeSpectrumAnalyser::FindPeaks()
{
  fEnergySpecCanvas = new TCanvas("fEnergySpecCanvas", "Uncalibrated spectrum", 800, 600);
  fEnergySpecCanvas->cd();
  nPeaksFound = fSpectrum->Search(fhEnergySpectrum, 5, "", 0.001); // must (maybe) be adapted to spectrum; peaks found are sorted by their height (y value)
  PeaksPosX = (Float_t *)fSpectrum->GetPositionX();
  PeaksPosY = (Float_t *)fSpectrum->GetPositionY();
  std::cout << "Found " << nPeaksFound << " Peaks:" << endl;
  for (Int_t i = 0; i < nPeaksFound; i++) {
    std::cout << "Peak " << i + 1 << ": X " << PeaksPosX[i] << "\t\tY " << PeaksPosY[i] << endl;
  }
  PeakSort(); // sorts peaks by their x position to make energy calibration easier
  return 0;
}

Int_t PndHypGeSpectrumAnalyser::FitPeaks()
{
  char buf[40];
  // TVirtualFitter::SetDefaultFitter("Minuit2");
  // TVirtualFitter::SetDefaultFitter("Fumili2");
  TVirtualFitter::SetPrecision(1e-5);
  PndHypGePeakFitFunction *func = new PndHypGePeakFitFunction();
  // FitFunc = new *TF1[nPeaksFound];
  // TF1 *FitFunc[nPeaksFound];
  Int_t npar = 0;
  if (!fEnergySpecCanvas)
    fEnergySpecCanvas = new TCanvas("fEnergySpecCanvas", "Uncalibrated spectrum", 800, 600);
  fEnergySpecCanvas->cd();

  fhEnergySpectrum->GetXaxis()->SetTitle("Pulse height [a.u.]");
  fhEnergySpectrum->GetYaxis()->SetTitle("Counts");
  for (Int_t i = 0; i < nPeaksFound; i++) {

    if (fFittingMethod == 0) // simple gaussian + step + lin bg
    // par[0]: amplitude gaus, par[1]: x0; par[2]: sigma, par[3]: amplitude step, par[4]: gradient of lin bg
    {
      npar = 6;
      sprintf(buf, "FitFunc_%d", i + 1);
      FitFunc[i] = new TF1(buf, func, &PndHypGePeakFitFunction::PeakFuncGaussian, (Double_t)PeaksPosX[i] - FuncWidth, (Double_t)PeaksPosX[i] + FuncWidth, npar,
                           "PndHypGePeakFitFunction", "PeakFuncGaussian"); // width has to be adapted
    }
    if (fFittingMethod == 1) {
      npar = 8;
      sprintf(buf, "FitFunc_%d", i + 1);
      FitFunc[i] = new TF1(buf, func, &PndHypGePeakFitFunction::PeakFunc, (Double_t)PeaksPosX[i] - FuncWidth, (Double_t)PeaksPosX[i] + FuncWidth, npar, "PndHypGePeakFitFunction",
                           "PeakFunc"); // width has to be adapted
    }

    if (fFittingMethod == 2) // more parameters!!!
    {
      npar = 10;
      sprintf(buf, "FitFunc_%d", i + 1);
      // FitFunc[i] = new TF1(buf,func,&PndHypGePeakFitFunction::PeakFuncFreeSkewedPosition,(Double_t) PeaksPosX[i]-FuncWidth,(Double_t)
      // PeaksPosX[i]+FuncWidth,npar,"PndHypGePeakFitFunction","PeakFuncFreeSkewedPosition");	// width has to be adapted
      FitFunc[i] = new TF1(buf, func, &PndHypGePeakFitFunction::PeakFuncFreeSkewedPositionWithoutFirstGausian, (Double_t)PeaksPosX[i] - 2 * FuncWidth,
                           (Double_t)PeaksPosX[i] + FuncWidth, npar, "PndHypGePeakFitFunction", "PeakFuncFreeSkewedPosition"); // width has to be adapted
    }

    if (fFittingMethod == 3) // second gausian as tail!!!
    {
      npar = 9;
      sprintf(buf, "FitFunc_%d", i + 1);
      FitFunc[i] = new TF1(buf, func, &PndHypGePeakFitFunction::PeakFuncDoubleGausian, (Double_t)PeaksPosX[i] - 2 * FuncWidth, (Double_t)PeaksPosX[i] + FuncWidth, npar,
                           "PndHypGePeakFitFunction", "PeakFuncDoubleGausian"); // width has to be adapted
    }
    if (fFittingMethod == 4) // second gausian as tail!!!
    {
      npar = 8;
      sprintf(buf, "FitFunc_%d", i + 1);
      FitFunc[i] = new TF1(buf, func, &PndHypGePeakFitFunction::NewFunction, (Double_t)PeaksPosX[i] - 2 * FuncWidth, (Double_t)PeaksPosX[i] + FuncWidth, npar,
                           "PndHypGePeakFitFunction", "NewFunction"); // width has to be adapted
    }
    FitFunc[i]->SetNpx(100000);

    if (fFittingMethod != 4) {
      // setting parameter names, start values and limits
      sprintf(buf, "AmplGaus_%d", i + 1);
      FitFunc[i]->SetParName(0, buf);
      if (fFittingMethod == 2)
        FitFunc[i]->FixParameter(0, 0);
      else {
        FitFunc[i]->SetParameter(0, PeaksPosY[i] * 1.001 - fhEnergySpectrum->GetBinContent(fhEnergySpectrum->FindBin(PeaksPosX[i] - FuncWidth)));

        // if (PeaksPosY[i]>1000)			// take small peaks (bigger portion of background) into account (lower lower limit needed)
        FitFunc[i]->SetParLimits(0, PeaksPosY[i] * 0.95 - fhEnergySpectrum->GetBinContent(fhEnergySpectrum->FindBin(PeaksPosX[i] - FuncWidth)),
                                 PeaksPosY[i] * 1.02 - fhEnergySpectrum->GetBinContent(fhEnergySpectrum->FindBin(PeaksPosX[i] - FuncWidth)));
      }
      // else
      //	FitFunc[i]->SetParLimits(0,PeaksPosY[i]*0.8,PeaksPosY[i]*1.01);

      sprintf(buf, "x0_%d", i + 1);
      FitFunc[i]->SetParName(1, buf);
      FitFunc[i]->SetParameter(1, PeaksPosX[i]);
      // FitFunc[i]->SetParLimits(1,PeaksPosX[i]-1,PeaksPosX[i]+1);
      FitFunc[i]->SetParLimits(1, PeaksPosX[i] - 10, PeaksPosX[i] + 10);
      if (fFittingMethod == 2)
        FitFunc[i]->FixParameter(1, 0);

      sprintf(buf, "sigma_%d", i + 1);
      FitFunc[i]->SetParName(2, buf);
      FitFunc[i]->SetParameter(2, 1);
      Double_t UpperSigmaFitLimit = FindUpperSigmaFitLimit(PeaksPosY[i] * 0.3, PeaksPosX[i]);
      FitFunc[i]->SetParLimits(2, 0, UpperSigmaFitLimit);
      // std::cout << FindUpperSigmaFitLimit(PeaksPosY[i]*0.5, PeaksPosX[i])<< endl;
      // if (fFittingMethod == 2)
      // FitFunc[i]->FixParameter(2,0);

      sprintf(buf, "AmplSmoStep_%d", i + 1);
      FitFunc[i]->SetParName(3, buf);
      //		FitFunc[i]->SetParameter(3,fhEnergySpectrum->GetBinContent(fhEnergySpectrum->FindBin(PeaksPosX[i]-FuncWidth/2)));
      FitFunc[i]->SetParameter(3, 40);

      // FitFunc[i]->FixParameter(3,0);
      FitFunc[i]->SetParLimits(3, 0.000001, 100);
      // cout << "AAAAA" << fhEnergySpectrum->GetBinContent(fhEnergySpectrum->FindBin(PeaksPosX[i]-FuncWidth)) << endl;

      sprintf(buf, "linBg_%d", i + 1);
      FitFunc[i]->SetParName(4, buf);
      FitFunc[i]->SetParameter(4, -0.3);
      FitFunc[i]->SetParLimits(4, -10, 0);
      if (fFittingMethod == 2)
        FitFunc[i]->FixParameter(4, 0);

      sprintf(buf, "constBg_%d", i + 1);
      FitFunc[i]->SetParName(5, buf);
      // FitFunc[i]->SetParameter(5,fhEnergySpectrum->GetBinContent(fhEnergySpectrum->FindBin(PeaksPosX[i]-FuncWidth))/2);
      FitFunc[i]->SetParameter(5, 300);
      FitFunc[i]->SetParLimits(5, 200, 700);

      //	FitFunc[i]->SetParLimits(5,0,fhEnergySpectrum->GetBinContent(fhEnergySpectrum->FindBin(PeaksPosX[i]-FuncWidth))+10*PeaksPosX[i]);
      if (fFittingMethod == 1 || fFittingMethod == 2) {
        // additional parameters for this fitting function
        // par[5]: amplitude skewed gaus, par[6]: beta

        sprintf(buf, "AmplSkGaus_%d", i + 1);
        FitFunc[i]->SetParName(6, buf);
        // FitFunc[i]->SetParameter(6,PeaksPosY[i]*0.01);
        FitFunc[i]->SetParameter(6, PeaksPosY[i] * 0.66);
        // FitFunc[i]->FixParameter(6,0);

        FitFunc[i]->SetParLimits(6, PeaksPosY[i] * 0.2, PeaksPosY[i]);

        sprintf(buf, "beta_%d", i + 1);
        FitFunc[i]->SetParName(7, buf);
        FitFunc[i]->SetParameter(7, 10);
        // FitFunc[i]->FixParameter(7,1);
        FitFunc[i]->SetParLimits(7, 0.000001, 100);
      }
      if (fFittingMethod == 2) {
        // additional parameters for this fitting function
        // par[7]: x0_s, par[8]: sigma_s

        sprintf(buf, "x0SkewedGaus_%d", i + 1);
        FitFunc[i]->SetParName(8, buf);
        FitFunc[i]->SetParameter(8, PeaksPosX[i]);
        // FitFunc[i]->FixParameter(8,0);
        FitFunc[i]->SetParLimits(8, PeaksPosX[i] - 1, PeaksPosX[i] + 10);

        sprintf(buf, "sigmaSkewedGausn_%d", i + 1);
        FitFunc[i]->SetParName(9, buf);
        FitFunc[i]->SetParameter(9, 1);
        FitFunc[i]->SetParLimits(9, 0.1, UpperSigmaFitLimit);
        // FitFunc[i]->FixParameter(9,1);
      }

      if (fFittingMethod == 3) {
        // additional parameters for this fitting function
        // par[5]: amplitude second gaus, par[6]: x0_ second gaus, par[7]: sigma second gaus

        sprintf(buf, "AmplSecondGaus_%d", i + 1);
        FitFunc[i]->SetParName(6, buf);
        FitFunc[i]->SetParameter(6, PeaksPosY[i] * 0.01);
        // FitFunc[i]->FixParameter(6,0);
        if (PeaksPosY[i] * 0.02 - fhEnergySpectrum->GetBinContent(fhEnergySpectrum->FindBin(PeaksPosX[i] - FuncWidth)) > 0)
          FitFunc[i]->SetParLimits(6, PeaksPosY[i] * 0.02 - fhEnergySpectrum->GetBinContent(fhEnergySpectrum->FindBin(PeaksPosX[i] - FuncWidth)),
                                   PeaksPosY[i] * 0.2 - fhEnergySpectrum->GetBinContent(fhEnergySpectrum->FindBin(PeaksPosX[i] - FuncWidth)));
        else
          FitFunc[i]->SetParLimits(6, 0, PeaksPosY[i] * 0.2 - fhEnergySpectrum->GetBinContent(fhEnergySpectrum->FindBin(PeaksPosX[i] - FuncWidth)));

        sprintf(buf, "x0SecondGaus_%d", i + 1);
        FitFunc[i]->SetParName(7, buf);
        FitFunc[i]->SetParameter(7, PeaksPosX[i] - 2);
        // FitFunc[i]->FixParameter(7,0);
        FitFunc[i]->SetParLimits(7, PeaksPosX[i] - 4, PeaksPosX[i] - 1);

        sprintf(buf, "sigmaSecondGaus_%d", i + 1);
        FitFunc[i]->SetParName(8, buf);
        FitFunc[i]->SetParameter(8, 1);
        FitFunc[i]->SetParLimits(8, 0.1, UpperSigmaFitLimit * 5);
        // FitFunc[i]->FixParameter(8,1);
      }
    } else {
      // setting parameter names, start values and limits
      sprintf(buf, "lambda_%d", i + 1);
      FitFunc[i]->SetParName(0, buf);
      FitFunc[i]->SetParameter(0, 0.1);
      FitFunc[i]->SetParLimits(0, 0.01, 1);

      sprintf(buf, "XPosSkew_%d", i + 1);
      FitFunc[i]->SetParName(1, buf);
      FitFunc[i]->SetParameter(1, PeaksPosX[i]);
      FitFunc[i]->SetParLimits(1, PeaksPosX[i] * 0.99, PeaksPosX[i] * 1.01);

      sprintf(buf, "SigmaSkew_%d", i + 1);
      FitFunc[i]->SetParName(2, buf);
      FitFunc[i]->SetParameter(2, 5);
      FitFunc[i]->SetParLimits(2, 0.1, 50);

      sprintf(buf, "AmplitudeSkew_%d", i + 1);
      FitFunc[i]->SetParName(3, buf);
      FitFunc[i]->SetParameter(3, PeaksPosY[i] * 10);
      FitFunc[i]->SetParLimits(3, 0.1, PeaksPosY[i] * 15);

      sprintf(buf, "AmplitudeGaus_%d", i + 1);
      FitFunc[i]->SetParName(4, buf);
      FitFunc[i]->SetParameter(4, PeaksPosY[i]);
      FitFunc[i]->SetParLimits(4, PeaksPosY[i] * 0.05, PeaksPosY[i] * 2.5);

      sprintf(buf, "XPosGaus_%d", i + 1);
      FitFunc[i]->SetParName(5, buf);
      FitFunc[i]->SetParameter(5, PeaksPosX[i]);
      FitFunc[i]->SetParLimits(5, PeaksPosX[i] * 0.99, PeaksPosX[i] * 1.01);

      sprintf(buf, "SigmaGaus_%d", i + 1);
      FitFunc[i]->SetParName(6, buf);
      FitFunc[i]->SetParameter(6, 5);
      FitFunc[i]->SetParLimits(6, 0.1, 20);

      sprintf(buf, "Offset_%d", i + 1);
      FitFunc[i]->SetParName(7, buf);
      FitFunc[i]->SetParameter(7, 500);
      FitFunc[i]->SetParLimits(7, 0.1, 1000);
    }
    std::cout << "Start Fitting Peak " << i << endl;
    fEnergySpecCanvas->cd();
    if (i == 0)
      fhEnergySpectrum->Fit(FitFunc[i], "R  E");
    else
      fhEnergySpectrum->Fit(FitFunc[i], "R  E +");

    // cout << npar << endl;

    fhFitErrorhistogram[i] =
      new TH1D("h", "Fitted gaussian with .95 conf.band", 3 * FuncWidth / fhEnergySpectrum->GetBinWidth(fhEnergySpectrum->FindBin(FitFunc[i]->GetMaximumX())),
               (Double_t)PeaksPosX[i] - 2 * FuncWidth, (Double_t)PeaksPosX[i] + FuncWidth);
    (TVirtualFitter::GetFitter())->GetConfidenceIntervals(fhFitErrorhistogram[i]);
    if (i == 0) {
      TFile *bla = new TFile("bla.root", "RECREATE");
      TCanvas *c123 = new TCanvas("c123", "", 800, 600);
      fhEnergySpectrum->Draw();
      fhFitErrorhistogram[i]->SetLineColor(kGreen);
      fhFitErrorhistogram[i]->Draw("e3 same");
      c123->Write();
      fhEnergySpectrum->Write();
      fhFitErrorhistogram[i]->Write();
      bla->Close();
    }
    // TCanvas *c1233 = new TCanvas("c1233","",800,600);
    // TVirtualFitter *fitter = TVirtualFitter::GetFitter();
    // TMatrixD CovMatrix(npar,npar,fitter->GetCovarianceMatrix());
    // TMatrixD Matrix2(npar,npar,fitter->GetCovarianceMatrix());

    // CovMatrix.Print();
    // cout << "Determinant: " << CovMatrix.Determinant() << endl;

    cout << "Chi²" << FitFunc[i]->GetChisquare() << endl;
    cout << "NDF" << FitFunc[i]->GetNDF() << endl;
    cout << "Chi²_red" << FitFunc[i]->GetChisquare() / FitFunc[i]->GetNDF() << endl;
    // PeakFitX.push_back(FitFunc[i]->GetParameter(1));
    // PeakFitXError.push_back(FitFunc[i]->GetParError(1));
    if (fFittingMethod != 4) {
      PeakFitX.push_back(FitFunc[i]->GetParameter(8));
      PeakFitXError.push_back(FitFunc[i]->GetParError(8));

      FitFuncWithoutBg[i] = new TF1(*FitFunc[i]);
      FitFuncWithoutBg[i]->SetParameter(3, 0);
      FitFuncWithoutBg[i]->SetParameter(4, 0);
    } else {
      PeakFitX.push_back(FitFunc[i]->GetParameter(5));
      PeakFitXError.push_back(FitFunc[i]->GetParError(5));

      FitFuncWithoutBg[i] = new TF1(*FitFunc[i]);
      FitFuncWithoutBg[i]->SetParameter(7, 0);
    }

    // cout << "test" << endl;
    // FitFuncWithoutBg[i]->Draw("same");
    PeakCounts.push_back(FitFuncWithoutBg[i]->Integral(FitFuncWithoutBg[i]->GetXmin(), FitFuncWithoutBg[i]->GetXmax()));
    // std::cout << "Integral:\t" <<FitFuncWithoutBg[i]->Integral(FitFuncWithoutBg[i]->GetXmin(),FitFuncWithoutBg[i]->GetXmax())<< endl;
  }
  // draw single components of fit -  new loop is needed to make all of them visible (drawing must be done after all the fiting above, otherwise only components of last peak are
  // drawn (possibly due to "+" option of multiple fits???))
  if (fFittingMethod != 4)
    for (Int_t i = 0; i < nPeaksFound; i++) {
      // basic components

      sprintf(buf, "FuncGaus_%d", i + 1);
      FuncGaus[i] =
        new TF1(buf, func, &PndHypGePeakFitFunction::GausOnly, (Double_t)PeaksPosX[i] - FuncWidth, (Double_t)PeaksPosX[i] + FuncWidth, npar, "PndHypGePeakFitFunction", "GausOnly");
      FuncGaus[i]->SetParameters(FitFunc[i]->GetParameters());
      FuncGaus[i]->SetNpx(100000);
      FuncGaus[i]->SetLineColor(kGreen);
      FuncGaus[i]->Draw("SAME");

      sprintf(buf, "FuncSmoothedStep_%d", i + 1);
      FuncSmoothedStep[i] = new TF1(buf, func, &PndHypGePeakFitFunction::SmoothedStepOnly, (Double_t)PeaksPosX[i] - FuncWidth, (Double_t)PeaksPosX[i] + FuncWidth, npar,
                                    "PndHypGePeakFitFunction", "SmoothedStepOnly");
      FuncSmoothedStep[i]->SetParameters(FitFunc[i]->GetParameters());
      FuncSmoothedStep[i]->SetNpx(100000);
      FuncSmoothedStep[i]->SetLineColor(kMagenta);
      FuncSmoothedStep[i]->Draw("SAME");

      sprintf(buf, "FuncLinear_%d", i + 1);
      FuncLinear[i] = new TF1(buf, func, &PndHypGePeakFitFunction::LinearOnly, (Double_t)PeaksPosX[i] - FuncWidth, (Double_t)PeaksPosX[i] + FuncWidth, npar,
                              "PndHypGePeakFitFunction", "LinearOnly");
      FuncLinear[i]->SetParameters(FitFunc[i]->GetParameters());
      FuncLinear[i]->SetNpx(100000);
      FuncLinear[i]->SetLineColor(kYellow);
      FuncLinear[i]->Draw("SAME");

      // components depending on the fitting model

      if (fFittingMethod == 1) {
        sprintf(buf, "SkewedGausian_%d", i + 1);
        FuncTail[i] = new TF1(buf, func, &PndHypGePeakFitFunction::SkewedOnly, (Double_t)PeaksPosX[i] - FuncWidth, (Double_t)PeaksPosX[i] + FuncWidth, npar,
                              "PndHypGePeakFitFunction", "SkewedOnly");
        FuncTail[i]->SetParameters(FitFunc[i]->GetParameters());
        FuncTail[i]->SetNpx(100000);
        FuncTail[i]->SetLineColor(kBlack);
        FuncTail[i]->Draw("SAME");
      }

      if (fFittingMethod == 2) {
        sprintf(buf, "SkewedGausian_%d", i + 1);
        FuncTail[i] = new TF1(buf, func, &PndHypGePeakFitFunction::FreeSkewedOnly, (Double_t)PeaksPosX[i] - FuncWidth, (Double_t)PeaksPosX[i] + FuncWidth, npar,
                              "PndHypGePeakFitFunction", "FreeSkewedOnly");
        FuncTail[i]->SetParameters(FitFunc[i]->GetParameters());
        FuncTail[i]->SetNpx(100000);
        FuncTail[i]->SetLineColor(kBlack);
        FuncTail[i]->Draw("SAME");
      }

      if (fFittingMethod == 3) {
        sprintf(buf, "SecondGausian_%d", i + 1);
        FuncTail[i] = new TF1(buf, func, &PndHypGePeakFitFunction::SecondGausOnly, (Double_t)PeaksPosX[i] - FuncWidth, (Double_t)PeaksPosX[i] + FuncWidth, npar,
                              "PndHypGePeakFitFunction", "SecondGausOnly");
        FuncTail[i]->SetParameters(FitFunc[i]->GetParameters());
        FuncTail[i]->SetNpx(100000);
        FuncTail[i]->SetLineColor(kBlack);
        FuncTail[i]->Draw("SAME");
      }
    }

  fhSubstractSpectrum = new TH1D("fhSubstractSpectrum", "Substracted spectrum; Channel; Counts", fhEnergySpectrum->GetNbinsX(), fhEnergySpectrum->GetXaxis()->GetXmin(),
                                 fhEnergySpectrum->GetXaxis()->GetXmax());
  fSubstractCanvas = new TCanvas("fSubstractCanvas", "Substracted spectrum", 800, 600);
  fSubstractCanvas->cd();
  for (Int_t i = 0; i < nPeaksFound; i++) {
    for (Double_t iValue = (Double_t)PeaksPosX[i] - FuncWidth; iValue <= (Double_t)PeaksPosX[i] + FuncWidth; iValue += fhEnergySpectrum->GetBinWidth(100)) {
      fhSubstractSpectrum->Fill(fhEnergySpectrum->GetBinCenter(fhEnergySpectrum->FindBin(iValue)),
                                fhEnergySpectrum->GetBinContent(fhEnergySpectrum->FindBin(iValue)) -
                                  FitFunc[i]->Eval(fhEnergySpectrum->GetBinCenter(fhEnergySpectrum->FindBin(iValue))));
      fhSubstractSpectrum->SetBinError(fhEnergySpectrum->FindBin(iValue), sqrt(fhEnergySpectrum->GetBinContent(fhEnergySpectrum->FindBin(iValue))));
      // cout <<fhEnergySpectrum->GetBinContent(fhEnergySpectrum->FindBin(iValue))-FitFunc[i]->Eval(fhEnergySpectrum->GetBinCenter(fhEnergySpectrum->FindBin(iValue))) <<" "
      // <<FitFunc[i]->Eval(fhEnergySpectrum->GetBinCenter(fhEnergySpectrum->FindBin(iValue))) <<endl;
    }
  }
  fhSubstractSpectrum->Draw("E1");
  return 0;
}
Int_t PndHypGeSpectrumAnalyser::DoEnergyCalibration()
{
  std::cout << "Starting energy calibration" << endl;
  fCalCanvas = new TCanvas("fCalCanvas", "Energy Calibration", 800, 600); // all "canvas stuff" may need changes for go4
  fCalCanvas->cd();

  fgCalibration = new TGraphErrors(nPeaksFound, &(PeakFitX[0]), &(Energies[0]), &(PeakFitXError[0]), &(EnergyErrors[0]));
  fgCalibration->SetMarkerSize(2);
  fgCalibration->GetXaxis()->SetTitle("Pulse height [a.u.]");
  fgCalibration->GetYaxis()->SetTitle("#gamma energy [keV]");

  // CalFunc = new TF1("CalFunc","pol2",0,8000);
  CalFunc = new TF1("CalFunc", "pol1", 0, 8000);
  fgCalibration->Draw("AP");
  fgCalibration->Fit(CalFunc);

  return 0;
}

Int_t PndHypGeSpectrumAnalyser::CalculateFWHM()
{
  for (Int_t i = 0; i < nPeaksFound; i++) {
    Amplitude[i] = FitFunc[i]->GetMaximum() - FitFunc[i]->GetMinimum(); // Easy way to substract the background, only the lower part on the high energy side is substracted
    FWHMHeight[i] = FitFunc[i]->GetMaximum() - Amplitude[i] / 2;
    FWHMlow[i] = FitFunc[i]->GetX(FWHMHeight[i], FitFunc[i]->GetXmin(), FitFunc[i]->GetMaximumX());
    cout << FitFunc[i]->Eval(FWHMlow[i]) << "t" << fhFitErrorhistogram[i]->GetBinContent(fhFitErrorhistogram[i]->FindBin(FWHMlow[i])) << endl;
    FWHMlowLeftBorder[i] = CalculateLowerErrorLeft(FWHMHeight[i], fhFitErrorhistogram[i], FWHMlow[i]);
    FWHMlowRightBorder[i] = CalculateUpperErrorLeft(FWHMHeight[i], fhFitErrorhistogram[i], FWHMlow[i]);

    FWHMhigh[i] = FitFunc[i]->GetX(FWHMHeight[i], FitFunc[i]->GetMaximumX(), FitFunc[i]->GetXmax());

    FWHMhighLeftBorder[i] = CalculateUpperErrorRight(FWHMHeight[i], fhFitErrorhistogram[i], FWHMhigh[i]);
    FWHMhighRightBorder[i] = CalculateLowerErrorRight(FWHMHeight[i], fhFitErrorhistogram[i], FWHMhigh[i]);

    FWHMlowEnergy[i] = Calibrate(FWHMlow[i]);
    FWHMhighEnergy[i] = Calibrate(FWHMhigh[i]);

    FWHMlowLeftBorderEnergy[i] = Calibrate(FWHMlowLeftBorder[i]);
    FWHMlowRightBorderEnergy[i] = Calibrate(FWHMlowRightBorder[i]);
    FWHMhighLeftBorderEnergy[i] = Calibrate(FWHMhighLeftBorder[i]);
    FWHMhighRightBorderEnergy[i] = Calibrate(FWHMhighRightBorder[i]);

    cout << "low  " << FWHMlowEnergy[i] << "\t" << FWHMlowLeftBorderEnergy[i] << "\t" << FWHMlowRightBorderEnergy[i] << endl;
    cout << "high " << FWHMhighEnergy[i] << "\t" << FWHMhighLeftBorderEnergy[i] << "\t" << FWHMhighRightBorderEnergy[i] << endl;
    FWHM_ch.insert(std::pair<double, double>(Energies[i], FWHMhigh[i] - FWHMlow[i]));
    FWHM_en.insert(std::pair<double, double>(Energies[i], FWHMhighEnergy[i] - FWHMlowEnergy[i]));
    // std::cout << "FWHMlow" << FWHMlow << "\t\tFWHMhigh" << FWHMhigh << endl;
    // std::cout << "FWHMlowEn" << FWHMlowEnergy << "\t\tFWHMhighEn" << FWHMhighEnergy << endl;
    // std::cout << FWHMhighEnergy-FWHMlowEnergy<< endl;
  }
  // std::map<double,double>::const_iterator it=FWHM_en.begin();
  // for (it=FWHM_en.begin(); it!=FWHM_en.end(); ++it)
  // std::cout << it->first << " => " << it->second << '\n';
  return 0;
}

Int_t PndHypGeSpectrumAnalyser::CalculateFWTM()
{
  for (Int_t i = 0; i < nPeaksFound; i++) {
    Amplitude[i] = FitFunc[i]->GetMaximum() - FitFunc[i]->GetMinimum(); // Easy way to substract the background, only the lower part on the high energy side is substracted
    FWTMHeight[i] = FitFunc[i]->GetMaximum() - Amplitude[i] * 9 / 10;
    FWTMlow[i] = FitFunc[i]->GetX(FWTMHeight[i], FitFunc[i]->GetXmin(), FitFunc[i]->GetMaximumX());
    FWTMhigh[i] = FitFunc[i]->GetX(FWTMHeight[i], FitFunc[i]->GetMaximumX(), FitFunc[i]->GetXmax());
    FWTMlowEnergy[i] = Calibrate(FWTMlow[i]);
    FWTMhighEnergy[i] = Calibrate(FWTMhigh[i]);
    FWTM_ch.insert(std::pair<double, double>(Energies[i], FWTMhigh[i] - FWTMlow[i]));
    FWTM_en.insert(std::pair<double, double>(Energies[i], FWTMhighEnergy[i] - FWTMlowEnergy[i]));
    // std::cout << "FWTMlow" << FWTMlow << "\t\tFWTMhigh" << FWTMhigh << endl;
    // std::cout << "FWTMlowEn" << FWTMlowEnergy << "\t\tFWTMhighEn" << FWTMhighEnergy << endl;
    // std::cout << FWTMhighEnergy-FWTMlowEnergy<< endl;
  }
  // std::map<double,double>::const_iterator it=FWTM_en.begin();
  // for (it=FWTM_en.begin(); it!=FWTM_en.end(); ++it)
  // std::cout << it->first << " => " << it->second << '\n';

  return 0;
}

Int_t PndHypGeSpectrumAnalyser::CompareNuclei(TString NucleiName)
{
  NucleiName.ToLower();
  if (!NucleiName.CompareTo("co60")) {
    // npeaks = 2;
    std::cout << "Found Co60" << endl;
    Energies.push_back(1172.23);
    EnergyErrors.push_back(0.030);
    Energies.push_back(1332.51);
    EnergyErrors.push_back(0.015);
    return Energies.size();
  }

  if (!NucleiName.CompareTo("co60bg")) {
    // npeaks = 2;
    std::cout << "Found Co60 and K40" << endl;
    Energies.push_back(1172.23);   // keV
    EnergyErrors.push_back(0.030); // keV
    Energies.push_back(1332.51);   // keV
    EnergyErrors.push_back(0.015); // keV
    Energies.push_back(1460.83);   // keV
    EnergyErrors.push_back(0.1);   // placeholder, real value unknown
    return Energies.size();        // 3 peaks
  }

  if (!NucleiName.CompareTo("eu152")) {
    std::cout << "Found Eu152" << endl;
    Energies.push_back(121.78);  // keV
    EnergyErrors.push_back(0.1); // placeholder, real value unknown
    Energies.push_back(244.70);  // keV
    EnergyErrors.push_back(0.1); // placeholder, real value unknown
    Energies.push_back(344.28);  // keV
    EnergyErrors.push_back(0.1); // placeholder, real value unknown
    Energies.push_back(411.1);   // keV
    EnergyErrors.push_back(0.1); // placeholder, real value unknown
    Energies.push_back(444.0);   // keV
    EnergyErrors.push_back(0.1); // placeholder, real value unknown
    Energies.push_back(778.90);  // keV
    EnergyErrors.push_back(0.1); // placeholder, real value unknown
    Energies.push_back(867.4);   // keV
    EnergyErrors.push_back(0.1); // placeholder, real value unknown
    Energies.push_back(964.08);  // keV
    EnergyErrors.push_back(0.1); // placeholder, real value unknown
    Energies.push_back(1085.9);  // keV
    EnergyErrors.push_back(0.1); // placeholder, real value unknown
    Energies.push_back(1112.1);  // keV
    EnergyErrors.push_back(0.1); // placeholder, real value unknown
    Energies.push_back(1408.0);  // keV
    EnergyErrors.push_back(0.1); // placeholder, real value unknown
    return Energies.size();      // 11 peaks
  }
  if (!NucleiName.CompareTo("eu152bg")) {
    std::cout << "Found Eu152 and K40" << endl;
    return Energies.size();
  }

  if (!NucleiName.CompareTo("cs137")) {
    std::cout << "Found Cs137" << endl;
    return Energies.size();
  }

  if (!NucleiName.CompareTo("cs137bg")) {
    std::cout << "Found Cs137 and K40" << endl;
    return Energies.size();
  }

  if (!NucleiName.CompareTo("na22")) {
    std::cout << "Found Na22" << endl;
    return Energies.size();
  }

  if (!NucleiName.CompareTo("na22bg")) {
    std::cout << "Found Na22 and K40" << endl;
    return Energies.size();
  }
  if (!NucleiName.CompareTo("jülich")) {
    // npeaks = 5;
    std::cout << "Found jülich beam time configuration" << endl;
    Energies.push_back(510.998928);
    EnergyErrors.push_back(0.000011);
    Energies.push_back(1172.23);
    EnergyErrors.push_back(0.030);
    Energies.push_back(1332.51);
    EnergyErrors.push_back(0.015);
    Energies.push_back(1778.969);
    EnergyErrors.push_back(0.012);
    Energies.push_back(2504.74);
    EnergyErrors.push_back(0.1); // 0,1keV random
    return Energies.size();
  }
  if (!NucleiName.CompareTo("jülich2")) {
    // npeaks = 5;
    std::cout << "Found jülich beam time configuration" << endl;
    Energies.push_back(510.998928);
    EnergyErrors.push_back(0.000011);
    Energies.push_back(1172.23);
    EnergyErrors.push_back(0.030);
    Energies.push_back(1332.51);
    EnergyErrors.push_back(0.015);
    Energies.push_back(1778.969);
    EnergyErrors.push_back(0.012);
    return Energies.size();
  }
  // std::cerr << "****************Nuclei not found!***************" <<endl;
  return -1;
}

Double_t PndHypGeSpectrumAnalyser::FindUpperSigmaFitLimit(Double_t threshold, Float_t StartingPoint)
{
  // find fitting range of sigma. this is done by starting at "starting point" and than finding the position when the peak goes bellow a certain threshold.
  Int_t StartBin = fhEnergySpectrum->FindBin((Double_t)StartingPoint);
  // std::cout << "StartBin" << StartBin;

  for (Int_t bin = StartBin; bin <= fhEnergySpectrum->GetNbinsX(); bin++) {
    // std::cout << bin<<"\t\t\tSpecValue\t\t"<<fhEnergySpectrum->GetBinContent(bin) << endl;
    if (fhEnergySpectrum->GetBinContent(bin) < threshold) {
      // std::cout <<  fhEnergySpectrum->GetBinCenter(bin)<< "\t\t"<<StartingPoint << endl;
      return fhEnergySpectrum->GetBinCenter(bin) - StartingPoint;
    }
  }
  return -1;
}

void PndHypGeSpectrumAnalyser::PeakSort()
{
  std::cout << "Sorting Peaks" << endl;
  Float_t *arrayOrig = new Float_t[nPeaksFound];
  for (int i = 0; i < nPeaksFound; ++i)
    arrayOrig[i] = PeaksPosX[i];
  std::sort(PeaksPosX, PeaksPosX + nPeaksFound); // sorting Peak Positions (lower to greater x)

  Float_t buffy[nPeaksFound];
  for (Int_t i = 0; i < nPeaksFound; ++i) // placing Amplitude to the coresponding Peak Position
  {
    for (Int_t j = 0; j < nPeaksFound; ++j) {
      if (arrayOrig[i] == PeaksPosX[j]) {
        buffy[j] = PeaksPosY[i];
      }
    }
  }
  for (int i = 0; i < nPeaksFound; ++i) {
    PeaksPosY[i] = buffy[i];
  }
  for (int i = 0; i < nPeaksFound; ++i)
    std::cout << "(" << PeaksPosX[i] << "," << PeaksPosY[i] << ") ";
  std::cout << endl;
}

Int_t PndHypGeSpectrumAnalyser::DrawCalibratedSpectrum()
{
  std::cout << "Drawing of CalibratedSpectrum" << endl;
  fCalSpecCanvas = new TCanvas("fCalSpecCanvas", "Calibrated spectrum", 800, 600); // all "canvas stuff" may need changes for go4
  fCalSpecCanvas->cd();
  fhEnergySpectrum->GetXaxis()->UnZoom();
  // fhCalibratedSpectrum = new TH1D("fhCalibratedSpectrum","Calibrated spectrum;Energy [keV];Counts
  // [a.u.]",fhEnergySpectrum->GetNbinsX(),Calibrate(fhEnergySpectrum->GetXaxis()->GetBinCenter(fhEnergySpectrum->GetXaxis()->GetFirst())),Calibrate(fhEnergySpectrum->GetXaxis()->GetBinCenter(fhEnergySpectrum->GetXaxis()->GetLast())));

  // create new bins for calibrated spectrum. These bins have variable bin size due to the non-linear energy calibration
  const int nbins = fhEnergySpectrum->GetNbinsX();
  double new_bins[nbins + 1];
  for (int i = 0; i <= nbins; i++) {
    new_bins[i] = Calibrate(fhEnergySpectrum->GetBinLowEdge(i + 1));
    // std::cout << new_bins[i]<<endl;
  }
  fhCalibratedSpectrum = new TH1D("fhCalibratedSpectrum", "Calibrated spectrum;Energy [keV];Counts [a.u.]", fhEnergySpectrum->GetNbinsX(), new_bins);
  for (Int_t i = 0; i <= fhEnergySpectrum->GetNbinsX(); i++) {
    fhCalibratedSpectrum->Fill(Calibrate(fhEnergySpectrum->GetXaxis()->GetBinCenter(i)), fhEnergySpectrum->GetBinContent(i));
  }
  fhCalibratedSpectrum->SetStats(0);
  fhCalibratedSpectrum->Draw();

  std::map<double, double>::const_iterator it = FWHM_en.begin();
  std::map<double, double>::const_iterator it2 = FWTM_en.begin();
  for (Int_t i = 0; i < nPeaksFound; i++) {

    CalibratedFunction[i] = new TF1(*FitFunc[i]);
    // adapt parameters to energy spectrum
    Double_t OldSigma = CalibratedFunction[i]->GetParameter(2);

    if (fFittingMethod != 4) {
      // adapt sigmas to new x-axis
      CalibratedFunction[i]->SetParameter(2, Calibrate(CalibratedFunction[i]->GetParameter(1)) -
                                               Calibrate(CalibratedFunction[i]->GetParameter(1) - CalibratedFunction[i]->GetParameter(2)));
      CalibratedFunction[i]->SetParameter(8, Calibrate(CalibratedFunction[i]->GetParameter(7)) -
                                               Calibrate(CalibratedFunction[i]->GetParameter(7) - CalibratedFunction[i]->GetParameter(8)));
      // adapt x0's to nes x-axis
      CalibratedFunction[i]->SetParameter(1, Calibrate(CalibratedFunction[i]->GetParameter(1)));
      CalibratedFunction[i]->SetParameter(7, Calibrate(CalibratedFunction[i]->GetParameter(7)));
      // not necessary atm!!!!!  --- adapt amplitudes, this has to be done, because of the par[2] (sigma) in the denominator in front of the gausian part. Since the gausian ampl.
      // (par[0]) is in the denominator of any other part their ampl. must be adapted too. The lin. bg gradient must not be adapted, because the factors from the change of x and
      // par[0] cancel each other
      // CalibratedFunction[i]->SetParameter(0,CalibratedFunction[i]->GetParameter(0)*CalibratedFunction[i]->GetParameter(2)/OldSigma);
      // std::cout << "new ampl" << CalibratedFunction[i]->GetParameter(0) << endl;
      // CalibratedFunction[i]->SetParameter(3,CalibratedFunction[i]->GetParameter(3)*CalibratedFunction[i]->GetParameter(2)/OldSigma);
      // CalibratedFunction[i]->SetParameter(5,CalibratedFunction[i]->GetParameter(5)*CalibratedFunction[i]->GetParameter(2)/OldSigma);
      // function range must be adapted

      // pol1 bg must be adapted to new y-axis, ATTENTION!!! This version is only viable for pol1 CalFunc!!!!!!!!!
      CalibratedFunction[i]->SetParameter(5, CalibratedFunction[i]->GetParameter(5) - CalibratedFunction[i]->GetParameter(4) * CalFunc->GetParameter(0));
      CalibratedFunction[i]->SetParameter(4, CalibratedFunction[i]->GetParameter(4) / CalFunc->GetParameter(1));
    } else {
      // after lunch
      // sigmagaus and sigmaskew
      CalibratedFunction[i]->SetParameter(2, Calibrate(CalibratedFunction[i]->GetParameter(1)) -
                                               Calibrate(CalibratedFunction[i]->GetParameter(1) - CalibratedFunction[i]->GetParameter(2)));
      CalibratedFunction[i]->SetParameter(6, Calibrate(CalibratedFunction[i]->GetParameter(5)) -
                                               Calibrate(CalibratedFunction[i]->GetParameter(5) - CalibratedFunction[i]->GetParameter(6)));

      // X0gaus and X0skew
      CalibratedFunction[i]->SetParameter(1, Calibrate(CalibratedFunction[i]->GetParameter(1)));
      CalibratedFunction[i]->SetParameter(5, Calibrate(CalibratedFunction[i]->GetParameter(5)));

      // lambda skew
      CalibratedFunction[i]->SetParameter(0, CalibratedFunction[i]->GetParameter(0) * OldSigma / CalibratedFunction[i]->GetParameter(2));

      // amplitude skew
      CalibratedFunction[i]->SetParameter(3, CalibratedFunction[i]->GetParameter(3) / OldSigma * CalibratedFunction[i]->GetParameter(2));
    }
    CalibratedFunction[i]->SetRange(Calibrate(CalibratedFunction[i]->GetXmin()), Calibrate(CalibratedFunction[i]->GetXmax()));
    // CalibratedFunction[i]->Print();
    CalibratedFunction[i]->Draw("same");
    // FitFunc[i]->Print();
    TArrow *FWHMArrow = new TArrow(FWHMlowEnergy[i], FWHMHeight[i], FWHMhighEnergy[i], FWHMHeight[i], 0.02, "<|>");
    FWHMArrow->Draw("");
    TArrow *FWTMArrow = new TArrow(FWTMlowEnergy[i], FWTMHeight[i], FWTMhighEnergy[i], FWTMHeight[i], 0.02, "<|>");
    FWTMArrow->Draw("");
    char TextBuffer[50];
    sprintf(TextBuffer, "%.2f keV", it->second);
    // TextBuffer = "test";
    TText *FWHMText = new TText(CalibratedFunction[i]->GetParameter(1), FWHMHeight[i] * 1.05, TextBuffer);
    FWHMText->SetTextAlign(22);
    FWHMText->SetTextFont(63);
    FWHMText->SetTextSizePixels(22);
    FWHMText->Draw();
    sprintf(TextBuffer, "%.2f keV", it2->second);
    TText *FWTMText = new TText(CalibratedFunction[i]->GetParameter(1), FWTMHeight[i] + FWHMHeight[i] * 0.05, TextBuffer);
    FWTMText->SetTextAlign(22);
    FWTMText->SetTextFont(63);
    FWTMText->SetTextSizePixels(22);
    FWTMText->Draw();
    it++;
    it2++;
  }
  return 0;
}

Int_t PndHypGeSpectrumAnalyser::DrawSimulationSpectrum() {}

Double_t PndHypGeSpectrumAnalyser::Calibrate(Double_t Channel)
{
  Double_t a = CalFunc->GetParameter(0);
  Double_t b = CalFunc->GetParameter(1);
  Double_t c = CalFunc->GetParameter(2);
  return a + b * Channel + c * Channel * Channel;
}

Int_t PndHypGeSpectrumAnalyser::ExportToTextFile(TString TxtFilename_ext)
{
  TString TxtFilenameWithPath;
  if (OutputPath)
    TxtFilenameWithPath = OutputPath;
  if (!TxtFilename)
    TxtFilename = TxtFilename_ext;
  TxtFilenameWithPath += TxtFilename;
  TxtFile.open(TxtFilenameWithPath.Data());
  TxtFile << "File:\t" << fhEnergySpectrum->GetTitle() << endl;
  TxtFile << "Nuclei:\t" << endl;
  TxtFile << "Energy [keV]\t\tFWHM [keV]\t\tFWTM [keV]\t\tFWTM/FWHM\t\tPeakCounts" << endl;

  std::map<double, double>::const_iterator it = FWHM_en.begin();
  std::map<double, double>::const_iterator it2 = FWTM_en.begin();
  Int_t j = 0;
  for (it = FWHM_en.begin(); it != FWHM_en.end(); ++it) {
    TxtFile << it->first << "\t\t" << it->second << "\t\t" << it2->second << "\t\t" << it2->second / it->second << "\t\t" << PeakCounts[j] << endl;
    it2++;
    j++;
  }
  TxtFile.close();
  return 0;
}
Int_t PndHypGeSpectrumAnalyser::ExportToRootFile(TString RootFilename_ext)
{
  TString RootFilenameWithPath;
  if (OutputPath)
    RootFilenameWithPath = OutputPath;
  if (!RootFilename)
    RootFilename += RootFilename_ext;
  RootFilenameWithPath += RootFilename;

  RootFile = (TFile *)gROOT->FindObject(RootFilenameWithPath);
  if (RootFile)
    RootFile->Close();
  RootFile = new TFile(RootFilenameWithPath, "RECREATE", RootFilenameWithPath);

  TNtuple *DataNTuple = new TNtuple("DataNTuple", "Data ntuple", "PeakNumber:Energy:FWHM:FWTM:FWTMtoFWHMratio:PeakCounts");
  Double_t wEnergy, wFWHM, wFWTM, wRatio, wPeakCounts; // w means "write"
  Int_t wPeakNumber;
  std::map<double, double>::const_iterator it = FWHM_en.begin();
  std::map<double, double>::const_iterator it2 = FWTM_en.begin();
  for (Int_t i = 0; i < nPeaksFound; i++) {
    wPeakNumber = i;
    wEnergy = it->first;
    wFWHM = it->second;
    wFWTM = it2->second;
    wRatio = it2->second / it->second;
    wPeakCounts = PeakCounts[i];
    DataNTuple->Fill(wPeakNumber, wEnergy, wFWHM, wFWTM, wRatio, wPeakCounts);
    it++;
    it2++;
  }
  DataNTuple->Write();
  FitFunc[2]->Write();
  fhEnergySpectrum->Write();
  fCalCanvas->Write();
  fCalSpecCanvas->Write();
  RootFile->Close();
  return 0;
}

void PndHypGeSpectrumAnalyser::SetSearchRange(Double_t RangeMin, Double_t RangeMax)
{
  fhEnergySpectrum->GetXaxis()->SetRangeUser(RangeMin, RangeMax);
}

void PndHypGeSpectrumAnalyser::SetTxtFileOutputName(TString TxtFilename_ext)
{
  TxtFilename = TxtFilename_ext;
}
void PndHypGeSpectrumAnalyser::SetRootFileOutputName(TString RootFilename_ext)
{
  RootFilename = RootFilename_ext;
}

void PndHypGeSpectrumAnalyser::SetOutputPath(TString OutputPath_ext)
{
  OutputPath = OutputPath_ext;
}

void PndHypGeSpectrumAnalyser::SetGaussianFitting()
{
  fFittingMethod = 0;
}

Bool_t PndHypGeSpectrumAnalyser::IsGaussianFitting()
{
  if (fFittingMethod == 0)
    return 1;
  else
    return 0;
}

void PndHypGeSpectrumAnalyser::SetFreeSkewedFitting()
{
  fFittingMethod = 2;
}

Bool_t PndHypGeSpectrumAnalyser::IsFreeSkewedFitting()
{
  if (fFittingMethod == 2)
    return 1;
  else
    return 0;
}

void PndHypGeSpectrumAnalyser::SetSecondGausianFitting()
{
  fFittingMethod = 3;
}

Bool_t PndHypGeSpectrumAnalyser::IsSecondGausianFitting()
{
  if (fFittingMethod == 3)
    return 1;
  else
    return 0;
}

void PndHypGeSpectrumAnalyser::SetNewFunctionFitting()
{
  fFittingMethod = 4;
}

Bool_t PndHypGeSpectrumAnalyser::IsNewFunctionFitting()
{
  if (fFittingMethod == 4)
    return 1;
  else
    return 0;
}
Double_t PndHypGeSpectrumAnalyser::GetFWHM511()
{
  return FWHM_en[510.998928];
}
Double_t PndHypGeSpectrumAnalyser::GetFWTM511()
{
  return FWTM_en[510.998928];
}

Double_t PndHypGeSpectrumAnalyser::GetFWHMCo1()
{
  return FWHM_en[1172.23];
}
Double_t PndHypGeSpectrumAnalyser::GetFWTMCo1()
{
  return FWTM_en[1172.23];
}

Double_t PndHypGeSpectrumAnalyser::GetFWHMCo()
{
  return FWHM_en[1332.51];
}
Double_t PndHypGeSpectrumAnalyser::GetFWTMCo()
{
  return FWTM_en[1332.51];
}
Double_t PndHypGeSpectrumAnalyser::GetFWHMAl()
{
  return FWHM_en[1778.969];
}
Double_t PndHypGeSpectrumAnalyser::GetFWTMAl()
{
  return FWTM_en[1778.969];
}
void PndHypGeSpectrumAnalyser::SetNoDrawingMode(Bool_t NoDrawingMode_ext)
{
  NoDrawingMode = NoDrawingMode_ext;
}
Bool_t PndHypGeSpectrumAnalyser::IsNoDrawingMode()
{
  return NoDrawingMode;
}

Double_t PndHypGeSpectrumAnalyser::GetCountsCo()
{
  return 0; // NIY
}

Double_t PndHypGeSpectrumAnalyser::CalculateLowerErrorLeft(Double_t threshold, TH1D *histo, Double_t StartingValue)
{
  Int_t i = 0;
  Double_t LowerErrorValue;
  while (threshold < histo->GetBinContent(histo->FindBin(StartingValue) - i) + histo->GetBinErrorUp(histo->FindBin(StartingValue) - i)) {
    cout << "t" << threshold << endl;
    cout << histo->GetBinContent(histo->FindBin(StartingValue)) << endl;
    i++;
    if (i == 100)
      break;
    LowerErrorValue = histo->GetBinCenter(histo->FindBin(StartingValue) - (i - 1));
    cout << "bla " << LowerErrorValue << endl;
  }
  LowerErrorValue = histo->GetBinCenter(histo->FindBin(StartingValue) - (i - 1));
  return LowerErrorValue;
}

Double_t PndHypGeSpectrumAnalyser::CalculateUpperErrorLeft(Double_t threshold, TH1D *histo, Double_t StartingValue)
{
  Int_t i = 0;
  Double_t UpperErrorValue;
  while (threshold > histo->GetBinContent(histo->FindBin(StartingValue) + i) - histo->GetBinErrorLow(histo->FindBin(StartingValue) + i)) {
    i++;
    if (i == 100)
      break;
    // cout << "i" << i << endl;
  }
  UpperErrorValue = histo->GetBinCenter(histo->FindBin(StartingValue) + (i - 1));
  return UpperErrorValue;
}
Double_t PndHypGeSpectrumAnalyser::CalculateLowerErrorRight(Double_t threshold, TH1D *histo, Double_t StartingValue)
{
  Int_t i = 0;
  Double_t LowerErrorValue;
  while (threshold > histo->GetBinContent(histo->FindBin(StartingValue) - i) - histo->GetBinErrorLow(histo->FindBin(StartingValue) - i)) {
    i++;
    if (i == 100)
      break;
  }
  LowerErrorValue = histo->GetBinCenter(histo->FindBin(StartingValue) - (i - 1));
  return LowerErrorValue;
}

Double_t PndHypGeSpectrumAnalyser::CalculateUpperErrorRight(Double_t threshold, TH1D *histo, Double_t StartingValue)
{
  Int_t i = 0;
  Double_t UpperErrorValue;
  while (threshold < histo->GetBinContent(histo->FindBin(StartingValue) + i) + histo->GetBinErrorUp(histo->FindBin(StartingValue) + i)) {
    i++;
    if (i == 100)
      break;
  }
  UpperErrorValue = histo->GetBinCenter(histo->FindBin(StartingValue) + (i - 1));
  return UpperErrorValue;
}

Double_t PndHypGeSpectrumAnalyser::CalculateEfficiency()
{
  Double_t CountsInPeak = FitFunc[0]->GetParameter(0) / (TMath::Sqrt(2 * TMath::Pi()) * FitFunc[0]->GetParameter(2)) / fInputHistogramResolution;
  fFEPEfficiency = CountsInPeak / fNumberOfSimEvents / fSolidAngleCorrectionFactor;
  fFEPEfficiencyError = TMath::Sqrt(CountsInPeak) / fNumberOfSimEvents / fSolidAngleCorrectionFactor;
  return fFEPEfficiency;
}

void PndHypGeSpectrumAnalyser::SetNumberOfSimEvents(Int_t NumberOfSimEvents_ext, Bool_t OnlyTwoPiSimulated)
{
  fNumberOfSimEvents = NumberOfSimEvents_ext;
  if (OnlyTwoPiSimulated)
    fSolidAngleCorrectionFactor = 2;
  else
    fSolidAngleCorrectionFactor = 1;
}
void PndHypGeSpectrumAnalyser::SetInputHistogramResolution(Double_t InputHistogramResolution_ext)
{
  fInputHistogramResolution = InputHistogramResolution_ext;
}
Double_t PndHypGeSpectrumAnalyser::GetEfficiency()
{
  return fFEPEfficiency;
}

Double_t PndHypGeSpectrumAnalyser::GetEfficiencyError()
{
  return fFEPEfficiencyError;
}
