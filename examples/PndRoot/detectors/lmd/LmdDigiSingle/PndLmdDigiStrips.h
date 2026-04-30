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

/*
 * PndLmdDigiStrips.h
 *
 *  Created on: Aug 9, 2009
 *      Author: huagen
 */

#ifndef PNDLMDDIGISTRIPS_H_
#define PNDLMDDIGISTRIPS_H_

#include "TObject.h"
#include <iostream>
#include "TString.h"
#include <vector>
using namespace std;

// define the class for the digis data, which include detName, detID
// FE numbers, FE channels, charge of the strip
// it shall be registered as a new data type
class PndLmdDigiStrips : public TObject {
 public:
  PndLmdDigiStrips();
  PndLmdDigiStrips(Int_t index, Int_t detID, TString detName, Int_t fe, Int_t channel, Double_t charge);
  //@param index is the position of PndLmdMCPoint in TClonesArray
  //@param detName is the sensor name
  //@param detID is the detID
  //@param FE is the FE module number
  //@param channel is the channel number in the FE moudule
  //@param charge is the charge of the related channel
  ~PndLmdDigiStrips(){};

  // public method
  std::vector<Int_t> GetIndex() const { return fIndex; }
  Int_t GetFe() const { return fFE; }
  Int_t GetChannel() const { return fChannel; }
  Double_t GetCharge() const { return fCharge; }
  TString GetDetName() const { return fdetName; }
  Int_t GetDetID() const { return fdetID; }

 private:
  TString fdetName;
  //	Int_t fIndex;
  std::vector<Int_t> fIndex; //??
  Int_t fdetID;
  Int_t fFE;
  Int_t fChannel;
  Double_t fCharge;

  ClassDef(PndLmdDigiStrips, 1);
};

#endif /* PNDLMDDIGISTRIPS_H_ */
