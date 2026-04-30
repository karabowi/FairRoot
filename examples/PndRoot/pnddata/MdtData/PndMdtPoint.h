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

#ifndef PNDMDTPOINT_H
#define PNDMDTPOINT_H

#include "TObject.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TMath.h"
#include "PndMdtID.h"
#include "PndMCPoint.h"

class PndMdtPoint : public PndMCPoint {

 public:
  /** Default constructor **/
  PndMdtPoint();

  /** Constructor with arguments
   *@param Int_t fEventID		The ID of the event
   *@param Int_t fTrackID		The ID of the track
   *@param Int_t fDetectorID    	The ID of the detector
   *@param TVector3 fPosIn        The position of the hit [cm] at the exitentrance point
   *@param TVector3 fMomIn	The momentum of the particle giving the hit [GeV] at the exitentrance point
   *@param TVector3 fPos		The position of the hit [cm] at the exit point
   *@param TVector3 fMom		The momentum of the particle giving the hit [GeV] at the exit point
   *@param Double_t fELoss		contains the total energy at the layer [GeV] because it is omitted in pars

   **/

  /** Initializing constructor - see the above order **/
  PndMdtPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Double_t eLoss, TVector3 pos_in, TVector3 mom_in);

  /** Copy constructor **/
  // PndMdtPoint(const PndMdtPoint& point) { *this = point; };

  /** Destructor **/
  virtual ~PndMdtPoint();

  /** Accessors **/
  inline TVector3 GetPosIn() const { return GetPosition(); }
  inline TVector3 GetMomIn() const { return GetMomentum(); }
  inline TVector3 GetPosOut() const { return GetPositionOut(); }
  inline TVector3 GetMomOut() const { return GetMomentumOut(); }

  Short_t GetModule() const { return PndMdtID::Module(GetDetectorID()); }
  Short_t GetSector() const { return PndMdtID::Sector(GetDetectorID()); }
  Short_t GetLayerID() const { return PndMdtID::Layer(GetDetectorID()); }
  Short_t GetBox() const { return PndMdtID::Box(GetDetectorID()); }
  Short_t GetWire() const { return PndMdtID::Wire(GetDetectorID()); }

  /** Modifiers **/

  /** Output to screen **/
  virtual void Print(const Option_t *opt) const;

private:
    double fRKValue{0.};

  ClassDef(PndMdtPoint, 3)
};

#endif /* !PNDMDTPOINT_H */
