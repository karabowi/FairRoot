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
#ifndef BSEMCFITTEDPULSESHAPE_HH
#define BSEMCFITTEDPULSESHAPE_HH

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
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup FwEndcap
 */
class BSEmcFittedPulseshape : public BSEmcAbsPulseshape {

 public:
  BSEmcFittedPulseshape();
  BSEmcFittedPulseshape(Double_t t_tau, Double_t t_n);
  virtual ~BSEmcFittedPulseshape(){};

  virtual Double_t operator()(const Double_t t_t, const Double_t t_amp, const Double_t t_toffset) const /*override*/;

 private:
  Double_t fTau{0};
  Double_t fN{0};

  ClassDef(BSEmcFittedPulseshape, 1)
};

#endif /*BSEMCFITTEDPULSESHAPE_HH*/
