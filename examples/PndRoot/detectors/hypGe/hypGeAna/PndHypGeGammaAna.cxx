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

/******************************************************

Analysis Task created by M.Steinen steinen@kph.uni-mainz.de
Analysis of Gamma Simulation with hypGe detectors
*******************************************************/

#include "PndHypGeGammaAna.h"
#include <iostream>
#include <TMath.h>
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"
#include <time.h>
#include <TCanvas.h>
#include <TSpectrum.h>

//#include "RhoBase/TFactory.h"

using namespace std;

PndHypGeGammaAna::PndHypGeGammaAna() : FairTask("Panda HypGeGammaAna	 Task"), xpeaks(0), ypeaks(0), fHitGe(0), fMCGam(0), fHitSi(0), fHitTargetOther(0)
{
  DoComptonCalculations = 0;
  fTargetAvailable = 0;
}
PndHypGeGammaAna::PndHypGeGammaAna(TString TxtFileName_Ext, Double_t GammaEnergy_Ext, Int_t nEvents, Int_t nPeaks_Ext, Int_t PeakToLook_Ext)
  : FairTask("Panda HypGeGammaAna	 Task"), xpeaks(0), ypeaks(0), fHitGe(0), fMCGam(0), fHitSi(0), fHitTargetOther(0)
{
  TxtFileName = TxtFileName_Ext;
  GammaEnergy = GammaEnergy_Ext;
  NumberOfEvents = nEvents;
  nPeaks = nPeaks_Ext;
  PeakToLook = PeakToLook_Ext;
  DoComptonCalculations = 0;
  fTargetAvailable = 0;
  useOmegaQuadrupolMode = 0;
  PeakWidtchStrechFactor = 1;
}

PndHypGeGammaAna::~PndHypGeGammaAna()
{
  // delete fStorage;
  delete hGamEnergy;
  delete hNumberOfHits;
  delete hAbsorption;
  delete Rng;
  delete xpeaks;
  delete ypeaks;
  delete fHitGe;
  delete fMCGam;
  delete fHitSi;
  delete fHitTargetOther;

  if (fMcTr) {
    fMcTr->Delete();
    delete fMcTr;
  }
  if (fMc) {
    fMc->Delete();
    delete fMc;
  }
  if (fGe) {
    fGe->Delete();
    delete fGe;
  }
  if (fGeAl) {
    fGeAl->Delete();
    delete fGeAl;
  }
  if (fSilicon) {
    fSilicon->Delete();
    delete fSilicon;
  }
  if (fTargetOther) {
    fTargetOther->Delete();
    delete fTargetOther;
  }
}

