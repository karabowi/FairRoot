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

#ifndef BSEMCPSAFPGALINFITTER_HH
#define BSEMCPSAFPGALINFITTER_HH
#include "RtypesCore.h"

class BSEmcPSAFPGALinFitter {
 public:
  BSEmcPSAFPGALinFitter();
  ~BSEmcPSAFPGALinFitter();
  void reset();
  void putPoint(Double_t t_x, Double_t t_y);
  void fit();
  Double_t offset();
  Double_t slope();
  Double_t average();
  Double_t averageX();

 private:
  Double_t x{0};
  Double_t xx{0};
  Double_t xy{0};
  Double_t y{0};
  Double_t a{0};
  Double_t k{0};
  UInt_t Np{0};
};

#endif /*BSEMCPSAFPGALINFITTER_HH*/
