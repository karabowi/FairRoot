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

#include "PndSdsDigiTopix4Header.h"

PndSdsDigiTopix4Header::PndSdsDigiTopix4Header()
  : fFrameCount(0), fFE(0), fChipAddress(0), fECC(0), fFrameCountIndependent(0), fDeltaFrameCount(0), fNumberOfEvents(0), fNumberOfEventsInFrame(0)
{
}

PndSdsDigiTopix4Header::PndSdsDigiTopix4Header(UInt_t framecount, Int_t fe, UInt_t chipaddress, UInt_t ecc, UInt_t framecountindependent, Int_t deltaframecount,
                                               UInt_t numberofevents, UInt_t numberofeventsinfame)
  : fFrameCount(framecount), fFE(fe), fChipAddress(chipaddress), fECC(ecc), fFrameCountIndependent(framecountindependent), fDeltaFrameCount(deltaframecount),
    fNumberOfEvents(numberofevents), fNumberOfEventsInFrame(numberofeventsinfame)
{
}

ClassImp(PndSdsDigiTopix4Header);
