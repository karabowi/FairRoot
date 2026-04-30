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

#ifndef BSEMCTHETAENERGYECF_HH
#define BSEMCTHETAENERGYECF_HH

#include <memory>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TH2.h"
#include "TObject.h"
#include "TString.h"

#include "BSEmcCorrection.h"
#include "BSEmcRecoHit.h"

class BSEmcRecoHit;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcThetaEnergyECF
 * @brief Theta and Energy dependent energy correction factor
 * @details based on PndEmcClusterHistCalibrator code and data
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcThetaEnergyECF : public BSEmcCorrection {
 public:
  /**
   * @brief Apply corrections
   *
   * @param t_recoHit to be corrected RecoHit
   */
  virtual void Correct(BSEmcRecoHit *t_recoHit) const;
  void SetVersion(Int_t t_version) { fVersion = t_version; }
  Int_t GetVersion() const { return fVersion; }

 protected:
  virtual Double_t GetCorrectedEnergy(Double_t t_energy, Double_t t_thetaRad, Double_t t_phiRad) const /*override*/;
  void SetupLookUpMap(const TString &t_filename, const TString &t_histname);
  Bool_t FindTheBin(Float_t t_value_x, Float_t t_value_y, Int_t &t_bin_x, Int_t &t_bin_y) const;
  Double_t GetValueInZ(Float_t t_value_x, Float_t t_value_y) const;

  std::unique_ptr<TH2F> fHEnergyRatioPhoton{nullptr};

  Int_t fVersion{3};
  ClassDef(BSEmcThetaEnergyECF, 1);
};

#endif /*BSEMCTHETAENERGYECF_HH*/
