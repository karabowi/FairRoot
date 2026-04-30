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

/*
 * PndLmdDigiPara.h*
 *  Created on: Jul 29, 2009
 *      Author: huagen
 * this digitization parameter contains two kinds of the parameters for straight strip and curve strips
 */

#ifndef PNDLMDDIGIPARA_H_
#define PNDLMDDIGIPARA_H_

#include "TString.h"
#include "TVector2.h"
#include "FairParGenericSet.h"
#include "FairParamList.h"

//#include "FairGeanePro.h"

class PndLmdDigiPara : public FairParGenericSet {
 public:
  // constructors of PndLmdDigiPara
  PndLmdDigiPara(const char *name = "PndLmdDigiParameter", const char *title = "PndLmd Strip Digi Parameter", const char *context = "Pnd Lmd Digitization parameters");
  ~PndLmdDigiPara(void) {}
  void clean(void) {}
  void putParams(FairParamList *list);
  Bool_t getParams(FairParamList *list);

  void print();

  // public accessor
  Double_t GetCirclePitch() const { return fCirclePitch; }
  Double_t GetLeftPitch() const { return fLeftPitch; }
  Double_t GetRightPitch() const { return fRightPitch; }
  Double_t GetRightOrient() const { return fRightOrient; }
  Double_t GetLeftOrient() const { return fLeftOrient; }
  TVector2 GetCircleAnchor() const { return fCircleAnchor; }
  TVector2 GetRightAnchor() const { return fRightAnchor; }
  TVector2 GetLeftAnchor() const { return fLeftAnchor; }
  Int_t GetNrFeChannels() const { return fNrFeChannels; }
  Int_t GetNrCircleFe() const { return fNrCircleFe; }
  Int_t GetNrRightFe() const { return fNrRightFe; }
  Int_t GetNrLeftFe() const { return fNrLeftFe; }
  Double_t GetGausSigma() const { return fSigma; }

  Double_t GetThreshold() const { return fThreshold; }
  Double_t GetNoise() const { return fNoise; }
  const char *GetSensType() const { return fSensType.Data(); }
  const char *GetFeType() const { return fFeType.Data(); }

  void SetCirclePitch(Double_t x) { fCirclePitch = x; }
  void SetLeftPitch(Double_t x) { fLeftPitch = x; }
  void SetRightPitch(Double_t x) { fRightPitch = x; }
  void SetLeftOrient(Double_t x) { fLeftOrient = x; }
  void SetRightOrient(Double_t x) { fRightOrient = x; }
  void SetCircleAnchor(TVector2 &x) { fCircleAnchor = x; }
  void SetRightAnchor(TVector2 &x) { fRightAnchor = x; }
  void SetLeftAnchor(TVector2 &x) { fLeftAnchor = x; }
  void SetNrFeChannels(Int_t x) { fNrFeChannels = x; }
  void SetNrCircleFe(Int_t x) { fNrCircleFe = x; }
  void SetNrLeftFe(Int_t x) { fNrLeftFe = x; }
  void SetNrRightFe(Int_t x) { fNrRightFe = x; }
  void SetSensType(TString x) { fSensType = x; }
  void SetFeType(TString x) { fFeType = x; }
  void SetGausSigma(Double_t x) { fSigma = x; }

 private:
  Double_t fCirclePitch;   // the pitch if the strip is curved
  Double_t fRightPitch;    // the pitch of right segment when the strip is straight
  Double_t fLeftPitch;     // the pitch of the left segment when the strip is straight
  Double_t fRightOrient;   // the orient of right pitch
  Double_t fLeftOrient;    // the orient of the left pitch
  Double_t fCircleAnchorX; // the anchor point of the circle
  Double_t fCircleAnchorY;
  Double_t fRightAnchorX; // the anchor point of the right segment
  Double_t fRightAnchorY;
  Double_t fLeftAnchorX; // the anchor point of the left segment
  Double_t fLeftAnchorY;
  TVector2 fCircleAnchor; // anchor point of the center of circle
  TVector2 fRightAnchor;  // anchor point of right segment
  TVector2 fLeftAnchor;   // anchor point of left segment
  Int_t fNrFeChannels;    // the number of channels per FE
  Int_t fNrCircleFe;      // the FE number of Circle strips
  Int_t fNrLeftFe;        // the number of FE
  Int_t fNrRightFe;
  Double_t fThreshold; // the threshold of the signal
  Double_t fNoise;     // the noise of the electronics
  Double_t fSigma;     // the sigma of gaussian distribution for charge diffusion parameters
  TString fSensType;   // the type of sensor
  TString fFeType;     // the type of FE

  ClassDef(PndLmdDigiPara, 3);
};

#endif /* PNDLMDDIGIPARA_H_ */
