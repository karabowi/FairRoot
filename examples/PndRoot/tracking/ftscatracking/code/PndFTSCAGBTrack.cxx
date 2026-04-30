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

// $Id: PndFTSCAGBTrack.cxx,v 1.2 2016/11/22 15:57:28 mpugach Exp $
//***************************************************************************
// This file is property of and copyright by the ALICE HLT Project          *
// ALICE Experiment at CERN, All rights reserved.                           *
//                                                                          *
// Primary Authors: Sergey Gorbunov <sergey.gorbunov@kip.uni-heidelberg.de> *
//                  Ivan Kisel <kisel@kip.uni-heidelberg.de>                *
//                  for The ALICE HLT Project.                              *
//                                                                          *
// Developed by:   Igor Kulakov <I.Kulakov@gsi.de>                          *
//                 Mykhailo Pugach <M.Pugach@gsi.de>                        *
//                 Maksym Zyzak <M.Zyzak@gsi.de>                            *
//                                                                          *
// Permission to use, copy, modify and distribute this software and its     *
// documentation strictly for non-commercial purposes is hereby granted     *
// without fee, provided that the above copyright notice appears in all     *
// copies and that both the copyright notice and this permission notice     *
// appear in the supporting documentation. The authors make no claims       *
// about the suitability of this software for any purpose. It is            *
// provided "as is" without express or implied warranty.                    *
//***************************************************************************

#include "PndFTSCAGBTrack.h"
#include <iostream>

std::ostream &operator<<(std::ostream &out, const PndFTSCAGBTrack &t)
{
  out << t.fNHits;
  out << t.fFirstHitRef;
  out << t.fDeDx;
  out << t.fInnerParam;
  out << t.fOuterParam;

  return out;
}

std::istream &operator>>(std::istream &in, PndFTSCAGBTrack &t)
{
  in >> t.fNHits;
  in >> t.fFirstHitRef;
  in >> t.fDeDx;
  in >> t.fInnerParam;
  in >> t.fOuterParam;

  return in;
}
