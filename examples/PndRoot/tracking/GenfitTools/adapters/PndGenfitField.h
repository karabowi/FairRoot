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

#ifndef PNDGENFITFIELD_H
#define PNDGENFITFIELD_H

#include "GFAbsBField.h"

/** @brief  Magnetic field
 *
 *  @author Promme (Prometeusz Jasinski)
 *
 */

class PndGenfitField : public GFAbsBField {

 public:
  //! define the field in this ctor (?)
  PndGenfitField();

  //! return value at position
  TVector3 get(const TVector3 &pos) const;

 private:
};

#endif
