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

#include "PndRichRecoTask.h"
#include "PndRichReco.h"
#include "PndRichBarPoint.h"
#include "PndRichResolution.h"

// fairroot
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairGeanePro.h"
#include "FairGeaneUtil.h"
#include "FairTrackParP.h"
#include "FairLogger.h"

// general
#include <iostream>
#include <cmath>
#include <iomanip>

using namespace std;

// -----   Default constructor   -------------------------------------------
PndRichRecoTask::PndRichRecoTask() : FairTask("Rich Reco task"), fPersistence(kTRUE), fNumberOfEvents(0), fEvent(0), fTrackPositionSecond(0, 0, 0), fTrackDirectionSecond(0, 0, 0)
{
  fVerbose = 1;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndRichRecoTask::~PndRichRecoTask() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndRichRecoTask::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    cout << "-E- PndRichRecoTask: "
         << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  fRichReco = new PndRichReco(fGeoVersion); // 13 - one of the flat mirror variant
  vnhits.reserve(fNumberOfEvents);
  vmean.reserve(fNumberOfEvents);
  vsigma.reserve(fNumberOfEvents);
  fRichBarPoint = dynamic_cast<TClonesArray *>(ioman->GetObject("RichBarPoint"));

  fRichResolution = new PndRichResolution();

  LOG(info) << " PndRichRecoTask: Intialisation successfull ";
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndRichRecoTask::Exec(Option_t *)
{
  if (fVerbose > 0) {
    // cout << "==================== EVENT " << evt << endl;
  }
  fEvent++;
  // fRichReco->RichFullReconstruction();

  TVector3 pos = fTrackPosition;
  TVector3 dir = fTrackDirection.Unit();
  Double_t mom = 0;
  Double_t tm = 0;
  Int_t pdg = 0;

  PndRichBarPoint *richHit = nullptr;
  Int_t richEntries = fRichBarPoint->GetEntriesFast();
  if (richEntries) {
    richHit = (PndRichBarPoint *)fRichBarPoint->At(0);
    // pos = richHit->GetPosition0();
    // dir = richHit->GetMomentum0().Unit();
    // mom = richHit->GetMomentum0().Mag();
    pos = TVector3(richHit->GetX(), richHit->GetY(), richHit->GetZ());
    TVector3 p = TVector3(richHit->GetPx(), richHit->GetPy(), richHit->GetPz());
    dir = p.Unit();
    mom = p.Mag();
    tm = richHit->GetTime();
    pdg = richHit->GetPdgCode();
  }

  Int_t richPhot = 0;
  Float_t richThetaC = -1000, richThetaCErr = 0;
  Float_t richQuality = 1000000;
  // first particle
  fRichReco->RichFullReconstruction(pos, dir, tm, richQuality, richThetaC, richThetaCErr, richPhot);
  std::vector<Double_t> dth = fRichReco->GetDThetas();

  std::cout << std::setprecision(10) << std::endl;
  std::cout << "PndRichRecoTask: " << fEvent << " " << richQuality << " " << richThetaC << " " << richThetaCErr << " " << richPhot << " " << 1 << " " << mom << " " << dir.Phi()
            << " " << dir.Theta() << " " << pos.X() << " " << pos.Y() << " " << pdg << std::endl;
  Double_t mass[5] = {0.000510998961, 0.1056583745, 0.13957018, 0.493667, 0.9382720813};
  Double_t pdf[5];
  Double_t pdfsum = 0;
  dbpoint pnt;
  for (Int_t i = 0; i < 5; i++) {
    Float_t richThetaCx = richThetaC;
    pnt.mass = mass[i];
    pnt.beta = richThetaCx;
    pnt.x = pos.X();
    pnt.y = pos.Y();
    pnt.theta = dir.Theta();
    pnt.phi = dir.Phi();
    pnt.beta += fRichResolution->Shift(pnt);
    richThetaCx += fRichResolution->Shift(pnt);

    std::cout << i << " " << fRichResolution->Shift(pnt) << " " << fRichResolution->Sigma(pnt) << " " << mass[i] << " " << pnt.beta << " " << pnt.x << " " << pnt.y << " "
              << pnt.theta << " " << pnt.phi << std::endl;

    Double_t beta = mom / TMath::Sqrt(mom * mom + mass[i] * mass[i]);
    TF1 *gausPdf = new TF1("gausPdf", "gausn", 0, 1);
    gausPdf->SetParameter(0, 1);
    gausPdf->SetParameter(1, richThetaCx);
    gausPdf->SetParameter(2, richThetaCErr);
    // gausPdf->SetParameter(2,fRichResolution->Sigma(pnt));
    pdf[i] = gausPdf->Eval(beta);
    pdfsum += pdf[i];
    delete gausPdf;
  }
  std::cout << "particles PID: ";
  for (Int_t i = 0; i < 5; i++)
    std::cout << pdf[i] << " " << (pdfsum ? pdf[i] / pdfsum : 0.2) << " ";
  std::cout << std::endl;

  vdth.insert(vdth.end(), dth.begin(), dth.end());
  vnhits.push_back(dth.size());
  vmean.push_back(richThetaC);
  vsigma.push_back(richThetaCErr);

  std::vector<Double_t> th = fRichReco->GetThetas();
  std::vector<Double_t> ph = fRichReco->GetPhis();
  vth.insert(vth.end(), th.begin(), th.end());
  vph.insert(vph.end(), ph.begin(), ph.end());

  for (size_t i = 0; i < ph.size(); i++)
    std::cout << "::HitPars()  " << fEvent << " " << ph.at(i) << " " << th.at(i) << " " << dth.at(i) << std::endl;

  if (fTrackDirectionSecond.Mag() != 0) {
    // secon particle
    fRichReco->RichFullReconstruction(fTrackPositionSecond, fTrackDirectionSecond.Unit(), 0., richQuality, richThetaC, richThetaCErr, richPhot);
    std::cout << "PndRichRecoTask: " << fEvent << " " << richQuality << " " << richThetaC << " " << richThetaCErr << " " << richPhot << " " << 2 << std::endl;
  }
}

void PndRichRecoTask::FinishEvent() {}

void PndRichRecoTask::FinishTask()
{
  std::cout << "::FinishTask()  " << vmean.size() << " " << vsigma.size() << std::endl;
  TF1 *f = GetPeakParameters(vmean, 1100, 0.95, 1.005, 0.0003);

  double chisq = f->GetChisquare();
  double ndf = f->GetNDF();
  double chisqdf = chisq / ndf;
  std::cout << "Chisquare: " << chisq << "/" << ndf << " : " << chisqdf << std::endl;

  // Double_t amp = f->GetParameter(0); //value of 0th parameter //[R.K. 03/2017] unused variable?
  // Double_t eamp = f->GetParError(0); //error on 0th parameter //[R.K. 03/2017] unused variable?
  Double_t mean = f->GetParameter(1); // value of 1st parameter
  Double_t emean = f->GetParError(1); // error on 1st parameter
  Double_t sig = f->GetParameter(2);  // value of 1st parameter
  Double_t esig = f->GetParError(2);  // error on 1st parameter

  size_t nhits = 0;
  for (size_t i = 0; i < vmean.size(); i++) {
    if (std::fabs(vmean.at(i) - mean) < 3 * sig)
      nhits++;
  }

  std::cout << "::FinishTask() " << mean << " " << sig << std::endl;
  std::cout << "CalData: " << chisq << std::endl;
  std::cout << "CalData: " << ndf << std::endl;
  std::cout << "CalData: " << nhits << std::endl;
  std::cout << "CalData: " << mean << std::endl;
  std::cout << "CalData: " << emean << std::endl;
  std::cout << "CalData: " << sig << std::endl;
  std::cout << "CalData: " << esig << std::endl;

  TF1 *f1 = GetPeakParameters(vdth, 500, -0.05, 0.05, 0.003);

  chisq = f1->GetChisquare();
  ndf = f1->GetNDF();
  chisqdf = chisq / ndf;
  std::cout << "Chisquare: " << chisq << "/" << ndf << " : " << chisqdf << std::endl;

  // amp = f1->GetParameter(0); //value of 0th parameter //[R.K. 03/2017] unused variable?
  // eamp = f1->GetParError(0); //error on 0th parameter //[R.K. 03/2017] unused variable?
  mean = f1->GetParameter(1); // value of 1st parameter
  emean = f1->GetParError(1); // error on 1st parameter
  sig = f1->GetParameter(2);  // value of 1st parameter
  esig = f1->GetParError(2);  // error on 1st parameter

  std::cout << "::FinishTask() " << mean << " " << sig << std::endl;
  std::cout << "CalData: " << chisq << std::endl;
  std::cout << "CalData: " << ndf << std::endl;
  std::cout << "CalData: " << mean << std::endl;
  std::cout << "CalData: " << emean << std::endl;
  std::cout << "CalData: " << sig << std::endl;
  std::cout << "CalData: " << esig << std::endl;
}

TF1 *PndRichRecoTask::GetPeakParameters(std::vector<Double_t> v, UInt_t nbins, Double_t xmin, Double_t xmax, Double_t sig)
{
  TH1F *hbetafit = new TH1F("hist", "for gauss fit", nbins, xmin, xmax);
  for (size_t i = 0; i < v.size(); i++)
    hbetafit->Fill(v.at(i));
  int ipeak = hbetafit->GetMaximumBin();
  double amp0 = hbetafit->GetMaximum();
  double peak = xmin + (ipeak - 0.5) * (xmax - xmin) / nbins;
  TF1 *gfit = new TF1("Gaussian", "gaus", xmin, xmax);
  gfit->SetParameter(0, amp0);
  gfit->SetParameter(1, peak);
  gfit->SetParameter(2, sig);
  for (size_t i = 0; i < 5; i++) {
    Double_t meanf = gfit->GetParameter(1);
    Double_t sigf = gfit->GetParameter(2);
    double xminf = meanf - 3 * sigf;
    double xmaxf = meanf + 3 * sigf;
    hbetafit->Fit("Gaussian", "Q", "", xminf, xmaxf);
  }
  delete hbetafit;
  return gfit;
}

ClassImp(PndRichRecoTask)
