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

// ------------------------------------------------------------------------
// -----                     CbmHypGepoint header file                  -----

// ------------------------------------------------------------------------

#ifndef PNDHYPGEPOINT_H
#define PNDHYPGEPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "FairMCPoint.h"

using namespace std;

class PndHypGePoint : public FairMCPoint {

 public:
  /** Default constructor **/
  PndHypGePoint();

  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param posIn    Ccoordinates at entrance to active volume [cm]
   *@param posOut   Coordinates at exit of active volume [cm]
   *@param momIn    Momentum of track at entrance [GeV]
   *@param momOut   Momentum of track at exit [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/

  PndHypGePoint(Int_t trackID, Int_t evtID, Int_t pdgCode, Double_t charge, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Double_t eLoss, Short_t copy);

  /** Copy constructor **/
  PndHypGePoint(const PndHypGePoint &point) { *this = point; };

  /** Destructor **/
  virtual ~PndHypGePoint();

  /** Accessors **/
  Int_t GetTrackID() const { return fTrackID; };
  Int_t GetEventID() const { return fEventID; };
  Int_t GetDetectorID() const { return fnCopy; };
  Double_t GetX() const { return fX; };
  Double_t GetY() const { return fY; };
  Double_t GetZ() const { return fZ; };
  Double_t GetPx() const { return fPx; };
  Double_t GetPy() const { return fPy; };
  Double_t GetPz() const { return fPz; };
  Double_t GetTime() const { return fTime; };
  Double_t GetLength() const { return fLength; };
  Double_t GetEnergyLoss() const { return fELoss; };
  Double_t GetpdgCode() const { return fpdgCode; };
  void Position(TVector3 &pos) { pos.SetXYZ(fX, fY, fZ); };
  void Momentum(TVector3 &mom) { mom.SetXYZ(fPx, fPy, fPz); };

  // Int_t GetCopy()        const { return ();};
  /** Modifiers **/
  void SetTrackID(Int_t id) { fTrackID = id; };
  void SetEventID(Int_t id) { fEventID = id; };
  void SetDetectorID(Short_t copy) { fnCopy = copy; };
  void SetTime(Double_t time) { fTime = time; };
  void SetLength(Double_t length) { fLength = length; };
  void SetEnergyLoss(Double_t eLoss) { fELoss = eLoss; };
  void SetpdgCode(Int_t pdgCode) { fpdgCode = pdgCode; };
  void SetPosition(const TVector3 &pos);
  void SetMomentum(const TVector3 &mom);

  /** Output to screen **/
  virtual void Print(const Option_t *opt) const;

 protected:
  Int_t fEventID;      // Event ID
  TLorentzVector fPos; //  position
  TLorentzVector fMom; //  momentum
  Double_t fELoss, fmass, fcharge, fTime;
  Short_t fnCopy; // Copy number
  Int_t fpdgCode;

  ClassDef(PndHypGePoint, 1)
};

inline void PndHypGePoint::SetPosition(const TVector3 &pos)
{
  fX = pos.X();
  fY = pos.Y();
  fZ = pos.Z();
}

inline void PndHypGePoint::SetMomentum(const TVector3 &mom)
{
  fPx = mom.Px();
  fPy = mom.Py();
  fPz = mom.Pz();
}

#endif
