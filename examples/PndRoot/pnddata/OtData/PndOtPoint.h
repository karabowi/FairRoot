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

#ifndef PNDOTPOINT_H
#define PNDOTPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "FairMCPoint.h"

class PndOtPoint : public FairMCPoint {

 public:
  /** Default constructor **/
  PndOtPoint();

  /** Constructor with arguments
   *@param trackID       Index of MCTrack
   *@param detID         Detector ID
   *tubeID               Number of tube
   *ChamberID            number of chamber
   *LayerID
   *@param pos           Coordinates at wire center of active volume [cm]
   *@param posInLocal    Local coordinates at entrance to active volume [cm]
   *@param posOutLocal   Local coordinates at exit of active volume [cm]
   *@param momIn         Momentum of track at entrance [GeV]
   *@param momOut        Momentum of track at exit [GeV]
   *@param tof           Time since event start [ns]
   *@param length        Track length since creation [cm]
   *@param eLoss         Energy deposit [GeV]
   **/
  PndOtPoint(Int_t trackID, Int_t detID, Int_t tubeID, Int_t chamberID, Int_t layerID, TVector3 pos, TVector3 posInLocal, TVector3 posOutLocal, TVector3 momIn, TVector3 momOut,
              Double_t tof, Double_t length, Double_t eLoss, Double_t mass);

  /** Copy constructor **/
  PndOtPoint(const PndOtPoint &point);

  /** Destructor **/
  virtual ~PndOtPoint();

  // da cancellare
  Double_t GetXtot() const { return GetX(); }
  Double_t GetYtot() const { return GetY(); }
  Double_t GetZtot() const { return GetZ(); }

  Double_t GetPxOut() const { return fPx_out; }
  Double_t GetPyOut() const { return fPy_out; }
  Double_t GetPzOut() const { return fPz_out; }

  //////

  /** Accessors **/
  Double_t GetXOutLocal() const { return fX_out_local; }
  Double_t GetYOutLocal() const { return fY_out_local; }
  Double_t GetZOutLocal() const { return fZ_out_local; }
  Double_t GetXInLocal() const { return fX_in_local; }
  Double_t GetYInLocal() const { return fY_in_local; }
  Double_t GetZInLocal() const { return fZ_in_local; }

  // Double_t GetXWireDirection() const { return fX_wire_dir; }
  // Double_t GetYWireDirection() const { return fY_wire_dir; }
  // Double_t GetZWireDirection() const { return fZ_wire_dir; }

  Double_t GetMass() const { return fMass; }

  void PositionOutLocal(TVector3 &pos) { pos.SetXYZ(fX_out_local, fY_out_local, fZ_out_local); }
  void PositionInLocal(TVector3 &pos) { pos.SetXYZ(fX_in_local, fY_in_local, fZ_in_local); }
  void MomentumOut(TVector3 &mom) { mom.SetXYZ(fPx_out, fPy_out, fPz_out); }
  // void WireDirection(TVector3& wire) { wire.SetXYZ(fX_wire_dir, fY_wire_dir, fZ_wire_dir); }

  /** Modifiers **/
  void SetPositionOutLocal(TVector3 pos);
  void SetPositionInLocal(TVector3 pos);
  void SetMomentumOut(TVector3 mom);
  // void SetWireDirection(TVector3 wire);

  // tube ID // CHECK added
  void SetTubeID(Int_t tubeid) { fTubeID = tubeid; }
  Int_t GetTubeID() { return fTubeID; }
  void SetChamberID(Int_t chamberid) { fChamberID = chamberid; }
  void SetLayerID(Int_t layerid) { fLayerID = layerid; }
  Int_t GetChamberID() { return fChamberID; }
  Int_t GetLayerID() { return fLayerID; }

  /** Output to screen **/
  virtual void Print(const Option_t *opt) const;

 protected:
  // exit coordinates in straw frame
  Double_t fX_out_local, fY_out_local, fZ_out_local;
  // entry coordinates in straw frame
  Double_t fX_in_local, fY_in_local, fZ_in_local;

  Double_t fPx_out, fPy_out, fPz_out;
  Double_t fPx_in, fPy_in, fPz_in;
  // wire direction
  // Double_t fX_wire_dir, fY_wire_dir, fZ_wire_dir;

  // particle mass
  Double_t fMass;
  Int_t fTubeID;
  Int_t fChamberID;
  Int_t fLayerID;

  //////

  ClassDef(PndOtPoint, 1)
};

inline void PndOtPoint::SetPositionOutLocal(TVector3 pos)
{
  fX_out_local = pos.X();
  fY_out_local = pos.Y();
  fZ_out_local = pos.Z();
}

inline void PndOtPoint::SetPositionInLocal(TVector3 pos)
{
  fX_in_local = pos.X();
  fY_in_local = pos.Y();
  fZ_in_local = pos.Z();
}

inline void PndOtPoint::SetMomentumOut(TVector3 mom)
{
  fPx_out = mom.Px();
  fPy_out = mom.Py();
  fPz_out = mom.Pz();
}

#endif
