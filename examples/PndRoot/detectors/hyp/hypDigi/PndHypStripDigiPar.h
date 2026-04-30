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

//
// C++ Interface: MvdDigiPar
//
// Description:
//
//
// Author: t.stockmanns <stockman@ikp455>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
// slightly modified by A. Sanchez for hyp purpose

#ifndef PNDHYPSTRIPDIGIPAR_H
#define PNDHYPSTRIPDIGIPAR_H

#include "FairParGenericSet.h"
#include "FairParamList.h"
#include <TVector2.h>
class PndHypStripDigiPar : public FairParGenericSet {
 public:
  PndHypStripDigiPar(const char *name = "PndHypParTest", const char *title = "Hyp digi parameter", const char *context = "TestDefaultContext");
  ~PndHypStripDigiPar(void){};
  void clear(void){};
  void putParams(FairParamList *list);
  Bool_t getParams(FairParamList *list);
  void print();

  /** Accessor functions **/
  Double_t GetTopPitch() const { return topPitch; }
  Double_t GetBotPitch() const { return botPitch; }
  Double_t GetOrient() const { return orient; }
  Double_t GetSkew() const { return skew; }
  TVector2 GetTopAnchor() const { return topAnchor; }
  TVector2 GetBotAnchor() const { return botAnchor; }
  Int_t GetNrTopFE() const { return topNrFE; }
  Int_t GetNrBotFE() const { return botNrFE; }
  Int_t GetNrFECh() const { return feChannels; }
  Double_t GetThreshold() const { return threshold; }
  Double_t GetNoise() const { return noise; }
  const char *GetSensType() const { return fSensType.Data(); }
  const char *GetFeType() const { return fFeType.Data(); }

  void SetTopPitch(Double_t x) { topPitch = x; }
  void SetBotPitch(Double_t x) { botPitch = x; }
  void SetOrient(Double_t x) { orient = x; }
  void SetSkew(Double_t x) { skew = x; }
  void SetTopAnchor(TVector2 x) { topAnchor = x; }
  void SetBotAnchor(TVector2 x) { botAnchor = x; }
  void SetNrTopFE(Int_t x) { topNrFE = x; }
  void SetNrBotFE(Int_t x) { botNrFE = x; }
  void SetNrFECh(Int_t x) { feChannels = x; }
  void SetThreshold(Double_t x) { threshold = x; }
  void SetNoise(Double_t x) { noise = x; }
  void SetSensType(TString x) { fSensType = x; }
  void SetFeType(TString x) { fFeType = x; }

 private:
  // Strip Parameters
  /// Strip pitch on top wafer side
  Double_t topPitch;
  /// Strip pitch on bottom wafer side
  Double_t botPitch;
  /// orientation angle of top strips
  Double_t orient;
  /// skew angle of bottom strips wrt top strips
  Double_t skew;
  /// Anchor point of top strip#0
  TVector2 topAnchor;
  /// Anchor point of bottom strip#0
  TVector2 botAnchor;
  /// Number of Channels per FE
  Int_t feChannels;
  /// Number of FE attached to top wafer side
  Int_t topNrFE;
  /// Number of FE attached to bottom wafer side
  Int_t botNrFE;

  /// Discriminator threshold
  Double_t threshold;
  /// Complete noise including threshold dispersion
  Double_t noise;
  TString fSensType; // Sensor type name (rect, trap...)
  TString fFeType;   // Frontend type name (APV25, CBM-XYTER, ...)

  //     Text_t   sensName;  // Sensor name (Strip, Pixel, etc...)
  //     Text_t   feName;    // Frontend name (APV25, CBM-XYTER, ...)

  ClassDef(PndHypStripDigiPar, 2);
};

#endif
