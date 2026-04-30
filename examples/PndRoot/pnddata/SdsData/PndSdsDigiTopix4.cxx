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

#include "PndSdsDigiTopix4.h"

PndSdsDigiTopix4::PndSdsDigiTopix4() : PndSdsDigiPixel(), fLeadingEdge(0), fTrailingEdge(0), fFrameCount(0), fHitCountCorrect(0), fHitCountIndependent(0), fTimeStampIndependent(0.)
{
}

PndSdsDigiTopix4::PndSdsDigiTopix4(std::vector<Int_t> index, Int_t detID, Int_t sensorID, Int_t fe, Int_t col, Int_t row, UInt_t leadingEdge, UInt_t trailingEdge,
                                   UInt_t frameCount, Double_t timeStamp, UInt_t hitCountCorrect, UInt_t hitCountIndependent, Double_t timeStampIndependent)
  : PndSdsDigiPixel(index, detID, sensorID, fe, col, row, 0, timeStamp), fLeadingEdge(leadingEdge), fTrailingEdge(trailingEdge), fFrameCount(frameCount),
    fHitCountCorrect(hitCountCorrect), fHitCountIndependent(hitCountIndependent), fTimeStampIndependent(timeStampIndependent)
{
  Double_t charge = 0;
  if (leadingEdge < trailingEdge) {
    charge = trailingEdge - leadingEdge;
  } else {
    charge = (trailingEdge + 4096) - leadingEdge;
  }
  SetCharge(charge);
}

ClassImp(PndSdsDigiTopix4);
