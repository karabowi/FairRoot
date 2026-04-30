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

#ifndef L1Field_h
#define L1Field_h 1

#include "PndFTSCADef.h"
#include <iostream>
using std::cout;
using std::endl;
using std::ostream;
#include <vector>
using std::max;
using std::min;
using std::vector;

#include "CAFieldValue.h"

#if 0 // use lookUpTable rather than polinom aproximation
class L1FieldSlice{

  public:

  vector<float> bX, bY, bZ;
  float xMin, yMin, dx, dy;
  int nXBins, nYBins; 

  void GetBBin( float x, float y, float &bX_, float &bY_, float &bZ_ ) const {
    int iXBin = static_cast<int>((x - xMin)/dx);
    int iYBin = static_cast<int>((y - yMin)/dy);
    iXBin = min( iXBin, nXBins-1 );
    iXBin = max( 0, iXBin );
    iYBin = min( iYBin, nYBins-1 );
    iYBin = max( 0, iYBin );
    const int iBin = iXBin*nYBins + iYBin;
    bX_ = bX[iBin];
    bY_ = bY[iBin];
    bZ_ = bZ[iBin];
  };
  
  void AlocateMemory() {
    const int NBins = nXBins*nYBins;
    bX.resize(NBins);
    bY.resize(NBins);
    bZ.resize(NBins);
  }
  
  L1FieldSlice(){};

  ~L1FieldSlice(){}
  
  void GetFieldValue( const float_v &x, const float_v &y, CAFieldValue &B, const float_m &mask = float_m(true) ) const
  {
    float bX_, bY_, bZ_;
    foreach_bit( int iV, mask ) {
      GetBBin( x[iV], y[iV], bX_, bY_, bZ_ );
      B.x[iV] = bX_;
      B.y[iV] = bY_;
      B.z[iV] = bZ_;
    }
  }
};

#else // 0
class L1FieldSlice {

 public:
  float_v cx[21], cy[21], cz[21]; // polinom coeff.

  L1FieldSlice()
  {
    for (int i = 0; i < 21; i++)
      cx[i] = cy[i] = cz[i] = Vc::Zero;
  }

  void GetFieldValue(const float_v &x, const float_v &y, CAFieldValue &B, const float_m &mask = float_m(true)) const
  {
    float_v x2 = x * x;
    float_v y2 = y * y;
    float_v xy = x * y;

    float_v x3 = x2 * x;
    float_v y3 = y2 * y;
    float_v xy2 = x * y2;
    float_v x2y = x2 * y;

    float_v x4 = x3 * x;
    float_v y4 = y3 * y;
    float_v xy3 = x * y3;
    float_v x2y2 = x2 * y2;
    float_v x3y = x3 * y;

    float_v x5 = x4 * x;
    float_v y5 = y4 * y;
    float_v xy4 = x * y4;
    float_v x2y3 = x2 * y3;
    float_v x3y2 = x3 * y2;
    float_v x4y = x4 * y;

    B.x(mask) = cx[0] + cx[1] * x + cx[2] * y + cx[3] * x2 + cx[4] * xy + cx[5] * y2 + cx[6] * x3 + cx[7] * x2y + cx[8] * xy2 + cx[9] * y3 + cx[10] * x4 + cx[11] * x3y +
                cx[12] * x2y2 + cx[13] * xy3 + cx[14] * y4 + cx[15] * x5 + cx[16] * x4y + cx[17] * x3y2 + cx[18] * x2y3 + cx[19] * xy4 + cx[20] * y5;

    B.y(mask) = cy[0] + cy[1] * x + cy[2] * y + cy[3] * x2 + cy[4] * xy + cy[5] * y2 + cy[6] * x3 + cy[7] * x2y + cy[8] * xy2 + cy[9] * y3 + cy[10] * x4 + cy[11] * x3y +
                cy[12] * x2y2 + cy[13] * xy3 + cy[14] * y4 + cy[15] * x5 + cy[16] * x4y + cy[17] * x3y2 + cy[18] * x2y3 + cy[19] * xy4 + cy[20] * y5;

    B.z(mask) = cz[0] + cz[1] * x + cz[2] * y + cz[3] * x2 + cz[4] * xy + cz[5] * y2 + cz[6] * x3 + cz[7] * x2y + cz[8] * xy2 + cz[9] * y3 + cz[10] * x4 + cz[11] * x3y +
                cz[12] * x2y2 + cz[13] * xy3 + cz[14] * y4 + cz[15] * x5 + cz[16] * x4y + cz[17] * x3y2 + cz[18] * x2y3 + cz[19] * xy4 + cz[20] * y5;
  }
};

#endif // 0

class L1FieldRegion {

 public:
  L1FieldRegion() : cx0(Vc::Zero), cx1(Vc::Zero), cx2(Vc::Zero), cy0(Vc::Zero), cy1(Vc::Zero), cy2(Vc::Zero), cz0(Vc::Zero), cz1(Vc::Zero), cz2(Vc::Zero), z0(Vc::Zero) {}

  L1FieldRegion(float reg[10]) : cx0(reg[0]), cx1(reg[1]), cx2(reg[2]), cy0(reg[3]), cy1(reg[4]), cy2(reg[5]), cz0(reg[6]), cz1(reg[7]), cz2(reg[8]), z0(reg[9]) {}

