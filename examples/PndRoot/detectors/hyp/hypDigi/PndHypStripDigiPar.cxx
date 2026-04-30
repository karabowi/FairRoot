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

#include <iostream>
#include "PndHypStripDigiPar.h"
#include "TMath.h"

PndHypStripDigiPar::PndHypStripDigiPar(const char *name, const char *title, const char *context) : FairParGenericSet(name, title, context)
{
  clear();
}

void PndHypStripDigiPar::putParams(FairParamList *list)
{
  if (!list)
    return;
  list->add("top_pitch", topPitch);
  list->add("bot_pitch", botPitch);
  list->add("orient", orient);
  list->add("skew", skew);
  list->add("top_anchor_x", (Double_t)topAnchor.X());
  list->add("top_anchor_y", (Double_t)topAnchor.Y());
  list->add("bottom_anchor_x", (Double_t)botAnchor.X());
  list->add("bottom_anchor_y", (Double_t)botAnchor.Y());
  list->add("nr_fe_channels", feChannels);
  list->add("nr_fe_top", topNrFE);
  list->add("nr_fe_bottom", botNrFE);

  list->add("charge_threshold", threshold);
  list->add("charge_noise", noise);
  list->add("sens_Type", fSensType);
  list->add("fe_Type", fFeType);
  //   list->add("sensName", sensName);
  //   list->add("feName", feName);
}

Bool_t PndHypStripDigiPar::getParams(FairParamList *list)
{
  if (!list)
    return kFALSE;

  if (!list->fill("top_pitch", &topPitch))
    return kFALSE;
  if (!list->fill("bot_pitch", &botPitch))
    return kFALSE;
  if (!list->fill("orient", &orient))
    return kFALSE;
  if (!list->fill("skew", &skew))
    return kFALSE;

  Double_t x, y;
  if (!list->fill("top_anchor_x", &x))
    return kFALSE;
  if (!list->fill("top_anchor_y", &y))
    return kFALSE;
  topAnchor.Set(x, y);
  if (!list->fill("bot_anchor_x", &x))
    return kFALSE;
  if (!list->fill("bot_anchor_y", &y))
    return kFALSE;
  botAnchor.Set(x, y);

  if (!list->fill("nr_fe_channels", &feChannels))
    return kFALSE;
  if (!list->fill("nr_fe_top", &topNrFE))
    return kFALSE;
  if (!list->fill("nr_fe_bottom", &botNrFE))
    return kFALSE;

  if (!list->fill("charge_threshold", &threshold))
    return kFALSE;
  if (!list->fill("charge_noise", &noise))
    return kFALSE;
  Text_t stName[80];
  if (!list->fill("sens_Type", stName, 80))
    return kFALSE;
  fSensType = stName;
  Text_t feName[80];
  if (!list->fill("fe_Type", feName, 80))
    return kFALSE;
  fFeType = feName;
  //   if (!list->fill("sensName",&sensName)) return kFALSE;
  //   if (!list->fill("feName",&feName)) return kFALSE;
  return kTRUE;
}

ClassImp(PndHypStripDigiPar);

void PndHypStripDigiPar::print()
{
  std::cout << "MVD Digitisation Parameters:" << std::endl;
  std::cout << "   Top Pitch    = " << topPitch << std::endl;
  std::cout << "   Bottom Pitch = " << botPitch << std::endl;
  std::cout << "   Strip Angle (Top) = " << orient << "rad = " << orient / TMath::Pi() * 180. << " deg" << std::endl;
  std::cout << "   Skew Angle (Top->Bottom) = " << skew << "rad = " << skew / TMath::Pi() * 180. << " deg" << std::endl;
  std::cout << "   Top Anchor   = (" << topAnchor.X() << "," << topAnchor.Y() << ")" << std::endl;
  std::cout << "   Bottom Anchor= (" << botAnchor.X() << "," << botAnchor.Y() << ")" << std::endl;
  std::cout << "   FE Channels  = " << feChannels << std::endl;
  std::cout << "   Nr of Frontends (Top Side)   = " << topNrFE << std::endl;
  std::cout << "   Nr of Frontends (Bottom Side)= " << botNrFE << std::endl;
  std::cout << "   Charge Threshold (e-)        = " << threshold << std::endl;
  std::cout << "   Noise (ENC+Dispersion) (e-)  = " << noise << std::endl;
}
