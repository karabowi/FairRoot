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

/** CbmstsMapsHitInfo source file
 *@author Michael Deveaux <m.deveaux@gsi.de>
 ** Acknowledgements to M. Al-Turany, D. Bertini, G. Gaycken
 ** Version beta 0.1 (02.02.2005)
 **/

#include "PndFtsHitInfo.h"

/** Constructor **/
PndFtsHitInfo::PndFtsHitInfo() : fFileNumber(-1), fEventNumber(-1), fTrackID(-1), fPointID(-1), fNMerged(0), fIsFake(kFALSE) {}

/** Constructor with all parameters **/
PndFtsHitInfo::PndFtsHitInfo(Int_t fileNumber, Int_t eventNumber, Int_t trackID, Int_t pointID, Int_t nMerged, Bool_t isFake)
  : fFileNumber(fileNumber), fEventNumber(eventNumber), fTrackID(trackID), fPointID(pointID), fNMerged(nMerged), fIsFake(isFake)
{
}

/** Destructor **/
PndFtsHitInfo::~PndFtsHitInfo() {}

/** Public method Clear **/
void PndFtsHitInfo::Clear()
{
  fFileNumber = -1;
  fEventNumber = -1;
  fTrackID = -1;
  fPointID = -1;
  fNMerged = 0;
  fIsFake = kFALSE;
}

ClassImp(PndFtsHitInfo);
