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

#include "BSEmcFwEndcapOnlineFeatureExtraction.h"

#include <stdlib.h>
#include <vector>

#include "TArrayD.h"
#include "TF1.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TObject.h"
#include "TString.h"

#include "FairParSet.h"
#include "fairlogger/Logger.h"

#include "PndParameterRegister.h"

#include "BSEmcAbsPSA.h"
#include "BSEmcDataBranchNames.h"
#include "BSEmcDigi.h"
#include "BSEmcFwEndcapDigiPar.h"
#include "BSEmcHighLowPSA.h"
#include "BSEmcPSAFPGAPileupAnalyser.h"

class BSEmcWaveform;

BSEmcFwEndcapOnlineFeatureExtraction::BSEmcFwEndcapOnlineFeatureExtraction() : BSEmcOnlineFeatureExtractionProcess(), fHighgainPSA(nullptr), fLowgainPSA(nullptr) {}

BSEmcFwEndcapOnlineFeatureExtraction::~BSEmcFwEndcapOnlineFeatureExtraction() {}

void BSEmcFwEndcapOnlineFeatureExtraction::SetDetectorName(const std::string &t_detectorName)
{
  BSEmcOnlineFeatureExtractionProcess::SetDetectorName(t_detectorName);
  this->fDetectorName = t_detectorName;
  fParameterList.push_back(BSEmcFwEndcapDigiPar::fgParameterName);
}

void BSEmcFwEndcapOnlineFeatureExtraction::SetupParameters(const PndParameterRegister *t_parameterRegister) /*override*/
{
  BSEmcOnlineFeatureExtractionProcess::SetupParameters(t_parameterRegister);
  fDigiPar = dynamic_cast<BSEmcFwEndcapDigiPar *>(t_parameterRegister->GetParameter(BSEmcFwEndcapDigiPar::fgParameterName.c_str()));
  DefinePSA();
}

void BSEmcFwEndcapOnlineFeatureExtraction::DefinePSA()
{
  LOG(debug) << "BSEmcFwEndcapOnlineFeatureExtraction: DefinePSA begin";

  if (!fDigiPar->GetPsaTypeLow().IsNull()) {

    if (fLowgainPSA != nullptr) {
      LOG(warn) << "BSEmcFwEndcapOnlineFeatureExtraction::Init: Lowgain PSA already set. Skipping default initialization";
    } else if (fDigiPar->GetPsaTypeLow().CompareTo("PSAFPGAPileupAnalyser") == 0) {
      BSEmcPSAFPGAPileupAnalyser *psa = new BSEmcPSAFPGAPileupAnalyser();

      TObjArray *rparas = fDigiPar->GetRValueParLow().Tokenize(";");
      TF1 R_thres("R_thres_Low", rparas->GetEntriesFast() > 0 ? rparas->UncheckedAt(0)->GetName() : "", 1, TMath::Power(2, fDigiPar->GetNBits()));
      TF1 R_mean("R_mean_Low", rparas->GetEntriesFast() > 1 ? rparas->UncheckedAt(1)->GetName() : "", 1, TMath::Power(2, fDigiPar->GetNBits()));

      delete rparas;

      psa->Init(std::vector<Double_t>(fDigiPar->GetPsaParLow().GetArray(), fDigiPar->GetPsaParLow().GetArray() + fDigiPar->GetPsaParLow().GetSize()),
                R_thres.IsZombie() ? nullptr : &R_thres, R_mean.IsZombie() ? nullptr : &R_mean);
      psa->setBaselineWindow(0, 15);

      fLowgainPSA = psa;

    } else {
      LOG(error) << "BSEmcFwEndcapOnlineFeatureExtraction::Init could not find PSA of type: " << fDigiPar->GetPsaTypeLow();
    }
  }

  if (!fDigiPar->GetPsaTypeHigh().IsNull()) {

    if (fHighgainPSA != nullptr) {
      LOG(warn) << "BSEmcFwEndcapOnlineFeatureExtraction::Init: Highgain PSA already set. Skipping default initialization";
    } else if (fDigiPar->GetPsaTypeHigh().CompareTo("PSAFPGAPileupAnalyser") == 0) {
      BSEmcPSAFPGAPileupAnalyser *psa = new BSEmcPSAFPGAPileupAnalyser();

      TObjArray *rparas = fDigiPar->GetRValueParHigh().Tokenize(";");
      TF1 R_thres("R_thres_High", rparas->GetEntriesFast() > 0 ? rparas->UncheckedAt(0)->GetName() : "", 1, TMath::Power(2, fDigiPar->GetNBits()));
      TF1 R_mean("R_mean_High", rparas->GetEntriesFast() > 1 ? rparas->UncheckedAt(1)->GetName() : "", 1, TMath::Power(2, fDigiPar->GetNBits()));

      delete rparas;

      psa->Init(std::vector<Double_t>(fDigiPar->GetPsaParHigh().GetArray(), fDigiPar->GetPsaParHigh().GetArray() + fDigiPar->GetPsaParHigh().GetSize()),
                R_thres.IsZombie() ? nullptr : &R_thres, R_mean.IsZombie() ? nullptr : &R_mean);
      psa->setBaselineWindow(0, 15);

      fHighgainPSA = psa;
    } else {
      LOG(error) << "BSEmcFwEndcapOnlineFeatureExtraction::Init could not find PSA of type: " << fDigiPar->GetPsaTypeHigh();
    }
  }

  if (fHighgainPSA != nullptr && fLowgainPSA != nullptr) {
    BSEmcHighLowPSA *psa = new BSEmcHighLowPSA();
    psa->Init(fHighgainPSA, fLowgainPSA, fDigiPar->GetSignalOverflowHigh(), 0, 1);
    fPSA = psa;
  } else {
    LOG(error) << "BSEmcFwEndcapOnlineFeatureExtraction::Init No highgain and/or lowgain psa";
    throw std::exception();
  }
  SetPSA(fPSA);
  LOG(debug) << "BSEmcFwEndcapOnlineFeatureExtraction: Intialization successfull";
}

BSEmcDigi::eGAIN BSEmcFwEndcapOnlineFeatureExtraction::GetGainType(const BSEmcWaveform * /*t_waveform*/, Int_t t_hit) const
{
  Int_t gainType = dynamic_cast<BSEmcHighLowPSA *>(fPSA)->GetWaveformIdx(t_hit);
  BSEmcDigi::eGAIN type = BSEmcDigi::eGAIN::kNONE;
  switch (gainType) {
  case 0:
    type = BSEmcDigi::eGAIN::kHIGH;
    LOG(trace) << "BSEmcFwEndcapOnlineFeatureExtraction GetGainType(hit: " << t_hit << ") is kHIGH";
    break;
  case 1:
    type = BSEmcDigi::eGAIN::kLOW;
    LOG(trace) << "BSEmcFwEndcapOnlineFeatureExtraction GetGainType(hit: " << t_hit << ") is kLOW";
    break;
  default: LOG(trace) << "BSEmcFwEndcapOnlineFeatureExtraction GetGainType(hit: " << t_hit << ") is kNONE"; break;
  }
  return type;
}

ClassImp(BSEmcFwEndcapOnlineFeatureExtraction);