InitStatus PndHypGeGammaAna::Init()
{
  cout << "Hyp Ge Gamma Ana:\t"
       << "Starting Init of HypGe Gamma Ana" << endl;

  //----------get FairRootManager-------------------------------------
  ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndEmcHitProducer::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  if (fVerbose == 3)
    cout << "Hyp Ge Gamma Ana:\t"
         << "Getting FairRootManager finished" << endl;

  //----------get input arrays---------------------------------------------
  fMcTr = (TClonesArray *)ioman->GetObject("MCTrack");
  fGe = (TClonesArray *)ioman->GetObject("HypGePoint");
  fGeAl = (TClonesArray *)ioman->GetObject("HypGeAlPoint");
  fSilicon = (TClonesArray *)ioman->GetObject("HypPoint");
  fTargetOther = (TClonesArray *)ioman->GetObject("HypSTMatBudPoint");

  if (fVerbose == 3)
    cout << "Hyp Ge Gamma Ana:\t"
         << "Getting input arrays finished" << endl;

  //-----------extracting file name--------------------------------------
  fName = ioman->GetInFile()->GetName();
  if (fVerbose == 3) {
    cout << "Hyp Ge Gamma Ana:\t"
         << "Full input file name with path: " << fName << endl;
    cout << "Hyp Ge Gamma Ana:\t"
         << "Chopping first " << fName.Last('/') + 1 << " chars" << endl;
  }
  fName.Remove(0, fName.Last('/') + 1);
  if (fVerbose == 3)
    cout << "Hyp Ge Gamma Ana:\t"
         << "File name: " << fName << endl;

  if (fVerbose == 3)
    cout << "Hyp Ge Gamma Ana:\t"
         << "File name extraction finished" << endl;

  //----------create histograms------------------------------------------
  if (useOmegaQuadrupolMode) {
    iHistoUpperLimit = 0.6;
  } else {
    iHistoUpperLimit = GammaEnergy * 1.2;
  }
  hGamEnergy = new TH1D("GamEnergy", "Total gamma energy deposit, " + fName, 10000 * iHistoUpperLimit, 0.0, iHistoUpperLimit);
  hGamEnergyTargetBackground = new TH1D("hGamEnergyTargetBackground", "gamma background due to reactions in target, " + fName, 10000 * iHistoUpperLimit, 0.0, iHistoUpperLimit);
  hNumberOfHits = new TH1D("NumberOfHits", "Number of Hits, " + fName, 16, 0, 15);
  hThetaCheck = new TH1D("hThetaCheck", "Theta Check, " + fName, 90, 90, 180);
  hThetaCheckPrimary = new TH1D("hThetaCheckPrimary", "Theta Check only primaries, " + fName, 90, 90, 180);

  hAbsorption = new TH1D("hAbsorption", "Part of Gamma interaction in target system," + fName, 10, 1, 11);
  hAbsorption->GetXaxis()->SetBinLabel(1, "Silicon");      // gamma interaction in si detectors
  hAbsorption->GetXaxis()->SetBinLabel(2, "Absorber");     // gamma interaction in absorber
  hAbsorption->GetXaxis()->SetBinLabel(3, "HoldingFrame"); // gamma interaction in holding frame
  hAbsorption->GetXaxis()->SetBinLabel(4, "Window foil");  // gamma interaction in window foil
  hAbsorption->GetXaxis()->SetBinLabel(5, "beam pipe");    // gamma interaction in beam pipe
  hAbsorption->GetXaxis()->SetBinLabel(6, "Other");        // gamma interaction in other parts
  h2AbsorptionDistanceAngle = new TH2D("h2AbsorptionDistanceAngle", "Correlation of absorption distance and angle," + fName, 45, 90, 180, 100, 0, 50);

  if (fVerbose == 3)
    cout << "Hyp Ge Gamma Ana:\t"
         << "Histogram creation finished" << endl;

  //----------set detector resolution---------------------------------------------
  // TO DO: more realistic resolution (smearing) modell 	M.Steinen
  ResConstPar = 1.40872e-07;
  ResGradientPar = 4.16193e-07;
  PeakWidth = CalculatePeakWidth(GammaEnergy);
  //----------initialize EvtCount----------------------------------------
  EvtCount = 0;

  //----------initialize Compton angles-----------------------
  if (DoComptonCalculations) {
    lowComptonAngle = invCompton(0.001332, 0.001040);
    highComptonAngle = invCompton(0.001332, 0.001096);
  }

  //----------Get number of events from FairRootManager-----------------------

  if (NumberOfEvents == 0)
    NumberOfEvents = ioman->GetInChain()->GetEntries();
  cout << "Hyp Ge Gamma Ana:\tNumber of analyzed events: " << NumberOfEvents << endl;

  //----------initialize random generator------------------------------------
  Rng = gRandom;
  cout << "Hyp Ge Gamma Ana:\t"
       << "Init of HypGe Gamma Ana finished successfully" << endl;

  //----------create storage in root file------------------------------------
  // fStorage = new PndHypGeGammaAnaStorage("AnaData","Analysis Data");
  //	fStorage-> SetAllEntries(NumberOfEvents);
  // fStorage-> SetInFile((ioman->GetInFile()->GetName()).Data());			// this line doen't work!!!!! why?!?!?!?!?!??!?!
  //----------open txt outputfile--------------------------------------------

  TxtFile.open(TxtFileName);
  TxtFile << "Hyp Ge Gamma Ana:\tFile to analyze: \t" << ioman->GetInFile()->GetName() << endl;
  TxtFile << "Hyp Ge Gamma Ana:\tNumber of analyzed events: \t" << NumberOfEvents << endl;

  LOG(info) << " gGeoManager = " << gGeoManager;
  return kSUCCESS;
}

void PndHypGeGammaAna::SetParContainers()
{
  // Get Base Container
  FairRun *ana = FairRun::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  LOG(info) << " rtdb = " << rtdb;
}

