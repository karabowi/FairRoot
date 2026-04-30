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

// -------------------------------------------------------------------------
// -----                       PndRichMirrorSegment source file        -----
// -----               Created 22/07/16  by Beloborodov Konstantin     -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include <iostream>

using std::cout;
using std::endl;

#include "PndRichMirrorSegment.h"

// -----   Default constructor   -------------------------------------------
PndRichMirrorSegment::PndRichMirrorSegment() : fMiddleFlatMirrorPoint(TVector3(0, 0, 0)), fSizeOfFlatMirror(TVector3(0, 0, 0)), fNormalOfFlatMirror(TVector3(0, 0, 0)) {}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------

PndRichMirrorSegment::PndRichMirrorSegment(TVector3 point, TVector3 dims, TVector3 normal) : fMiddleFlatMirrorPoint(point), fSizeOfFlatMirror(dims), fNormalOfFlatMirror(normal) {}

// -----   Destructor   ----------------------------------------------------
PndRichMirrorSegment::~PndRichMirrorSegment() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndRichMirrorSegment::Print(const Option_t *opt) const
{
  //  cout << "RICH Photon hit: Time = " << fTime << endl;
  cout << " opt=" << opt << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndRichMirrorSegment)
