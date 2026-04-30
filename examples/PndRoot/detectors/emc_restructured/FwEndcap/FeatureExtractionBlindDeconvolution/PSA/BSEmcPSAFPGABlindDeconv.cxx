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

#include "BSEmcPSAFPGABlindDeconv.h"
#include "FairLogger.h"
#include "TFile.h"
#include "TSystem.h"
#include <memory>

BSEmcPSAFPGABlindDeconv::BSEmcPSAFPGABlindDeconv() {}

BSEmcPSAFPGABlindDeconv::~BSEmcPSAFPGABlindDeconv() {}

void BSEmcPSAFPGABlindDeconv::set(const TString &t_filename)
{
  TString filename = gSystem->Getenv("VMCWORKDIR");
  filename += "/input/" + t_filename;

  TH1D *globalH{nullptr};
  auto HistoFile = std::unique_ptr<TFile>(new TFile(filename, "READ"));
  HistoFile->GetObject("FFT Backward;", globalH);

  if (globalH == nullptr) {
    LOG(info) << "globalH is a nullptr";
  }

  fNumberOfXBins = globalH->GetNbinsX();
  fDataDeconv.resize(fNumberOfXBins);
  for (int i = 0; i < fNumberOfXBins; i++) {
    fDataDeconv[i] = globalH->GetBinContent(i);
  }
}

// Alternative implementation, slightly faster, but results need to be checked.
// std::vector<Double_t> BSEmcPSAFPGABlindDeconv::put(const std::vector<Double_t> &t_buffer) const
// {
//   const Int_t bufferSize = t_buffer.size();
//   std::vector<Double_t> deconv;
//   deconv.resize(bufferSize);

//   for (Int_t k = 0; k < bufferSize; ++k) {
//     const Int_t n = k + 100;
//     deconv[k] = 0.;
//     const Int_t min = std::min(n, bufferSize-1);
//     const Int_t start = std::max(0, n-fNumberOfXBins+1);
//     for (Int_t j = start; j <= min; ++j) {
//         deconv[k] += t_buffer[j] * fDataDeconv[n - j];
//     }
//   }

//   return deconv;
// }

std::vector<Double_t> BSEmcPSAFPGABlindDeconv::put(const std::vector<Double_t> &t_buffer) const
{
  const Int_t bufferSize = t_buffer.size();
  std::vector<Double_t> deconv;
  deconv.resize(bufferSize);

  for (Int_t k = 0; k < bufferSize; k++) {
    const Int_t n = k + 100;
    deconv[k] = 0.;
    for (Int_t j = 0; j <= n; j++) {
      if (j >= bufferSize || (n - j) >= fNumberOfXBins) {
        deconv[k] += (0.);
      } else {
        deconv[k] += t_buffer[j] * fDataDeconv[n - j];
      }
    }
  }

  return deconv;
}
