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

/** CbmStsMapsHit
 *@author Michael Deveaux <m.deveaux@gsi.de>
 **Acknowledgements to M. Al-Turany, D. Bertini, G. Gaycken
 **Version beta 0.1 (02.02.2005)
 **/

#include "PndSttSkewedHit.h"

#include "PndDetectorList.h"

#include <iostream>
#include "TMath.h"

using namespace std;

/** Default constructor **/
PndSttSkewedHit::PndSttSkewedHit() : FairHit(), fTubeIDs() {}

PndSttSkewedHit::PndSttSkewedHit(Int_t detID, Int_t tubeID1, Int_t tubeID2, Int_t mcindex, TVector3 &pos, TVector3 &dpos) : FairHit(detID, pos, dpos, mcindex), fTubeIDs()
{
  SetTubeIDs(tubeID1, tubeID2);
}

/** Destructor **/
PndSttSkewedHit::~PndSttSkewedHit() {}

ClassImp(PndSttSkewedHit)
