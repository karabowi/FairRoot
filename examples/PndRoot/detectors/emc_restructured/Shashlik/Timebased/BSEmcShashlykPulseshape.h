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
#ifndef BSEMCSHASHLYKPULSESHAPE_H
#define BSEMCSHASHLYKPULSESHAPE_H

#include "BSEmcAbsPulseshape.h"

/**
 * @brief Experimentally derived Pulseshape
 *
 * of the form f(x) = A * (t')^ N * exp(-N t')
 * @ingroup BSEmc
 */
class BSEmcShashlykPulseshape : public BSEmcAbsPulseshape {

 public:
  BSEmcShashlykPulseshape();
  BSEmcShashlykPulseshape(Double_t mu, Double_t sigma);
  virtual ~BSEmcShashlykPulseshape(){};

  virtual Double_t operator()(const Double_t t, const Double_t amp, const Double_t toffset) const;

 private:
  Double_t fMu;
  Double_t fSigma;

  ClassDef(BSEmcShashlykPulseshape, 0)
};

#endif /*BSEMCSHASHLYKPULSESHAPE_H*/