void PndHypGeGammaAna::Exec(Option_t *)
{
  Double_t EnergyPerCrystal[48] = {0};
  Double_t CollectedEnergy = 0;
  SetOfCrystalHit.clear();
  bool isFirstInteraction = 0;
  if (fTargetAvailable) {
    if (fSilicon->GetEntriesFast() || fTargetOther->GetEntriesFast()) {
      bool isEndl = 0;
      fMCGamPrimary = (PndMCTrack *)fMcTr->At(0);
      TVector3 PrimaryVertex = fMCGamPrimary->GetStartVertex();
      TVector3 PrimaryMomentum = fMCGamPrimary->GetMomentum();
      Double_t PrimaryDistance;
      Double_t PrimaryTheta;
      //			if (fSilicon->GetEntriesFast())
      //			{
      //				fHitSi = (PndHypPoint*)fSilicon->At(0);
      //				fMCGam = (PndMCTrack*)fMcTr->At(fHitSi->GetTrackID());
      //				if (fHitSi->GetTrackID() == 1 && fMCGam->GetMotherID() <1)
      //				{
      //					isFirstInteraction = 1;
      //					//cout << "TrPIDSi\t\t" << fMCGam->GetPdgCode()<<"\t\t";
      //					isEndl = 1;
      //					fStartVertex = fMCGam->GetStartVertex();
      //					PrimaryDistance = (fStartVertex - PrimaryVertex).Mag();
      //					PrimaryTheta = PrimaryMomentum.Theta()*180/TMath::Pi();
      //					fVertexVolumeName = fHitSi->GetDetName();//=gGeoManager->FindNode(fStartVertex.X(),fStartVertex.Y(),fStartVertex.Z())->GetName();
      //					if(PrimaryTheta >110 && PrimaryTheta <165) // range of germaniums
      //							hAbsorption->Fill(1);
      //				}
      //			}
      if (fTargetOther->GetEntriesFast()) {
        fHitTargetOther = (PndHypPoint *)fTargetOther->At(0);
        fMCGam = (PndMCTrack *)fMcTr->At(fHitTargetOther->GetTrackID());
        if (fHitTargetOther->GetTrackID() == 1 && fMCGam->GetMotherID() < 1) {
          // isFirstInteraction = 1;
          // cout << "TrPIDOt\t\t" << fMCGam->GetPdgCode()<<"\t\t";
          isEndl = 1;
          fStartVertex = fMCGam->GetStartVertex();
          TVector3 InVector(fHitTargetOther->GetXin(), fHitTargetOther->GetZin(), fHitTargetOther->GetZin());
          TVector3 InDiffVector = fStartVertex - InVector;
          TVector3 DiffVector = fStartVertex - PrimaryVertex;
          PrimaryDistance = DiffVector.Mag();
          PrimaryTheta = DiffVector.Theta() * 180 / TMath::Pi();
          fVertexVolumeName = fHitTargetOther->GetDetName(); //=gGeoManager->FindNode(fStartVertex.X(),fStartVertex.Y(),fStartVertex.Z())->GetName();
          TString StartVolumeName = gGeoManager->FindNode(fStartVertex.X(), fStartVertex.Y(), fStartVertex.Z())->GetName();
          if (PrimaryTheta > 110 && PrimaryTheta < 165 &&
              (StartVolumeName.Contains("Sensor") || StartVolumeName.Contains("Absorber") || StartVolumeName.Contains("HoldingFrame") || StartVolumeName.Contains("Window") ||
               StartVolumeName.Contains("Frame"))) // range of germaniums
          {
            isFirstInteraction = 1;
            if (PrimaryDistance > 10) {
              cout << PrimaryDistance << endl;
              cout << PrimaryTheta << endl;

              cout << fVertexVolumeName.Data() << endl;
              cout << StartVolumeName.Data() << endl;
              fStartVertex.Print();
              PrimaryVertex.Print();
              DiffVector.Print();
              // InDiffVector.Print();
              cout << fHitTargetOther->GetLength() << endl;
            }
            if (fVertexVolumeName.Contains("Sensor")) {
              hAbsorption->Fill(1);
            } else {
              if (fVertexVolumeName.Contains("Absorber")) {
                hAbsorption->Fill(2);
                // cout << fVertexVolumeName.Data()<< endl;
              } else {
                if (fVertexVolumeName.Contains("HoldingFrame"))
                  hAbsorption->Fill(3);
                else {
                  if (fVertexVolumeName.Contains("Window"))
                    hAbsorption->Fill(4);
                  else {
                    if (fVertexVolumeName.Contains("Frame"))
                      hAbsorption->Fill(5);
                    else {
                      hAbsorption->Fill(6);
                      cout << fVertexVolumeName.Data() << endl;
                      fStartVertex.Print();
                    }
                  }
                }
              }
            }
          }
        }
      } // end of if (fTargetOther->GetEntriesFast())
      if (isFirstInteraction) {
        if (PrimaryTheta > 110 && PrimaryTheta < 165)
          h2AbsorptionDistanceAngle->Fill(PrimaryTheta, PrimaryDistance);
      }
      // if (isEndl)
      // cout << endl;
    } // end of if( fSilicon->GetEntriesFast() || fTargetOther->GetEntriesFast())
  }   // end of if (fTargetAvailable)
  // loop over hits and save energy loss per crystal and crystals hit

  // theta angle of gammas hiting the germanium
  fMCGam = (PndMCTrack *)fMcTr->At(0);
  TVector3 GammaMomentum = fMCGam->GetMomentum();
  // bool ishThetaCheckPrimaryFilled=0;
  if (fGe->GetEntriesFast()) {
    hThetaCheck->Fill(GammaMomentum.Theta() * 180 / TMath::Pi());
    if (fTargetAvailable) {
      if (!(fSilicon->GetEntriesFast() || fTargetOther->GetEntriesFast()))
        hThetaCheckPrimary->Fill(GammaMomentum.Theta() * 180 / TMath::Pi());
    } else {
      hThetaCheckPrimary->Fill(GammaMomentum.Theta() * 180 / TMath::Pi());
      // ishThetaCheckPrimaryFilled=1;
    }
  }
  // if (!ishThetaCheckPrimaryFilled && fGe->GetEntriesFast()&& fTargetAvailable && !(fSilicon->GetEntriesFast() || fTargetOther->GetEntriesFast()))
  //{
  // hThetaCheck->Fill(GammaMomentum.Theta()*180/TMath::Pi());
  // if (GammaMomentum.Theta()*180/TMath::Pi() > 170)
  //{
  // cout << GammaMomentum.Theta()*180/TMath::Pi() << endl;
  // cout << fMCGam->GetMotherID() << endl;
  //}
  //}
  // gamma energy part
  for (Int_t i = 0; i < fGe->GetEntriesFast(); i++) {
    fHitGe = (PndHypGePoint *)fGe->At(i);
    fMCGam = (PndMCTrack *)fMcTr->At(fHitGe->GetTrackID());
    if (fHitGe->GetEnergyLoss() != 0) {
      EnergyPerCrystal[fHitGe->GetDetectorID() % 100] += fHitGe->GetEnergyLoss() * 1000; // in MeV
      SetOfCrystalHit.insert(fHitGe->GetDetectorID() % 100);
    }
  }

  // add up the whole collected energy by all crystals, smear every crystal seperatly
  for (std::set<int>::iterator it = SetOfCrystalHit.begin(); it != SetOfCrystalHit.end(); ++it) {
    CollectedEnergy += SmearHit(EnergyPerCrystal[*it]);
  }
  // fill histogram if there is any collected Energy
  if (CollectedEnergy > 0) {
    hGamEnergy->Fill(CollectedEnergy);
    if (fTargetAvailable) {
      if (fSilicon->GetEntriesFast() || fTargetOther->GetEntriesFast())
        hGamEnergyTargetBackground->Fill(CollectedEnergy);
    }
    if (fVerbose >= 2 && CollectedEnergy != 0)
      cout << "Hyp Ge Gamma Ana:\t"
           << "Energy " << CollectedEnergy << endl;
  }
  // fill number of hits histo when full gamma energy is collected, NoH = size of the set
  if (TMath::Abs(CollectedEnergy - GammaEnergy) <
      10 * PeakWidth) // Peak = 10 * PeakWidth of 1 crystal --> takes multiple hits into account (faster than fitting and than doing it again, error is small)
  {
    hNumberOfHits->Fill(SetOfCrystalHit.size()); // Fill # of Hits -diagramm
    if (fVerbose >= 2)
      cout << "Hyp Ge Gamma Ana:\t"
           << "NoHFill: " << SetOfCrystalHit.size() << endl;
  }
  // show progress every 10 % of events
  if (!((EvtCount++ * 10) % NumberOfEvents)) {
    if (EvtCount == 1)
      cout << "Hyp Ge Gamma Ana:\t"
           << "0  % done!" << endl;
    else
      cout << "Hyp Ge Gamma Ana:\t" << 100 * Double_t(EvtCount - 1) / NumberOfEvents << " % done!" << endl;
  }
  if (fVerbose == 3)
    cout << "Hyp Ge Gamma Ana:\t"
         << "Event Number" << EvtCount << endl;
}

