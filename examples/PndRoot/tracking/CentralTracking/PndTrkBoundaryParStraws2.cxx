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

#include "PndTrkBoundaryParStraws2.h"
#include "PndSttTube.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include <stdlib.h>
#include <iostream>
#include <cmath>

// Root includes
#include "TROOT.h"

using namespace std;

//------------------ begin function  PndTrkBoundaryParStraws2::CalculateSpecialRegion
void PndTrkBoundaryParStraws2::CalculateSpecialRegion(Double_t APOTEMASTRAWDETECTORMIN, Double_t APOTEMAMAXINNERPARSTRAW, Double_t APOTEMAMINOUTERPARSTRAW, Double_t VERTICALGAP,
                                                      Double_t &x, Double_t &y)
{
  double a1 = 16., a2 = 23., a3 = 31.86, delta = 2.;

  double px, py, qx, qy;
  double alfa, beta, D, A, B, x1, y1; // x2, y2,  //[R.K.02/2017] Unused variable?

  a1 = APOTEMASTRAWDETECTORMIN;
  a2 = APOTEMAMAXINNERPARSTRAW;
  a3 = APOTEMAMINOUTERPARSTRAW;
  delta = VERTICALGAP;
  px = delta / 2.;
  qx = -px;
  py = (2. * a2 - 0.5 * delta) / sqrt(3.);
  qy = (-0.5 * delta + 2. * a1) / sqrt(3.);

  beta = -(px * px + py * py + qx * qx + qy * qy) / (py + qy);

  alfa = (-beta * py - px * px - py * py) / px;

  A = -4. * a3 * sqrt(3.) - alfa * sqrt(3.) + beta;
  B = 4. * a3 * a3 + 2. * alfa * a3;

  D = A * A - 16. * B;
  if (D < 0.) {
    cout << "delta = " << D << ", stop!\n";
    exit(-1);
  };
  D = sqrt(D);
  //	cout<<"A "<<A<<", B "<<B<<", delta "<<D<<endl;
  y1 = (-A + D) / 8.;
  // y2 = (-A - D)/8. ; //[R.K.02/2017] Unused variable?

  x1 = 2. * a3 - sqrt(3.) * y1;
  // x2 = 2.*a3 - sqrt(3.)*y2; //[R.K.02/2017] Unused variable?

  // il punto nel 1 quadrante ha coordinate (x1,y1);
  // la soluzione (x2,y2) e' l'intersezione che non interessa;

  //	cout<<"soluzione 1 , x1 "<<x1<<", y1 "<<y1<<endl;
  //	cout<<"soluzione 2 , x2 "<<x2<<", y2 "<<y2<<endl;

  x = x1;
  y = y1;

  return;
}
//------------------ end function  PndTrkBoundaryParStraws2::CalculateSpecialRegion

//----------begin of function PndTrkBoundaryParStraws2::Set

void PndTrkBoundaryParStraws2::Set(
  // inputs :
  Double_t APOTEMAMAXINNERPARSTRAW, Double_t APOTEMAMINOUTERPARSTRAW, Short_t NUMBER_STRAWS, Double_t APOTEMASTRAWDETECTORMIN, Double_t RSTRAWDETECTORMAX, bool stampa,
  TClonesArray *SttTubeArray, Double_t STRAWRADIUS, Double_t VERTICALGAP,
  // outputs :
  Short_t *StrawCode, Short_t *StrawCode2)
{
  // return true for those parallel Straws that are at the boundary
  // of the Stt central tracker;

  // the Stt Straw original number goes from 1 to 4542 included;

  // StrawCode convention (in the following left or right is looking to the beam from downstream) :
  //   -1 = not a boundary straw;
  //   10= inner axial boundary left;
  //   20= inner axial boundary right;
  //   12= outer VERTICAL (BUT NOT OUTERMOST) axial boundary left;
  //   22= outer VERTICAL (BUT NOT OUTERMOST)  axial boundary right;
  //   13= outermost axial boundary left;
  //   23= outermost axial boundary right;

  for (int i = 0; i < NUMBER_STRAWS; i++) {
    StrawCode[i] = -1;
    StrawCode2[i] = -1;
  }

  SttTubeList(
    // inputs :
    SttTubeArray, APOTEMASTRAWDETECTORMIN, RSTRAWDETECTORMAX, APOTEMAMAXINNERPARSTRAW, APOTEMAMINOUTERPARSTRAW, VERTICALGAP, STRAWRADIUS, NUMBER_STRAWS, stampa,
    // outputs :
    StrawCode, StrawCode2);

  return;
}
//----------end of function PndTrkBoundaryParStraws2::Set

