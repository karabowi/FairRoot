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

/** PndSdsHitInfo source file
 *@author
 ** Acknowledgements to M. Al-Turany, D. Bertini, G. Gaycken, Michael Deveaux <m.deveaux@gsi.de>
 ** Version beta 0.1 (02.02.2005)
 **/

#include "PndSdsHitInfo.h"

/** Constructor **/
PndSdsHitInfo::PndSdsHitInfo()
{
  fFileNumber = -1;
  fEventNumber = -1;
  fTrackID = -1;
  fPointID = -1;
  fNMerged = 0;
  fIsFake = kFALSE;
}

/** Constructor with all parameters **/
PndSdsHitInfo::PndSdsHitInfo(Int_t fileNumber, Int_t eventNumber, Int_t trackID, Int_t pointID, Int_t nMerged, Bool_t isFake)
{
  fFileNumber = fileNumber;
  fEventNumber = eventNumber;
  fTrackID = trackID;
  fPointID = pointID;
  fNMerged = nMerged;
  fIsFake = isFake;
}

/** Destructor **/
PndSdsHitInfo::~PndSdsHitInfo() {}

/** Public method Clear **/
void PndSdsHitInfo::Clear()
{
  fFileNumber = -1;
  fEventNumber = -1;
  fTrackID = -1;
  fPointID = -1;
  fNMerged = 0;
  fIsFake = kFALSE;
}

ClassImp(PndSdsHitInfo);
