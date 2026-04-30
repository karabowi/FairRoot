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

//-------------------------------------------------------------------------
// Author:      Mustafa Schmidt (Mustafa.A.Schmidt@physik.uni-giessen.de)
// Changes:
// Date:        30.11.2015
// Description: Reconstruction
//-------------------------------------------------------------------------

#ifndef DISCDIRC_RECONRESULT_HH
#define DISCDIRC_RECONRESULT_HH

#include "TObject.h"

class PndDiscReconResult : public TObject {
 public:
  PndDiscReconResult();
  ~PndDiscReconResult();

  double cherenkov_angle;

  int particle;
  int hypothesis;
  int sensor;
  int pixel;
  double time;

  ClassDef(PndDiscReconResult, 2)
};

#endif // DISCDIRC_RECONRESULT_HH