//------------------ begin function  PndTrkBoundaryParStraws2::SttTubeList
void PndTrkBoundaryParStraws2::SttTubeList(
  // inputs :
  TClonesArray *SttTubeArray, Double_t APOTEMASTRAWDETECTORMIN, Double_t RSTRAWDETECTORMAX, Double_t APOTEMAMAXINNERPARSTRAW, Double_t APOTEMAMINOUTERPARSTRAW,
  Double_t VERTICALGAP, Double_t STRAWRADIUS, Short_t NUMBER_STRAWS, bool stampa,
  // outputs :
  Short_t *StrawCode, // goes from 0 to NUMBER_STRAWS-1 included;
  Short_t *StrawCode2 // goes from 0 to NUMBER_STRAWS-1 included;
)
{
  int code, i, tubeID;
  double apotema, dist_x, dist_y, Rrr, x, y, xcircle, ycircle;

  PndSttTube *pSttTube;
  TVector3 center;
  TVector3 wiredirection;

  //-----------------------------
  int num = NUMBER_STRAWS;

  STRAWRADIUS += 0.005; // take into account the 30 micron Mylar thickness;

  // calcolo delle regioni 'speciali';

  CalculateSpecialRegion(APOTEMASTRAWDETECTORMIN, APOTEMAMAXINNERPARSTRAW, APOTEMAMINOUTERPARSTRAW, VERTICALGAP,
                         x, // ascissa of intersection point in 1st quadrant;
                         y  // ordinate of intersection point in 1st quadrant;
  );

  //---------------------------------

  if (stampa) {
    cout << "numero totale di tubi " << num << endl;
    // il primo tubo e' il n. 1;
    tubeID = 1;
    pSttTube = (PndSttTube *)SttTubeArray->At(tubeID);
    center = pSttTube->GetPosition();
    wiredirection = pSttTube->GetWireDirection();
    Rrr = sqrt(center.X() * center.X() + center.Y() * center.Y());
    cout << "\tFirst STT straw tubeID;  n.  " << tubeID << ", centro X " << center.X() << ", centro Y " << center.Y() << ", centro Z " << center.Z() << ", fR = " << Rrr << endl;
    //	double HL = pSttTube->GetHalfLength();
    // ultimo tubo;
    tubeID = num;
    pSttTube = (PndSttTube *)SttTubeArray->At(tubeID);
    center = pSttTube->GetPosition();
    wiredirection = pSttTube->GetWireDirection();
    Rrr = sqrt(center.X() * center.X() + center.Y() * center.Y());
    cout << "\tLast STT straw tubeID,  n.  " << tubeID << ", centro X " << center.X() << ", centro Y " << center.Y() << ", centro Z " << center.Z() << ", fR = " << Rrr << endl
         << endl;
  }
  //---------------------------------

  //  Stt a sinistra (col beam in faccia) -->  + 10 nello StrawCode;
  //  Stt a destra (col beam in faccia) -->  + 20 nello StrawCode;

  //   Verticali al centro :
  // abs( X ) < 3 ;
  // 16 < abs(Y) < 25 -->  inner Stt axial section;
  // 35 < abs(Y) < 40 -->  outer Stt axial section;

  // STT  al bordo verticale centrale;
  cout << "\tBOUNDARY Straws : Central Vertical Boundary, axial STT straws Center positions :\n";
  for (i = 1; i <= num; i++) {
    pSttTube = (PndSttTube *)SttTubeArray->At(i);
    center = pSttTube->GetPosition();
    wiredirection = pSttTube->GetWireDirection();
    Rrr = sqrt(center.X() * center.X() + center.Y() * center.Y());
    // solo STT parallel;
    if (!(fabs(wiredirection.X()) < 0.00001 && fabs(wiredirection.Y()) < 0.00001))
      continue;
    if (fabs(center.X()) > 3.)
      continue;
    if (stampa) {
      cout << "\tSTT straw || tubeID n.  " << i << ", centro X " << center.X() << ", centro Y " << center.Y() << ", centro Z " << center.Z() << ", fR = " << Rrr;
    }
    if (center.X() < 0.) {
      if (fabs(center.Y()) < 30.) {
        code = 10;
      } else {
        code = 12;
      };
    } else {
      if (fabs(center.Y()) < 30.) {
        code = 20;
      } else {
        code = 22;
      };
    }

    if (stampa)
      cout << ", SttCode = " << code << endl;

    if (StrawCode[i - 1] > -1 && StrawCode[i - 1] != code) { // to this Tube a StrawCode was already previously assigned;
      // therefore set StrawCode2;
      StrawCode2[i - 1] = code;
    } else {
      StrawCode[i - 1] = code;
    }

  } // end for(i=1;i<=num;i++)

  // STT  al bordo esterno;
  if (stampa)
    cout << "\n\tExternal Round Boundary || STT straws :\n";
  for (i = 1; i <= num; i++) {
    pSttTube = (PndSttTube *)SttTubeArray->At(i);
    center = pSttTube->GetPosition();
    wiredirection = pSttTube->GetWireDirection();
    // solo STT parallel;
    if (!(fabs(wiredirection.X()) < 0.00001 && fabs(wiredirection.Y()) < 0.00001))
      continue;

    // add also 30 micron of Mylar in the calculation;
    xcircle = sqrt(RSTRAWDETECTORMAX * RSTRAWDETECTORMAX - center.Y() * center.Y());
    ycircle = sqrt(RSTRAWDETECTORMAX * RSTRAWDETECTORMAX - center.X() * center.X());
    dist_x = fabs(center.X()) + 3.5 * STRAWRADIUS;
    dist_y = fabs(center.Y()) + 3.5 * STRAWRADIUS;
    if (dist_x < xcircle && dist_y < ycircle)
      continue; // this is not a Outer Stt Straw on the boundary;
    // special exclusions :
    if (i == 3628 || i == 3514 || i == 3513 || i == 3399 || i == 3631 || i == 3738 || i == 3743 || i == 3850 || i == 3855 || i == 3949 || i == 3954 || i == 4048)
      continue;

    if (i == 3856 || i == 3948 || i == 3955 || i == 4047 || i == 4045 || i == 4137 || i == 4144 || i == 4227)
      continue;

    if (i == 4055 || i == 4136 || i == 4145 || i == 4226)
      continue;
    if (i == 4235 || i == 4301 || i == 4310 || i == 4376)
      continue;
    if (i == 4236 || i == 4300 || i == 4311 || i == 4375)
      continue;
    if (4386 <= i && i <= 4393)
      continue;
    if (4422 <= i && i <= 4429)
      continue;
    if (4440 <= i && i <= 4447)
      continue;
    if (4476 <= i && i <= 4483)
      continue;
    if (i == 3883 || i == 3921 || i == 3982 || i == 4020)
      continue;
    if (i == 3434 || i == 3478 || i == 3549 || i == 3593)
      continue;
    if (i == 3211 || i == 3250 || i == 3323 || i == 3362)
      continue;
    if (i == 3438 || i == 3474 || i == 3553 || i == 3589)
      continue;
    if (i == 3669 || i == 3700 || i == 3781 || i == 3812)
      continue;
    if (i == 3888 || i == 3916 || i == 3987 || i == 4015)
      continue;
    if (4084 <= i && i <= 4085)
      continue;
    if (4106 <= i && i <= 4107)
      continue;
    if (4174 <= i && i <= 4175)
      continue;
    if (4196 <= i && i <= 4197)
      continue;

    if (4260 <= i && i <= 4261)
      continue;
    if (4275 <= i && i <= 4276)
      continue;
    if (4335 <= i && i <= 4336)
      continue;
    if (4350 <= i && i <= 4351)
      continue;

    if (4404 <= i && i <= 4407)
      continue;
    if (4408 <= i && i <= 4411)
      continue;
    if (4458 <= i && i <= 4461)
      continue;
    if (4462 <= i && i <= 4465)
      continue;

    if (stampa)
      cout << "\tSTT straw || tubeID n.  " << i << ", centro X " << center.X() << ", centro Y " << center.Y() << ", centro Z " << center.Z() << ", fR = " << Rrr;
    if (center.X() < 0.) {
      code = 13;
      if (stampa)
        cout << ", SttCode = 13\n";
    } else {
      code = 23;
      if (stampa)
        cout << ", SttCode = 23\n";
    }

    if (StrawCode[i - 1] > -1 && StrawCode[i - 1] != code) { // to this Tube a StrawCode was already previously assigned;
      // therefore set StrawCode2;
      StrawCode2[i - 1] = code;
    } else {
      StrawCode[i - 1] = code;
    }
  } // end for(i=1;i<=num;i++)

  // boundary Inner delle Stt parallele a sinistra;

  //  apotema = DISTANZA DA (0,0) del lato dell'esagono;

  // Stt lato inner, del 2 quadrante; code 0 + n*10;
  apotema = APOTEMASTRAWDETECTORMIN;
  // equazione del bordo : x - sqrt(3)*y +2*apotema = 0 ;
  if (stampa)
    cout << "\n\tInner Boundary (smaller) with code 10; 2nd quadrant || STT straws :\n";
  double distanza;
  for (i = 1; i <= num; i++) {
    pSttTube = (PndSttTube *)SttTubeArray->At(i);
    center = pSttTube->GetPosition();
    wiredirection = pSttTube->GetWireDirection();
    // solo STT parallel;
    if (center.X() > 0.)
      continue;
    if (!(fabs(wiredirection.X()) < 0.00001 && fabs(wiredirection.Y()) < 0.00001))
      continue;
    distanza = fabs(sqrt(3.) * center.Y() - center.X() - 2 * apotema) / 2.;
    if (distanza > 2. * STRAWRADIUS || center.X() < -apotema - 2. * STRAWRADIUS)
      continue;
    Rrr = sqrt(center.X() * center.X() + center.Y() * center.Y());
    if (stampa)
      cout << "\tSTT straw || tubeID n.  " << i << ", centro X " << center.X() << ", centro Y " << center.Y() << ", centro Z " << center.Z() << ", fR = " << Rrr
           << ", SttCode = 10\n";
    code = 10;
    if (StrawCode[i - 1] > -1 && StrawCode[i - 1] != code) { // to this Tube a StrawCode was already previously assigned;
      // therefore set StrawCode2;
      StrawCode2[i - 1] = code;
    } else {
      StrawCode[i - 1] = code;
    }
  } // end for(i=1;i<=num;i++)

  // Stt lato inner, lato verticale; code 0 + n*10;
  apotema = APOTEMASTRAWDETECTORMIN;
  // equazione del bordo :  x = -apotema ;
  if (stampa)
    cout << "\n\tInner Vertical Boundary (smaller) with code 10; 2nd-3rd quadrant || STT straws :\n";
  for (i = 1; i <= num; i++) {
    pSttTube = (PndSttTube *)SttTubeArray->At(i);
    center = pSttTube->GetPosition();
    wiredirection = pSttTube->GetWireDirection();
    // solo STT parallel;
    if (!(fabs(wiredirection.X()) < 0.00001 && fabs(wiredirection.Y()) < 0.00001))
      continue;
    if (center.X() > 0.)
      continue;
    if (center.X() < -apotema - 2. * STRAWRADIUS)
      continue;
    if (fabs(center.Y()) > apotema / sqrt(3.))
      continue;
    Rrr = sqrt(center.X() * center.X() + center.Y() * center.Y());
    if (stampa)
      cout << "\tSTT straw || tubeID n.  " << i << ", centro X " << center.X() << ", centro Y " << center.Y() << ", centro Z " << center.Z() << ", fR = " << Rrr
           << ", SttCode = 10\n";
    code = 10;
    if (StrawCode[i - 1] > -1 && StrawCode[i - 1] != code) { // to this Tube a StrawCode was already previously assigned;
      // therefore set StrawCode2;
      StrawCode2[i - 1] = code;
    } else {
      StrawCode[i - 1] = code;
    }
  } // end for(i=1;i<=num;i++)

  // Stt lato inner, del 3 quadrante; code 0 + n*10;
  apotema = APOTEMASTRAWDETECTORMIN;
  // equazione del bordo : x + sqrt(3)*y +2*apotema = 0 ;
  if (stampa)
    cout << "\n\tInner Boundary (smaller) with code 10; 3nd quadrant || STT straws :\n";
  for (i = 1; i <= num; i++) {
    pSttTube = (PndSttTube *)SttTubeArray->At(i);
    center = pSttTube->GetPosition();
    wiredirection = pSttTube->GetWireDirection();
    // solo STT parallel;
    if (center.X() > 0.)
      continue;
    if (!(fabs(wiredirection.X()) < 0.00001 && fabs(wiredirection.Y()) < 0.00001))
      continue;
    distanza = fabs(-sqrt(3.) * center.Y() - center.X() - 2 * apotema) / 2.;
    if (distanza > 2. * STRAWRADIUS || center.X() < -apotema - 2. * STRAWRADIUS)
      continue;
    Rrr = sqrt(center.X() * center.X() + center.Y() * center.Y());
    if (stampa)
      cout << "\tSTT straw || tubeID n.  " << i << ", centro X " << center.X() << ", centro Y " << center.Y() << ", centro Z " << center.Z() << ", fR = " << Rrr
           << ", SttCode = 10\n";
    code = 10;
    if (StrawCode[i - 1] > -1 && StrawCode[i - 1] != code) { // to this Tube a StrawCode was already previously assigned;
      // therefore set StrawCode2;
      StrawCode2[i - 1] = code;
    } else {
      StrawCode[i - 1] = code;
    }
  } // end for(i=1;i<=num;i++)

  //  parte a destra dell'inner Stt boundary; considero la parte a sinistra e cambio il segno a tutte le X;

  // Stt lato inner, del 1 quadrante; code 0 + n*10;
  apotema = APOTEMASTRAWDETECTORMIN;
  // equazione del bordo : x +  sqrt(3)*y -2*apotema = 0 ;
  if (stampa)
    cout << "\n\tInner Boundary (smaller) with code 20; 1nd quadrant || STT straws :\n";
  for (i = 1; i <= num; i++) {
    pSttTube = (PndSttTube *)SttTubeArray->At(i);
    center = pSttTube->GetPosition();
    wiredirection = pSttTube->GetWireDirection();
    // solo STT parallel;
    if (center.X() < 0.)
      continue;
    if (!(fabs(wiredirection.X()) < 0.00001 && fabs(wiredirection.Y()) < 0.00001))
      continue;
    distanza = fabs(sqrt(3.) * center.Y() + center.X() - 2 * apotema) / 2.;
    if (distanza > 2. * STRAWRADIUS || center.X() > apotema + 2. * STRAWRADIUS)
      continue;
    Rrr = sqrt(center.X() * center.X() + center.Y() * center.Y());
    if (stampa)
      cout << "\tSTT straw || tubeID n.  " << i << ", centro X " << center.X() << ", centro Y " << center.Y() << ", centro Z " << center.Z() << ", fR = " << Rrr
           << ", SttCode = 20\n";
    code = 20;
    if (StrawCode[i - 1] > -1 && StrawCode[i - 1] != code) { // to this Tube a StrawCode was already previously assigned;
      // therefore set StrawCode2;
      StrawCode2[i - 1] = code;
    } else {
      StrawCode[i - 1] = code;
    }
  } // end for(i=1;i<=num;i++)

  // Stt lato inner, lato verticale + n*10;
  apotema = APOTEMASTRAWDETECTORMIN;
  // equazione del bordo :  x = apotema ;
  if (stampa)
    cout << "\n\tInner Vertical Boundary (smaller) with code 20; 1st-2nd quadrant || STT straws :\n";
  for (i = 1; i <= num; i++) {
    pSttTube = (PndSttTube *)SttTubeArray->At(i);
    center = pSttTube->GetPosition();
    wiredirection = pSttTube->GetWireDirection();
    // solo STT parallel;
    if (!(fabs(wiredirection.X()) < 0.00001 && fabs(wiredirection.Y()) < 0.00001))
      continue;
    if (center.X() < 0.)
      continue;
    if (center.X() > apotema + 2. * STRAWRADIUS)
      continue;
    if (fabs(center.Y()) > apotema / sqrt(3.))
      continue;
    Rrr = sqrt(center.X() * center.X() + center.Y() * center.Y());
    if (stampa)
      cout << "\tSTT straw || tubeID n.  " << i << ", centro X " << center.X() << ", centro Y " << center.Y() << ", centro Z " << center.Z() << ", fR = " << Rrr
           << ", SttCode = 20\n";
    code = 20;
    if (StrawCode[i - 1] > -1 && StrawCode[i - 1] != code) { // to this Tube a StrawCode was already previously assigned;
      // therefore set StrawCode2;
      StrawCode2[i - 1] = code;
    } else {
      StrawCode[i - 1] = code;
    }
  } // end for(i=1;i<=num;i++)

  // Stt lato inner, del 4 quadrante + n*10;
  apotema = APOTEMASTRAWDETECTORMIN;
  // equazione del bordo : x -sqrt(3)*y -2*apotema = 0 ;
  if (stampa)
    cout << "\n\tInner Boundary (smaller) with code 20; 4nd quadrant || STT straws :\n";
  for (i = 1; i <= num; i++) {
    pSttTube = (PndSttTube *)SttTubeArray->At(i);
    center = pSttTube->GetPosition();
    wiredirection = pSttTube->GetWireDirection();
    // solo STT parallel;
    if (center.X() < 0.)
      continue;
    if (!(fabs(wiredirection.X()) < 0.00001 && fabs(wiredirection.Y()) < 0.00001))
      continue;
    distanza = fabs(-sqrt(3.) * center.Y() + center.X() - 2 * apotema) / 2.;
    if (distanza > 2. * STRAWRADIUS || center.X() > apotema + 2. * STRAWRADIUS)
      continue;
    Rrr = sqrt(center.X() * center.X() + center.Y() * center.Y());
    if (stampa)
      cout << "\tSTT straw || tubeID n.  " << i << ", centro X " << center.X() << ", centro Y " << center.Y() << ", centro Z " << center.Z() << ", fR = " << Rrr
           << ", SttCode = 20\n";
    code = 20;
    if (StrawCode[i - 1] > -1 && StrawCode[i - 1] != code) { // to this Tube a StrawCode was already previously assigned;
      // therefore set StrawCode2;
      StrawCode2[i - 1] = code;
    } else {
      StrawCode[i - 1] = code;
    }
  } // end for(i=1;i<=num;i++)

  //--------------

  // la rimanente parte Inner a destra : parte obliqua del 1 quadrante; code 1 + n*10;
  // Stt lato inner, del 1 quadrante;
  apotema = APOTEMAMAXINNERPARSTRAW; // = 23.25 cm;
  // equazione del bordo : x +  sqrt(3)*y -2*apotema = 0 ;
  if (stampa)
    cout << "\n\tInner Boundary (the larger), the one with code 21; 1nd quadrant || STT straws :\n";
  for (i = 1; i <= num; i++) {
    pSttTube = (PndSttTube *)SttTubeArray->At(i);
    center = pSttTube->GetPosition();
    wiredirection = pSttTube->GetWireDirection();
    // solo STT parallel;
    if (center.X() < 0.)
      continue;
    if (!(fabs(wiredirection.X()) < 0.00001 && fabs(wiredirection.Y()) < 0.00001))
      continue;
    distanza = fabs(sqrt(3.) * center.Y() + center.X() - 2 * apotema) / 2.;
    if (distanza > 2. * STRAWRADIUS || center.X() > apotema + 2. * STRAWRADIUS)
      continue;
    Rrr = sqrt(center.X() * center.X() + center.Y() * center.Y());
    if (stampa)
      cout << "\tSTT straw || tubeID n.  " << i << ", centro X " << center.X() << ", centro Y " << center.Y() << ", centro Z " << center.Z() << ", fR = " << Rrr
           << ", SttCode = 21\n";
    code = 21;
    if (StrawCode[i - 1] > -1 && StrawCode[i - 1] != code) { // to this Tube a StrawCode was already previously assigned;
      // therefore set StrawCode2;
      StrawCode2[i - 1] = code;
    } else {
      StrawCode[i - 1] = code;
    }
  } // end for(i=1;i<=num;i++)

  // la rimanente parte Inner a destra : parte verticale a cavallo tra il 1 ed il 4 quadrante; code 1 + n*10;
  // Stt lato inner, lato verticale;
  apotema = APOTEMAMAXINNERPARSTRAW; // = 23.25 cm;
  // equazione del bordo :  x = apotema ;
  if (stampa)
    cout << "\n\tInner Vertical Boundary (the larger) the one with code 21; 1st-4th quadrant || STT straws :\n";
  for (i = 1; i <= num; i++) {
    pSttTube = (PndSttTube *)SttTubeArray->At(i);
    center = pSttTube->GetPosition();
    wiredirection = pSttTube->GetWireDirection();
    // solo STT parallel;
    if (!(fabs(wiredirection.X()) < 0.00001 && fabs(wiredirection.Y()) < 0.00001))
      continue;
    if (center.X() < 0.)
      continue;
    if (fabs(center.X() - apotema) > 2. * STRAWRADIUS)
      continue;
    if (fabs(center.Y()) > apotema / sqrt(3.))
      continue;
    Rrr = sqrt(center.X() * center.X() + center.Y() * center.Y());
    if (stampa)
      cout << "\tSTT straw || tubeID n.  " << i << ", centro X " << center.X() << ", centro Y " << center.Y() << ", centro Z " << center.Z() << ", fR = " << Rrr
           << ", SttCode = 21\n";
    code = 21;
    if (StrawCode[i - 1] > -1 && StrawCode[i - 1] != code) { // to this Tube a StrawCode was already previously assigned;
      // therefore set StrawCode2;
      StrawCode2[i - 1] = code;
    } else {
      StrawCode[i - 1] = code;
    }
  } // end for(i=1;i<=num;i++)

  // la rimanente parte Inner a destra : parte obliqua del 4 quadrante; code 1 + n*10;
  // Stt lato inner, del 4 quadrante;
  apotema = APOTEMAMAXINNERPARSTRAW; // = 23.25 cm;
  // equazione del bordo : x -sqrt(3)*y -2*apotema = 0 ;
  if (stampa)
    cout << "\n\tInner Boundary (the larger) the one with code 21; 4nd quadrant || STT straws :\n";
  for (i = 1; i <= num; i++) {
    pSttTube = (PndSttTube *)SttTubeArray->At(i);
    center = pSttTube->GetPosition();
    wiredirection = pSttTube->GetWireDirection();
    // solo STT parallel;
    if (center.X() < 0.)
      continue;
    if (!(fabs(wiredirection.X()) < 0.00001 && fabs(wiredirection.Y()) < 0.00001))
      continue;
    distanza = fabs(-sqrt(3.) * center.Y() + center.X() - 2 * apotema) / 2.;
    if (distanza > 2. * STRAWRADIUS || center.X() > apotema + 2. * STRAWRADIUS)
      continue;
    Rrr = sqrt(center.X() * center.X() + center.Y() * center.Y());
    if (stampa)
      cout << "\tSTT straw || tubeID n.  " << i << ", centro X " << center.X() << ", centro Y " << center.Y() << ", centro Z " << center.Z() << ", fR = " << Rrr
           << ", SttCode = 21\n";
    code = 21;
    if (StrawCode[i - 1] > -1 && StrawCode[i - 1] != code) { // to this Tube a StrawCode was already previously assigned;
      // therefore set StrawCode2;
      StrawCode2[i - 1] = code;
    } else {
      StrawCode[i - 1] = code;
    }
  } // end for(i=1;i<=num;i++)

  // la rimanente parte Inner a sinistra : parte obliqua del 2 quadrante; code 1 + n*10;
  // Stt lato inner (the larger) the one with code 11, del 2 quadrante;
  apotema = APOTEMAMAXINNERPARSTRAW; // = 23.25 cm;
  // equazione del bordo : x - sqrt(3)*y +2*apotema = 0 ;
  if (stampa)
    cout << "\n\tInner Boundary (the larger) the one with code 11, 2nd quadrant || STT straws :\n";
  for (i = 1; i <= num; i++) {
    pSttTube = (PndSttTube *)SttTubeArray->At(i);
    center = pSttTube->GetPosition();
    wiredirection = pSttTube->GetWireDirection();
    // solo STT parallel;
    if (center.X() > 0.)
      continue;
    if (!(fabs(wiredirection.X()) < 0.00001 && fabs(wiredirection.Y()) < 0.00001))
      continue;
    distanza = fabs(sqrt(3.) * center.Y() - center.X() - 2 * apotema) / 2.;
    if (distanza > 2. * STRAWRADIUS || center.X() < -apotema - 2. * STRAWRADIUS)
      continue;
    Rrr = sqrt(center.X() * center.X() + center.Y() * center.Y());
    if (stampa)
      cout << "\tSTT straw || tubeID n.  " << i << ", centro X " << center.X() << ", centro Y " << center.Y() << ", centro Z " << center.Z() << ", fR = " << Rrr
           << ", SttCode = 11\n";
    code = 11;
    if (StrawCode[i - 1] > -1 && StrawCode[i - 1] != code) { // to this Tube a StrawCode was already previously assigned;
      // therefore set StrawCode2;
      StrawCode2[i - 1] = code;
    } else {
      StrawCode[i - 1] = code;
    }
  } // end for(i=1;i<=num;i++)

  // la rimanente parte Inner a sinistra : parte verticale a cavallo tra il 2 e 3 quadrante; code 1 + n*10;
  // Stt lato inner, lato verticale (larger) the one with code 11;
  apotema = APOTEMAMAXINNERPARSTRAW; // = 23.25 cm;
  // equazione del bordo :  x = -apotema ;
  if (stampa)
    cout << "\n\tInner Vertical Boundary (the larger) the one with code 11; 2nd-3rd quadrant || STT straws :\n";
  for (i = 1; i <= num; i++) {
    pSttTube = (PndSttTube *)SttTubeArray->At(i);
    center = pSttTube->GetPosition();
    wiredirection = pSttTube->GetWireDirection();
    // solo STT parallel;
    if (!(fabs(wiredirection.X()) < 0.00001 && fabs(wiredirection.Y()) < 0.00001))
      continue;
    if (center.X() > 0.)
      continue;
    if (fabs(center.X() + apotema) > 2. * STRAWRADIUS)
      continue;
    if (fabs(center.Y()) > apotema / sqrt(3.))
      continue;
    Rrr = sqrt(center.X() * center.X() + center.Y() * center.Y());
    if (stampa)
      cout << "\tSTT straw || tubeID n.  " << i << ", centro X " << center.X() << ", centro Y " << center.Y() << ", centro Z " << center.Z() << ", fR = " << Rrr
           << ", SttCode = 11\n";
    code = 11;
    if (StrawCode[i - 1] > -1 && StrawCode[i - 1] != code) { // to this Tube a StrawCode was already previously assigned;
      // therefore set StrawCode2;
      StrawCode2[i - 1] = code;
    } else {
      StrawCode[i - 1] = code;
    }
  } // end for(i=1;i<=num;i++)

  // la rimanente parte Inner a sinistra : parte obliqua del 3 quadrante; code 1 + n*10;
  // Stt lato inner, del 3 quadrante (larger);
  apotema = APOTEMAMAXINNERPARSTRAW; // = 23.25 cm;
  // equazione del bordo : x + sqrt(3)*y +2*apotema = 0 ;
  if (stampa)
    cout << "\n\tInner Boundary 3nd quadrant (the larger) the one with code 11;  || STT straws :\n";
  for (i = 1; i <= num; i++) {
    pSttTube = (PndSttTube *)SttTubeArray->At(i);
    center = pSttTube->GetPosition();
    wiredirection = pSttTube->GetWireDirection();
    // solo STT parallel;
    if (center.X() > 0.)
      continue;
    if (!(fabs(wiredirection.X()) < 0.00001 && fabs(wiredirection.Y()) < 0.00001))
      continue;
    distanza = fabs(-sqrt(3.) * center.Y() - center.X() - 2 * apotema) / 2.;
    if (distanza > 2. * STRAWRADIUS || center.X() < -apotema - 2. * STRAWRADIUS)
      continue;
    Rrr = sqrt(center.X() * center.X() + center.Y() * center.Y());
    if (stampa)
      cout << "\tSTT straw || tubeID n.  " << i << ", centro X " << center.X() << ", centro Y " << center.Y() << ", centro Z " << center.Z() << ", fR = " << Rrr
           << ", SttCode = 11\n";
    code = 11;
    if (StrawCode[i - 1] > -1 && StrawCode[i - 1] != code) { // to this Tube a StrawCode was already previously assigned;
      // therefore set StrawCode2;
      StrawCode2[i - 1] = code;
    } else {
      StrawCode[i - 1] = code;
    }
  } // end for(i=1;i<=num;i++)

  //--------------------------------------------------------------------------------------------------------------

  // la rimanente parte Outer a destra : parte obliqua del 1 quadrante; code 25 per un piccolo tratto che
  // puo' essere investito da tracce che provengono da (0,0) senza aver intersecato prima gli Straws
  // assiali, e code  24 per il resto;
  // Stt lato inner, del 1 quadrante;
  apotema = APOTEMAMINOUTERPARSTRAW; // = 31.86 cm;
  // equazione del bordo : x +  sqrt(3)*y -2*apotema = 0 ;
  if (stampa)
    cout << "\n\tOuter Boundary (the larger), the one with code 24 or 25; 1nd quadrant || STT straws :\n";
  for (i = 1; i <= num; i++) {
    pSttTube = (PndSttTube *)SttTubeArray->At(i);
    center = pSttTube->GetPosition();
    wiredirection = pSttTube->GetWireDirection();
    // solo STT parallel;
    if (center.X() < 0.)
      continue;
    if (!(fabs(wiredirection.X()) < 0.00001 && fabs(wiredirection.Y()) < 0.00001))
      continue;
    distanza = fabs(sqrt(3.) * center.Y() + center.X() - 2 * apotema) / 2.;
    if (distanza > 2. * STRAWRADIUS || center.X() > apotema + 2. * STRAWRADIUS)
      continue;

    // condition to belong to the special section;
    if (center.X() - STRAWRADIUS < x) {
      code = 25;
    } else {
      code = 24;
    }

    Rrr = sqrt(center.X() * center.X() + center.Y() * center.Y());
    if (stampa)
      cout << "\tSTT straw || tubeID n.  " << i << ", centro X " << center.X() << ", centro Y " << center.Y() << ", centro Z " << center.Z() << ", fR = " << Rrr
           << ", SttCode = " << code << endl;
    if (StrawCode[i - 1] > -1 && StrawCode[i - 1] != code) { // to this Tube a StrawCode was already previously assigned;
      // therefore set StrawCode2;
      StrawCode2[i - 1] = code;
    } else {
      StrawCode[i - 1] = code;
    }
  } // end for(i=1;i<=num;i++)

  // la rimanente parte Outer a destra : parte verticale a cavallo tra il 1 ed il 4 quadrante;
  // Stt lato Outer, lato verticale;
  apotema = APOTEMAMINOUTERPARSTRAW; // = 31.86 cm;
  // equazione del bordo :  x = apotema ;
  if (stampa)
    cout << "\n\tOuter Vertical Boundary (the larger) the one with code 24; 1st-4th quadrant || STT straws :\n";
  for (i = 1; i <= num; i++) {
    pSttTube = (PndSttTube *)SttTubeArray->At(i);
    center = pSttTube->GetPosition();
    wiredirection = pSttTube->GetWireDirection();
    // solo STT parallel;
    if (!(fabs(wiredirection.X()) < 0.00001 && fabs(wiredirection.Y()) < 0.00001))
      continue;
    if (center.X() < 0.)
      continue;
    if (fabs(center.X() - apotema) > 2. * STRAWRADIUS)
      continue;
    if (fabs(center.Y()) > apotema / sqrt(3.))
      continue;
    Rrr = sqrt(center.X() * center.X() + center.Y() * center.Y());
    if (stampa)
      cout << "\tSTT straw || tubeID n.  " << i << ", centro X " << center.X() << ", centro Y " << center.Y() << ", centro Z " << center.Z() << ", fR = " << Rrr
           << ", SttCode = 24\n";
    code = 24;
    if (StrawCode[i - 1] > -1 && StrawCode[i - 1] != code) { // to this Tube a StrawCode was already previously assigned;
      // therefore set StrawCode2;
      StrawCode2[i - 1] = code;
    } else {
      StrawCode[i - 1] = code;
    }
  } // end for(i=1;i<=num;i++)

  // la rimanente parte Outer a destra : parte obliqua del 4 quadrante;
  // Stt lato Outer, del 4 quadrante;
  apotema = APOTEMAMINOUTERPARSTRAW; // = 31.86 cm;
  // equazione del bordo : x -sqrt(3)*y -2*apotema = 0 ;
  if (stampa)
    cout << "\n\tOuter Boundary (the larger) the one with code 24 or 25; 4nd quadrant || STT straws :\n";
  for (i = 1; i <= num; i++) {
    pSttTube = (PndSttTube *)SttTubeArray->At(i);
    center = pSttTube->GetPosition();
    wiredirection = pSttTube->GetWireDirection();
    // solo STT parallel;
    if (center.X() < 0.)
      continue;
    if (!(fabs(wiredirection.X()) < 0.00001 && fabs(wiredirection.Y()) < 0.00001))
      continue;
    distanza = fabs(-sqrt(3.) * center.Y() + center.X() - 2 * apotema) / 2.;
    if (distanza > 2. * STRAWRADIUS || center.X() > apotema + 2. * STRAWRADIUS)
      continue;

    // condition to belong to the special section;
    if (center.X() - STRAWRADIUS < x) {
      code = 25;
    } else {
      code = 24;
    }

    Rrr = sqrt(center.X() * center.X() + center.Y() * center.Y());
    if (stampa)
      cout << "\tSTT straw || tubeID n.  " << i << ", centro X " << center.X() << ", centro Y " << center.Y() << ", centro Z " << center.Z() << ", fR = " << Rrr
           << ", SttCode = " << code << endl;
    if (StrawCode[i - 1] > -1 && StrawCode[i - 1] != code) { // to this Tube a StrawCode was already previously assigned;
      // therefore set StrawCode2;
      StrawCode2[i - 1] = code;
    } else {
      StrawCode[i - 1] = code;
    }
  } // end for(i=1;i<=num;i++)

  // la rimanente parte Outer a sinistra : parte obliqua del 2 quadrante;
  // Stt lato Outer (the larger) the one with code 12, del 2 quadrante;
  apotema = APOTEMAMINOUTERPARSTRAW; // = 31.86 cm;
  // equazione del bordo : x - sqrt(3)*y +2*apotema = 0 ;
  if (stampa)
    cout << "\n\tOuter Boundary (the larger) the one with code 14 or 15, 2nd quadrant || STT straws :\n";
  for (i = 1; i <= num; i++) {
    pSttTube = (PndSttTube *)SttTubeArray->At(i);
    center = pSttTube->GetPosition();
    wiredirection = pSttTube->GetWireDirection();
    // solo STT parallel;
    if (center.X() > 0.)
      continue;
    if (!(fabs(wiredirection.X()) < 0.00001 && fabs(wiredirection.Y()) < 0.00001))
      continue;
    distanza = fabs(sqrt(3.) * center.Y() - center.X() - 2 * apotema) / 2.;
    if (distanza > 2. * STRAWRADIUS || center.X() < -apotema - 2. * STRAWRADIUS)
      continue;

    // condition to belong to the special section;
    if (center.X() + STRAWRADIUS > -x) {
      code = 15;
    } else {
      code = 14;
    }

    Rrr = sqrt(center.X() * center.X() + center.Y() * center.Y());
    if (stampa)
      cout << "\tSTT straw || tubeID n.  " << i << ", centro X " << center.X() << ", centro Y " << center.Y() << ", centro Z " << center.Z() << ", fR = " << Rrr
           << ", SttCode = " << code << endl;
    if (StrawCode[i - 1] > -1 && StrawCode[i - 1] != code) { // to this Tube a StrawCode was already previously assigned;
      // therefore set StrawCode2;
      StrawCode2[i - 1] = code;
    } else {
      StrawCode[i - 1] = code;
    }
  } // end for(i=1;i<=num;i++)

  // la rimanente parte Outer a sinistra : parte verticale a cavallo tra il 2 e 3 quadrante;
  // Stt lato Outer, lato verticale (larger) the one with code 14;
  apotema = APOTEMAMINOUTERPARSTRAW; // = 31.86 cm;
  // equazione del bordo :  x = -apotema ;
  if (stampa)
    cout << "\n\tOuter Vertical Boundary (the larger) the one with code 12; 2nd-3rd quadrant || STT straws :\n";
  for (i = 1; i <= num; i++) {
    pSttTube = (PndSttTube *)SttTubeArray->At(i);
    center = pSttTube->GetPosition();
    wiredirection = pSttTube->GetWireDirection();
    // solo STT parallel;
    if (!(fabs(wiredirection.X()) < 0.00001 && fabs(wiredirection.Y()) < 0.00001))
      continue;
    if (center.X() > 0.)
      continue;
    if (fabs(center.X() + apotema) > 2. * STRAWRADIUS)
      continue;
    if (fabs(center.Y()) > apotema / sqrt(3.))
      continue;
    Rrr = sqrt(center.X() * center.X() + center.Y() * center.Y());
    if (stampa)
      cout << "\tSTT straw || tubeID n.  " << i << ", centro X " << center.X() << ", centro Y " << center.Y() << ", centro Z " << center.Z() << ", fR = " << Rrr
           << ", SttCode = 14\n";
    code = 14;
    if (StrawCode[i - 1] > -1 && StrawCode[i - 1] != code) { // to this Tube a StrawCode was already previously assigned;
      // therefore set StrawCode2;
      StrawCode2[i - 1] = code;
    } else {
      StrawCode[i - 1] = code;
    }
  } // end for(i=1;i<=num;i++)

  // la rimanente parte Outer a sinistra : parte obliqua del 3 quadrante;
  // Stt lato Outer, del 3 quadrante (larger);
  apotema = APOTEMAMINOUTERPARSTRAW; // = 31.86 cm;
  // equazione del bordo : x + sqrt(3)*y +2*apotema = 0 ;
  if (stampa)
    cout << "\n\tOuter Boundary 3nd quadrant (the larger) the one with code 14 or 15;  || STT straws :\n";
  for (i = 1; i <= num; i++) {
    pSttTube = (PndSttTube *)SttTubeArray->At(i);
    center = pSttTube->GetPosition();
    wiredirection = pSttTube->GetWireDirection();
    // solo STT parallel;
    if (center.X() > 0.)
      continue;
    if (!(fabs(wiredirection.X()) < 0.00001 && fabs(wiredirection.Y()) < 0.00001))
      continue;
    distanza = fabs(-sqrt(3.) * center.Y() - center.X() - 2 * apotema) / 2.;
    if (distanza > 2. * STRAWRADIUS || center.X() < -apotema - 2. * STRAWRADIUS)
      continue;

    // condition to belong to the special section;
    if (center.X() + STRAWRADIUS > -x) {
      code = 15;
    } else {
      code = 14;
    }

    Rrr = sqrt(center.X() * center.X() + center.Y() * center.Y());
    if (stampa)
      cout << "\tSTT straw || tubeID n.  " << i << ", centro X " << center.X() << ", centro Y " << center.Y() << ", centro Z " << center.Z() << ", fR = " << Rrr
           << ", SttCode = " << code << endl;
    if (StrawCode[i - 1] > -1 && StrawCode[i - 1] != code) { // to this Tube a StrawCode was already previously assigned;
      // therefore set StrawCode2;
      StrawCode2[i - 1] = code;
    } else {
      StrawCode[i - 1] = code;
    }
  } // end for(i=1;i<=num;i++)

  return;
}

//------------------ end function  PndTrkBoundaryParStraws2::SttTubeList

ClassImp(PndTrkBoundaryParStraws2);
