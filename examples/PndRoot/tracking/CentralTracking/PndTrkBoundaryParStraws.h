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

#ifndef PndTrkBoundaryParStraws_H
#define PndTrkBoundaryParStraws_H 1

// Root includes
#include "TROOT.h"

class PndTrkBoundaryParStraws : public TObject {

 public:
  /** Default constructor **/
  PndTrkBoundaryParStraws(){};
  /** Destructor **/
  ~PndTrkBoundaryParStraws(){};

  bool Set(int straw_number);

  ClassDef(PndTrkBoundaryParStraws, 1);
};

#endif
