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
 * PndLmdDigiPara.cxx
 *  Created on: Jul 29, 2009     Author: huagen
 */
#include "PndLmdDigiPara.h"
#include "TMath.h"
#include "TVector2.h"
#include <iostream>
using namespace std;

PndLmdDigiPara ::PndLmdDigiPara(const char *name, const char *title, const char *context) : FairParGenericSet(name, title, context)
{
  //	const char* name="PndLmdDigiPara";
  //	const char* title="PndLmdDigi Parameter";
  //	const char* context="PndLmdDigi";
  clear();
}

// public method putParams to read the parameters from  container
void PndLmdDigiPara ::putParams(FairParamList *list)
{
  if (!list)
    return;
  list->add("circle_pitch", fCirclePitch);
  list->add("right_pitch", fRightPitch);
  list->add("left_pitch", fLeftPitch);
  list->add("left_orient", fLeftOrient);
  list->add("right_orient", fRightOrient);
  list->add("circle_anchor_x", (Double_t)fCircleAnchor.X());
  list->add("circle_anchor_y", (Double_t)fCircleAnchor.Y());
  list->add("right_anchor_x", (Double_t)fRightAnchor.X());
  list->add("right_anchor_y", (Double_t)fRightAnchor.Y());
  list->add("left_anchor_x", (Double_t)fLeftAnchor.X());
  list->add("left_anchor_y", (Double_t)fLeftAnchor.Y());
  list->add("nr_fe_channels", fNrFeChannels);
  list->add("nr_fe_circle", fNrCircleFe);
  list->add("nr_fe_right", fNrRightFe);
  list->add("nr_fe_left", fNrLeftFe);
  list->add("charge_threshold", fThreshold);
  list->add("charge_noise", fNoise);
  list->add("sens_type", fSensType);
  list->add("fe_type", fFeType);
  list->add("gaus_sigma", fSigma);
}
/*
circle_pitch:Double_t 0.0050
right_pitch:Double_t 0.0050
left_pitch:Double_t 0.0050
left_orient:Double_t 0.1607815213
right_orient:Double_t 2.9808111327
circle_anchor_x:Double_t 0.0
circle_anchor_y:Double_t 2.500
right_anchor_x:Double_t 2.665
right_anchor_y:Double_t -2.500
left_anchor_x:Double_t -2.665.
left_anchor_y:Double_t -2.500
nr_fe_channels:Int_t 128
nr_fe_circle:Int_t 10
nr_fe_right:Int_t 10
nr_fe_left:Int_t 10
charge_threshold:Double_t 10000
charge_noise:Double_t 2000
sens_type:Text_t Trap
fe_type:Text_t APV25
*/
// public method setParams to set parameters
Bool_t PndLmdDigiPara::getParams(FairParamList *list)
{
  if (!list)
    return kFALSE;
  if (!list->fill("circle_pitch", &fCirclePitch))
    return kFALSE;
  if (!list->fill("right_pitch", &fRightPitch))
    return kFALSE;
  if (!list->fill("left_pitch", &fLeftPitch))
    return kFALSE;
  if (!list->fill("left_orient", &fLeftOrient))
    return kFALSE;
  if (!list->fill("right_orient", &fRightOrient))
    return kFALSE;
  Double_t x, y;
  if (!list->fill("circle_anchor_x", &x))
    return kFALSE;
  if (!list->fill("circle_anchor_y", &y))
    return kFALSE;
  fCircleAnchor.Set(x, y);
  if (!list->fill("right_anchor_x", &x))
    return kFALSE;
  if (!list->fill("right_anchor_y", &y))
    return kFALSE;
  fRightAnchor.Set(x, y);
  if (!list->fill("left_anchor_x", &x))
    return kFALSE;
  if (!list->fill("left_anchor_y", &y))
    return kFALSE;
  fLeftAnchor.Set(x, y);

  if (!list->fill("nr_fe_channels", &fNrFeChannels))
    return kFALSE;
  if (!list->fill("nr_fe_circle", &fNrCircleFe))
    return kFALSE;
  if (!list->fill("nr_fe_right", &fNrRightFe))
    return kFALSE;
  if (!list->fill("nr_fe_left", &fNrLeftFe))
    return kFALSE;
  if (!list->fill("charge_threshold", &fThreshold))
    return kFALSE;
  if (!list->fill("charge_noise", &fNoise))
    return kFALSE;
  Text_t stName[80];
  if (!list->fill("sens_type", stName, 80))
    return kFALSE;
  fSensType = stName;
  Text_t feName[80];
  if (!list->fill("fe_type", feName, 80))
    return kFALSE;
  fFeType = feName;
  if (!list->fill("gaus_sigma", &fSigma))
    return kFALSE;

  return kTRUE;
}

// public method print implementation which print out the digitization parameters
void PndLmdDigiPara::print()
{
  cout << "The digitization parameters for short strips or curve strips" << endl;
  cout << "Circle_pitch = " << fCirclePitch << endl;
  cout << "Right_pitch =" << fRightPitch << endl;
  cout << "Left_pitch =" << fLeftPitch << endl;
  cout << "Left_orient =" << fLeftOrient << " rad =" << fLeftOrient / TMath::Pi() * 180 << " degree" << endl;
  cout << "Right_orient =" << fRightOrient << " rad = " << fRightOrient / TMath::Pi() * 180 << " degree" << endl;
  cout << "Circle_anchor = (" << fCircleAnchor.X() << "," << fCircleAnchor.Y() << ")" << endl;
  cout << "Right_anchor = (" << fRightAnchor.X() << "," << fRightAnchor.Y() << ")" << endl;
  cout << "Left_anchor = (" << fLeftAnchor.X() << "," << fLeftAnchor.Y() << ")" << endl;
  cout << "Sigma of diffusion = " << fSigma << endl;
  cout << "Threshold =" << fThreshold << endl;
  cout << "Noise = " << fNoise << endl;
  cout << "Sensor_type =" << fSensType << endl;
  cout << "FE_type=" << fFeType << endl;
}

ClassImp(PndLmdDigiPara);
