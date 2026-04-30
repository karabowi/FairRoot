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

#ifndef PNDMDTID_HH
#define PNDMDTID_HH

#include "Rtypes.h"

class PndMdtID {
 public:
  // provide an unique number for a wire
  static Int_t Identifier(Int_t iMod, Int_t iOct, Int_t iLayer, Int_t iBox, Int_t iWire) { return iWire + 10 * iBox + 10000 * iLayer + 1000000 * iOct + 10000000 * iMod; }
  // provide an unique number for a strip
  static Int_t Identifier(Int_t iMod, Int_t iOct, Int_t iLayer, Int_t iStrip) { return (iStrip + 2000) + (10000 * iLayer + 1000000 * iOct + 10000000 * iMod); }
  // provide an unique number for a layer
  static Int_t LayerID(Int_t iMod, Int_t iOct, Int_t iLayer) { return 10000 * iLayer + 1000000 * iOct + 10000000 * iMod; }
  static Int_t LayerID(Int_t detID) { return detID / 10000 * 10000; }
  // inverse operation
  static Short_t Module(Int_t detID) { return (detID / 10000000); }
  static Short_t Sector(Int_t detID) { return ((detID / 1000000) % 10); }
  static Short_t Layer(Int_t detID) { return ((detID / 10000) % 100); }
  static Short_t Box(Int_t detID) { return ((detID / 10) % 1000); }
  static Short_t Wire(Int_t detID) { return (detID % 10); }
  static Short_t Strip(Int_t detID) { return (detID % 10000 - 2000); }
  static Bool_t isWire(Int_t detID) { return detID % 10000 < 2000; }
  static Bool_t isStrip(Int_t detID) { return detID % 10000 >= 2000; }

 private:
  PndMdtID();
  ~PndMdtID();
};

#endif
