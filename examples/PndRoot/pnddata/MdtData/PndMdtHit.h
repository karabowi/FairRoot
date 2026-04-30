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

#ifndef PNDMDTHIT_HH
#define PNDMDTHIT_HH

#include "FairHit.h"
#include "TVector3.h"
#include "TMath.h"
#include "PndMdtID.h"

class PndMdtHit : public FairHit {

 public:
  PndMdtHit();

  PndMdtHit(Int_t detID, TVector3 &pos, TVector3 &dpos, Int_t index);
  PndMdtHit(Int_t detID, Int_t stripID, TVector3 &pos, TVector3 &dpos, Int_t bIndex, Int_t sIndex);

  PndMdtHit(Int_t detID, Int_t detID1, TVector3 &pos, TVector3 &dpos, Int_t bIndex);

  virtual ~PndMdtHit();

  /** Output to screen (not yet implemented) **/
  virtual void Print(const Option_t *opt = nullptr) const;

  /** Public method Clear
   ** Resets the flag to -1
   **/
  // void Clear();  // not implemented

  /** Accessors **/
  Short_t GetModule() const { return PndMdtID::Module(GetDetectorID()); }
  Short_t GetSector() const { return PndMdtID::Sector(GetDetectorID()); }
  Short_t GetLayerID() const { return PndMdtID::Layer(GetDetectorID()); }
  Short_t GetBox() const { return PndMdtID::Box(GetDetectorID()); }
  Short_t GetWire() const { return PndMdtID::Wire(GetDetectorID()); }
  Short_t GetStrip1() const { return PndMdtID::Strip(fDetID1); }
  Short_t GetStrip() const { return fStripID; };
  Int_t GetStripIndex() const { return fStripIndex; };

 private:
  Int_t fDetID1;
  Short_t fStripID;
  Int_t fStripIndex;

  ClassDef(PndMdtHit, 3);
};

#endif
