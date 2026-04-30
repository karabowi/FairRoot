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

#ifndef BSEMCPSAFPGAPEAKFEATURES_T_HH
#define BSEMCPSAFPGAPEAKFEATURES_T_HH

struct BSEmcPSAFPGAPeakFeatures_t {
  Double_t Baseline{-1};
  Int_t MaximumValue{-1};
  Int_t MaximumBin{-1};
  Int_t Integral{-1};
  Int_t BinCounter{-1};
  Bool_t PileUp{kFALSE};
  Double_t PeakTime{-1};
  Double_t CFDTime{-1};
};

#endif /*BSEMCPSAFPGAPEAKFEATURES_T_HH*/
