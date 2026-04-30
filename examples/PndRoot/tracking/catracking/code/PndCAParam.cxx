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

// @(#) $Id: PndCAParam.cxx,v 1.4 2011/10/01 00:23:44 perev Exp $
// **************************************************************************
// This file is property of and copyright by the ALICE HLT Project          *
// ALICE Experiment at CERN, All rights reserved.                           *
//                                                                          *
// Primary Authors: Sergey Gorbunov <sergey.gorbunov@kip.uni-heidelberg.de> *
//                  Ivan Kisel <kisel@kip.uni-heidelberg.de>                *
//                  for The ALICE HLT Project.                              *
//                                                                          *
// Developed by:   Igor Kulakov <I.Kulakov@gsi.de>                          *
//                 Maksym Zyzak <M.Zyzak@gsi.de>                            *
//                                                                          *
// Permission to use, copy, modify and distribute this software and its     *
// documentation strictly for non-commercial purposes is hereby granted     *
// without fee, provided that the above copyright notice appears in all     *
// copies and that both the copyright notice and this permission notice     *
// appear in the supporting documentation. The authors make no claims       *
// about the suitability of this software for any purpose. It is            *
// provided "as is" without express or implied warranty.                    *
//                                                                          *
//***************************************************************************

#include "PndCAParam.h"
#include "PndCAMath.h"
#include "PndCATrackParam.h"
#include "PndCATrackParamVector.h"

#include <iostream>
//#include "debug.h"

PndCAParam::PndCAParam() : fNStations(0), fStations(nullptr) {}

// std::ostream &operator<<( std::ostream &out, const PndCAParam &p )
// {
//   // write settings to the file
//   int iSl = 0;
//   float tmp=0;
//   out << iSl << std::endl;
//   out << p.fNStations << std::endl;
//   out << tmp << std::endl;
//   out << tmp << std::endl;
//   out << tmp << std::endl;
//   out << tmp << std::endl;
//   out << tmp << std::endl;
//   out << tmp << std::endl;
//   out << tmp << std::endl;
//   out << tmp << std::endl;
//   out << tmp << std::endl;
//   out << tmp << std::endl;
//   out << tmp << std::endl;
//   out << tmp << std::endl;
//   out << tmp << std::endl;
//   out << tmp << std::endl;
//   out << p.fBz << std::endl;
//
//   return out;
// }

std::istringstream &operator>>(std::istringstream &in, PndCAParam &p)
{
  // Read settings from the file
  in >> p.fNStations;
  in >> p.fBz;
  p.fVtxFieldValue = p.cBz();
  if (p.fStations)
    delete[] p.fStations;
  p.fStations = new PndCAStation[p.fNStations];
  for (int i = 0; i < p.fNStations; i++) {
    int inttmp;
    in >> inttmp;
    in >> p.fStations[i].r;
    in >> p.fStations[i].xOverX0;
    in >> p.fStations[i].xTimesRho;

    float beta;
    in >> beta;
    p.fStations[i].f.sin = sin(beta);
    p.fStations[i].f.cos = cos(beta);

    in >> inttmp;
    p.fStations[i].NDF = inttmp;
    in >> inttmp;
    p.fStations[i].CellLength = inttmp;
  }

  p.fMaxZ = 75 + 20;
  p.fMaxR = 41;
  p.fMinR = 0;
  p.fMinZ = -75 + 20;

  return in;
}