void PndHypGeGammaAna::Finish()
{
  //-------------Analysis of the spectrum-------------------------------------
  cout << "Hyp Ge Gamma Ana:\tEvent loop finished" << endl;
  // print number of entries in spectrum
  if (fVerbose >= 2)
    cout << "Hyp Ge Gamma Ana:\tEntries in spectrum: " << hGamEnergy->GetEntries() << endl;
  TxtFile << "Hyp Ge Gamma Ana:\tEntries in spectrum: \t" << hGamEnergy->GetEntries() << endl;

  // FindPeaks();

  // if (DoComptonCalculations)
  //	CalculateCompton();

  // FitSpectrum();
  // GetPeakContent();
  // FittingOutput();
  //--------------make up for the histograms----------------------------------

  WriteHistogramsToFile();

  TxtFile.close();
  cout << "Hyp Ge Gamma Ana:\tAnalysis finished succesfully" << endl;
}

Double_t PndHypGeGammaAna::Compton(Double_t E, Double_t Th)
{
  return E - E / (1 + E / 0.000511 * (1 - TMath::Cos(Th)));
}
Double_t PndHypGeGammaAna::invCompton(Double_t E, Double_t Eprime)
{
  return TMath::ACos((E * E - E * Eprime - Eprime * 0.000511) / (E * (E - Eprime)));
}

