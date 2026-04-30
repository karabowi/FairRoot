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

#ifndef PNDMDTDIGI_HH
#define PNDMDTDIGI_HH

//#include "FairMultiLinkedData.h"
#include "FairTimeStamp.h"
#include "TVector3.h"
#include <vector>
#include "PndMdtID.h"

class PndMdtDigi : public FairTimeStamp {

 public:
  PndMdtDigi();

  PndMdtDigi(Int_t detID, TVector3 &pos, std::vector<Int_t> pointList);
  PndMdtDigi(Int_t detID, TVector3 &pos, Int_t evtNo);

  virtual ~PndMdtDigi();

  /** Output to screen (not yet implemented) **/
  virtual void Print(const Option_t *opt = nullptr) const
  {
    std::cout << " opt = " << opt << std::endl;
    return;
  }

  bool operator<(const PndMdtDigi &) const;

  /** Public method Clear
   ** Resets the flag to -1
   **/
  // void Clear();  // not implemented

  /** Accessors **/
  void SetPosition(const TVector3 &pos) { fLabPos = pos; };

  Double_t X() const { return fLabPos.X(); }
  Double_t Y() const { return fLabPos.Y(); }
  Double_t Z() const { return fLabPos.Z(); }
  Int_t GetEvtNumber() const { return fEvtNo; }
  TVector3 GetLabPosition() const { return fLabPos; }
  Int_t GetDetectorID() const { return fDetectorID; }
  Short_t GetModule() const { return PndMdtID::Module(GetDetectorID()); }
  Short_t GetSector() const { return PndMdtID::Sector(GetDetectorID()); }
  Short_t GetLayer() const { return PndMdtID::Layer(GetDetectorID()); }
  Short_t GetLayerID() const { return PndMdtID::Layer(GetDetectorID()); }
  Short_t GetBox() const { return PndMdtID::Box(GetDetectorID()); }
  Short_t GetWire() const { return PndMdtID::Wire(GetDetectorID()); }
  Short_t GetStrip() const { return PndMdtID::Strip(GetDetectorID()); }
  Bool_t isWire() const { return PndMdtID::isWire(GetDetectorID()); }

  Short_t GetNPoints() const { return fPointList.size(); }
  const std::vector<Int_t> &GetPointList() { return fPointList; }

  // time-based simulation support
  PndMdtDigi(const PndMdtDigi &);

 private:
  Int_t fDetectorID;             // Detectior ID
  TVector3 fLabPos;              // Position in the la frame
  std::vector<Int_t> fPointList; // List of MdtPoint indices
  Double_t fSignalWidth;
  Int_t fEvtNo;

  ClassDef(PndMdtDigi, 2);
};

#endif
