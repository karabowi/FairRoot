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

//-*- Mode: C++ -*-
// *****************************************************************************
//                                                                             *
// @Autors: I.Kulakov; M.Pugach; M.Zyzak; I.Kisel                                        *
// @e-mail: I.Kulakov@gsi.de; M.Pugach@gsi.de; M.Zyzak@gsi.de; I.Kisel@compeng.uni-frankfurt.de *
//                                                                             *
// *****************************************************************************

#ifndef FTSCAStation_H
#define FTSCAStation_H

#ifdef PANDA_FTS

#include "L1MaterialInfo.h"
#include "L1XYMeasurementInfo.h"
#include <vector>
using std::vector;
#endif

struct FTSCAStripInfo {
  float sin, cos;
};

struct FTSCAStripInfoVector {
  float_v sin, cos;
};

struct FTSCAStation {
  float ErrY;
  float ErrZ;

  FTSCAStripInfo f;

  float x0;

#ifdef PANDA_FTS
  L1MaterialInfo materialInfo;
  L1FieldSlice fieldSlice;
  vector<L1FieldSlice> fieldVirtualSlice;
  float dZVirtualStation;
  float xMax, yMax;
#else
  float xOverX0, xTimesRho;
#endif

  char NDF;
  char CellLength;
};

#endif