Double_t PndHypGeGammaAna::SmearHit(Double_t Energy)
{
  Double_t Resolution = CalculatePeakWidth(Energy);
  Double_t SmearedEnergy = Energy + Rng->Gaus(0, Resolution);
  return SmearedEnergy;
}

Double_t PndHypGeGammaAna::CalculatePeakWidth(Double_t Energy)
{
  return PeakWidtchStrechFactor * sqrt(ResConstPar + Energy * ResGradientPar);
}

void PndHypGeGammaAna::HistogramCosmetics(TH1D *histo, TString XTitle, Double_t XTitleSize, Double_t XTitleOffset, Double_t XLabelSize, TString YTitle, Double_t YTitleSize,
                                          Double_t YTitleOffset, Double_t YLabelSize)
{
  histo->SetXTitle(XTitle.Data());
  histo->GetXaxis()->SetTitleSize(XTitleSize);
  histo->GetXaxis()->SetTitleOffset(XTitleOffset);
  histo->GetXaxis()->SetLabelSize(XLabelSize);

  histo->SetYTitle(YTitle.Data());
  histo->GetYaxis()->SetTitleSize(YTitleSize);
  histo->GetYaxis()->SetTitleOffset(YTitleOffset);
  histo->GetYaxis()->SetLabelSize(YLabelSize);
}
void PndHypGeGammaAna::HistogramCosmetics2D(TH2D *histo, TString XTitle, Double_t XTitleSize, Double_t XTitleOffset, Double_t XLabelSize, TString YTitle, Double_t YTitleSize,
                                            Double_t YTitleOffset, Double_t YLabelSize)
{
  histo->SetXTitle(XTitle.Data());
  histo->GetXaxis()->SetTitleSize(XTitleSize);
  histo->GetXaxis()->SetTitleOffset(XTitleOffset);
  histo->GetXaxis()->SetLabelSize(XLabelSize);

  histo->SetYTitle(YTitle.Data());
  histo->GetYaxis()->SetTitleSize(YTitleSize);
  histo->GetYaxis()->SetTitleOffset(YTitleOffset);
  histo->GetYaxis()->SetLabelSize(YLabelSize);
}
void PndHypGeGammaAna::FitSpectrum()
{
  // fitting the spectrum						modell is not good!!!! M.Steinen
  GausBG = new TF1("GausBG", "gausn", (PeakX - 1000) / 100000, (PeakX + 1000) / 100000);
  GausBG->SetParName(0, "Ampl");
  GausBG->SetParName(1, "x0");
  GausBG->SetParName(2, "sigma");
  GausBG->SetParameter(0, PeakY / 10000);
  GausBG->SetParameter(1, PeakX / 100000.);
  GausBG->SetParameter(2, 100. / 100000.);
  GausBG->SetParLimits(0, 0.000001, 1000000);
  GausBG->SetParLimits(1, (PeakX - 100) / 100000., (PeakX + 100) / 100000.);
  GausBG->SetParLimits(2, 1. / 100000., 1000. / 100000.);
  /*GausBG->SetParameter(0,1);
  GausBG->SetParameter(1,1);
  GausBG->SetParameter(2,0);
  GausBG->FixParameter(2,0);
  GausBG->FixParameter(1,0);
  GausBG->FixParameter(0,0);
  */
  GausBG->SetNpx(100000);
  GausBG->SetLineColor(kRed);
  hGamEnergy->Fit(GausBG, "R");
}

