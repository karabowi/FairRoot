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

#ifndef BSEMCPSAFPGABLINDDECONV_HH
#define BSEMCPSAFPGABLINDDECONV_HH

#include "RtypesCore.h"

#include <vector>
#include <string>
#include "TH1.h"



/**
 * @class BSEmcPSAFPGABlindDeconv
 * @brief Deconvolution of waveforms based on a chosen target function (gaussian)
 * @details waveforms are numerically convolved with function H derived via dividing gaussian by mean waveform in fourier space
 * @author Celina Frenkel <frenkel@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2022-04
 * @ingroup PndEmc
 */

class BSEmcPSAFPGABlindDeconv {
 public:
  BSEmcPSAFPGABlindDeconv();
  ~BSEmcPSAFPGABlindDeconv();
  void set(const TString &t_filename = "GlobalH_Gaus10_Final.root");
  std::vector<Double_t> put(const std::vector<Double_t> &t_buffer) const;

 private:
  std::vector<Double_t> fDataDeconv{};
  Int_t fNumberOfXBins{-1};
};

#endif /*BSEMCPSAFPGAFILTERWD_HH*/
