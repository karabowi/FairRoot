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
// ----			PndSdsDigiTopix4 header file	---
// ----			Created 19.6.06 by T.Stockmanns ---
// --------------------------------------------------------

/** PndSdsDigiTopix4.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 ** \brief Data class to store the digi output of a pixel module
 **
 ** \sa PndSdsDigi.h
 **/

#ifndef PndSdsDigiTopix4_H
#define PndSdsDigiTopix4_H

#include "PndSdsDigiPixel.h"
#include "PndDetectorList.h"

#include "TObject.h"
#include "TString.h"
#include <iostream>
#include <vector>

class PndSdsDigiTopix4 : public PndSdsDigiPixel {
  friend std::ostream &operator<<(std::ostream &out, PndSdsDigiTopix4 &digi)
  {
    out << "PndSdsDigiTopix4 in Sensor: " << digi.GetSensorID() << " FE: " << digi.GetFE() << " Col/Row " << digi.GetPixelColumn() << "/" << digi.GetPixelRow() << " charge "
        << digi.GetCharge() << " e"
        << " timestamp " << digi.GetTimeStamp() << " leadingEdge " << digi.GetLeadingEdge() << " trailingEdge " << digi.GetTrailingEdge() << " frameCount " << digi.GetFrameCount()
        << std::endl;

    // out << std::endl;

    return out;
  }

 public:
  PndSdsDigiTopix4();
  PndSdsDigiTopix4(std::vector<Int_t> index, Int_t detID, Int_t sensorID, Int_t fe, Int_t col, Int_t row, UInt_t leadingEdge, UInt_t trailingEdge, UInt_t frameCount,
                   Double_t timeStamp, UInt_t hitcountcorrect, UInt_t hitCounterIndependent, Double_t timeStampIndependent);

  UInt_t GetLeadingEdge() const { return fLeadingEdge; }
  UInt_t GetTrailingEdge() const { return fTrailingEdge; }
  UInt_t GetFrameCount() const { return fFrameCount; }
  Double_t GetTimeStampIndependent() const { return fTimeStampIndependent; }
  UInt_t GetHitCountIndependent() const { return fHitCountIndependent; }
  UInt_t GetHitCountCorrect() const { return fHitCountCorrect; }

  void SetLeadingEdge(UInt_t le) { fLeadingEdge = le; }
  void SetTrailingEdge(UInt_t te) { fTrailingEdge = te; }
  void SetFrameCount(UInt_t fc) { fFrameCount = fc; }
  void SetHitCountIndependent(UInt_t hci) { fHitCountIndependent = hci; }
  void SetTimeStampIndependent(Double_t tsi) { fTimeStampIndependent = tsi; }
  void SetHitCountCorrect(UInt_t hcc) { fHitCountCorrect = hcc; }

  ~PndSdsDigiTopix4(){};

  void Print() { std::cout << *this; }

  PndSdsDigiTopix4 &operator=(const PndSdsDigiTopix4 &pix)
  {
    if (this != &pix) {
      this->PndSdsDigiPixel::operator=(pix);
    }
    SetLeadingEdge(pix.GetLeadingEdge());
    SetTrailingEdge(pix.GetTrailingEdge());
    SetFrameCount(pix.GetFrameCount());
    SetHitCountIndependent(pix.GetHitCountIndependent());
    SetTimeStampIndependent(pix.GetTimeStampIndependent());
    SetHitCountCorrect(pix.GetHitCountCorrect());
    return *this;
  }

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar &boost::serialization::base_object<PndSdsDigiPixel>(*this);
    ar &fLeadingEdge;
    ar &fTrailingEdge;
    ar &fFrameCount;
  }

 private:
#ifndef __CINT__ // for BOOST serialization
  friend class boost::serialization::access;

#endif // for BOOST serialization
  UInt_t fLeadingEdge;
  UInt_t fTrailingEdge;
  UInt_t fFrameCount;
  UInt_t fHitCountCorrect;
  UInt_t fHitCountIndependent;
  Double_t fTimeStampIndependent;

  ClassDef(PndSdsDigiTopix4, 3);
};

#endif
