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
// File and Version Information:
// $Id:$
//
// Description:
//      EMC Digitization
//      abstract Base class for Pulseshapes
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//-----------------------------------------------------------
#ifndef BSEMCABSPULSESHAPE_HH
#define BSEMCABSPULSESHAPE_HH

#include "TObject.h"

/**
 * @brief pulseshape interface
 * @author Sebastian Neubert    TUM            (original author)
 * @ingroup EmcBase
 */
class BSEmcAbsPulseshape : public TObject {
 public:
  BSEmcAbsPulseshape() { ; }
  virtual ~BSEmcAbsPulseshape() { ; }

  virtual Double_t operator()(const Double_t t_t, const Double_t t_amp, const Double_t t_toffset) const = 0;
  virtual Double_t value(const Double_t t_t, const Double_t t_amp, const Double_t t_toffset) const { return operator()(t_t, t_amp, t_toffset); }

  ClassDef(BSEmcAbsPulseshape, 1)
};

#endif /*BSEMCABSPULSESHAPE_HH*/
