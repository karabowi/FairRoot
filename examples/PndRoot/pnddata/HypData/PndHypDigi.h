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

// --------------------------------------------------------
// ----			MvdDigi header file	---
// ----			Created 19.10.07 by T.Stockmanns ---
// --------------------------------------------------------

/** MvdDigi.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** \brief Base class for Digi information
 **
 **
 **/

#ifndef PNDHYPDIGI_H
#define PNDHYPDIGI_H

#include "TObject.h"
#include "TString.h"
#include <iostream>
#include <vector>

class PndHypDigi : public TObject {

  friend std::ostream &operator<<(std::ostream &out, PndHypDigi &digi)
  {
    out << "Hyp Digi in: " << digi.GetDetName() << " FE: " << digi.GetFE() << " "
        << " charge " << digi.GetCharge() << " e";
    std::vector<Int_t> indices = digi.GetIndices();
    for (unsigned int i = 0; i < indices.size(); i++) {
      std::cout << digi.GetIndex(i) << " " << std::endl;
    }
    return out;
  }

 public:
  PndHypDigi();
  PndHypDigi(Int_t index, Int_t detID, TString detName, Int_t fe, Double_t charge);
  /**<constructor
   * \param index position of PndHypMCPoint in TClonesArray
   * \param detID detector ID (not used for MVD)
   * \param detName geoH->GetID(gGeoManager->GetPath()) information
   * \param fe front end ID
   * \param charge deposited charge in electrons
   */

  virtual ~PndHypDigi(){};

  Int_t GetFE() const { return fFE; }
  TString GetDetName() const { return fDetName; }
  Double_t GetCharge() const { return fCharge; }
  Int_t GetDetID() const { return fDetID; }
  std::vector<Int_t> GetIndices() const { return fIndex; }
  Int_t GetIndex(int i = 0) const { return fIndex[i]; }

  void AddIndex(int index) { fIndex.push_back(index); }
  void AddCharge(double charge) { fCharge += charge; }

  virtual void print() { std::cout << *this; }

 protected:
  std::vector<Int_t> fIndex;
  Int_t fDetID;
  TString fDetName;
  Double_t fCharge;
  Int_t fFE;

  ClassDef(PndHypDigi, 1);
};

#endif
