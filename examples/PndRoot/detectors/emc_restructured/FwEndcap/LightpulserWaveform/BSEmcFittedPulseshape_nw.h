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
#ifndef BSEMCFITTEDPULSESHAPE_NW_HH
#define BSEMCFITTEDPULSESHAPE_NW_HH

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
 * @author Viktor Rodin
 * @ingroup FwEndcap
 */
class BSEmcFittedPulseshape_nw : public BSEmcAbsPulseshape {

 public:
  BSEmcFittedPulseshape_nw();
  BSEmcFittedPulseshape_nw(Double_t v1, Double_t tcr, Double_t tcf, Double_t t_r, Double_t res, Double_t tau, Double_t N);
  virtual ~BSEmcFittedPulseshape_nw(){};

  virtual Double_t operator()(const Double_t t, const Double_t amp, const Double_t toffset) const;

 private:
  Double_t fTau;
  Double_t fN;
  Double_t fv1;  // = 0;
  Double_t ftcr; // = 26.3995;//2.11196*12.5;
  Double_t ftcf; // = 302.76625;//24.2213*12.5;
  Double_t ft_r; // = 52.69625;//4.2157*12.5;
  Double_t fres; // = 0;
  ClassDef(BSEmcFittedPulseshape_nw, 1)
};

#endif /*BSEMCFITTEDPULSESHAPE_NW_HH*/
