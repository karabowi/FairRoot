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

/** PndTrkIndivisibleHit
 ** Class for pattern recognition hit
 ** @author Lia Lavezzi
 **
 ** fSensorID: identifies the specific sensor of the MVD,
 the specific tube of the STT
**/

#ifndef PNDTRKINDIVISIBLEHIT_H
#define PNDTRKINDIVISIBLEHIT_H 1

#include "PndTrkHit.h"
#include "TVector3.h"
#include "TArrayI.h"
#include "PndTrkParameters.h"
/* #include "TObject.h" */

class PndTrkHit;
class PndTrkIndivisibleHit : public PndTrkHit {

 public:
  PndTrkIndivisibleHit();
  PndTrkIndivisibleHit(TArrayI hitid, TVector3 &pos);
  PndTrkIndivisibleHit(const PndTrkIndivisibleHit &hit);

  ~PndTrkIndivisibleHit();

  // virtual Bool_t IsEqual(const TObject* obj) const {
  //     return (((PndTrkIndivisibleHit *) obj)->fDetectorID == fDetectorID) && (((PndTrkIndivisibleHit *) obj)->fHitID == fHitID);
  //   }

  //   virtual Bool_t IsSortable() { return !(fSortVariable == -1); }
  // Int_t Compare(const TObject *hit)  const;

  //  Bool_t operator==(const PndTrkIndivisibleHit & hit1);
  //   Bool_t operator<(const PndTrkIndivisibleHit & hit1);

  // CHECK add other set
  void SetSortVariable(Double_t sortvar) { fSortVariable = sortvar; }

  void SetPosition(TVector3 pos) { fPosition = pos; }
  void SetPhi(Double_t phi) { fPhi = phi; }

  // void SetUsedFlag(Bool_t used) { fUsed = used; }

  // inline get
  /*   inline Bool_t   IsUsed()            { return fUsed; } */
  inline TVector3 GetPosition() { return fPosition; }
  inline Double_t GetSortVariable() { return fSortVariable; }
  inline Double_t GetPhi() { return fPhi; }

  // info
  void Draw(Color_t color);
  void Print();

  // variables
  TArrayI fHitIDs;

  ClassDef(PndTrkIndivisibleHit, 1);
};

#endif
