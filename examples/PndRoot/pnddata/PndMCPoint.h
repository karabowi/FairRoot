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

/**
 ** @class PndMCPoint
 ** Base class to handle MC data for points. Derived from FairMCPoint to add missing out position and momentum
 **
 ** @date 15.11.2022
 ** @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 **/

#ifndef PNDDATA_PNDMCPOINT_H_
#define PNDDATA_PNDMCPOINT_H_

#include <FairMCPoint.h>

class PndMCPoint : public FairMCPoint {
 public:
  PndMCPoint();
  virtual ~PndMCPoint();

  PndMCPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 posOut, TVector3 mom, TVector3 momOut, Double_t tof, Double_t length, Double_t eLoss, UInt_t EventId = 0);

  Double_t GetXOut() const { return fXOut; };
  Double_t GetYOut() const { return fYOut; };
  Double_t GetZOut() const { return fZOut; };

  Double_t GetPxOut() const { return fPxOut; }
  Double_t GetPyOut() const { return fPyOut; }
  Double_t GetPzOut() const { return fPzOut; }

  TVector3 GetPosition() const { return TVector3(fX, fY, fZ); }
  TVector3 GetPositionOut() const { return TVector3(fXOut, fYOut, fZOut); }
  TVector3 GetMeanPosition() const { return TVector3((fX + fXOut) / 2., (fY + fYOut) / 2., (fZ + fZOut) / 2.0); }
  void PositionOut(TVector3 &pos) const { pos.SetXYZ(fXOut, fYOut, fZOut); };

  TVector3 GetMomentum() const { return TVector3(fPx, fPy, fPz); }
  TVector3 GetMomentumOut() const { return TVector3(fPxOut, fPyOut, fPzOut); }
  void MomentumOut(TVector3 &mom) const { mom.SetXYZ(fPxOut, fPyOut, fPzOut); }

  void SetPositionOut(TVector3 pos);
  void SetMomentumOut(TVector3 mom);

 protected:
  Double32_t fXOut = 0.;
  Double32_t fYOut = 0.;
  Double32_t fZOut = 0.;

  Double32_t fPxOut = 0.;
  Double32_t fPyOut = 0.;
  Double32_t fPzOut = 0.;

  ClassDef(PndMCPoint, 1);
};

inline void PndMCPoint::SetPositionOut(TVector3 pos)
{
  fXOut = pos.X();
  fYOut = pos.Y();
  fZOut = pos.Z();
}

inline void PndMCPoint::SetMomentumOut(TVector3 mom)
{
  fPxOut = mom.Px();
  fPyOut = mom.Py();
  fPzOut = mom.Pz();
}

#endif /* PNDDATA_PNDMCPOINT_H_ */
