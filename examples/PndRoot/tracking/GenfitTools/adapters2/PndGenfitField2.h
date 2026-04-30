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

#ifndef PNDGENFITFIELD2_H
#define PNDGENFITFIELD2_H

#include "AbsBField.h"

/** @brief  Magnetic field
 *
 *  @author Promme (Prometeusz Jasinski)
 * //  modified by Elisabetta Prencipe 19/5/2014
 */

class PndGenfitField2 : public genfit::AbsBField {

 public:
  //! define the field in this ctor (?)
  PndGenfitField2();

  //! return value at position
  TVector3 get(const TVector3 &pos) const;
  virtual void get(const double &posX, const double &posY, const double &posZ, double &Bx, double &By, double &Bz) const;

 private:
};

#endif
