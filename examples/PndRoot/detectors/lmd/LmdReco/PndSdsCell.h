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

// -------------------------------------------------------------------------
// -----                 PndSdsCell header file                   -----
// -----                  Created by A. Karavdina            -----
// -------------------------------------------------------------------------

/** PndSdsCell.h
 *@author A.Karavdina <karavdin@kph.uni-mainz.de>
 **
 ** A container for a cell (straight track's segment)
 **/

#ifndef PNDSDSCELL_H
#define PNDSDSCELL_H

#include <stdio.h>
#include <iostream>
#include "TObject.h"

class PndSdsCell : public TObject {
 public:
  /** Default constructor (not for use) **/
  PndSdsCell();

  PndSdsCell(unsigned int hitUp, unsigned int hitDw);

  /** Destructor **/
  ~PndSdsCell();

  /** Accessors **/

  void SetHitUp(unsigned int &ih) { fhit_upstream = ih; }
  void SetHitDw(unsigned int &ih) { fhit_downstream = ih; }
  void SetPV(int &ipv) { fpv = ipv; }
  int GetHitUp() { return fhit_upstream; }
  int GetHitDw() { return fhit_downstream; }
  int GetPV() { return fpv; }

 private:
  unsigned int fhit_upstream;   // hit upstream
  unsigned int fhit_downstream; // hit downstream
  int fpv;                      // cell position in evolution

  /* Int_t fsecMC; ///< unique sensor ID */
  /* bool fisMerged; */
  ClassDef(PndSdsCell, 1);
};

#endif
