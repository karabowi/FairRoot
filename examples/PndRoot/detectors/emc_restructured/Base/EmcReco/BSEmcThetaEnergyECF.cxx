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

#include "BSEmcThetaEnergyECF.h"

#include <stdlib.h>

#include "TAxis.h"
#include "TFile.h"
#include "TH2.h"
#include "TMath.h"
#include "TObject.h"
#include "TString.h"
#include "TVector3.h"

#include "fairlogger/Logger.h"

#include "BSEmcRecoHit.h"

void BSEmcThetaEnergyECF::SetupLookUpMap(const TString &t_filename, const TString &t_histname)
{
  std::unique_ptr<TFile> file = std::unique_ptr<TFile>(new TFile(t_filename, "READ"));
  if (file->IsZombie()) {
    LOG(error) << "EMC cluster correction file " << t_filename << " for photons does not exist.";
    exit(-1);
  } else {
    TH2F *tmp = dynamic_cast<TH2F *>(file->Get(t_histname.Data()));
    tmp->SetDirectory(nullptr);
    fHEnergyRatioPhoton.reset(tmp);
  }
  file->Close();
}

Bool_t BSEmcThetaEnergyECF::FindTheBin(Float_t t_value_x, Float_t t_value_y, Int_t &t_bin_x, Int_t &t_bin_y) const
{
  t_bin_x = fHEnergyRatioPhoton->GetXaxis()->FindBin(t_value_x);
  t_bin_y = fHEnergyRatioPhoton->GetYaxis()->FindBin(t_value_y);

  if ((t_bin_x < 1) || (t_bin_x > fHEnergyRatioPhoton->GetXaxis()->GetNbins())) {
    t_bin_x = -1;
    t_bin_y = -1;
    return kFALSE;
  }

  if ((t_bin_y < 1) || (t_bin_y > fHEnergyRatioPhoton->GetYaxis()->GetNbins())) {
    t_bin_x = -1;
    t_bin_y = -1;
    return kFALSE;
  }

  return kTRUE;
}

Double_t BSEmcThetaEnergyECF::GetValueInZ(Float_t t_value_x, Float_t t_value_y) const
{
  Int_t binx = 0, biny = 0;
  const Bool_t found = FindTheBin(t_value_x, t_value_y, binx, biny);
  if (!found) {
    LOG(error) << "BSEmcThetaEnergyECF in FindTheBin, check your table and input values: " << t_value_x << ", " << t_value_y << "!";
    return 1.0;
  }

  return fHEnergyRatioPhoton->Interpolate(t_value_x, t_value_y);
}

Double_t BSEmcThetaEnergyECF::GetCorrectedEnergy(Double_t t_energy, Double_t t_thetaRad, Double_t /*t_phiRad*/) const
{
  const Double_t thetaDeg = t_thetaRad * (180. / TMath::Pi());

  const Double_t energyCorrected = t_energy / GetValueInZ(t_energy, thetaDeg);

  return energyCorrected;
}

void BSEmcThetaEnergyECF::Correct(BSEmcRecoHit *t_recoHit) const
{
  const Double_t thetaRad = t_recoHit->GetPosition().Theta();
  const Double_t phiRad = t_recoHit->GetPosition().Phi();
  const Double_t corrEnergy = GetCorrectedEnergy(t_recoHit->GetRawEnergy(), thetaRad, phiRad);

  t_recoHit->SetCorrectedEnergy(corrEnergy);
}
