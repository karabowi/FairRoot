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

#ifndef FTSCASTRIP_H
#define FTSCASTRIP_H

class FTSCAStrip {
 public:
  FTSCAStrip() : fS(0), fIsUsed(false) {}

  FTSCAStrip(float f) : fS(f), fIsUsed(false) {}

  operator float() const { return fS; }

  bool IsUsed() const { return fIsUsed; }

  void SetAsUsed() { fIsUsed = true; }

 private:
  float fS;

  bool fIsUsed;
};

#endif
