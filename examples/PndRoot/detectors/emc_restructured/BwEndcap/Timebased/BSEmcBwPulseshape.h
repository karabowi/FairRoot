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

//-----------------------------------------------------------
// Description:
//      Experimentally derived Pulseshape of the form:
//	f(x) = A * (t')^ N * exp(-N t')
//-----------------------------------------------------------
//#pragma once
#ifndef BSEMCBWPULSESHAPE_HH
#define BSEMCBWPULSESHAPE_HH

#include "Rtypes.h"
#include "RtypesCore.h"

#include "BSEmcAbsPulseshape.h"

class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @brief Experimentally derived Pulseshape
 *
 * of the form f(x) = A * (t')^ N * exp(-N t')
 * @ingroup BwEndcap
 */
class BSEmcBwPulseshape : public BSEmcAbsPulseshape {

 public:
  BSEmcBwPulseshape();
  BSEmcBwPulseshape(Double_t t_tau, Double_t t_n);
  virtual ~BSEmcBwPulseshape(){};

  virtual Double_t operator()(const Double_t t_time, const Double_t t_amp, const Double_t t_offset) const /*override*/;

 private:
  Double_t fTau{0};
  Double_t fN{0};

  ClassDef(BSEmcBwPulseshape, 1)
};

#endif /*BSEMCBWPULSESHAPE_HH*/
