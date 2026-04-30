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

#include "BSEmcFwEndcapDigi.h"

#include <stdlib.h>
#include <vector>

#include "TArrayD.h"
#include "TF1.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TObject.h"

#include "BSEmcDigi.h"
#include "BSEmcFwEndcapDigiPar.h"
#include "BSEmcHighLowPSA.h"
#include "BSEmcPSAFPGAPileupAnalyser.h"

class BSEmcWaveform;

BSEmcFwEndcapDigi::BSEmcFwEndcapDigi(Bool_t t_storedigis)
  : BSEmcExtractDigisFromWaveforms<BSEmcFwEndcapDigiPar>("FwEndcap", t_storedigis), fHighgainPSA(nullptr), fLowgainPSA(nullptr)
{
}

//--------------
// Destructor --
//--------------
BSEmcFwEndcapDigi::~BSEmcFwEndcapDigi() {}

/**
 * @brief Init Task
 *
 * Prepares the TClonesArray of BSEmcMultiWaveform for reading and BSEmcDigi for writing.
 * Also reads the EMC parameters and prepares the pulseshapes
 * (BSEmcAbsPulseshape) and pulse shape analyser (BSEmcAbsPSA) as well as the
 * calibrator (BSEmcSimCrystalCalibrator).
 *
 * @return InitStatus
 * @retval kSUCCESS success
 */
void BSEmcFwEndcapDigi::DefinePSA()
{

  // psa
  if (!fDigiPar->GetPsaTypeLow().IsNull()) {

    if (fLowgainPSA != nullptr) {
      LOG(warn) << "BSEmcFwEndcapDigi::Init: Lowgain PSA already set. Skipping default initialization";
    } else if (fDigiPar->GetPsaTypeLow().CompareTo("PSAFPGAPileupAnalyser") == 0) {
      BSEmcPSAFPGAPileupAnalyser *psa = new BSEmcPSAFPGAPileupAnalyser();
      psa->SetVerbose(fVerbose);

      TObjArray *rparas = fDigiPar->GetRValueParLow().Tokenize(";");
      TF1 R_thres("R_thres_Low", rparas->GetEntriesFast() > 0 ? rparas->UncheckedAt(0)->GetName() : "", 1, TMath::Power(2, fDigiPar->GetNBits()));
      TF1 R_mean("R_mean_Low", rparas->GetEntriesFast() > 1 ? rparas->UncheckedAt(1)->GetName() : "", 1, TMath::Power(2, fDigiPar->GetNBits()));

      delete rparas;

      psa->Init(std::vector<Double_t>(fDigiPar->GetPsaParLow().GetArray(), fDigiPar->GetPsaParLow().GetArray() + fDigiPar->GetPsaParLow().GetSize()),
                R_thres.IsZombie() ? nullptr : &R_thres, R_mean.IsZombie() ? nullptr : &R_mean);
      psa->setBaselineWindow(0, 15);
      fLowgainPSA = psa;

    } else {
      LOG(error) << "BSEmcFwEndcapDigi::Init could not find PSA of type: " << fDigiPar->GetPsaTypeLow();
    }
  }

  if (!fDigiPar->GetPsaTypeHigh().IsNull()) {

    if (fHighgainPSA != nullptr) {
      LOG(warn) << "BSEmcFwEndcapDigi::Init: Highgain PSA already set. Skipping default initialization";
    } else if (fDigiPar->GetPsaTypeHigh().CompareTo("PSAFPGAPileupAnalyser") == 0) {
      BSEmcPSAFPGAPileupAnalyser *psa = new BSEmcPSAFPGAPileupAnalyser();
      psa->SetVerbose(fVerbose);

      TObjArray *rparas = fDigiPar->GetRValueParHigh().Tokenize(";");
      TF1 R_thres("R_thres_High", rparas->GetEntriesFast() > 0 ? rparas->UncheckedAt(0)->GetName() : "", 1, TMath::Power(2, fDigiPar->GetNBits()));
      TF1 R_mean("R_mean_High", rparas->GetEntriesFast() > 1 ? rparas->UncheckedAt(1)->GetName() : "", 1, TMath::Power(2, fDigiPar->GetNBits()));

      delete rparas;

      psa->Init(std::vector<Double_t>(fDigiPar->GetPsaParHigh().GetArray(), fDigiPar->GetPsaParHigh().GetArray() + fDigiPar->GetPsaParHigh().GetSize()),
                R_thres.IsZombie() ? nullptr : &R_thres, R_mean.IsZombie() ? nullptr : &R_mean);

      fHighgainPSA = psa;
      psa->setBaselineWindow(0, 15);

    } else {
      LOG(error) << "BSEmcFwEndcapDigi::Init could not find PSA of type: " << fDigiPar->GetPsaTypeHigh();
    }
  }

  if (fHighgainPSA != nullptr && fLowgainPSA != nullptr) {
    BSEmcHighLowPSA *psa = new BSEmcHighLowPSA();
    psa->Init(fHighgainPSA, fLowgainPSA, fDigiPar->GetSignalOverflowHigh(), 0, 1);
    fPSA = psa;
  } else {
    LOG(error) << "BSEmcFwEndcapDigi::Init No highgain and/or lowgain psa";
    exit(-1);
  }

  LOG(debug) << "BSEmcFwEndcapDigi: Intialization successfull";
}

BSEmcDigi::eGAIN BSEmcFwEndcapDigi::GetGainType(BSEmcWaveform * /*t_waveform*/, Int_t t_hit) const
{
  Int_t gainType = dynamic_cast<BSEmcHighLowPSA *>(fPSA)->GetWaveformIdx(t_hit);
  BSEmcDigi::eGAIN type = BSEmcDigi::eGAIN::kNONE;
  switch (gainType) {
  case 0:
    type = BSEmcDigi::eGAIN::kHIGH;
    LOG(trace) << "BSEmcFwEndcapDigi GetGainType(hit: " << t_hit << ") is kHIGH";
    break;
  case 1:
    type = BSEmcDigi::eGAIN::kLOW;
    LOG(trace) << "BSEmcFwEndcapDigi GetGainType(hit: " << t_hit << ") is kLOW";
    break;
  default: LOG(trace) << "BSEmcFwEndcapDigi GetGainType(hit: " << t_hit << ") is kNONE"; break;
  }
  return type;
}

ClassImp(BSEmcFwEndcapDigi);
