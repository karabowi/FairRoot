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

// --------------------------------------------------------
// ----			MvdDigiStrip header file	                  ---
// ----			Created 10.07.07 by Ralf Kliemt             ---
// --------------------------------------------------------

/** MvdDigiStrip.h
*@author R.Kliemt <r.kliemt@physik.tu-dresden.de>
**
++ An addition to the MvdHit which only stores the backcalculated
** 3d-space point this class stores the fired strips
**/

#ifndef PNDHYPDIGISTRIP_H
#define PNDHYPDIGISTRIP_H

#include "TObject.h"
#include "TString.h"
#include <iostream>

#include "PndHypDigi.h"
// class PndHypStrip;

class PndHypDigiStrip : public PndHypDigi {
 public:
  PndHypDigiStrip();
  PndHypDigiStrip(Int_t index, Int_t detID, TString detName, Int_t fe, Int_t chan, Double_t charge);
  ~PndHypDigiStrip(){};

  // Int_t GetIndex() const { return fIndex; }
  // Int_t GetDetID() const { return fDetID;}
  // TString GetDetName() const { return fDetName; }
  // Int_t GetFE() const { return fFE; }
  Int_t GetChannel() const { return fChannel; }
  // Double_t GetCharge() const { return fCharge; }
  // Int_t GetMCID() const { return fMCID; }
  // SensorSide GetSensorSide() const {return fSide;};

  // void SetSensorSide(SensorSide s) {fSide = s;}
  /* void SetIndex ( Int_t index ) { fIndex = index; } */
  /*         void SetDetID ( Int_t detID ) { fDetID = detID; } */
  /*         void SetDetName ( const TString& detName ) { fDetName = detName; } */
  /*         void SetFE ( Int_t fe ) { fFE = fe; } */
  /*         void SetChannel ( Int_t channel ) { fChannel = channel; } */
  /*         void SetCharge ( Double_t charge ) { fCharge = charge; } */
  /*         void SetMCID ( Int_t mcID ) { fMCID = mcID; } */

  /*         const void print(); */

  Bool_t operator==(const PndHypDigiStrip &d2) const;
  // Bool_t const HasNeighbour(const PndHypDigiStrip& d2);

 private:
  /*  Int_t fIndex;     // MC Point index */
  /*        Int_t fDetID; */
  /* 	   TString fDetName; */
  /* 	   Int_t fFE; */
  Int_t fChannel;
  /*  Double_t fCharge; */
  /* 	   Int_t fMCID;       // MC Track index */

  ClassDef(PndHypDigiStrip, 2);
};

#endif