void PndHypGeGammaAna::FittingOutput()
{
  cout << "Hyp Ge Gamma Ana:\tCHi²red of the fit \t" << GausBG->GetChisquare() / GausBG->GetNDF() << endl;
  cout << "Hyp Ge Gamma Ana:\tCHi²red of the fit \t" << GausBG->GetChisquare() << "  " << GausBG->GetNDF() << endl;
  TxtFile << "Hyp Ge Gamma Ana:\tCHi²red of the fit \t" << GausBG->GetChisquare() / GausBG->GetNDF() << endl;
  TxtFile << "Hyp Ge Gamma Ana:\tFitparameter Spectrum (model: gausn):" << endl;
  for (Int_t i = 0; i < 3; i++) {
    TxtFile << "Hyp Ge Gamma Ana:\t" << GausBG->GetParName(i) << ":\t" << GausBG->GetParameter(i) << endl;
  }
  if (fVerbose >= 2) {
    cout << "Hyp Ge Gamma Ana:\tFitparameter Spectrum (model: gausn):" << endl;
    for (Int_t i = 0; i < 3; i++) {
      cout << "Hyp Ge Gamma Ana:\t" << GausBG->GetParName(i) << ":\t" << GausBG->GetParameter(i) << endl;
    }
  }

  Double_t FWHM = GausBG->GetParameter(2) * 2.3548200;
  if (fVerbose >= 2)
    cout << "Hyp Ge Gamma Ana:\tFWHM[keV]: " << FWHM * 1000 << endl;
  TxtFile << "Hyp Ge Gamma Ana:\tFWHM[keV]:\t" << FWHM * 1000 << endl;

  if (fVerbose >= 2)
    cout << "Hyp Ge Gamma Ana:\tFull-Energy-Peak-Eff. [%]: " << double(int(GausBG->GetParameter(0) * 100000 / (NumberOfEvents * 2) * 100 * 1000)) / 1000 << " ± "
         << double(int(GausBG->GetParError(0) * 100000 / (NumberOfEvents * 2) * 100 * 1000)) / 1000 << endl; // *2 because only 2Pi of solid angle is simulated
  TxtFile << "Hyp Ge Gamma Ana:\tFull-Energy-Peak-Eff. [%]: \t" << double(int(GausBG->GetParameter(0) * 100000 / (NumberOfEvents * 2) * 100 * 1000)) / 1000 << " ± "
          << double(int(GausBG->GetParError(0) * 100000 / (NumberOfEvents * 2) * 100 * 1000)) / 1000 << endl; // *2 because only 2Pi of solid angle is simulated
}

