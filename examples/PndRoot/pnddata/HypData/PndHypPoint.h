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
// -----                     CbmHyppoint header file                  -----
// -----               Created by A.Sanchez                -----
// ------------------------------------------------------------------------

#ifndef PNDHYPPOINT_H
#define PNDHYPPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "FairMCPoint.h"
#include "TString.h"

using namespace std;

class PndHypPoint : public FairMCPoint {

 public:
  /** Default constructor **/
  PndHypPoint();

  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param posIn    Ccoordinates at entrance to active volume [cm]
   *@param posOut   Coordinates at exit of active volume [cm]
   *@param momIn    Momentum of track at entrance [GeV]
   *@param momOut   Momentum of track at exit [GeV]
   *@param posInLocal    Local coordinates at entrance to active volume [cm]
   *@param posOutLocal   Local coordinates at exit of active volume [cm]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/

  PndHypPoint(Int_t trackID, Int_t evtID, Int_t detID, TString detName, TVector3 posin, TVector3 momin, TVector3 posout, TVector3 momout, Double_t tof, Double_t length,
              Double_t eLoss, Double_t charge, Double_t mass, Int_t pdgCode, Double_t distance, Double_t PLin, Double_t PLout);

  /** Copy constructor **/
  PndHypPoint(const PndHypPoint &point)
    : FairMCPoint(point), fEventID(point.fEventID), fPLin(point.fPLin), fPLout(point.fPLout), fmass(point.fmass), fcharge(point.fcharge), fXin(point.fXin), fYin(point.fYin),
      fZin(point.fZin), fPxin(point.fPxin), fPyin(point.fPyin), fPzin(point.fPzin), fXout(point.fXout), fYout(point.fYout), fZout(point.fZout), fPxout(point.fPxout),
      fPyout(point.fPyout), fPzout(point.fPzout), fdist(point.fdist), fDetName(point.fDetName), fVolumeID(point.fVolumeID), fpdgCode(point.fpdgCode)
  {
    *this = point;
  };

  /** Destructor **/
  virtual ~PndHypPoint();

  /** Accessors **/
  // Int_t    GetTrackID()    const { return fTrackID; };
  Int_t GetEventID() const { return fEventID; };
  Int_t GetVolumeID() const { return fVolumeID; };
  Double_t GetXin() const { return fXin; };
  Double_t GetYin() const { return fYin; };
  Double_t GetZin() const { return fZin; };
  Double_t GetPxin() const { return fPxin; };
  Double_t GetPyin() const { return fPyin; };
  Double_t GetPzin() const { return fPzin; };

  /* Double_t GetXOutLocal()  const { return fXout_local; }; */
  /*   Double_t GetYOutLocal()  const { return fYout_local; }; */
  /*   Double_t GetZOutLocal()  const { return fZout_local; }; */

  /*   Double_t GetXInLocal()  const { return fXin_local; }; */
  /*   Double_t GetYInLocal()  const { return fYin_local; }; */
  /*   Double_t GetZInLocal()  const { return fZin_local; }; */

  Double_t GetPLin() const { return fPLin; };
  Double_t GetPLout() const { return fPLout; };

  Double_t GetXout() const { return fXout; };
  Double_t GetYout() const { return fYout; };
  Double_t GetZout() const { return fZout; };
  Double_t GetPxout() const { return fPxout; };
  Double_t GetPyout() const { return fPyout; };
  Double_t GetPzout() const { return fPzout; };
  Double_t Getdist() const { return fdist; };
  // Double_t Gettheta()         const { return ftheta; };

  TString GetDetName() const { return fDetName; };
  Double_t GetMass() const { return fmass; };
  // Double_t GetTime()       const { return fTime; };
  // Double_t GetLength()     const { return fLength; };
  // Double_t GetEnergyLoss() const { return fELoss; };

  void PositionIn(TVector3 &pos) { pos.SetXYZ(fXin, fYin, fZin); };
  void MomentumIn(TVector3 &mom) { mom.SetXYZ(fPxin, fPyin, fPzin); };

  void PositionOut(TVector3 &pos) { pos.SetXYZ(fXout, fYout, fZout); };
  void MomentumOut(TVector3 &mom) { mom.SetXYZ(fPxout, fPyout, fPzout); };

  /*   void PositionOutLocal(TVector3& pos) { pos.SetXYZ(fXout_local, */
  /* 						    fYout_local, */
  /* 						    fZout_local); }; */
  /*   void PositionInLocal(TVector3& pos)  { pos.SetXYZ(fXin_local, */
  /* 						    fYin_local, */
  /* 						    fZin_local); }; */

  /** Modifiers **/
  // void SetTrackID(Int_t id)          { fTrackID    = id; } ;
  void SetEventID(Int_t ev) { fEventID = ev; };
  // void SetTime(Double_t tof)        { fTime       = tof; };
  // void SetLength(Double_t length)    { fLength     = length; };
  // void SetEnergyLoss(Double_t eLoss) { fELoss      = eLoss; };

  void SetPositionIn(const TVector3 &pos);
  void SetMomentumIn(const TVector3 &mom);
  void SetPositionOut(const TVector3 &pos);
  void SetMomentumOut(const TVector3 &mom);

  /*  void SetPositionOutLocal(const TVector3& pos); */
  /*   void SetPositionInLocal(const TVector3& pos); */

  /** Output to screen **/
  virtual void Print(const Option_t *opt) const;

 protected:
  Int_t fEventID; // Event ID

  Double_t fPLin, fPLout; //  momentum
  // Int_t fTrackID ;
  Double_t fmass, fcharge;

  Double_t fXin, fYin, fZin;
  Double_t fPxin, fPyin, fPzin;
  Double_t fXout, fYout, fZout;
  Double_t fPxout, fPyout, fPzout, fdist;
  /*  Double_t  fXout_local,  fYout_local,  fZout_local; */
  /*   Double_t fXin_local,  fYin_local,  fZin_local; */
  TString fDetName;

  Int_t fVolumeID, fpdgCode;

  ClassDef(PndHypPoint, 7)
};

inline void PndHypPoint::SetPositionIn(const TVector3 &pos)
{
  fXin = pos.X();
  fYin = pos.Y();
  fZin = pos.Z();
}

inline void PndHypPoint::SetMomentumIn(const TVector3 &mom)
{
  fPxin = mom.X();
  fPyin = mom.Py();
  fPzin = mom.Pz();
}

inline void PndHypPoint::SetPositionOut(const TVector3 &pos)
{
  fXout = pos.X();
  fYout = pos.Y();
  fZout = pos.Z();
}

inline void PndHypPoint::SetMomentumOut(const TVector3 &mom)
{
  fPxout = mom.Px();
  fPyout = mom.Py();
  fPzout = mom.Pz();
}

/* inline void PndHypPoint::SetPositionOutLocal(const TVector3& pos) { */
/*   fXout_local = pos.X(); */
/*   fYout_local = pos.Y(); */
/*   fZout_local = pos.Z(); */
/* } */

/* inline void PndHypPoint::SetPositionInLocal(const TVector3& pos) { */
/*   fXin_local = pos.X(); */
/*   fYin_local = pos.Y(); */
/*   fZin_local = pos.Z(); */
/* } */

#endif
