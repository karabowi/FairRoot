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
// ----	       PndSdsDigiTopix4Header header file	---
// ----	       Created 21.11.14 by S.Esch               ---
// --------------------------------------------------------

/** PndSdsDigiTopix4Header.h
 *@author S.Esch <s.esch@fz-juelich.de>
 ** \brief Data class to store the header of topix4 frames
 **/

#ifndef PndSdsDigiTopix4Header_H
#define PndSdsDigiTopix4Header_H

#include "PndSdsDigiPixel.h"
#include "PndDetectorList.h"

#include "TObject.h"
#include "TString.h"
#include <iostream>
#include <vector>

class PndSdsDigiTopix4Header : public TObject {
  friend std::ostream &operator<<(std::ostream &out, PndSdsDigiTopix4Header &digi)
  {
    out << "PndSdsDigiTopix4Header : Frame Count" << digi.GetFrameCount() << " FE: " << digi.GetFE() << " Chip Address " << digi.GetChipAddress() << " Error Correction Code "
        << digi.GetECC() << " Frame Count Independent " << digi.GetFrameCountIndependent() << std::endl;

    return out;
  }

 public:
  PndSdsDigiTopix4Header();
  PndSdsDigiTopix4Header(UInt_t framecount, Int_t fe, UInt_t chipaddress, UInt_t ecc, UInt_t framecountindependent, Int_t deltaframecount, UInt_t numberofevents,
                         UInt_t numberofeventsinframe);

  PndSdsDigiTopix4Header &operator=(const PndSdsDigiTopix4Header &pix)
  {

    SetFrameCount(pix.GetFrameCount());
    SetFE(pix.GetFE());
    SetChipAddress(pix.GetChipAddress());
    SetECC(pix.GetECC());
    SetFrameCountIndependent(pix.GetFrameCountIndependent());
    SetDeltaFrameCount(pix.GetDeltaFrameCount());
    SetNumberOfEvents(pix.GetNumberOfEvents());
    SetNumberOfEventsInFrame(pix.GetNumberOfEventsInFrame());
    return *this;
  }

  UInt_t GetFrameCount() const { return fFrameCount; }
  UInt_t GetFE() const { return fFE; }
  UInt_t GetChipAddress() const { return fChipAddress; }
  UInt_t GetECC() const { return fECC; }
  UInt_t GetFrameCountIndependent() const { return fFrameCountIndependent; }
  Int_t GetDeltaFrameCount() const { return fDeltaFrameCount; }
  UInt_t GetNumberOfEvents() const { return fNumberOfEvents; }
  UInt_t GetNumberOfEventsInFrame() const { return fNumberOfEvents; }

  void SetFrameCount(UInt_t fc) { fFrameCount = fc; }
  void SetFE(Int_t fe) { fFE = fe; }
  void SetChipAddress(UInt_t ca) { fChipAddress = ca; }
  void SetECC(UInt_t ecc) { fECC = ecc; }
  void SetFrameCountIndependent(UInt_t fc) { fFrameCountIndependent = fc; }
  void SetDeltaFrameCount(Int_t dfc) { fDeltaFrameCount = dfc; }
  void SetNumberOfEvents(UInt_t noe) { fNumberOfEvents = noe; }
  void SetNumberOfEventsInFrame(UInt_t noe) { fNumberOfEventsInFrame = noe; }

  ~PndSdsDigiTopix4Header(){};

  void Print() { std::cout << *this; }

 private:
  UInt_t fFrameCount;
  Int_t fFE;
  UInt_t fChipAddress;
  UInt_t fECC;
  UInt_t fFrameCountIndependent;
  Int_t fDeltaFrameCount;
  UInt_t fNumberOfEvents;
  UInt_t fNumberOfEventsInFrame;

  ClassDef(PndSdsDigiTopix4Header, 1);
};

#endif