void PndHypGeGammaAna::FindPeaks()
{
  TSpectrum *s = new TSpectrum(nPeaks);
  s->Search(hGamEnergy, 0.01, "goffnodraw", 0.01);

  if (fVerbose >= 2)
    cout << "Hyp Ge Gamma Ana:\tSpectrum search finished: " << hGamEnergy->GetEntries() << endl;

  xpeaks = s->GetPositionX();
  ypeaks = s->GetPositionY();

  if (fVerbose == 3) {
    for (Int_t i = 0; i < nPeaks; i++) {
      cout << "Hyp Ge Gamma Ana:\tPeak @ (" << xpeaks[i] << "," << ypeaks[i] << ")" << endl;
    }
  }
  PeakX = xpeaks[PeakToLook - 1] * 100000;
  PeakY = ypeaks[PeakToLook - 1];
  DPeakY = sqrt(PeakY);

  delete s;
}

void PndHypGeGammaAna::GetPeakContent()
{
  // get content of peak
  SumPeak = 0;
  for (Int_t i = PeakX - PeakWidth * 10 * 1000000 * 100; i <= PeakX + PeakWidth * 10 * 1000000 * 100;
       i++) // Peak = 10 * PeakWidth of 1 crystal --> takes multiple hits into account (faster than fitting and than doing it again, error is small)
    SumPeak += hGamEnergy->GetBinContent(i);
  DSumPeak = sqrt(SumPeak);
  if (fVerbose >= 2) {
    cout << "Hyp Ge Gamma Ana:\tPeak at bin " << PeakX << endl;
    cout << "Hyp Ge Gamma Ana:\tPeak bin height" << PeakY << " ± " << DPeakY << endl;
    cout << "Hyp Ge Gamma Ana:\tSumPeak " << SumPeak << " ± " << DSumPeak << endl;
  }
  TxtFile << "Hyp Ge Gamma Ana:\tPeak at bin \t" << PeakX << endl;
  TxtFile << "Hyp Ge Gamma Ana:\tPeak bin height\t" << PeakY << " ± " << DPeakY << endl;
  TxtFile << "Hyp Ge Gamma Ana:\tSumPeak \t" << SumPeak << " ± " << DSumPeak << endl;
}

void PndHypGeGammaAna::ActivateComptonCalculations(Bool_t isActivated)
{
  DoComptonCalculations = isActivated;
}

void PndHypGeGammaAna::CalculateCompton()
{
  // get number of compton events

  for (Int_t i = Compton(GammaEnergy, lowComptonAngle) * 1000 * 100000; i <= Compton(GammaEnergy, highComptonAngle) * 1000 * 100000; i++) {
    SumCompton += hGamEnergy->GetBinContent(i);
  }
  DSumCompton = sqrt(SumCompton);

  if (fVerbose >= 2) {
    cout << "Hyp Ge Gamma Ana:\tCompton from bins [" << Compton(GammaEnergy, lowComptonAngle) * 1000 * 100000 << " to " << Compton(GammaEnergy, highComptonAngle) * 1000 * 100000
         << "]" << endl;
    cout << "Hyp Ge Gamma Ana:\tNumber of compton events: " << SumCompton << " +- " << DSumCompton << endl;
  }
  TxtFile << "Hyp Ge Gamma Ana:\tCompton range [bins] \t[" << Compton(GammaEnergy, lowComptonAngle) * 1000 * 100000 << " to "
          << Compton(GammaEnergy, highComptonAngle) * 1000 * 100000 << "]" << endl;
  TxtFile << "Hyp Ge Gamma Ana:\tNumber of compton events: \t" << SumCompton << " +- " << DSumCompton << endl;

  // calculate Peak/Compton ratio

  if (SumCompton != 0) {
    PeakToCompton = PeakY / SumCompton * (Compton(GammaEnergy, highComptonAngle) - Compton(GammaEnergy, lowComptonAngle)) * 1000 * 100000;
    DPeakToCompton = sqrt(pow(DPeakY / SumCompton, 2) + pow(PeakY * DSumCompton / SumCompton / SumCompton, 2)) *
                     (Compton(GammaEnergy, highComptonAngle) - Compton(GammaEnergy, lowComptonAngle)) * 1000 * 100000;
    if (fVerbose >= 2)
      cout << "Hyp Ge Gamma Ana:\tPeak/Compton : " << PeakToCompton << " ± " << DPeakToCompton << endl;
    TxtFile << "Hyp Ge Gamma Ana:\tPeak/Compton : \t" << PeakToCompton << " ± " << DPeakToCompton << endl;
  } else {
    if (fVerbose >= 2)
      cout << "Hyp Ge Gamma Ana:\tSumCompton = 0 -> no PeakToCompton possible!" << endl;
    TxtFile << "Hyp Ge Gamma Ana:\tSumCompton = 0 -> no PeakToCompton possible!" << endl;
  }
}