  float_v cx0, cx1, cx2; // Bx(z) = cx0 + cx1*(z-z0) + cx2*(z-z0)^2
  float_v cy0, cy1, cy2; // By(z) = cy0 + cy1*(z-z0) + cy2*(z-z0)^2
  float_v cz0, cz1, cz2; // Bz(z) = cz0 + cz1*(z-z0) + cz2*(z-z0)^2
  float_v z0;

  CAFieldValue Get(const float_v z)
  {
    float_v dz = (z - z0);
    float_v dz2 = dz * dz;
    CAFieldValue B;
    B.x = cx0 + cx1 * dz + cx2 * dz2;
    B.y = cy0 + cy1 * dz + cy2 * dz2;
    B.z = cz0 + cz1 * dz + cz2 * dz2;
    return B;
  }

  void Set(const CAFieldValue &B0, const float_v B0z, const CAFieldValue &B1, const float_v B1z, const CAFieldValue &B2, const float_v B2z)
  {
    z0 = B0z;
    float_v dz1 = B1z - B0z, dz2 = B2z - B0z;

    float_v deti = dz1 * dz2 * (dz2 - dz1);
    float_m mask = abs(deti) > float_v(1.e-8f);

    float_v det = rcp(float_v(dz1 * dz2 * (dz2 - dz1)));
    float_v w21 = -dz2 * det;
    float_v w22 = dz1 * det;
    float_v w11 = -dz2 * w21;
    float_v w12 = -dz1 * w22;

    float_v dB1 = B1.x - B0.x;
    float_v dB2 = B2.x - B0.x;
    cx0(mask) = B0.x;
    cx1(mask) = dB1 * w11 + dB2 * w12;
    cx2(mask) = dB1 * w21 + dB2 * w22;

    dB1 = B1.y - B0.y;
    dB2 = B2.y - B0.y;
    cy0(mask) = B0.y;
    cy1(mask) = dB1 * w11 + dB2 * w12;
    cy2(mask) = dB1 * w21 + dB2 * w22;

    dB1 = B1.z - B0.z;
    dB2 = B2.z - B0.z;
    cz0(mask) = B0.z;
    cz1(mask) = dB1 * w11 + dB2 * w12;
    cz2(mask) = dB1 * w21 + dB2 * w22;
  }

  void Set(const CAFieldValue &B0, const float_v B0z, const CAFieldValue &B1, const float_v B1z)
  {
    z0 = B0z[0];
    float_v dzi = rcp(float_v(B1z - B0z));
    cx0 = B0.x;
    cy0 = B0.y;
    cz0 = B0.z;
    cx1 = (B1.x - B0.x) * dzi;
    cy1 = (B1.y - B0.y) * dzi;
    cz1 = (B1.z - B0.z) * dzi;
    cx2 = cy2 = cz2 = 0;
  }

  void Shift(float_v z)
  {
    float_v dz = z - z0;
    float_v cx2dz = cx2 * dz;
    float_v cy2dz = cy2 * dz;
    float_v cz2dz = cz2 * dz;
    z0 = float_v(z[0]);
    cx0 += (cx1 + cx2dz) * dz;
    cy0 += (cy1 + cy2dz) * dz;
    cz0 += (cz1 + cz2dz) * dz;
    cx1 += cx2dz + cx2dz;
    cy1 += cy2dz + cy2dz;
    cz1 += cz2dz + cz2dz;
  }

  void SetOneEntry(const int i0, const L1FieldRegion &f1, const int i1)
  {
    cx0[i0] = f1.cx0[i1];
    cx1[i0] = f1.cx1[i1];
    cx2[i0] = f1.cx2[i1];
    cy0[i0] = f1.cy0[i1];
    cy1[i0] = f1.cy1[i1];
    cy2[i0] = f1.cy2[i1];
    cz0[i0] = f1.cz0[i1];
    cz1[i0] = f1.cz1[i1];
    cz2[i0] = f1.cz2[i1];
    z0[i0] = f1.z0[i1];
  }

  void SetOneEntry(const L1FieldRegion &f1, const int i1)
  {
    cx0 = f1.cx0[i1];
    cx1 = f1.cx1[i1];
    cx2 = f1.cx2[i1];
    cy0 = f1.cy0[i1];
    cy1 = f1.cy1[i1];
    cy2 = f1.cy2[i1];
    cz0 = f1.cz0[i1];
    cz1 = f1.cz1[i1];
    cz2 = f1.cz2[i1];
    z0 = f1.z0[i1];
  }

  void GetOneEntry(float reg[10], const int iVec)
  {
    reg[0] = cx0[iVec];
    reg[1] = cx1[iVec];
    reg[2] = cx2[iVec];
    reg[3] = cy0[iVec];
    reg[4] = cy1[iVec];
    reg[5] = cy2[iVec];
    reg[6] = cz0[iVec];
    reg[7] = cz1[iVec];
    reg[8] = cz2[iVec];
    reg[9] = z0[iVec];
  }

  friend ostream &operator<<(ostream &out, L1FieldRegion &B)
  {
    return out << " FieldRegion " << endl
               << B.cx0 << endl
               << B.cx1 << endl
               << B.cx2 << endl
               << B.cy0 << endl
               << B.cy1 << endl
               << B.cy2 << endl
               << B.cz0 << endl
               << B.cz1 << endl
               << B.cz2 << endl
               << B.z0 << endl;
  };
};

#endif
