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

#include "PndMdtHit.h"
#include "PndDetectorList.h"

// -----   Default constructor   -------------------------------------------
PndMdtHit::PndMdtHit() : FairHit(), fDetID1(-1), fStripID(-1), fStripIndex(-1) {}

PndMdtHit::PndMdtHit(Int_t detID, TVector3 &pos, TVector3 &dpos, Int_t index) : FairHit(detID, pos, dpos, index), fDetID1(-1), fStripID(-1), fStripIndex(-1)
{
  SetLink(FairLink("MdtPoint", index));
}

PndMdtHit::PndMdtHit(Int_t detID, Int_t stripID, TVector3 &pos, TVector3 &dpos, Int_t bIndex, Int_t sIndex)
  : FairHit(detID, pos, dpos, bIndex), fDetID1(-1), fStripID(stripID), fStripIndex(sIndex)
{
  SetLink(FairLink("MdtDigiBox", bIndex));
  SetLink(FairLink("MdtDigiStrip", sIndex));
}

PndMdtHit::PndMdtHit(Int_t detID, Int_t detID1, TVector3 &pos, TVector3 &dpos, Int_t bIndex) : FairHit(detID, pos, dpos, bIndex), fDetID1(detID1), fStripID(-1), fStripIndex(-1)
{
  fStripID = GetStrip1();
  SetLink(FairLink("MdtDigiBox", bIndex));
  // SetLink(FairLink("MdtDigiStrip", sIndex));
}

/** Destructor **/
PndMdtHit::~PndMdtHit() {}

// -----   Public method Print   -------------------------------------------
void PndMdtHit::Print(const Option_t *opt) const
{
  std::cout << "Mdt hit in detector: Module " << GetModule() << " Sector " << GetSector() << " Layer " << GetLayerID() << " Box " << GetBox() << " Wire " << GetWire() << " Strip "
            << GetStrip() << " at (" << fX << ", " << fY << ", " << fZ << ") cm "
            << ", Point " << fRefIndex << " opt = " << opt << std::endl;
}

ClassImp(PndMdtHit)