void PndHypGeGammaAna::WriteHistogramsToFile()
{
  //-------------writing and drawing the histograms-------------------------

  HistogramCosmetics(hGamEnergy, "#gamma Energy [MeV]", 0.06, 0.7, 0.045, "Counts", 0.06, 0.8, 0.045);
  HistogramCosmetics(hGamEnergyTargetBackground, "#gamma Energy [MeV]", 0.06, 0.7, 0.045, "Counts", 0.06, 0.8, 0.045);
  HistogramCosmetics(hNumberOfHits, "Number of Crystals hit", 0.06, 0.7, 0.045, "Counts", 0.06, 0.8, 0.045);
  HistogramCosmetics(hThetaCheck, "#Theta [#circ]", 0.06, 0.7, 0.045, "Counts", 0.06, 0.8, 0.045);
  HistogramCosmetics(hThetaCheckPrimary, "#Theta [#circ]", 0.06, 0.7, 0.045, "Counts", 0.06, 0.8, 0.045);
  HistogramCosmetics(hAbsorption, "part of absorption", 0.06, 0.7, 0.045, "Counts", 0.06, 0.8, 0.045);
  HistogramCosmetics2D(h2AbsorptionDistanceAngle, "#Theta [#circ]", 0.06, 0.7, 0.045, "Distance [cm]", 0.06, 0.8, 0.045);

  hGamEnergy->Write();
  hGamEnergyTargetBackground->Write();
  hNumberOfHits->Write();
  hThetaCheck->Write();
  hThetaCheckPrimary->Write();
  hAbsorption->Write();
  h2AbsorptionDistanceAngle->Write();
  TCanvas *CanGamEnergy = new TCanvas("CanGamEnergy", "Gamma Energy", 1000, 600);
  hGamEnergy->Draw();

  TCanvas *CanGamEnergyTarget = new TCanvas("CanGamEnergyTarget", "Gamma Energy target", 1000, 600);
  hGamEnergyTargetBackground->Draw();

  TCanvas *CanNoH = new TCanvas("CanNoH", "Number of Hits", 1000, 600);
  hNumberOfHits->Draw();

  TCanvas *CanTH = new TCanvas("CanTH", "TH", 1000, 600);
  hThetaCheck->Draw();

  TCanvas *CanTHPrimary = new TCanvas("CanTHPrimary", "TH prim", 1000, 600);
  hThetaCheckPrimary->Draw();

  TCanvas *CanAbsorption = new TCanvas("CanAbsorption", "part of absorption", 1000, 600);
  hAbsorption->Draw();

  TCanvas *CanAbsorption2D = new TCanvas("CanAbsorption2D", "correlation of absorption distance and theta", 1000, 600);
  h2AbsorptionDistanceAngle->Draw("colz");
}

bool PndHypGeGammaAna::InTargetAbsorbedGammaWouldHitGermanium()
{
  // to be implemented
  // this function checks if a gamma that was absorbed in the target system would have hit the solid angle of the germanium, only than it's important for the material budget
  // to do this,  the start vertex and the momentum vector of the primary gamma have to be projected and checked of this line crosses a germanium detector
  return 0;
}

void PndHypGeGammaAna::SetTarget(Bool_t TargetAvailable)
{
  fTargetAvailable = TargetAvailable;
}
void PndHypGeGammaAna::SetOmegaQuadrupolMode(Bool_t useOmegaQuadrupolMode_ext)
{
  useOmegaQuadrupolMode = useOmegaQuadrupolMode_ext;
}

void PndHypGeGammaAna::SetPeakWidtchStrechFactor(Double_t PeakWidtchStrechFactor_ext)
{
  PeakWidtchStrechFactor = PeakWidtchStrechFactor_ext;
}

ClassImp(PndHypGeGammaAna)
