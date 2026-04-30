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

#ifndef THPARTICLE_H
#define THPARTICLE_H

#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TString.h"

using namespace std;

class THParticle : public TParticle {

 public:
  /** Default constructor **/
  THParticle();

  THParticle(Int_t pdg, Int_t status, Int_t evtID, Int_t index, Double_t mass, Int_t A, Int_t Z, Int_t H, const TLorentzVector &p, const TLorentzVector &v);

  /** Copy constructor **/
  THParticle(const THParticle &point)
    : TParticle(point), fstatus(point.fstatus), fEventID(point.fEventID), fpdgCode(point.fpdgCode), fCalcMass(point.fCalcMass), fVx(point.fVx), fVy(point.fVy), fVz(point.fVz),
      fIndex(point.fIndex), fA(point.fA), fZ(point.fZ), fH(point.fH)
  {
    return;
  }

  /** Destructor **/
  virtual ~THParticle();

  /** Accessors **/
  Int_t GetPdgCode() const { return fpdgCode; }
  Int_t GetEventID() const { return fEventID; };
  Int_t GetIndex() const { return fIndex; };
  Double_t GetCalcMass() const { return fCalcMass; }

  Int_t GetA() const { return fA; };
  Int_t GetZ() const { return fZ; };
  Int_t GetH() const { return fH; };

  Double_t Vx() const { return fVx; };
  Double_t Vy() const { return fVy; };
  Double_t Vz() const { return fVz; };

  /** Modifiers **/

  void SetEventID(Int_t ev) { fEventID = ev; };
  void SetIndex(Int_t id) { fIndex = id; };

  /** Output to screen **/
  virtual void Print(const Option_t *opt) const;

 protected:
  Int_t fstatus;
  Int_t fEventID; // Event ID
  Int_t fpdgCode; // PDG code of the particle

  Double_t fCalcMass; //,fPx,fPy,fPz;             // Calculated mass
  Double_t fVx, fVy, fVz;

  Int_t fIndex, fA, fZ, fH;

  ClassDef(THParticle, 1)
};

#endif
