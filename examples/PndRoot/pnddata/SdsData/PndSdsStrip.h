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

#ifndef fSDSSTRIP_HH
#define fSDSSTRIP_HH

#include "FairGeoVector.h"
#include <iostream>
#include <vector>

//! Class representing strips on wafer-scale
/**
 * This is just a thumb representation of strips on a wafer
 * with index and assigned charge. Do not confuse with PndSdsDigiStrip, which represents
 * a digitised frontend-channel.
 **/
class PndSdsStrip {
 public:
  PndSdsStrip() : fNumber(-1), fCharge(-1.0) {}

  PndSdsStrip(Int_t nr, Double_t charge) : fNumber(nr), fCharge(charge) {}

  void SetIndex(Int_t nr) { fNumber = nr; }
  void SetCharge(Double_t charge) { fCharge = charge; }

  Int_t GetIndex() const { return fNumber; }
  Double_t GetCharge() const { return fCharge; }

  friend std::ostream &operator<<(std::ostream &out, const PndSdsStrip &strip)
  {
    out << " Strip Nr.: " << strip.GetIndex() << " "
        << " Charge: " << strip.GetCharge();
    return out;
  }

 private:
  Int_t fNumber;
  Double_t fCharge;
};

#endif // fSDSSTRIP_HH
