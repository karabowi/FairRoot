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
// @Autors: I.Kulakov; M.Zyzak; I.Kisel                                        *
// @e-mail: I.Kulakov@gsi.de; M.Zyzak@gsi.de; I.Kisel@compeng.uni-frankfurt.de *
//                                                                             *
// *****************************************************************************

#ifndef CAFieldValue_H
#define CAFieldValue_H

#ifdef PANDA_FTS

class CAFieldValue {

  //* parameters of the magnetic field

 public:
  CAFieldValue() : x(Vc::Zero), y(Vc::Zero), z(Vc::Zero){};

  float_v x, y, z;

  void Combine(CAFieldValue &B, float_v w)
  {
    x += w * (B.x - x);
    y += w * (B.y - y);
    z += w * (B.z - z);
  }

  void UpdateValue(const CAFieldValue &B, const float_m &mask)
  {
    x(mask) = B.x;
    y(mask) = B.y;
    z(mask) = B.z;
  };

  friend ostream &operator<<(ostream &out, CAFieldValue &B) { return out << B.x << endl << B.y << endl << B.z << endl; };
};

#else // PANDA_FTS

struct CAFieldValue {
 public:
  CAFieldValue() : fcBz(0) {}

  CAFieldValue(float f) : fcBz(f) {}
  operator float() const { return fcBz; }

 private:
  float fcBz;
};

#endif // PANDA_FTS

#endif
