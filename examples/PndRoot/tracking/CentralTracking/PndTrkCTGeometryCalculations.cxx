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

#include "PndTrkCTGeometryCalculations.h"
#include "PndTrkMergeSort.h"
#include "PndTrkConstants.h"
#include <iostream>
#include <cmath>
#include <math.h>
// Root includes
#include "TROOT.h"

using namespace std;

//----------begin of function PndTrkCTGeometryCalculations::CalculateArcLength

Double_t PndTrkCTGeometryCalculations::CalculateArcLength(Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t charge,
                                                          Double_t *Xcross, // entrance-exit point
                                                          Double_t *Ycross  // entrance-exit point
)
{
  Double_t dis, theta1, theta2;

  theta1 = atan2(Ycross[0] - Oyy, Xcross[0] - Oxx);
  theta2 = atan2(Ycross[1] - Oyy, Xcross[1] - Oxx);

  if (charge > 0) { // the rotation was clockwise.
    dis = theta1 - theta2;
  } else { // the rotation was counterclockwise.
    dis = theta2 - theta1;
  }

  if (dis < 0.)
    dis += TWO_PI;
  if (dis < 0.)
    dis = 0.;

  dis *= Rr;

  return dis;
}
//----------end of function PndTrkCTGeometryCalculations::CalculateArcLength

//----------begin of function PndTrkCTGeometryCalculations::CalculateCircleThru3Points

bool PndTrkCTGeometryCalculations::CalculateCircleThru3Points(Double_t x1, Double_t y1, Double_t x2, Double_t y2, Double_t x3, Double_t y3, Double_t *o_x, Double_t *o_y,
                                                              Double_t *r_r)
{
  // inspiration from  http://mathworld.wolfram.com/Circle.html

  Double_t a, d, e, q1, q2, q3;

  a = x1 * y2 + y1 * x3 + x2 * y3 - x3 * y2 - x2 * y1 - x1 * y3;

  if (fabs(a) < 1.e-10)
    return false;

  q1 = x1 * x1 + y1 * y1;
  q2 = x2 * x2 + y2 * y2;
  q3 = x3 * x3 + y3 * y3;

  d = -(q1 * y2 + y1 * q3 + q2 * y3 - q3 * y2 - q2 * y1 - q1 * y3);

  e = -(x1 * q2 + q1 * x3 + x2 * q3 - x3 * q2 - x2 * q1 - x1 * q3);

  *o_x = -0.5 * d / a;
  *o_y = -0.5 * e / a;
  *r_r = sqrt((x1 - *o_x) * (x1 - *o_x) + (y1 - *o_y) * (y1 - *o_y));

  return true;
}
//----------end of function PndTrkCTGeometryCalculations::CalculateCircleThru3Points

//-------------------  begin of function   PndTrkCTGeometryCalculations::calculateintersections

void PndTrkCTGeometryCalculations::calculateintersections(Double_t Oxx, Double_t Oyy, Double_t Rr, Double_t C0x, Double_t C0y, Double_t C0z, Double_t /*r*/, //[R.K. 9/2018] unused
                                                          Double_t vx, Double_t vy, Double_t vz, Int_t *STATUS, Double_t *POINTS)
{

  //-------------------------------------------

  // Double_t P1x, P1y, P1z, P2x, P2y, P2z; //[R.K. 01/2017] unused variable?

  Double_t AAA, DELTA, ax, ay; //, aaa; //[R.K. 01/2017] unused variable?

  /*
   INPUTS :

    Oxx, Oyy        = abscissa and ordinate of the center of the circular trajectory of
                    the particle;
    Rr             = radius of such trajectory;
    C0x, C0y, C0z = x, y, z coordinates of a point belonging to the axis of the
                    skewed straw;
    r  = radius of equidrift of such skewed straw;
    vx, vy, vz    =  versor of the direction along which the skewed straw lies.


   OUTPUTS :

    P1x, P1y, P1z  =  x, y, z coordinates of the point intersection between the
                      particle trajectory circle and the equidrift cylinder of
                      the skewed straw calculated as a function of theta (first
                      solution);
    P2x, P2y, P2z  =  x, y, z coordinates of the point intersection between the
                      particle trajectory circle and the equidrift cylinder of
                      the skewed straw calculated as a function of theta (second
                      solution);

   P1x, P1y, .... , P2z  are stored in the array POINTS[0-5];  the status is stored
   in *STATUS.

  */

  //--------------------------------------------------------------------------

  // from the resolving formula on page 23 of Gianluigi's notes, setting  r=0.

  ax = C0x - Oxx;
  ay = C0y - Oyy;
  DELTA = Rr * Rr * (vx * vx + vy * vy) - (vx * ay - vy * ax) * (vx * ay - vy * ax);
  AAA = vx * vx + vy * vy;

  if (DELTA < 0.) {
    *STATUS = -2;
  } else if (AAA == 0.) {
    *STATUS = -3;
  } else if (DELTA == 0.) {
    *STATUS = 1;
    POINTS[0] = C0x - vx * (vx * ax + vy * ay) / AAA;
    POINTS[1] = C0y - vy * (vx * ax + vy * ay) / AAA;
    POINTS[2] = C0z - vz * (vx * ax + vy * ay) / AAA;
  } else {
    *STATUS = 0;
    DELTA = sqrt(DELTA);
    POINTS[0] = C0x - vx * (vx * ax + vy * ay - DELTA) / AAA;
    POINTS[1] = C0y - vy * (vx * ax + vy * ay - DELTA) / AAA;
    POINTS[2] = C0z - vz * (vx * ax + vy * ay - DELTA) / AAA;
    POINTS[3] = C0x - vx * (vx * ax + vy * ay + DELTA) / AAA;
    POINTS[4] = C0y - vy * (vx * ax + vy * ay + DELTA) / AAA;
    POINTS[5] = C0z - vz * (vx * ax + vy * ay + DELTA) / AAA;
  }

  return;
}

//----------end of function PndTrkCTGeometryCalculations::calculateintersections

//------------------------- begin of function  PndTrkCTGeometryCalculations::CalculateSandZ

void PndTrkCTGeometryCalculations::CalculateSandZ(Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t skewnum, Double_t info[][7], Double_t *WDX, Double_t *WDY, Double_t *WDZ,
                                                  Double_t S[2],      // output;
                                                  Double_t Z[2],      // output; Zcoordinate of the central wire.
                                                  Double_t Zdrift[2], // output; drift radius projected onto the Helix.
                                                  Double_t Zerror[2]  // output;  150 micron projected onto the Helix.
)
{

  // this method is the same as CalculateSandZ2 only it does NOT return the Sdrift[2] array;

  Double_t Sdrift[2];

  CalculateSandZ2(Oxx,     // input;
                  Oyy,     // input;
                  Rr,      // input;
                  skewnum, // input;
                  info,    // input;
                  WDX,     // input;
                  WDY,     // input;
                  WDZ,     // input;
                  S,       // output;
                  Sdrift,  // output; drift radius projected onto the Helix S direction.
                  Z,       // output;  Zcoordinate of the central wire.
                  Zdrift,  // output; drift radius projected onto the Helix Z direction.
                  Zerror   // output;  150 micron projected onto the Helix.
  );
}

//-------------------------  end of function  PndTrkCTGeometryCalculations::CalculateSandZ

//------------------------- begin of function  PndTrkCTGeometryCalculations::CalculateSandZ2

void PndTrkCTGeometryCalculations::CalculateSandZ2(Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t skewnum, Double_t info[][7], Double_t *WDX, Double_t *WDY, Double_t *WDZ,
                                                   Double_t S[2],      // output;
                                                   Double_t Sdrift[2], // output; drift radius projected onto the Helix S direction;
                                                   Double_t Z[2],      // output;  Zcoordinate of the central wire.
                                                   Double_t Zdrift[2], // output; drift radius projected onto the Helix Z direction;
                                                   Double_t Zerror[2]  // output;  150 micron projected onto the Helix.
)
{

  Int_t STATUS;
  Short_t i, j, ii;

  Double_t aaa, bbb, Bmax, distance, Aellipsis1, Bellipsis1, Rx, Ry, sinDelta, sinTheta, vx1, vy1, vz1, C0x1, C0y1, C0z1, POINTS1[6];

  // necessary initialization of the intersection position;
  Z[0] = 999999.;
  Z[1] = 999999.;
  i = skewnum;

  aaa = sqrt(WDX[i] * WDX[i] + WDY[i] * WDY[i] + WDZ[i] * WDZ[i]);
  vx1 = WDX[i] / aaa;
  vy1 = WDY[i] / aaa;
  vz1 = WDZ[i] / aaa;
  C0x1 = info[i][0];
  C0y1 = info[i][1];
  C0z1 = info[i][2];

  calculateintersections(Oxx, Oyy, Rr, C0x1, C0y1, C0z1, info[i][3], vx1, vy1, vz1, &STATUS, POINTS1);

  if (STATUS < 0)
    return;

  // loop on the two possible intersections of the middle wire the straw with the
  // trajectory circle; if that is unphysical then the intersection
  // remains 999999.
  for (ii = 0; ii < 2; ii++) {
    j = 3 * ii;
    distance = sqrt((POINTS1[j] - C0x1) * (POINTS1[j] - C0x1) + (POINTS1[1 + j] - C0y1) * (POINTS1[1 + j] - C0y1) + (POINTS1[2 + j] - C0z1) * (POINTS1[2 + j] - C0z1));

    Rx = POINTS1[j] - Oxx;     //  x component Radial vector of cylinder of trajectory
    Ry = POINTS1[1 + j] - Oyy; //  y direction Radial vector of cylinder of trajectory

    //  the tilt direction of this ellipse is (1,0)  when major axis along Z direction

    sinTheta = vx1 * vx1 + vy1 * vy1;
    if (sinTheta < 1.e-10)
      continue;
    sinTheta = sqrt(sinTheta);
    Aellipsis1 = info[i][3] / sinTheta;

    // Bellipsis1 must be in radians;
    sinDelta = 1. - (-Ry * vx1 + Rx * vy1) * (-Ry * vx1 + Rx * vy1) / (Rx * Rx + Ry * Ry);
    // for the derivation of the maximum Bellipsis1 possible see Logbook Panda II on page 109;
    bbb = info[i][3] * (2. * Rr - info[i][3]);
    if (bbb < 1.e-10)
      continue;
    Bmax = 2. * info[i][3] / sqrt(bbb);
    if (sinDelta < 1.e-10)
      Bellipsis1 = Bmax;
    sinDelta = sqrt(sinDelta);
    Bellipsis1 = info[i][3] / (Rr * sinDelta);
    if (Bmax < Bellipsis1)
      Bellipsis1 = Bmax;

    // if the intersection point is unphysical, out of the straw length, quit but set Z[ii] to 1000000.+distance ;
    if (distance >= info[i][4] + Aellipsis1) {
      Z[ii] = 1000000. + distance;
      continue;
    }

    //--------------------------
    S[ii] = atan2(POINTS1[j + 1] - Oyy, POINTS1[j] - Oxx); // atan2 returns radians in (-pi and +pi]
    if (S[ii] < 0.)
      S[ii] += TWO_PI;

    Sdrift[ii] = Bellipsis1;

    Z[ii] = POINTS1[j + 2];
    Zdrift[ii] = Aellipsis1;
    Zerror[ii] = STRAWRESOLUTION / sinTheta;

    //	Double_t rotation1 = 180.*atan2(Tiltdirection1[1],Tiltdirection1[0])/PI;

  } //  end of    for( ii=0; ii<2; ii++)

  return;
}

//-------------------------  end of function  PndTrkCTGeometryCalculations::CalculateSandZ2

//----------star of function PndTrkCTGeometryCalculations::ChooseEntranceExitbis
void PndTrkCTGeometryCalculations::ChooseEntranceExitbis(Double_t Oxx, Double_t Oyy, Short_t Charge, Double_t FiStart, Short_t nIntersections, Double_t *XintersectionList,
                                                         Double_t *YintersectionList,
                                                         Double_t Xcross[2], // output
                                                         Double_t Ycross[2]  // output
)
{

  Short_t i;

  PndTrkMergeSort MergeSort;

  // this method works under the hypothesis that there are at least 2 intersections.
  // It orders the nIntersections  intersections using as order parameter the azimuthal
  // angle fi (in the reference frame of the trajectory helix); then it returns the FIRST
  // TWO INTERSECTIONS assuming that the track was originated from (0,0,0) and taking into
  // account its charge;

  if (nIntersections < 2)
    return;

  if (Charge > 0) {
    Int_t auxIndex[100];
    Double_t fi[100];
    for (i = 0; i < nIntersections; i++) {
      fi[i] = atan2(YintersectionList[i] - Oyy, XintersectionList[i] - Oxx);
      if (fi[i] < 0.)
        fi[i] += TWO_PI;
      if (fi[i] > FiStart)
        fi[i] -= TWO_PI;
      if (fi[i] > FiStart)
        fi[i] = FiStart;
      auxIndex[i] = i;
    } // end of for( i=0;i<nIntersections;i++)
    MergeSort.Merge_Sort(nIntersections, fi, auxIndex);
    Xcross[0] = XintersectionList[auxIndex[nIntersections - 1]];
    Ycross[0] = YintersectionList[auxIndex[nIntersections - 1]];
    Xcross[1] = XintersectionList[auxIndex[nIntersections - 2]];
    Ycross[1] = YintersectionList[auxIndex[nIntersections - 2]];

  } else { // case in which Charge is negative.
    Int_t auxIndex[nIntersections];
    Double_t fi[nIntersections];
    for (i = 0; i < nIntersections; i++) {
      fi[i] = atan2(YintersectionList[i] - Oyy, XintersectionList[i] - Oxx);
      if (fi[i] < 0.)
        fi[i] += TWO_PI;
      if (fi[i] < FiStart)
        fi[i] += TWO_PI;
      if (fi[i] < FiStart)
        fi[i] += FiStart;
      auxIndex[i] = i;
    } // end of for( i=0;i<nIntersections;i++)
    MergeSort.Merge_Sort(nIntersections, fi, auxIndex);
    Xcross[0] = XintersectionList[auxIndex[0]];
    Ycross[0] = YintersectionList[auxIndex[0]];
    Xcross[1] = XintersectionList[auxIndex[1]];
    Ycross[1] = YintersectionList[auxIndex[1]];
  }
}
//----------end of function PndTrkCTGeometryCalculations::ChooseEntranceExitbis

//----------star of function PndTrkCTGeometryCalculations::ChooseEntranceExit3
void PndTrkCTGeometryCalculations::ChooseEntranceExit3(Double_t Oxx, Double_t Oyy, Short_t Charge,
                                                       Double_t FiStart, // input; it must be >0.; Fi angle of the starting point
                                                                         // of the trajectory in the Helix reference frame;
                                                       Short_t nIntersections,
                                                       Double_t *XintersectionList, // input and output;
                                                       Double_t *YintersectionList, // input and output;
                                                       Double_t *FiOrderedList      // output
)
{

  Short_t i;
  // j; //[R.K. 01/2017] unused variable?

  Int_t auxIndex[nIntersections];

  Double_t auxX[nIntersections], auxY[nIntersections], fi[nIntersections];

  PndTrkMergeSort MergeSort;

  // this method works under the hypothesis that there are at least 2 intersections.
  // It is the same as ChooseEntranceExitbis but it gives in output the values of the ordered
  // Fi ( in FiOrderedLis);

  if (nIntersections < 2)
    return;

  if (Charge > 0) { // charge positive, particle rotates clockwise when looking into the beam;
    for (i = 0; i < nIntersections; i++) {
      fi[i] = atan2(YintersectionList[i] - Oyy, XintersectionList[i] - Oxx);
      if (fi[i] < 0.)
        fi[i] += TWO_PI;
      if (fi[i] > FiStart)
        fi[i] -= TWO_PI;
      if (fi[i] > FiStart)
        fi[i] = FiStart;
      auxIndex[i] = i;
    } // end of for( i=0;i<nIntersections;i++)
    MergeSort.Merge_Sort(nIntersections, fi, auxIndex);
    for (i = 0; i < nIntersections; i++) {
      auxX[i] = XintersectionList[auxIndex[nIntersections - i - 1]];
      auxY[i] = YintersectionList[auxIndex[nIntersections - i - 1]];
      FiOrderedList[i] = fi[nIntersections - i - 1];
    } // end of for( i=0;i<nIntersections;i++)

  } else { // case in which charge is negative; particle rotates counterclockwise when looking into the beam;
    for (i = 0; i < nIntersections; i++) {
      fi[i] = atan2(YintersectionList[i] - Oyy, XintersectionList[i] - Oxx);
      if (fi[i] < 0.)
        fi[i] += TWO_PI;
      if (fi[i] < FiStart)
        fi[i] += TWO_PI;
      if (fi[i] < FiStart)
        fi[i] += FiStart;
      auxIndex[i] = i;

    } // end of for( i=0;i<nIntersections;i++)
    MergeSort.Merge_Sort(nIntersections, fi, auxIndex);

    for (i = 0; i < nIntersections; i++) {
      auxX[i] = XintersectionList[auxIndex[i]];
      auxY[i] = YintersectionList[auxIndex[i]];
      FiOrderedList[i] = fi[i];
    } // end of for( i=0;i<nIntersections;i++)

  } // end of if(Charge > 0)

  for (i = 0; i < nIntersections; i++) {
    XintersectionList[i] = auxX[i];
    YintersectionList[i] = auxY[i];
  } // end of for( i=0;i<nIntersections;i++)
}
//----------end of function PndTrkCTGeometryCalculations::ChooseEntranceExit3

//-------------------------  begin of function  PndTrkCTGeometryCalculations::Dist_SZ

Double_t PndTrkCTGeometryCalculations::Dist_SZ(Double_t Rr, Double_t KAPPA, Double_t FI0, Double_t ZED, Double_t S, Int_t *nrounds)
{

  //	Defining :	ZZ = (S-FI0)/KAPPA
  //	this method returns the distance (WITH ITS SIGN ) :  ZZ - ZED.  Therefore this number
  //	can be negative.
  //	Care is taken to calculate this distance properly taking into
  //	account that we are dealing with the function  FI = mod(KAPPA*Z + FI0, 2*3.14).

  // the limits of an Int_t are : -2147483648 <= n <= 2147483647

  Double_t aaa,
    // ABSdis1, //[R.K. 01/2017] unused variable?
    dis1, dis2, dis_segments;
  // gap; //[R.K. 01/2017] unused variable?

  if (fabs(KAPPA) < 1.e-10) {
    return -999999999.;
  } else if (fabs(KAPPA) > 1.e10) {
    return -ZED;
  }

  //	gap = fabs(TWO_PI/KAPPA);

  aaa = (KAPPA * ZED) / TWO_PI;
  if (aaa < -2147483648.) {
    *nrounds = -2147483647;
  } else if (aaa > 2147483647.) {
    *nrounds = 2147483647;
  } else {
    *nrounds = (Int_t)aaa;
  }

  dis_segments = TWO_PI * Rr / sqrt(1. + KAPPA * KAPPA * Rr * Rr); // distance between
                                                                   // two consecutive segments
                                                                   //  of trajectory.

  dis1 = fabs(Rr * S - Rr * KAPPA * ZED - Rr * FI0) / sqrt(1. + KAPPA * KAPPA * Rr * Rr);
  dis1 = fmod(dis1, dis_segments);
  dis2 = dis_segments - dis1;
  if (dis2 < 0.)
    dis2 = 0.;

  if (dis1 < dis2) {
    return dis1;
  } else {
    return dis2;
  }
}

//-------------------------  end of function  PndTrkCTGeometryCalculations::Dist_SZ

//-------------------------  begin of function  PndTrkCTGeometryCalculations::Dist_SZ_bis

Double_t PndTrkCTGeometryCalculations::Dist_SZ_bis(Double_t /*Rr*/,          //[R.K. 9/2018] unused// input
                                                   Double_t KAPPA,           // input
                                                   Double_t FI0,             // input
                                                   Double_t ZED,             // input
                                                   Double_t S,               // input
                                                   Short_t n_allowed_rounds, // input, number of maximum allowed turns. That means that the number of turns
                                                                             // can go from 0 to  n_allowed_rounds.  It canNOT be negative even when Pz<0 !!!;
                                                   Double_t signPz,          // input, it indicates if the tracks goes forward (Pz>0) or backwords (Pz<0);
                                                   Double_t &chosenS         // output, the S of the point closer to the trajectory and with an
                                                                             // allowed number of roounds;
)
{

  Short_t i;
  // nMax, //[R.K. 01/2017] unused variable?
  // nMin //[R.K. 01/2017] unused variable?

  Double_t boundaries[n_allowed_rounds + 3], chosenZ, deltaz, dis, dis_new, z_on_trajectory, z2pi;

  // check that KAPPA is not zero;
  if (fabs(KAPPA) < 1.e-10) {
    cout << "PdnTrkCTGeometryCalculations::Dist_SZ_bis  KAPPA < 1.e-10, returning dist=999999.\t";
    return 999999.;
  }

  // calculate z2pi, see logbook on page 140 for meaning;

  // calculate deltaz, see logbook on page 140 for meaning; the following formula is valid for any combination
  // of KAPPA and signPz;
  deltaz = TWO_PI / fabs(KAPPA);

  // all the calculations are shown in Gianluigi's logbook on page 140;
  // case with Pz > 0, the particle is moving forward;

  // for the LITTLE SKEW straws the following is not true, the calculation
  // should be much more conservative; leave it the same as for the long Skew straw for the moment;
  boundaries[0] = 0.;
  if (signPz > 0) {
    // particle moving forward;

    // calculate z2pi, see logbook on page 140 for meaning;
    if (KAPPA > 0.) {
      z2pi = (TWO_PI - FI0) / KAPPA;
      if (z2pi < 0.)
        z2pi = 0.;
    } else {
      z2pi = -FI0 / KAPPA;
    }

    // the intervals relevant for the trajectory in SZ are  n_allowed_rounds+2 (logbook page 141);
    // loading first the limits of the boundaries;

    //		boundaries[n_allowed_rounds+2] = (n_allowed_rounds+1)*deltaz;

    // the last boundary is the maximum length of the Skew Straw IN Z (NOT the physical length)
    // and therefore :
    boundaries[n_allowed_rounds + 2] = ZCENTER_STRAIGHT + SEMILENGTH_STRAIGHT;
    for (i = 1; i < n_allowed_rounds + 2; i++) {
      boundaries[i] = z2pi + (i - 1) * deltaz;
    } // end of for (i=1; i<n_allowed_rounds+2;i++)

    dis = 99999999999.;
    for (i = 0; i < n_allowed_rounds + 2; i++) {
      z_on_trajectory = (S - FI0 + i * TWO_PI) / KAPPA;
      // don't accept this solution if it is out of the Z range of this
      // piece of trajectory in SZ; see logbook on page 141;
      if (z_on_trajectory < boundaries[i] || z_on_trajectory > boundaries[i + 1])
        continue;
      dis_new = fabs(ZED - z_on_trajectory);
      if (dis_new < dis) {
        dis = dis_new;
        chosenZ = z_on_trajectory; // used later to calculate chosenS;
      }
    } // end of  for(i=0; i< n_allowed_rounds+2; i++)

  } else { // in this case Pz <0;
    // particle moving backward;

    // calculate z2pi, see logbook on page 140 for meaning;
    if (KAPPA > 0.) {
      z2pi = FI0 / KAPPA;
    } else {
      z2pi = -(TWO_PI - FI0) / KAPPA;
      if (z2pi < 0.)
        z2pi = 0.;
    }

    // the intervals relevant for the trajectory in SZ are  n_allowed_rounds+2 (logbook page 143);
    // loading first the limits of the boundaries;

    //		boundaries[n_allowed_rounds+2] = -(n_allowed_rounds+1)*deltaz;

    // the last boundary is the minimum length of the Skew Straw IN Z (NOT the physical length)
    // and therefore :
    boundaries[n_allowed_rounds + 2] = ZCENTER_STRAIGHT - SEMILENGTH_STRAIGHT;
    for (i = 1; i < n_allowed_rounds + 2; i++) {
      boundaries[i] = -z2pi - (i - 1) * deltaz;
    } // end of for (i=1; i<n_allowed_rounds+2;i++)

    dis = 99999999999.;
    for (i = 0; i < n_allowed_rounds + 2; i++) {
      z_on_trajectory = (S - FI0 + i * TWO_PI) / KAPPA;
      // don't accept this solution if it is out of the Z range of this
      // piece of trajectory in SZ; see logbook on page 143;
      // since here we are with Z < 0, the boundaries are exchanged compared to the
      // case whe Pz >  0;
      if (z_on_trajectory > boundaries[i] || z_on_trajectory < boundaries[i + 1])
        continue;
      dis_new = fabs(ZED - z_on_trajectory);
      if (dis_new < dis) {
        dis = dis_new;
        chosenZ = z_on_trajectory; // used later to calculate chosenS;
      }
    } // end of  for(i=0; i< n_allowed_rounds+2; i++)

  } // end of  if(signPz>0)

  // calculate the S corresponding to this hit; this S can be < 0. or > 2PI; so it coincides
  // with the input S only when the number of rounds of this hit are 0;
  chosenS = FI0 + KAPPA * chosenZ;

  return dis;
}

//-------------------------  end of function  PndTrkCTGeometryCalculations::Dist_SZ_bis

//------------------------- begin of function  PndTrkCTGeometryCalculations::FindDistance

Double_t PndTrkCTGeometryCalculations::FindDistance(Double_t Oxx, //  center from wich distance is calculated
                                                    Double_t Oyy, //  center from wich distance is calculated
                                                    Double_t Rr, Double_t tanlow, Double_t tanmid, Double_t tanup,
                                                    Double_t alfa, //  intersection circumference parameter
                                                    Double_t beta, //  intersection circumference parameter
                                                    Double_t gamma //  intersection circumference parameter
)
{

  Short_t i, n;

  Double_t Delta, m[3], q, dist, dist1, dist2,
    // distlow, //[R.K. 01/2017] unused variable?
    // distmid, //[R.K. 01/2017] unused variable?
    // distup, //[R.K. 01/2017] unused variable?
    totaldist, x1, x2, y1, y2;

  // int nevento=1; //[R.K. 01/2017] unused variable?

  m[0] = tanlow;
  m[1] = tanmid;
  m[2] = tanup;

  n = 0;
  totaldist = 0.;

  for (i = 0; i < 3; i++) {

    if (tanlow < 999998.) {
      q = Oyy - m[i] * Oxx;
      Delta = alfa * alfa + beta * beta * m[i] * m[i] - 4. * gamma * m[i] * m[i] - 4. * q * q + 4. * m[i] * q * alfa + +2. * alfa * beta * m[i] - 4. * beta * q - 4. * gamma;
      if (Delta < 0.) {
        dist = -1.;
      } else if (Delta == 0.) {
        x1 = 0.5 * (-alfa - 2. * m[i] * q - beta * m[i]) / (1. + m[i] * m[i]);
        y1 = m[i] * x1 + q;
        dist = fabs(sqrt((Oxx - x1) * (Oxx - x1) + (Oyy - y1) * (Oyy - y1)) - Rr);
      } else {
        Delta = sqrt(Delta);
        x1 = 0.5 * (-alfa - 2. * m[i] * q - beta * m[i] - Delta) / (1. + m[i] * m[i]);
        x2 = 0.5 * (-alfa - 2. * m[i] * q - beta * m[i] + Delta) / (1. + m[i] * m[i]);
        y1 = m[i] * x1 + q;
        y2 = m[i] * x2 + q;
        dist1 = fabs(sqrt((Oxx - x1) * (Oxx - x1) + (Oyy - y1) * (Oyy - y1)) - Rr);
        dist2 = fabs(sqrt((Oxx - x2) * (Oxx - x2) + (Oyy - y2) * (Oyy - y2)) - Rr);
        if (dist1 < dist2) {
          dist = dist1;
        } else {
          dist = dist2;
        }
      }
    } else {
      Delta = beta * beta - 4. * Oxx * Oxx - 4. * Oxx * alfa - 4. * gamma;

      if (Delta < 0.) {
        dist = -1.;
      } else if (Delta == 0.) {
        dist = fabs(fabs(Oyy + 0.5 * beta) - Rr);
      } else {
        Delta = sqrt(Delta);
        y1 = -0.5 * beta + Delta / 2.;
        y2 = -0.5 * beta - Delta / 2.;
        dist1 = fabs(fabs(Oyy + 0.5 * beta + Delta / 2.) - Rr);
        dist2 = fabs(fabs(Oyy + 0.5 * beta - Delta / 2.) - Rr);
        if (dist1 < dist2)
          dist = dist1;
        else
          dist = dist2;
      }
    }

    if (dist > -0.5) {
      totaldist += dist;
      n++;
    }

  } //   end of  for(i=0;i<3;i++)

  if (n != 3)
    totaldist = -1.;
  else
    totaldist = totaldist / n;

  return totaldist;
}

//------------------------- end of function  PndTrkCTGeometryCalculations::FindDistance

//----------start  function PndTrkCTGeometryCalculations::FindingParallelTrackAngularRange

void PndTrkCTGeometryCalculations::FindingParallelTrackAngularRange(Double_t oX, Double_t oY, Double_t Rr, Short_t Charge, Double_t *Fi_low_limit, Double_t *Fi_up_limit,
                                                                    Short_t *status,
                                                                    Double_t Rmi, // Rmin of cylindrical volume intersected by track;
                                                                    Double_t Rma  // Rmax of cylindrical volume intersected by track;
)
{
  // -------------- calculate the maximum fi and minimum fi spanned by this track,

  // see logbook pag.270; by using the Rmin and Rmax of the straw detector.
  // this function works also when circular trajectory in XY doesn't pass through (0,0).

  bool intersection_inner, intersection_outer;
  Double_t // teta1, //[R.K. 01/2017] unused variable?
           // teta2, //[R.K. 01/2017] unused variable?
           // tetavertex, //[R.K. 01/2017] unused variable?
    a,
    // cosT, //[R.K. 01/2017] unused variable?
    // cost, //[R.K. 01/2017] unused variable?
    cosFi, cosfi, Fi, fi, FI0;
  // Px, //[R.K. 01/2017] unused variable?
  // Py, //[R.K. 01/2017] unused variable?
  // tmp; //[R.K. 01/2017] unused variable?

  Rma += 1.; // add a safety margin.
  Rmi -= 1.; // add a safety margin.

  a = sqrt(oX * oX + oY * oY);

  //  preliminary condition
  if (a + Rr <= Rmi) // in this case there might be hits at radius < Rmi.
  {
    *status = -1;
    return;
  }
  if (a >= Rr + Rma || Rr >= a + Rma) // in this case there can be no hits with radius < Rma.
  {
    *status = -2;
    return;
  }

  if (a - Rr >= Rmi)
    intersection_inner = false;
  else
    intersection_inner = true;

  if (a + Rr <= Rma || a - Rr >= Rma)
    intersection_outer = false;
  else
    intersection_outer = true;

  if ((!intersection_inner) && (!intersection_outer)) {
    *Fi_low_limit = 0.;
    *Fi_up_limit = TWO_PI;
    *status = 1;
    return;
  }

  //	now the calculation

  FI0 = atan2(-oY, -oX);
  if (intersection_outer) {
    cosFi = (a * a + Rr * Rr - Rma * Rma) / (2. * Rr * a);
    if (cosFi < -1.)
      cosFi = -1.;
    else if (cosFi > 1.)
      cosFi = 1.;
    Fi = acos(cosFi);
  }

  if (intersection_inner) {
    cosfi = (a * a + Rr * Rr - Rmi * Rmi) / (2. * Rr * a);
    if (cosfi < -1.)
      cosfi = -1.;
    else if (cosfi > 1.)
      cosfi = 1.;
    fi = acos(cosfi);
  }

  if (Charge < 0) { // this particle rotates counterclockwise when looking into the beam
    if (intersection_outer && intersection_inner) {
      *Fi_low_limit = FI0 + fi;
      *Fi_up_limit = FI0 + Fi;

    } else if (intersection_inner) {
      *Fi_low_limit = FI0 + fi;
      *Fi_up_limit = FI0 - fi;
    } else { // case with intersection_outer=true; intersection_inner=false;
      *Fi_low_limit = FI0 - Fi;
      *Fi_up_limit = FI0 + Fi;
    } // end of    if( intersection_outer && intersection_inner

  } else { // continuation of   if( Charge < 0.)

    if (intersection_outer && intersection_inner) {
      *Fi_low_limit = FI0 - Fi;
      *Fi_up_limit = FI0 - fi;

    } else if (intersection_inner) {
      *Fi_low_limit = FI0 + fi; // must invert because low limit must be < up limit
      *Fi_up_limit = FI0 - fi;
    } else { // case with intersection_outer=true; intersection_inner=false;
      *Fi_low_limit = FI0 - Fi;
      *Fi_up_limit = FI0 + Fi;
    } // end of    if( intersection_outer && intersection_inner

  } // end of  if( Charge < 0.)

  if (*Fi_low_limit < 0.) {
    *Fi_low_limit = fmod(*Fi_low_limit, TWO_PI);
    *Fi_low_limit += TWO_PI;
  } else if (*Fi_low_limit >= TWO_PI) {
    *Fi_low_limit = fmod(*Fi_low_limit, TWO_PI);
  }
  if (*Fi_up_limit < 0.) {
    *Fi_up_limit = fmod(*Fi_up_limit, TWO_PI);
    *Fi_up_limit += TWO_PI;
  } else if (*Fi_up_limit >= TWO_PI) {
    *Fi_up_limit = fmod(*Fi_up_limit, TWO_PI);
  }

  //	Modify *Fi_up_limit by adding
  //	2PI if it is the case, in order to make *Fi_up_limit > *Fi_low_limit.
  if (*Fi_up_limit < *Fi_low_limit)
    *Fi_up_limit += TWO_PI;
  if (*Fi_up_limit < *Fi_low_limit)
    *Fi_up_limit = *Fi_low_limit;

  return;
}

//---------- end of  function PndTrkCTGeometryCalculations::FindingParallelTrackAngularRange

//----------start  function PndTrkCTGeometryCalculations::FindingParallelTrackAngularRange2

void PndTrkCTGeometryCalculations::FindingParallelTrackAngularRange2(Double_t oX,              // input;
                                                                     Double_t oY,              // input;
                                                                     Double_t Rma,             // Rmax of cylindrical volume intersected by track;
                                                                     Double_t Rmi,             // Rmin of cylindrical volume intersected by track;
                                                                     Double_t Rr,              // input; radius of trajectory;
                                                                     Double_t Fi_low_limit[2], // output; Fi (in XY Helix frame) lower limit using
                                                                                               // the Stt detector minimum/maximum radius
                                                                                               // Fi_low_limit is ALWAYS between 0. and 2PI;
                                                                     Double_t Fi_up_limit[2],  // output; Fi (in XY Helix frame) upper limit using
                                                                                               // the Stt detector maximum/minimum radius
                                                                                               // Fi_up_limit is ALWAYS > Fi_low_limit and
                                                                                               // possibly > 2PI;
                                                                     Short_t *status           // output;
)
{
  // -------------- calculate the maximum fi and minimum fi spanned by this track,

  // see logbook pag.270; by using the Rmin and Rmax of the straw detector.
  // this function works also when circular trajectory in XY doesn't pass through (0,0).

  bool intersection_inner, intersection_outer;
  Double_t // teta1, //[R.K. 01/2017] unused variable?
           // teta2, //[R.K. 01/2017] unused variable?
           // tetavertex, //[R.K. 01/2017] unused variable?
    a,
    // cosT, //[R.K. 01/2017] unused variable?
    // cost, //[R.K. 01/2017] unused variable?
    cosFi, cosfi, Fi, fi, FI0;
  // Px, //[R.K. 01/2017] unused variable?
  // Py, //[R.K. 01/2017] unused variable?
  // tmp; //[R.K. 01/2017] unused variable?

  // Fi_low_limit is an array of dimensionality 2;
  // Fi_up_limit is an array of dimensionality 2;
  // Fi_low_limit[0], Fi_up_limit[0] is the solution (radians) corresponding to the intersection
  //	on the RIGHT side, in the XY projection of the trajectory [looking into the beam] with respect
  //	to the segment joining the Center of the Helix with the origin (0,0);
  // Fi_low_limit[1], Fi_up_limit[1] is the solution corresponding to the LEFT intersection; in case
  //			there is no second solution it is set at -100.;
  // the charge is not considered here; the two solution are just determined by the intersection of the trajectory
  // circle with the inner and outer boundary of the STT detector system;

  // the trajectory NEEDS NOT to come from the origin ;

  // Fi_low_limit is ALWAY between 0 and 2PI radians;
  // Fi_up_limit  is ALWAYS bigger than Fi_low_limit (not necessarily < 2PI), in radians;

  Rma += 1.; // add a safety margin.
  Rmi -= 1.; // add a safety margin.

  a = sqrt(oX * oX + oY * oY);

  //  preliminary condition
  if (a + Rr <= Rmi) // in this case there might be hits at radius < Rmi.
  {
    *status = -1;
    return;
  }
  if (a >= Rr + Rma || Rr >= a + Rma) // in this case there can be no hits with radius < Rma.
  {
    *status = -2;
    return;
  }

  if (a - Rr >= Rmi)
    intersection_inner = false;
  else
    intersection_inner = true;

  if (a + Rr <= Rma || a - Rr >= Rma)
    intersection_outer = false;
  else
    intersection_outer = true;

  if ((!intersection_inner) && (!intersection_outer)) {
    // in this case the trajectory is contained completely between the inner
    // and outer regions of the STT;
    Fi_low_limit[0] = 0.;
    Fi_up_limit[0] = TWO_PI;
    *status = 1;
    return;
  }

  //	now the calculation

  FI0 = atan2(-oY, -oX);
  if (intersection_outer) {
    cosFi = (a * a + Rr * Rr - Rma * Rma) / (2. * Rr * a);
    if (cosFi < -1.)
      cosFi = -1.;
    else if (cosFi > 1.)
      cosFi = 1.;
    Fi = acos(cosFi);
  }

  if (intersection_inner) {
    cosfi = (a * a + Rr * Rr - Rmi * Rmi) / (2. * Rr * a);
    if (cosfi < -1.)
      cosfi = -1.;
    else if (cosfi > 1.)
      cosfi = 1.;
    fi = acos(cosfi);
  }

  if (intersection_outer && intersection_inner) {

    Fi_low_limit[1] = FI0 - Fi;
    Fi_up_limit[1] = FI0 - fi;

    Fi_low_limit[0] = FI0 + fi;
    Fi_up_limit[0] = FI0 + Fi;

  } else if (intersection_inner) {
    Fi_low_limit[0] = FI0 + fi;
    Fi_up_limit[0] = FI0 - fi;

    Fi_low_limit[1] = -100.;
    Fi_up_limit[1] = -100.;
  } else { // case with intersection_outer=true; intersection_inner=false;
    Fi_low_limit[0] = FI0 - Fi;
    Fi_up_limit[0] = FI0 + Fi;

    Fi_low_limit[1] = -100.;
    Fi_up_limit[1] = -100.;
  } // end of    if( intersection_outer && intersection_inner

  for (int i = 0; i < 2; i++) {
    if (Fi_low_limit[i] < -99.)
      continue;
    if (Fi_low_limit[i] < 0.) {
      Fi_low_limit[i] = fmod(Fi_low_limit[i], TWO_PI);
      Fi_low_limit[i] += TWO_PI;
    } else if (Fi_low_limit[i] >= TWO_PI) {
      Fi_low_limit[i] = fmod(Fi_low_limit[i], TWO_PI);
    }
    if (Fi_up_limit[i] < 0.) {
      Fi_up_limit[i] = fmod(Fi_up_limit[i], TWO_PI);
      Fi_up_limit[i] += TWO_PI;
    } else if (Fi_up_limit[i] >= TWO_PI) {
      Fi_up_limit[i] = fmod(Fi_up_limit[i], TWO_PI);
    }

    //	Modify *Fi_up_limit by adding
    //	2PI if it is the case, in order to make *Fi_up_limit > *Fi_low_limit.
    if (Fi_up_limit[i] < Fi_low_limit[i])
      Fi_up_limit[i] += TWO_PI;
    if (Fi_up_limit[i] < Fi_low_limit[i])
      Fi_up_limit[i] = Fi_low_limit[i];
  } // end of for(int i=0; i<2; i++)
  *status = 0;

  return;
}

//---------- end of  function PndTrkCTGeometryCalculations::FindingParallelTrackAngularRange2

//----------begin of function PndTrkCTGeometryCalculations::FindIntersectionsOuterCircle

Short_t PndTrkCTGeometryCalculations::FindIntersectionsOuterCircle(Double_t oX, Double_t oY,
                                                                   Double_t Rr,  // radius of the trajectory intersecting originating from the vertex at (0,0)
                                                                                 // intersecting with the circle centered at (0,0);
                                                                   Double_t Rma, // radius of the circle centered at (0,0);
                                                                   Double_t Xcross[2], Double_t Ycross[2])
{

  // return -1 --> non-intersection;
  // return 0  --> 2 intersections.

  Double_t a, cosFi, Fi, FI0;
  a = sqrt(oX * oX + oY * oY);

  // case with no intersections or just 1 intersection;
  if (a >= Rr + Rma || Rr >= a + Rma || a + Rr <= Rma)
    return -1;

  FI0 = atan2(-oY, -oX);
  cosFi = (a * a + Rr * Rr - Rma * Rma) / (2. * Rr * a);
  if (cosFi < -1.)
    cosFi = -1.;
  else if (cosFi > 1.)
    cosFi = 1.;
  Fi = acos(cosFi);

  Xcross[0] = oX + Rr * cos(FI0 + Fi);
  Ycross[0] = oY + Rr * sin(FI0 + Fi);
  Xcross[1] = oX + Rr * cos(FI0 - Fi);
  Ycross[1] = oY + Rr * sin(FI0 - Fi);

  return 0;
}
//----------end of function PndTrkCTGeometryCalculations::FindIntersectionsOuterCircle

//----------begin of function PndTrkCTGeometryCalculations::FindTrackEntranceExitbiHexagonLeft

Short_t PndTrkCTGeometryCalculations::FindTrackEntranceExitbiHexagonLeft(Double_t vgap, Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                                                         Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                                                         Double_t ApotemaMax, Double_t Xcross[2], Double_t Ycross[2])
{
  Short_t flag;
  Short_t nIntersections;
  Double_t FiStart,
    XintersectionList[16], // all the possible intersections
    YintersectionList[16]; // (up to 16 intersections).

  // The following is the form of the Left (looking from downstream into the beam) biHexagon
  // geometrical shape considered in this method :
  //
  /*

          /|
         / |
        /  |
       /  /
      /  /
     /  /
    /  /
    |  |
    |  |
    |  |
    |  |
    \  \
     \  \
      \  \
       \  \
        \  |
         \ |
          \|

  */
  // finding all possible intersections with inner parallel straw region.
  // The inner parallel straw region is delimited by two hexagons.

  // flag meaning :
  // -1 -->  track outside outer perimeter;
  // 0 -->  at least 1 intersection with polygon, therefore a possible entry and an exit;
  // 1 -->  track contained completely between the two polygons;

  flag = IntersectionsWithClosedbiHexagonLeft(vgap, Oxx, Oyy, Rr,
                                              ApotemaMin, // Apotema of the inner Hexagon,
                                              ApotemaMax, // Apotema of the outer Hexagon.
                                              &nIntersections,
                                              XintersectionList, // XintersectionList[..][0] --> inner polygon,
                                                                 // XintersectionList[..][1] --> outer polygon.
                                              YintersectionList);

  // IMPORTANT :
  // this is true because here it is assumed that the track comes from (0,0,0)
  // otherwise the code must be changed!

  if (!(flag == 0))
    return flag;
  if (nIntersections < 2)
    return -1;

  //-------  among all  possible intersection find the entrance point (Xcross[0], Ycross[0])
  //	   and the exit point (Xcross[1], Ycross[1])  of this track.

  //-------- the starting point of the track.
  FiStart = atan2(Start[1] - Oyy, Start[0] - Oxx);
  if (FiStart < 0.)
    FiStart += TWO_PI;
  if (FiStart < 0.)
    FiStart = 0.;

  // this method selects the entrance and exit points of the trajectory among all
  // geometrical intersections of the circular trajectory with the straw particular
  // volume.

  // so at this point, the intersections are at least 2.

  ChooseEntranceExitbis(Oxx, Oyy, Charge, FiStart, nIntersections, XintersectionList, YintersectionList,
                        Xcross, // output
                        Ycross  // output
  );

  //----------------------

  return flag;
}

//----------end of function PndTrkCTGeometryCalculations::FindTrackEntranceExitbiHexagonLeft

//----------begin of function PndTrkCTGeometryCalculations::FindTrackEntranceExitbiHexagonLeft2

Short_t PndTrkCTGeometryCalculations::FindTrackEntranceExitbiHexagonLeft2(Double_t vgap, Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                                                          Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                                                          Double_t ApotemaMax, Double_t XintersectionList[16], Double_t YintersectionList[16],
                                                                          Double_t FiOrderedList[16])
{
  Short_t flag, nIntersections;
  Double_t FiStart;

  // The following is the form of the Left (looking from downstream into the beam) biHexagon
  // geometrical shape considered in this method :
  //

  //	      /|    //
  //	     / |    //
  //	    /  |    //
  //	   /  /     //
  //	  /  /      //
  //	 /  /       //
  //	/  /        //
  //	|  |        //
  //	|  |        //
  //	|  |        //
  //	|  |        //
  //	\  \        //
  //	 \  \       //
  //	  \  \      //
  //	   \  \     //
  //	    \  |    //
  //	     \ |    //
  //	      \|    //

  // finding all possible intersections with inner parallel straw region.
  // The inner parallel straw region is delimited by two hexagons.

  // flag meaning :
  // -1 -->  track outside outer perimeter;
  // 0 -->  at least 1 intersection with polygon, therefore a possible entry and an exit;
  // 1 -->  track contained completely between the two polygons;

  // nIntersections is the number of intersections found;

  // This method return nIntersections (it can be 0);

  flag = IntersectionsWithClosedbiHexagonLeft(vgap, Oxx, Oyy, Rr,
                                              ApotemaMin, // Apotema of the inner Hexagon,
                                              ApotemaMax, // Apotema of the outer Hexagon.
                                              &nIntersections,
                                              XintersectionList, // XintersectionList[..][0] --> inner polygon,
                                                                 // XintersectionList[..][1] --> outer polygon.
                                              YintersectionList);

  // IMPORTANT :
  // this is true because here it is assumed that the track comes from (0,0,0)
  // otherwise the code must be changed!

  if (!(flag == 0))
    return 0;

  //-------  among all  possible intersection find the entrance point (Xcross[0], Ycross[0])
  //	   and the exit point (Xcross[1], Ycross[1])  of this track.

  //-------- the starting point of the track.
  FiStart = atan2(Start[1] - Oyy, Start[0] - Oxx);
  if (FiStart < 0.)
    FiStart += TWO_PI;
  if (FiStart < 0.)
    FiStart = 0.;

  // this method selects the entrance and exit points of the trajectory among all
  // geometrical intersections of the circular trajectory with the straw particular
  // volume.

  // so at this point, the intersections are at least 2.

  // XintersectionList and YintersectionList are ordered clockwise or counterclockwise
  // according to the charge;
  ChooseEntranceExit3(Oxx, Oyy, Charge, FiStart, nIntersections,
                      XintersectionList, // input and output;
                      YintersectionList, // input and output;
                      FiOrderedList);

  //----------------------

  return nIntersections;
}

//----------end of function PndTrkCTGeometryCalculations::FindTrackEntranceExitbiHexagonLeft2

//----------begin of function PndTrkCTGeometryCalculations::FindTrackEntranceExitbiHexagonRight

Short_t PndTrkCTGeometryCalculations::FindTrackEntranceExitbiHexagonRight(Double_t vgap, Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                                                          Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                                                          Double_t ApotemaMax, Double_t Xcross[2], Double_t Ycross[2])
{
  Short_t flag;
  Short_t nIntersections;
  Double_t FiStart,
    XintersectionList[16], // all the possible intersections
    YintersectionList[16]; // (up to 16 intersections).

  // The following is the form of the Left (looking from downstream into the beam) biHexagon
  // geometrical shape considered in this method :
  //
  //
  //	|\           //
  //	| \          //
  //	|  \         //
  //	 \  \        //
  //	  \  \       //
  //	   |  |      //
  //	   |  |      //
  //	   /  /      //
  //	  /  /       //
  //	 /  /        //
  //	|  /         //
  //	| /          //
  //	|/           //
  //

  // finding all possible intersections with inner parallel straw region.
  // The inner parallel straw region is delimited by two hexagons.

  // flag meaning :
  // -1 -->  track outside outer perimeter;
  // 0 -->  at least 1 intersection with polygon, therefore a possible entry and an exit;
  // 1 -->  track contained completely between the two polygons;

  flag = IntersectionsWithClosedbiHexagonRight(vgap, Oxx, Oyy, Rr,
                                               ApotemaMin, // Apotema of the inner Hexagon,
                                               ApotemaMax, // Apotema of the outer Hexagon.
                                               &nIntersections, XintersectionList, YintersectionList);

  // IMPORTANT :
  // this is true because here it is assumed that the track comes from (0,0,0)
  // otherwise the code must be changed!

  if (!(flag == 0))
    return flag;
  if (nIntersections < 2)
    return -1;

  //-------  among all  possible intersection find the entrance point (Xcross[0], Ycross[0])
  //	   and the exit point (Xcross[1], Ycross[1])  of this track.

  //-------- the starting point of the track.
  FiStart = atan2(Start[1] - Oyy, Start[0] - Oxx);
  if (FiStart < 0.)
    FiStart += TWO_PI;
  if (FiStart < 0.)
    FiStart = 0.;

  // this method selects the entrance and exit points of the trajectory among all
  // geometrical intersections of the circular trajectory with the straw particular
  // volume.

  // so at this point, the intersections are at least 2.

  ChooseEntranceExitbis(Oxx, Oyy, Charge, FiStart, nIntersections, XintersectionList, YintersectionList,
                        Xcross, // output
                        Ycross  // output
  );

  //----------------------

  return flag;
}

//----------end of function PndTrkCTGeometryCalculations::FindTrackEntranceExitbiHexagonRight

//----------begin of function PndTrkCTGeometryCalculations::FindTrackEntranceExitbiHexagonRight2

Short_t PndTrkCTGeometryCalculations::FindTrackEntranceExitbiHexagonRight2(Double_t vgap, Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                                                           Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                                                           Double_t ApotemaMax, Double_t XintersectionList[16], Double_t YintersectionList[16],
                                                                           Double_t FiOrderedList[16])
{
  Short_t flag, nIntersections;

  Double_t FiStart;

  // The following is the form of the Left (looking from downstream into the beam) biHexagon
  // geometrical shape considered in this method :
  //
  //
  //	|\           //
  //	| \          //
  //	|  \         //
  //	 \  \        //
  //	  \  \       //
  //	   |  |      //
  //	   |  |      //
  //	   /  /      //
  //	  /  /       //
  //	 /  /        //
  //	|  /         //
  //	| /          //
  //	|/           //
  //

  // finding all possible intersections with inner parallel straw region.
  // The inner parallel straw region is delimited by two hexagons.

  // flag meaning :
  // -1 -->  track outside outer perimeter;
  // 0 -->  at least 1 intersection with polygon, therefore a possible entry and an exit;
  // 1 -->  track contained completely between the two polygons;

  // This method returns the number of intersections (it can be 0);

  flag = IntersectionsWithClosedbiHexagonRight(vgap, Oxx, Oyy, Rr,
                                               ApotemaMin, // Apotema of the inner Hexagon,
                                               ApotemaMax, // Apotema of the outer Hexagon.
                                               &nIntersections, XintersectionList, YintersectionList);

  // IMPORTANT :
  // this is true because here it is assumed that the track comes from (0,0,0)
  // otherwise the code must be changed!

  if (!(flag == 0))
    return 0;

  //-------  among all  possible intersection find the entrance point (Xcross[0], Ycross[0])
  //	   and the exit point (Xcross[1], Ycross[1])  of this track.

  //-------- the starting point of the track.
  FiStart = atan2(Start[1] - Oyy, Start[0] - Oxx);
  if (FiStart < 0.)
    FiStart += TWO_PI;
  if (FiStart < 0.)
    FiStart = 0.;

  // this method selects the entrance and exit points of the trajectory among all
  // geometrical intersections of the circular trajectory with the straw particular
  // volume.

  // so at this point, the intersections are at least 2.

  // XintersectionList and YintersectionList are ordered clockwise or counterclockwise
  // according to the charge;
  ChooseEntranceExit3(Oxx, Oyy, Charge, FiStart, nIntersections,
                      XintersectionList, // input and output;
                      YintersectionList, // input and output;
                      FiOrderedList      // output;
  );

  //----------------------

  return nIntersections;
}

//----------end of function PndTrkCTGeometryCalculations::FindTrackEntranceExitbiHexagonRight2

//----------begin function PndTrkCTGeometryCalculations::FindTrackEntranceExitHexagonCircleLeft

Short_t PndTrkCTGeometryCalculations::FindTrackEntranceExitHexagonCircleLeft(Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                                                             Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                                                             Double_t Rma,        // outer radius of the Circle.
                                                                             Double_t GAP,        // gap between left and right sections of STT;
                                                                             Double_t Xcross[2], Double_t Ycross[2])
{

  //  This methods finds the intersections between a trajectory coming from (0,0) and
  //  parameters  Oxx, Oyy, Rr   with the closed geometrical figure (in XY) formed by the STT
  //  external Left semicircle and the Outer STT parallel Left straw semi-Hexagon + Gap for
  //  the pellet target target.
  //  It returns -1 if there are 0 or 1 intersections, or it returns 0
  //  if they are at least 2.

  //------------------

  Short_t nIntersectionsCircle, nIntersections;
  Double_t FiStart, XintersectionList[12],
    YintersectionList[12]; // all the possible intersections (up to 12 intersections).

  // finding all possible intersections with inner parallel straw region.

  Double_t Side_x[] = {-GAP / 2., -GAP / 2., -ApotemaMin, -ApotemaMin, -GAP / 2., -GAP / 2.},
           Side_y[] = {sqrt(Rma * Rma - GAP * GAP / 4.), (2. * ApotemaMin - GAP / 2.) / sqrt(3.),  ApotemaMin / sqrt(3.),
                       -ApotemaMin / sqrt(3.),           -(2. * ApotemaMin - GAP / 2.) / sqrt(3.), -sqrt(Rma * Rma - GAP * GAP / 4.)},
           a[] = {1., -1. / sqrt(3.), 1., 1. / sqrt(3.), 1.}, b[] = {0., 1., 0., 1., 0.},
           c[] = {GAP / 2., -2. * ApotemaMin / sqrt(3.), ApotemaMin, 2. * ApotemaMin / sqrt(3.), GAP / 2.};

  nIntersections = IntersectionsWithOpenPolygon(Oxx, Oyy, Rr,
                                                5, //  n. Sides of open Polygon.
                                                a, //  coefficient of formula :  aX + bY + c = 0 defining the Polygon sides.
                                                b, c,
                                                Side_x,            // X,Y coordinate of the Sides vertices (in sequence, following
                                                Side_y,            // the Polygon along.
                                                XintersectionList, // XintersectionList
                                                YintersectionList  // YintersectionList.
  );

  //-------------------------------------------------------------------------
  // finding intersections of trajectory [assumed to originate from (0,0) ]
  // with outer semicircle, the Left part.

  nIntersectionsCircle = IntersectionsWithGapSemicircle(Oxx,                                // input from trajectory
                                                        Oyy,                                // input from trajectory
                                                        Rr,                                 // input from trajectory
                                                        GAP,                                // input, vertical gap in XY plane of STT detector.
                                                        true,                               // true --> Left semicircle, false --> Right semicircle.
                                                        Rma,                                // radius of external Stt containment.
                                                        &XintersectionList[nIntersections], // output, X list of intersections (maximal 2).
                                                        &YintersectionList[nIntersections]);
  nIntersections += nIntersectionsCircle;

  //-------- the starting point of the track.

  if (nIntersections < 2)
    return -1;

  FiStart = atan2(Start[1] - Oyy, Start[0] - Oxx);
  if (FiStart < 0.)
    FiStart += TWO_PI;
  if (FiStart < 0.)
    FiStart = 0.;

  // this method selects the entrance and exit points of the trajectory among all
  // geometrical intersections of the circular trajectory with the straw particular
  // volume.

  ChooseEntranceExitbis(Oxx, Oyy, Charge, FiStart, nIntersections, XintersectionList, YintersectionList,
                        Xcross, // output
                        Ycross  // output
  );

  return 0;
}

//----------end of function PndTrkCTGeometryCalculations::FindTrackEntranceExitHexagonCircleLeft

//----------begin function PndTrkCTGeometryCalculations::FindTrackEntranceExitHexagonCircleLeft2

Short_t PndTrkCTGeometryCalculations::FindTrackEntranceExitHexagonCircleLeft2(Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                                                              Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                                                              Double_t Rma,        // outer radius of the Circle.
                                                                              Double_t GAP,        // gap between left and right sections of STT;
                                                                              Double_t XintersectionList[12], Double_t YintersectionList[12], Double_t FiOrderedList[12])
{

  // THIS METHOD IS THE SAME AS FindTrackEntranceExitHexagonCircleLeft BUT IT PROVIDES ALL THE
  // INTERSECTIONS ORDERED FROM CENTER TO PERIFERY;
  //  This methods finds the intersections between a trajectory coming from (0,0) and
  //  parameters  Oxx, Oyy, Rr   with the closed geometrical figure (in XY) formed by the STT
  //  external Left semicircle and the Outer STT parallel Left straw semi-Hexagon + Gap for
  //  the pellet target target.
  //  It returns the number of intersections (0 in case).

  // At the most the number of such intersection is 12, that's why XintersectionList[12];
  // also the intersections are ordered from center to perifery according to the charge of the
  // track that dictates the sense of rotation;

  Short_t nIntersections;

  //------------------

  Short_t nIntersectionsCircle;

  Double_t FiStart;

  // This method returns the number of intersections (it can be 0);

  // finding all possible intersections with inner parallel straw region.

  Double_t Side_x[] = {-GAP / 2., -GAP / 2., -ApotemaMin, -ApotemaMin, -GAP / 2., -GAP / 2.},
           Side_y[] = {sqrt(Rma * Rma - GAP * GAP / 4.), (2. * ApotemaMin - GAP / 2.) / sqrt(3.),  ApotemaMin / sqrt(3.),
                       -ApotemaMin / sqrt(3.),           -(2. * ApotemaMin - GAP / 2.) / sqrt(3.), -sqrt(Rma * Rma - GAP * GAP / 4.)},
           a[] = {1., -1. / sqrt(3.), 1., 1. / sqrt(3.), 1.}, b[] = {0., 1., 0., 1., 0.},
           c[] = {GAP / 2., -2. * ApotemaMin / sqrt(3.), ApotemaMin, 2. * ApotemaMin / sqrt(3.), GAP / 2.};

  nIntersections = IntersectionsWithOpenPolygon(Oxx, Oyy, Rr,
                                                5, //  n. Sides of open Polygon.
                                                a, //  coefficient of formula :  aX + bY + c = 0 defining the Polygon sides.
                                                b, c,
                                                Side_x,            // X,Y coordinate of the Sides vertices (in sequence, following
                                                Side_y,            // the Polygon along.
                                                XintersectionList, // XintersectionList
                                                YintersectionList  // YintersectionList.
  );

  //-------------------------------------------------------------------------
  // finding intersections of trajectory [assumed to originate from (0,0) ]
  // with outer semicircle, the Left part.

  nIntersectionsCircle = IntersectionsWithGapSemicircle(Oxx,                                // input from trajectory
                                                        Oyy,                                // input from trajectory
                                                        Rr,                                 // input from trajectory
                                                        GAP,                                // input, vertical gap in XY plane of STT detector.
                                                        true,                               // true --> Left semicircle, false --> Right semicircle.
                                                        Rma,                                // radius of external Stt containment.
                                                        &XintersectionList[nIntersections], // output, X list of intersections (maximal 2).
                                                        &YintersectionList[nIntersections]);
  nIntersections += nIntersectionsCircle;

  //-------- the starting point of the track.

  if (nIntersections == 0)
    return 0;

  FiStart = atan2(Start[1] - Oyy, Start[0] - Oxx);
  if (FiStart < 0.)
    FiStart += TWO_PI;
  if (FiStart < 0.)
    FiStart = 0.;

  // this method selects the entrance and exit points of the trajectory among all
  // geometrical intersections of the circular trajectory with the straw particular
  // volume; IT RETURNS all the intersections ordered from center to perifery according to the sense of
  // rotation dictated by the charge of the track;
  // the methods requires at least 2 intersection;

  ChooseEntranceExit3(Oxx, Oyy, Charge, FiStart, nIntersections,
                      XintersectionList, // input and output;
                      YintersectionList, // input and output;
                      FiOrderedList      // output;
  );

  return nIntersections;
}

//----------end of function PndTrkCTGeometryCalculations::FindTrackEntranceExitHexagonCircleLeft2

//----------begin of function PndTrkCTGeometryCalculations::FindTrackEntranceExitHexagonCircleRight

Short_t PndTrkCTGeometryCalculations::FindTrackEntranceExitHexagonCircleRight(Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                                                              Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                                                              Double_t Rma,        // outer radius of the Circle.
                                                                              Double_t GAP, Double_t Xcross[2], Double_t Ycross[2])
{

  //  This methods finds the intersections between a trajectory coming from (0,0) and
  //  parameters  Oxx, Oyy, Rr  with the closed geometrical figure (in XY) formed by the STT
  //  external Left semicircle and the Outer STT parallel Left straw semi-Hexagon + Gap for
  //  the pellet target target.
  //  It returns -1 if there are 0 or 1 intersections, or it returns 0
  //  if they are at least 2.

  //------------------

  Short_t nIntersectionsCircle, nIntersections;
  Double_t FiStart, XintersectionList[12],
    YintersectionList[12]; // all the possible intersections (up to 10 intersections).

  // finding all possible intersections with inner parallel straw region.

  Double_t Side_x[] = {GAP / 2., GAP / 2., ApotemaMin, ApotemaMin, GAP / 2., GAP / 2.},
           Side_y[] = {sqrt(Rma * Rma - GAP * GAP / 4.), (2. * ApotemaMin - GAP / 2.) / sqrt(3.),  ApotemaMin / sqrt(3.),
                       -ApotemaMin / sqrt(3.),           -(2. * ApotemaMin - GAP / 2.) / sqrt(3.), -sqrt(Rma * Rma - GAP * GAP / 4.)},
           a[] = {1., 1. / sqrt(3.), 1., -1. / sqrt(3.), 1.}, b[] = {0., 1., 0., 1., 0.},
           c[] = {-GAP / 2., -2. * ApotemaMin / sqrt(3.), -ApotemaMin, 2. * ApotemaMin / sqrt(3.), -GAP / 2.};

  nIntersections = IntersectionsWithOpenPolygon(Oxx, Oyy, Rr,
                                                5, //  n. Sides of open Polygon.
                                                a, //  coefficient of formula :  aX + bY + c = 0 defining the Polygon sides.
                                                b, c,
                                                Side_x,            // X,Y coordinate of the Sides vertices (in sequence, following
                                                Side_y,            // the Polygon along.
                                                XintersectionList, // XintersectionList
                                                YintersectionList  // YintersectionList.
  );

  //-------------------------------------------------------------------------
  // finding intersections of trajectory [assumed to originate from (0,0) ]
  // with outer semicircle, the Left part.

  nIntersectionsCircle = IntersectionsWithGapSemicircle(Oxx,                                // input from trajectory
                                                        Oyy,                                // input from trajectory
                                                        Rr,                                 // input from trajectory
                                                        GAP,                                // input, vertical gap in XY plane of STT detector.
                                                        false,                              // true --> Left semicircle, false --> Right semicircle.
                                                        Rma,                                // radius of external Stt containment.
                                                        &XintersectionList[nIntersections], // output, X list of intersections (maximal 2).
                                                        &YintersectionList[nIntersections]);

  nIntersections += nIntersectionsCircle;

  //-------- the starting point of the track.

  if (nIntersections < 2)
    return -1;

  FiStart = atan2(Start[1] - Oyy, Start[0] - Oxx);
  if (FiStart < 0.)
    FiStart += TWO_PI;
  if (FiStart < 0.)
    FiStart = 0.;

  // this method selects the entrance and exit points of the trajectory among all
  // geometrical intersections of the circular trajectory with the straw particular
  // volume.

  ChooseEntranceExitbis(Oxx, Oyy, Charge, FiStart, nIntersections, XintersectionList, YintersectionList,
                        Xcross, // output
                        Ycross  // output
  );

  return 0;
}

//----------end of function PndTrkCTGeometryCalculations::FindTrackEntranceExitHexagonCircleRight

//----------begin of function PndTrkCTGeometryCalculations::FindTrackEntranceExitHexagonCircleRight2

Short_t PndTrkCTGeometryCalculations::FindTrackEntranceExitHexagonCircleRight2(Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                                                               Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                                                               Double_t Rma,        // outer radius of the Circle.
                                                                               Double_t GAP, Double_t XintersectionList[12], Double_t YintersectionList[12],
                                                                               Double_t FiOrderedList[12])
{

  // THIS METHOD IS ALMOST THE SAME AS FindTrackEntranceExitHexagonCircleLeft BUT IT PROVIDES ALL THE
  // INTERSECTIONS ORDERED FROM CENTER TO PERIFERY;
  //  This methods finds the intersections between a trajectory coming from (0,0) and
  //  parameters  Oxx, Oyy, Rr   with the closed geometrical figure (in XY) formed by the STT
  //  external Left semicircle and the Outer STT parallel Left straw semi-Hexagon + Gap for
  //  the pellet target target.
  //  It returns the number of intersections (0 if there are no intersections);

  // At the most the number of such intersection is 12, that's why XintersectionList[12];
  // also the intersections are ordered from center to perifery according to the charge of the
  // track that dictates the sense of rotation;

  Short_t nIntersections;

  // Double_t	cosFi, //[R.K. 01/2017] unused variable?
  // theta1, //[R.K. 01/2017] unused variable?
  // theta2, //[R.K. 01/2017] unused variable?
  // Theta1, //[R.K. 01/2017] unused variable?
  // Theta2, //[R.K. 01/2017] unused variable?
  // aaa, //[R.K. 01/2017] unused variable?
  // Fi, //[R.K. 01/2017] unused variable?
  // FI0, //[R.K. 01/2017] unused variable?
  // x1, //[R.K. 01/2017] unused variable?
  // x2, //[R.K. 01/2017] unused variable?
  // y1, //[R.K. 01/2017] unused variable?
  // y2; //[R.K. 01/2017] unused variable?
  //------------------

  Short_t nIntersectionsCircle;
  Double_t FiStart;

  // finding all possible intersections with inner parallel straw region.

  Double_t Side_x[] = {GAP / 2., GAP / 2., ApotemaMin, ApotemaMin, GAP / 2., GAP / 2.},
           Side_y[] = {sqrt(Rma * Rma - GAP * GAP / 4.), (2. * ApotemaMin - GAP / 2.) / sqrt(3.),  ApotemaMin / sqrt(3.),
                       -ApotemaMin / sqrt(3.),           -(2. * ApotemaMin - GAP / 2.) / sqrt(3.), -sqrt(Rma * Rma - GAP * GAP / 4.)},
           a[] = {1., 1. / sqrt(3.), 1., -1. / sqrt(3.), 1.}, b[] = {0., 1., 0., 1., 0.},
           c[] = {-GAP / 2., -2. * ApotemaMin / sqrt(3.), -ApotemaMin, 2. * ApotemaMin / sqrt(3.), -GAP / 2.};

  nIntersections = IntersectionsWithOpenPolygon(Oxx, Oyy, Rr,
                                                5, //  n. Sides of open Polygon.
                                                a, //  coefficient of formula :  aX + bY + c = 0 defining the Polygon sides.
                                                b, c,
                                                Side_x,            // X,Y coordinate of the Sides vertices (in sequence, following
                                                Side_y,            // the Polygon along.
                                                XintersectionList, // XintersectionList
                                                YintersectionList  // YintersectionList.
  );

  //-------------------------------------------------------------------------
  // finding intersections of trajectory [assumed to originate from (0,0) ]
  // with outer semicircle, the Left part.

  nIntersectionsCircle = IntersectionsWithGapSemicircle(Oxx,                                // input from trajectory
                                                        Oyy,                                // input from trajectory
                                                        Rr,                                 // input from trajectory
                                                        GAP,                                // input, vertical gap in XY plane of STT detector.
                                                        false,                              // true --> Left semicircle, false --> Right semicircle.
                                                        Rma,                                // radius of external Stt containment.
                                                        &XintersectionList[nIntersections], // output, X list of intersections (maximal 2).
                                                        &YintersectionList[nIntersections]);

  nIntersections += nIntersectionsCircle;

  //-------- the starting point of the track.

  if (nIntersections == 0)
    return 0;

  FiStart = atan2(Start[1] - Oyy, Start[0] - Oxx);
  if (FiStart < 0.)
    FiStart += TWO_PI;
  if (FiStart < 0.)
    FiStart = 0.;

  // this method selects the entrance and exit points of the trajectory among all
  // geometrical intersections of the circular trajectory with the straw particular
  // volume; IT RETURNS all the intersections ordered from center to perifery according to the sense of
  // rotation dictated by the charge of the track;
  // the methods requires at least 2 intersection;

  ChooseEntranceExit3(Oxx, Oyy, Charge, FiStart, nIntersections,
                      XintersectionList, // input and output;
                      YintersectionList, // input and output;
                      FiOrderedList      // output;
  );

  return nIntersections;
}

//----------end of function PndTrkCTGeometryCalculations::FindTrackEntranceExitHexagonCircleRight2

//----------begin of function PndTrkCTGeometryCalculations::IntersectionCircle_Segment

bool PndTrkCTGeometryCalculations::IntersectionCircle_Segment(Double_t a, // coefficients implicit equation.
                                                              Double_t b, // of segment : a*x + b*y + c =0.
                                                              Double_t c,
                                                              Double_t P1x, // point delimiting the segment.
                                                              Double_t P2x, // point delimiting the segment.
                                                              Double_t P1y, // point delimiting the segment.
                                                              Double_t P2y, // point delimiting the segment.
                                                              Double_t Oxx, // center of circle.
                                                              Double_t Oyy,
                                                              Double_t Rr, // Radius of circle;
                                                              Short_t *Nintersections, Double_t XintersectionList[2], Double_t YintersectionList[2], Double_t *distance)
{
  // this method finds the intersection of a circle with a segment. If the circle
  // passes through an endpoint of the segment, that is considered an intersection also.

  bool status;

  Short_t ipossibility;

  Double_t aperp, bperp, cperp, det, distq,
    // dist1, //[R.K. 01/2017] unused variable?
    // dist2, //[R.K. 01/2017] unused variable?
    length, length_segmentq, Rq, x, y, Xintersection, Yintersection;

  *Nintersections = 0;
  det = a * a + b * b;
  if (det < 1.e-20) {
    cout << "from  PndTrkCTGeometryCalculations::IntersectionCircle_Segment :"
         << " this is not the equation of a segment; a = " << a << ", b = " << b << ", return false!\n";
    return false;
  }
  //  find if intersection circle - segment is possible.
  // check if there is an intersection (with the squares, it's the same!).
  distq = (a * Oxx + b * Oyy + c) * (a * Oxx + b * Oyy + c) / det;
  *distance = sqrt(distq);
  Rq = Rr * Rr;
  length = Rq - distq;
  if (length <= 0.)
    return false; // no intersection between trajectory and this
                  // segment.
  length = sqrt(length);
  // coefficients of line perpendicular to input segment and passing for (Oxx, Oyy).
  aperp = -b;
  bperp = a;
  cperp = -aperp * Oxx - bperp * Oyy;

  // find intersection of segment with perpendicular : no need to check if
  // det is different from 0.
  Xintersection = (-bperp * c + b * cperp) / det;
  Yintersection = (-a * cperp + aperp * c) / det;
  det = sqrt(det);
  length_segmentq = (P1x - P2x) * (P1x - P2x) + (P1y - P2y) * (P1y - P2y);
  status = false;
  for (ipossibility = -1; ipossibility < 2; ipossibility += 2) {
    x = Xintersection + ipossibility * length * b / det;
    y = Yintersection - ipossibility * length * a / det;
    if ((x - P1x) * (x - P1x) + (y - P1y) * (y - P1y) > length_segmentq // factor  to take
                                                                        // (grossly) errors in trajectory into account;
        || (x - P2x) * (x - P2x) + (y - P2y) * (y - P2y) > length_segmentq)
      continue;
    status = true;
    XintersectionList[*Nintersections] = x;
    YintersectionList[*Nintersections] = y;
    (*Nintersections)++;
  } //  end of  for(ipossibility=-1; ...

  return status;
}

//----------end of function PndTrkCTGeometryCalculations::IntersectionCircle_Segment

//----------begin of function PndTrkCTGeometryCalculations::IntersectionCircle_Segment_forScitil

bool PndTrkCTGeometryCalculations::IntersectionCircle_Segment_forScitil(Double_t a, // coefficients implicit equation.
                                                                        Double_t b, // of segment : a*x + b*y + c =0.
                                                                        Double_t c,
                                                                        Double_t P1x, // point delimiting the segment.
                                                                        Double_t P2x, // point delimiting the segment.
                                                                        Double_t P1y, // point delimiting the segment.
                                                                        Double_t P2y, // point delimiting the segment.
                                                                        Double_t Oxx, // center of circle.
                                                                        Double_t Oyy,
                                                                        Double_t Rr,     // Radius of circle;
                                                                        Double_t factor, // to take into account errors in the determination of the circumference;
                                                                        Short_t *Nintersections, Double_t XintersectionList[2], Double_t YintersectionList[2], Double_t *distance)
{
  // this method finds the intersection of a circle with a segment. If the circle
  // passes through an endpoint of the segment, that is considered an intersection also.

  bool status;

  Short_t ipossibility;

  Double_t aperp, bperp, cperp, det, distq,
    // dist1, //[R.K. 01/2017] unused variable?
    // dist2, //[R.K. 01/2017] unused variable?
    length, length_segmentq, Rq, x, y, Xintersection, Yintersection;

  *Nintersections = 0;
  det = a * a + b * b;
  if (det < 1.e-20) {
    cout << "from  PndTrkCTGeometryCalculations::IntersectionCircle_Segment :"
         << " this is not the equation of a segment; a = " << a << ", b = " << b << ", return false!\n";
    return false;
  }
  //  find if intersection circle - segment is possible.
  // check if there is an intersection (with the squares, it's the same!).
  distq = (a * Oxx + b * Oyy + c) * (a * Oxx + b * Oyy + c) / det;
  *distance = sqrt(distq);
  Rq = Rr * Rr;
  length = Rq - distq;
  if (length <= 0.)
    return false; // no intersection between trajectory and this
                  // segment.
  length = sqrt(length);
  // coefficients of line perpendicular to input segment and passing for (Oxx, Oyy).
  aperp = -b;
  bperp = a;
  cperp = -aperp * Oxx - bperp * Oyy;

  // find intersection of segment with perpendicular : no need to check if
  // det is different from 0.
  Xintersection = (-bperp * c + b * cperp) / det;
  Yintersection = (-a * cperp + aperp * c) / det;
  det = sqrt(det);
  length_segmentq = (P1x - P2x) * (P1x - P2x) + (P1y - P2y) * (P1y - P2y);
  status = false;
  for (ipossibility = -1; ipossibility < 2; ipossibility += 2) {
    x = Xintersection + ipossibility * length * b / det;
    y = Yintersection - ipossibility * length * a / det;
    if ((x - P1x) * (x - P1x) + (y - P1y) * (y - P1y) > factor * length_segmentq // factor  to take
                                                                                 // (grossly) errors in trajectory into account;
        || (x - P2x) * (x - P2x) + (y - P2y) * (y - P2y) > factor * length_segmentq)
      continue;
    status = true;
    XintersectionList[*Nintersections] = x;
    YintersectionList[*Nintersections] = y;
    (*Nintersections)++;
  } //  end of  for(ipossibility=-1; ...

  return status;
}

//----------end of function PndTrkCTGeometryCalculations::IntersectionCircle_Segment_forScitil

//----------begin of function PndTrkCTGeometryCalculations::IntersectionSciTil_Circle
bool PndTrkCTGeometryCalculations::IntersectionSciTil_Circle(
  //	Double_t DIMENSIONSCITIL,
  Double_t posizSciTilx, Double_t posizSciTily,
  Double_t Oxx, // center of circle.
  Double_t Oyy,
  Double_t Rr, // Radius of circle.
  Short_t *Nintersections, Double_t XintersectionList[2], Double_t YintersectionList[2])
{

  bool intersect;

  Double_t distance, QQ, sqrtRR, SIGN;

  QQ = posizSciTilx * posizSciTilx + posizSciTily * posizSciTily;
  sqrtRR = sqrt(QQ);

  if (posizSciTily < 0.)
    SIGN = -1.;
  else
    SIGN = 1.;

  intersect = IntersectionCircle_Segment_forScitil(
    posizSciTilx, posizSciTily, -QQ, posizSciTilx - SIGN * 0.5 * DIMENSIONSCITIL * posizSciTily / sqrtRR, posizSciTilx + SIGN * 0.5 * DIMENSIONSCITIL * posizSciTily / sqrtRR,
    posizSciTily + SIGN * posizSciTilx * 0.5 * DIMENSIONSCITIL / sqrtRR, posizSciTily - SIGN * posizSciTilx * 0.5 * DIMENSIONSCITIL / sqrtRR, Oxx, Oyy, Rr,
    2.25,              // factor to take into account errors in the determination of the trajectory circle;
    Nintersections,    // output
    XintersectionList, // output
    YintersectionList, // output
    &distance          // output
  );

  return intersect;
}
//----------end of function PndTrkCTGeometryCalculations::IntersectionSciTil_Circle

//----------start  function PndTrkCTGeometryCalculations::IntersectionsWithClosedbiHexagonLeft

Short_t PndTrkCTGeometryCalculations::IntersectionsWithClosedbiHexagonLeft(
  //-------- inputs
  Double_t vgap, Double_t Oxx, Double_t Oyy, Double_t Rr,
  Double_t Ami, // min Apotema of hexagonal  volume intersected by track;
  Double_t Ama, // max Apotema of hexagonal volume intersected by track;
  //-------- outputs
  Short_t *nIntersections, Double_t *XintersectionList, Double_t *YintersectionList)
{

  // return integer convention :
  // -1 -->  track outside outer perimeter;
  // 0 -->  at least 1 intersection with polygon;
  // 1 -->  track contained completely between the two polygons;

  //  inner Hexagon --> 0
  //  outer Hexagon --> 1

  bool internal, AtLeast1;

  Short_t is, j, Nintersections;

  Double_t aaa, maxdistq, distance,
    //-------------------
    // a,b,c == coefficients of the implicit equations of the 3 sides of the half inner Hexagon
    // plus 3 sides of the half outer Hexagon plus 2 vertical sides corresponding to the Gap :
    //    a*x + b*y +c =0; the numbering of these 8 sides follows the convention of Gianluigi's
    // logbook on page 286.
    a[] = {-1. / sqrt(3.), 1., 1. / sqrt(3.), 1., 1. / sqrt(3.), 1., -1. / sqrt(3.), 1.}, b[] = {1., 0., 1., 0., 1., 0., 1., 0.},
    c[] = {-2. * Ama / sqrt(3.), Ama, 2. * Ama / sqrt(3.), vgap / 2., 2. * Ami / sqrt(3.), Ami, -2. * Ami / sqrt(3.), vgap / 2.},
    //----------------------

    tempX[2], tempY[2];

  // side_x and side_y are ordered as side1, side2, ..... in Gianluigi's logbook on page 286.
  Double_t side_x[] = {-vgap / 2., -Ama, -Ama, -vgap / 2., -vgap / 2., -Ami, -Ami, -vgap / 2., -vgap / 2.},
           side_y[] = {(-0.5 * vgap + 2. * Ama) / sqrt(3.),  Ama / sqrt(3.),  -Ama / sqrt(3.), -(-0.5 * vgap + 2. * Ama) / sqrt(3.),
                       -(-0.5 * vgap + 2. * Ami) / sqrt(3.), -Ami / sqrt(3.), Ami / sqrt(3.),  (-0.5 * vgap + 2. * Ami) / sqrt(3.),
                       (-0.5 * vgap + 2. * Ama) / sqrt(3.)};

  //-----------------------

  //   find intersections (maximum 16) with the 8 sides.

  AtLeast1 = false;
  *nIntersections = 0;
  internal = true;
  maxdistq = -9999.;
  for (is = 0; is < 8; is++) {
    aaa = (side_x[is] - Oxx) * (side_x[is] - Oxx) + (side_y[is] - Oyy) * (side_y[is] - Oyy);
    if (aaa > maxdistq)
      maxdistq = aaa;
    aaa = (side_x[is + 1] - Oxx) * (side_x[is + 1] - Oxx) + (side_y[is + 1] - Oyy) * (side_y[is + 1] - Oyy);
    if (aaa > maxdistq)
      maxdistq = aaa;
    if (IntersectionCircle_Segment(a[is], b[is], c[is], side_x[is], side_x[is + 1], side_y[is], side_y[is + 1], Oxx, Oyy, Rr, &Nintersections, tempX, tempY,
                                   &distance // distance of (Oxx,Oyy) from line
                                             // defined by  a*x+b*y+c=0.
                                   )) {
      AtLeast1 = true;
      for (j = 0; j < Nintersections; j++) {
        XintersectionList[*nIntersections] = tempX[j];
        YintersectionList[*nIntersections] = tempY[j];
        (*nIntersections)++;
      }
    } // end of if ( IntersectionCircle_Segment( .....

    // the definition of 'internal' here is when the given Point
    // stays at the same side of the origin (0,0) with respect to
    // the given line of equation   a*x+b*y+c=0.
    if (is < 3) {
      internal = internal && IsInternal(Oxx, Oyy, a[is], b[is], c[is]);
    } else {
      internal = internal && (!IsInternal(Oxx, Oyy, a[is], b[is], c[is]));
    }

  } // end of  for(is=0; is<8; is++)

  if (!AtLeast1) {
    if (!internal)
      return -1; // trajectory outside polygon.
    if (maxdistq < Rr * Rr)
      return -1; // trajectory outside polygon.
    return 1;    // trajectory completely contained inside this Polygon.
  }
  return 0;
}

//---------- end of  function PndTrkCTGeometryCalculations::IntersectionsWithClosedbiHexagonLeft

//----------start  function PndTrkCTGeometryCalculations::IntersectionsWithClosedbiHexagonRight

Short_t PndTrkCTGeometryCalculations::IntersectionsWithClosedbiHexagonRight(
  //-------- inputs
  Double_t vgap, Double_t Oxx, Double_t Oyy, Double_t Rr,
  Double_t Ami, // min Apotema of hexagonal  volume intersected by track;
  Double_t Ama, // max Apotema of hexagonal volume intersected by track;
  //-------- outputs
  Short_t *nIntersections, Double_t *XintersectionList, Double_t *YintersectionList)
{

  // return integer convention :
  // -1 -->  track outside outer perimeter;
  // 0 -->  at least 1 intersection with polygon;
  // 1 -->  track contained completely between the two polygons;

  //  inner Hexagon --> 0
  //  outer Hexagon --> 1

  bool internal, AtLeast1;

  Short_t // i, //[R.K. 01/2017] unused variable?
    is,
    j, Nintersections;

  Double_t aaa, maxdistq, distance,
    //-------------------
    // a,b,c == coefficients of the implicit equations of the 3 sides of the half inner Hexagon
    // plus 3 sides of the half outer Hexagon plus 2 vertical sides corresponding to the Gap :
    //    a*x + b*y +c =0; the numbering of these 8 sides foolows the convention of Gianluigi's
    // logbook on page 286.
    a[] = {1. / sqrt(3.), 1., -1. / sqrt(3.), 1., -1. / sqrt(3.), 1., 1. / sqrt(3.), 1.}, b[] = {1., 0., 1., 0., 1., 0., 1., 0.},
    c[] = {-2. * Ama / sqrt(3.), -Ama, 2. * Ama / sqrt(3.), -vgap / 2., 2. * Ami / sqrt(3.), -Ami, -2. * Ami / sqrt(3.), -vgap / 2.},
    //----------------------

    tempX[2], tempY[2];

  // side_x and side_y are ordered as side1, side2, ..... in Gianluigi's logbook on page 286.
  Double_t side_x[] = {vgap / 2., Ama, Ama, vgap / 2., vgap / 2., Ami, Ami, vgap / 2., vgap / 2.},
           side_y[] = {(-0.5 * vgap + 2. * Ama) / sqrt(3.),  Ama / sqrt(3.),  -Ama / sqrt(3.), -(-0.5 * vgap + 2. * Ama) / sqrt(3.),
                       -(-0.5 * vgap + 2. * Ami) / sqrt(3.), -Ami / sqrt(3.), Ami / sqrt(3.),  (-0.5 * vgap + 2. * Ami) / sqrt(3.),
                       (-0.5 * vgap + 2. * Ama) / sqrt(3.)};

  //-----------------------

  //   find intersections (maximum 16) with the 8 sides.

  AtLeast1 = false;
  *nIntersections = 0;
  internal = true;
  maxdistq = -9999.;
  for (is = 0; is < 8; is++) {
    aaa = (side_x[is] - Oxx) * (side_x[is] - Oxx) + (side_y[is] - Oyy) * (side_y[is] - Oyy);
    if (aaa > maxdistq)
      maxdistq = aaa;
    aaa = (side_x[is + 1] - Oxx) * (side_x[is + 1] - Oxx) + (side_y[is + 1] - Oyy) * (side_y[is + 1] - Oyy);
    if (aaa > maxdistq)
      maxdistq = aaa;
    if (IntersectionCircle_Segment(a[is], b[is], c[is], side_x[is], side_x[is + 1], side_y[is], side_y[is + 1], Oxx, Oyy, Rr, &Nintersections, tempX, tempY,
                                   &distance // distance of (Oxx,Oyy) from line
                                             // defined by  a*x+b*y+c=0.
                                   )) {
      AtLeast1 = true;
      for (j = 0; j < Nintersections; j++) {
        XintersectionList[*nIntersections] = tempX[j];
        YintersectionList[*nIntersections] = tempY[j];
        (*nIntersections)++;
      }
    } // end of if ( IntersectionCircle_Segment( .....

    // the definition of 'internal' here is when the given Point
    // stays at the same side of the origin (0,0) with respect to
    // the given line of equation   a*x+b*y+c=0.
    if (is < 3) {
      internal = internal && IsInternal(Oxx, Oyy, a[is], b[is], c[is]);
    } else {
      internal = internal && (!IsInternal(Oxx, Oyy, a[is], b[is], c[is]));
    }

  } // end of  for(is=0; is<8; is++)

  if (!AtLeast1) {
    if (!internal)
      return -1; // trajectory outside polygon.
    if (maxdistq < Rr * Rr)
      return -1; // trajectory outside polygon.
    return 1;    // trajectory completely contained inside this Polygon.
  }
  return 0;
}

//---------- end of  function PndTrkCTGeometryCalculations::IntersectionsWithClosedbiHexagonRight

//----------start  function PndTrkCTGeometryCalculations::IntersectionsWithClosedPolygon

Short_t PndTrkCTGeometryCalculations::IntersectionsWithClosedPolygon(
  //-------- inputs
  Double_t Oxx, Double_t Oyy, Double_t Rr,
  Double_t Rmi, // min Apotema of hexagonal  volume intersected by track;
  Double_t Rma, // max Apotema of hexagonal volume intersected by track;
  //-------- outputs
  Short_t nIntersections[2], Double_t XintersectionList[][2], Double_t YintersectionList[][2])
{

  // return integer convention :
  // -2 -->  track outside outer polygon;
  // -1 -->  track contained completely within inner polygon;
  // 0 -->  at least 1 intersection with inner polygon, at least 1 with outer polygon;
  // 1 -->  track contained completely between the two polygons;
  // 2 -->  track contained completely by larger polygons, with intersections in the smaller;
  // 3 -->  track completely outsiede the small polygon with intersections in the bigger.

  //  inner Hexagon --> 0
  //  outer Hexagon --> 1

  bool internal[2], AtLeast1[2];

  Short_t i, is, j, Nintersections;

  Double_t mindist[2], distance,
    //-------------------
    // a,b,c == coefficients of the implicit equations of the six sides of the Hexagon
    // centered at 0 :   a*x + b*y +c =0; see Gianluigi's logbook on page 277;
    // the coefficient  c  has to be multiplied by Erre.
    // The first side is
    a[] = {1. / sqrt(3.), 1., -1. / sqrt(3.), 1. / sqrt(3.), 1., -1. / sqrt(3.)}, b[] = {1., 0., 1., 1., 0., 1.},
    c[] = {-2. / sqrt(3.), -1., 2. / sqrt(3.), 2. / sqrt(3.), 1., -2. / sqrt(3.)},
    //----------------------

    Erre[] = {Rmi, Rma}, //  this is the distance from (0,0)
                         // of the SIDES of the Hexagon delimiting the Skew area
    tempX[2], tempY[2];

  // both hexagon_side_xlow and hexagon_side_xup must be multiplied by appropriate Erre;
  // sides of Hexagon ordered as a, b, c ..... in Gianluigi's logbook on page 280.
  Double_t hexagon_side_x[] = {0., 1., 1., 0., -1., -1., 0.},
           hexagon_side_y[] = {2. / sqrt(3.), 1. / sqrt(3.), -1. / sqrt(3.), -2. / sqrt(3.), -1. / sqrt(3.), 1. / sqrt(3.), 2. / sqrt(3.)};

  //-----------------------

  //   find intersection with the 6 sides of the small exhagon delimiting the skew straws zone
  //   see on page 277 of Gianluigi's logbook.

  // status  =0, at least 1 intersection with inner Hexagon, at least 1 intersection
  // with the outer Hexagon; =1, track contained completely between the
  // two Hexagons; = -1 track contained within inner Hexagon;
  // =-2 track outside outer Hexagon.

  for (i = 0; i < 2; i++) { // i=0 --> inner Hexagon, i= 1 --> outer Hexagon.
    AtLeast1[i] = false;
    nIntersections[i] = 0;
    internal[i] = true;
    mindist[i] = 999999.;
    for (is = 0; is < 6; is++) {
      if (IntersectionCircle_Segment(a[is], b[is], c[is] * Erre[i], hexagon_side_x[is] * Erre[i], hexagon_side_x[is + 1] * Erre[i], hexagon_side_y[is] * Erre[i],
                                     hexagon_side_y[is + 1] * Erre[i], Oxx, Oyy, Rr, &Nintersections, tempX, tempY,
                                     &distance // distance of (Oxx,Oyy) from line
                                               // defined by  a*x+b*y+c=0.
                                     )) {
        AtLeast1[i] = true;
        for (j = 0; j < Nintersections; j++) {
          XintersectionList[nIntersections[i]][i] = tempX[j];
          YintersectionList[nIntersections[i]][i] = tempY[j];
          nIntersections[i]++;
        }
      } // end of if ( IntersectionCircle_Segment( .....

      if (mindist[i] > distance)
        mindist[i] = distance;

      // the definition of 'internal' here is when the given Point
      // stays at the same side of the origin (0,0) with respect to
      // the given line of equation   a*x+b*y+c=0.
      internal[i] = internal[i] && IsInternal(Oxx, Oyy, a[is], b[is], c[is] * Erre[i]);

    } // end of  for(is=0; is<6; is++)
  }   // end of  for(i=0;i<2;i++)

  if ((!AtLeast1[0]) && (!AtLeast1[1])) {
    if (!internal[1])
      return -2; // trajectory outside outer Hexagon.
    if (Rr > mindist[1])
      return -2;
    if (!internal[0])
      return 1; // trajectory contained between inner and outer Hexagon.
    if (mindist[0] >= Rr)
      return -1;                           //  trajectory contained in inner Hexagon.
    return 1;                              // trajectory contained between inner and outer Hexagon.
  } else if (AtLeast1[0] && AtLeast1[1]) { // continuation of  if( (!AtLeast1[0]) && ...
    return 0;
  } else if (AtLeast1[0]) {
    return 2;
  }

  return 3;
}

//---------- end of  function PndTrkCTGeometryCalculations::IntersectionsWithClosedPolygon

//----------begin of function PndTrkCTGeometryCalculations::IntersectionsWithGapSemicircle

Short_t PndTrkCTGeometryCalculations::IntersectionsWithGapSemicircle(Double_t Oxx, Double_t Oyy, Double_t Rr, Double_t GAP,
                                                                     bool left, // if true --> Left semicircle; false --> Right semicircle.
                                                                     Double_t Rma, Double_t *XintersectionList, Double_t *YintersectionList)
{

  Short_t nIntersectionsCircle;

  Double_t cosFi, theta1, theta2, Theta1, Theta2, aaa, Fi, FI0, x1, x2, y1, y2;

  nIntersectionsCircle = 0;
  aaa = sqrt(Oxx * Oxx + Oyy * Oyy);

  //  preliminary condition for having intersections between trajectory and  Circle.

  if (!(aaa >= Rr + Rma || Rr >= aaa + Rma) && !(aaa + Rr <= Rma || aaa - Rr >= Rma)) {

    //	now the calculation

    FI0 = atan2(-Oyy, -Oxx);
    cosFi = (aaa * aaa + Rr * Rr - Rma * Rma) / (2. * Rr * aaa);
    if (cosFi < -1.)
      cosFi = -1.;
    else if (cosFi > 1.)
      cosFi = 1.;
    Fi = acos(cosFi);

    //  (x1, y1) and (x2,y2) are the intersections between the trajectory
    //  and the circle, in the laboratory reference frame.
    x1 = Oxx + Rr * cos(FI0 - Fi);
    y1 = Oyy + Rr * sin(FI0 - Fi);
    theta1 = atan2(y1, x1); // in this way theta1 is between -PI and PI.
    x2 = Oxx + Rr * cos(FI0 + Fi);
    y2 = Oyy + Rr * sin(FI0 + Fi);
    theta2 = atan2(y2, x2); // in this way theta2 is between -PI and PI.
    //  Theta1, Theta2 = angle of the edges of the outer circle + Gap in the laboratory frame.
    //  Theta1, Theta2 are angles between -PI/2 and +PI/2.
    if (!left) { //  Right (looking into the beam) Semicircle.
      Theta2 = atan2(sqrt(Rma * Rma - GAP * GAP / 4.), GAP / 2.);
      Theta1 = atan2(-sqrt(Rma * Rma - GAP * GAP / 4.), GAP / 2.);
      if (Theta1 <= theta1 && theta1 <= Theta2) {
        XintersectionList[nIntersectionsCircle] = x1;
        YintersectionList[nIntersectionsCircle] = y1;
        nIntersectionsCircle++;
      }
      if (Theta1 <= theta2 && theta2 <= Theta2) {
        XintersectionList[nIntersectionsCircle] = x2;
        YintersectionList[nIntersectionsCircle] = y2;
        nIntersectionsCircle++;
      }
    } else { //  Left (looking into the beam) Semicircle.
      Theta2 = atan2(-sqrt(Rma * Rma - GAP * GAP / 4.), -GAP / 2.);
      Theta1 = atan2(sqrt(Rma * Rma - GAP * GAP / 4.), -GAP / 2.);
      if (Theta1 <= theta1 || theta1 <= Theta2) {
        XintersectionList[nIntersectionsCircle] = x1;
        YintersectionList[nIntersectionsCircle] = y1;
        nIntersectionsCircle++;
      }
      if (Theta1 <= theta2 || theta2 <= Theta2) {
        XintersectionList[nIntersectionsCircle] = x2;
        YintersectionList[nIntersectionsCircle] = y2;
        nIntersectionsCircle++;
      }
    }

  } // end of   if (!( a >= Rr + Rma || .....

  //---------------------------- end of calculation intersection with outer circle.

  return nIntersectionsCircle;
}
//----------end of function PndTrkCTGeometryCalculations::IntersectionsWithGapSemicircle

//----------start  function PndTrkCTGeometryCalculations::IntersectionsWithOpenPolygon

Short_t PndTrkCTGeometryCalculations::IntersectionsWithOpenPolygon(
  //-------- inputs
  Double_t Oxx,   // Track parameter
  Double_t Oyy,   // Track parameter
  Double_t Rr,    // Track parameter
  Short_t nSides, // input, n. of Sides of open Polygon.
  Double_t *a,    //  coefficient of formula :  aX + bY + c = 0 defining
  Double_t *b,    //  the Polygon sides.
  Double_t *c,
  Double_t *Side_x, // X,Y coordinate of the Sides vertices (in sequence, following
  Double_t *Side_y, // the Polygon along.
  //-------- outputs
  Double_t *XintersectionList, // XintersectionList
  Double_t *YintersectionList  // YintersectionList.
)
{

  // this methods returns the n. of intersections.

  Short_t // i, //[R.K. 01/2017] unused variable?
    is,
    j, nIntersections, Nintersections;

  Double_t // mindist, //[R.K. 01/2017] unused variable?
    distance,
    //-------------------
    // a,b,c == coefficients of the implicit equations of the six sides of the Hexagon
    // centered at 0 :   a*x + b*y +c =0; see Gianluigi's logbook on page 277;
    // the coefficient  c  has to be multiplied by Erre.

    tempX[2], tempY[2];

  //-----------------------

  nIntersections = 0;
  for (is = 0; is < nSides; is++) {
    if (IntersectionCircle_Segment(a[is], b[is], c[is], Side_x[is], Side_x[is + 1], Side_y[is], Side_y[is + 1], Oxx, Oyy, Rr, &Nintersections, tempX, tempY,
                                   &distance // distance of (Oxx,Oyy) from line
                                             // defined by  a*x+b*y+c=0.
                                   )) {

      for (j = 0; j < Nintersections; j++) {
        XintersectionList[nIntersections] = tempX[j];
        YintersectionList[nIntersections] = tempY[j];
        nIntersections++;
      }
    } // end of if ( IntersectionCircle_Segment( .....

  } // end of  for(is=0; is<nSides; is++)
    //	}  // end of  for(i=0;i<2;i++)

  return nIntersections;
}

//---------- end of  function PndTrkCTGeometryCalculations::IntersectionsWithOpenPolygon

//----------begin of function PndTrkCTGeometryCalculations::IsInsideArc

bool PndTrkCTGeometryCalculations::IsInsideArc(Double_t Oxx, Double_t Oyy, Short_t Charge, Double_t Xcross[2], Double_t Ycross[2],
                                               Double_t f // f has to be between 0 and 2PI.
)
{

  Double_t f1, f2;

  // Xcross[0],Ycross[0] is the point of entrance.

  f1 = atan2(Ycross[0] - Oyy, Xcross[0] - Oxx);
  if (f1 < 0.)
    f1 += TWO_PI;
  if (f1 < 0.)
    f1 = 0.;
  f2 = atan2(Ycross[1] - Oyy, Xcross[1] - Oxx);
  if (f2 < 0.)
    f2 += TWO_PI;
  if (f2 < 0.)
    f2 = 0.;

  if (Charge < 0) {
    if (f1 > f2)
      f2 += TWO_PI;
    if (f1 > f2)
      f2 = f1;
    if (f > f1) {
      if (f < f2)
        return true;
      else
        return false;
    } else {
      f += TWO_PI;
      if (f < f1)
        f = f1;
      if (f < f2)
        return true;
      else
        return false;
    }
  } else { // Charge > 0.
    if (f1 < f2)
      f1 += TWO_PI;
    if (f1 < f2)
      f1 = f2;
    if (f > f2) {
      if (f < f1)
        return true;
      else
        return false;
    } else {
      f += TWO_PI;
      if (f < f2)
        f = f2;
      if (f < f1)
        return true;
      else
        return false;
    }
  } // end of if(Charge<0)
}

//----------end of function PndTrkCTGeometryCalculations::IsInsideArc

//----------begin of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk1_97to1_99

bool PndTrkCTGeometryCalculations::IsInMvdMiniDisk1_97to1_99(Double_t X, //  X coordinate of point;
                                                             Double_t Y  //  Y coordinate of point;
)
{
  //	this is a set of sensors placed vertically perpendicularly to the Z direction;
  //	they are placed between 1.97 and 1.99 in Z;
  //	look at Gianluigi's logbook on page 193 for the geometry detail;

  if ((X > -2.33 && X < -1.17 && Y > -2.26 && Y < 2.26) || (X > 0. && X < 1.21 && fabs(Y) < 3.43 && fabs(Y) > 1.19) || (X > 2.32 && X < 3.48 && fabs(Y) < 1.12))
    return true;

  return false;
}

//----------end of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk1_97to1_99

//----------begin of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk1_97to1_99withMargin

bool PndTrkCTGeometryCalculations::IsInMvdMiniDisk1_97to1_99withMargin(Double_t X,       //  X coordinate of point;
                                                                       Double_t Y,       //  Y coordinate of point;
                                                                       Double_t xmargin, //  safety margin in X coordinate;
                                                                       Double_t ymargin  //  safety margin in Y coordinate;
)
{
  //	this is a set of sensors placed vertically perpendicularly to the Z direction;
  //	they are placed between 1.97 and 1.99 in Z;
  //	look at Gianluigi's logbook on page 193 for the geometry detail;

  if ((X > -2.33 + xmargin && X < -1.17 - xmargin && Y > -2.26 + ymargin && Y < 2.26 - ymargin) || (X > 0. + xmargin && X < 1.21 - xmargin && fabs(Y) < 3.43 && fabs(Y) > 1.19) ||
      (X > 2.32 + xmargin && X < 3.48 - xmargin && fabs(Y) < 1.12 - ymargin))
    return true;

  return false;
}

//----------end of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk1_97to1_99withMargin

//----------begin of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk2_41to2_43

bool PndTrkCTGeometryCalculations::IsInMvdMiniDisk2_41to2_43(Double_t X, //  X coordinate of point;
                                                             Double_t Y  //  Y coordinate of point;
)
{
  //	this is a set of sensors placed vertically perpendicularly to the Z direction;
  //	they are placed between 2.41 and 2.43 in Z;
  //	look at Gianluigi's logbook on page 194 for the geometry detail;

  if ((X > 1.17 && X < 2.33 && Y > -2.26 && Y < 2.26) || (X < 0. && X > -1.21 && fabs(Y) < 3.43 && fabs(Y) > 1.19) || (X < -2.32 && X > -3.48 && fabs(Y) < 1.12))
    return true;

  return false;
}

//----------end of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk2_41to2_43

//----------begin of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk2_41to2_43withMargin

bool PndTrkCTGeometryCalculations::IsInMvdMiniDisk2_41to2_43withMargin(Double_t X,       //  X coordinate of point;
                                                                       Double_t Y,       //  Y coordinate of point;
                                                                       Double_t xmargin, //  safety margin in X coordinate;
                                                                       Double_t ymargin  //  safety margin in Y coordinate;
)
{
  //	this is a set of sensors placed vertically perpendicularly to the Z direction;
  //	they are placed between 2.41 and 2.43 in Z;
  //	look at Gianluigi's logbook on page 194 for the geometry detail;

  if ((X > 1.17 + xmargin && X < 2.33 - xmargin && Y > -2.26 + ymargin && Y < 2.26 - ymargin) ||
      (X < 0. - xmargin && X > -1.21 + xmargin && fabs(Y) < 3.43 - ymargin && fabs(Y) > 1.19 + ymargin) || (X < -2.32 - xmargin && X > -3.48 + xmargin && fabs(Y) < 1.12 - ymargin))
    return true;

  return false;
}

//----------end of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk2_41to2_43withMargin

//----------begin of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk3_97to3_99

bool PndTrkCTGeometryCalculations::IsInMvdMiniDisk3_97to3_99(Double_t X, //  X coordinate of point;
                                                             Double_t Y  //  Y coordinate of point;
)
{
  //	this is a set of sensors placed vertically perpendicularly to the Z direction;
  //	they are placed between 3.97 and 3.99 in Z;
  //	look at Gianluigi's logbook on page 195 for the geometry detail;

  if ((X > 1.17 && X < 2.33 && Y > -2.26 && Y < 2.26) || (X < 0. && X > -1.21 && fabs(Y) < 3.43 && fabs(Y) > 1.19) || (X < -2.32 && X > -3.48 && fabs(Y) < 1.12))
    return true;

  return false;
}

//----------end of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk3_97to3_99

//----------begin of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk3_97to3_99withMargin

bool PndTrkCTGeometryCalculations::IsInMvdMiniDisk3_97to3_99withMargin(Double_t X,       //  X coordinate of point;
                                                                       Double_t Y,       //  Y coordinate of point;
                                                                       Double_t xmargin, //  safety margin in X coordinate;
                                                                       Double_t ymargin  //  safety margin in Y coordinate;
)
{
  //	this is a set of sensors placed vertically perpendicularly to the Z direction;
  //	they are placed between 3.97 and 3.99 in Z;
  //	look at Gianluigi's logbook on page 193 for the geometry detail;

  if ((X > 1.17 + xmargin && X < 2.33 - xmargin && Y > -2.26 + ymargin && Y < 2.26 - ymargin) ||
      (X < 0. - xmargin && X > -1.21 + xmargin && fabs(Y) < 3.43 - ymargin && fabs(Y) > 1.19 + ymargin) || (X < -2.32 - xmargin && X > -3.48 + xmargin && fabs(Y) < 1.12 - ymargin))
    return true;

  return false;
}

//----------end of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk3_97to3_99withMargin

//----------begin of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk4_41to4_43

bool PndTrkCTGeometryCalculations::IsInMvdMiniDisk4_41to4_43(Double_t X, //  X coordinate of point;
                                                             Double_t Y  //  Y coordinate of point;
)
{
  //	this is a set of sensors placed vertically perpendicularly to the Z direction;
  //	they are placed between 4.41 and 4.43 in Z;
  //	look at Gianluigi's logbook on page 195 for the geometry detail;

  if ((X > -2.33 && X < -1.17 && Y > -2.26 && Y < 2.26) || (X > 0. && X < 1.21 && fabs(Y) < 3.43 && fabs(Y) > 1.19) || (X > 2.32 && X < 3.48 && fabs(Y) < 1.12))
    return true;

  return false;
}

//----------end of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk4_41to4_43

//----------begin of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk4_41to4_43withMargin

bool PndTrkCTGeometryCalculations::IsInMvdMiniDisk4_41to4_43withMargin(Double_t X,       //  X coordinate of point;
                                                                       Double_t Y,       //  Y coordinate of point;
                                                                       Double_t xmargin, //  safety margin in X coordinate;
                                                                       Double_t ymargin  //  safety margin in Y coordinate;
)
{
  //	this is a set of sensors placed vertically perpendicularly to the Z direction;
  //	they are placed between 4.41 and 4.43 in Z;
  //	look at Gianluigi's logbook on page 195 for the geometry detail;

  if ((X > -2.33 + xmargin && X < -1.17 - xmargin && Y > -2.26 + ymargin && Y < 2.26 - ymargin) || (X > 0. + xmargin && X < 1.21 - xmargin && fabs(Y) < 3.43 && fabs(Y) > 1.19) ||
      (X > 2.32 + xmargin && X < 3.48 - xmargin && fabs(Y) < 1.12 - ymargin))
    return true;

  return false;
}

//----------end of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk4_41to4_43withMargin

//----------begin of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk6_97to6_99

bool PndTrkCTGeometryCalculations::IsInMvdMiniDisk6_97to6_99(Double_t X, //  X coordinate of point;
                                                             Double_t Y  //  Y coordinate of point;
)
{
  //	this is a set of sensors placed vertically perpendicularly to the Z direction;
  //	they are placed between 6.97 and 6.99 in Z;
  //	look at Gianluigi's logbook on page 195 for the geometry detail;

  if ((X > -6.96 && X < -5.8 && fabs(Y) < 2.31 && fabs(Y) > 0.07) || (X > -4.64 && X < -3.48 && fabs(Y) < 5.73 && fabs(Y) > 0.07) ||
      (X > -2.33 && X < -1.17 && fabs(Y) < 6.87 && fabs(Y) > 0.07) || (X > 0. && X < 1.16 && fabs(Y) < 6.85 && fabs(Y) > 1.19) ||
      (X > 2.3 && X < 3.46 && fabs(Y) < 5.73 && fabs(Y) > 0.07) || (X > 4.64 && X < 5.8 && fabs(Y) < 4.59 && fabs(Y) > 0.07))
    return true;

  return false;
}

//----------end of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk6_97to6_99

//----------begin of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk6_97to6_99withMargin

bool PndTrkCTGeometryCalculations::IsInMvdMiniDisk6_97to6_99withMargin(Double_t X,       //  X coordinate of point;
                                                                       Double_t Y,       //  Y coordinate of point;
                                                                       Double_t xmargin, //  safety margin in X coordinate;
                                                                       Double_t ymargin  //  safety margin in Y coordinate;
)
{
  //	this is a set of sensors placed vertically perpendicularly to the Z direction;
  //	they are placed between 6.97 and 6.99 in Z;
  //	look at Gianluigi's logbook on page 195 for the geometry detail;

  if ((X > -6.96 + xmargin && X < -5.8 - xmargin && fabs(Y) < 2.31 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (X > -4.64 + xmargin && X < -3.48 - xmargin && fabs(Y) < 5.73 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (X > -2.33 + xmargin && X < -1.17 - xmargin && fabs(Y) < 6.87 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (X > 0. + xmargin && X < 1.16 - xmargin && fabs(Y) < 6.85 - ymargin && fabs(Y) > 1.19 + ymargin) ||
      (X > 2.3 + xmargin && X < 3.46 - xmargin && fabs(Y) < 5.73 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (X > 4.64 + xmargin && X < 5.8 - xmargin && fabs(Y) < 4.59 - ymargin && fabs(Y) > 0.07 + ymargin))
    return true;

  return false;
}

//----------end of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk6_97to6_99withMargin

//----------begin of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk7_41to7_43

bool PndTrkCTGeometryCalculations::IsInMvdMiniDisk7_41to7_43(Double_t X, //  X coordinate of point;
                                                             Double_t Y  //  Y coordinate of point;
)
{
  //	this is a set of sensors placed vertically perpendicularly to the Z direction;
  //	they are placed between 7.41 and 7.43 in Z;
  //	look at Gianluigi's logbook on page 196 for the geometry detail;

  // this geometry is the mirror-reflected with respect of Y axis of the Z=6.97_6.99 geometry;

  if ((-X > -6.96 && -X < -5.8 && fabs(Y) < 2.31 && fabs(Y) > 0.07) || (-X > -4.64 && -X < -3.48 && fabs(Y) < 5.73 && fabs(Y) > 0.07) ||
      (-X > -2.33 && -X < -1.17 && fabs(Y) < 6.87 && fabs(Y) > 0.07) || (-X > 0. && -X < 1.16 && fabs(Y) < 6.85 && fabs(Y) > 1.19) ||
      (-X > 2.3 && -X < 3.46 && fabs(Y) < 5.73 && fabs(Y) > 0.07) || (-X > 4.64 && -X < 5.8 && fabs(Y) < 4.59 && fabs(Y) > 0.07))
    return true;

  return false;
}

//----------end of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk7_41to7_43

//----------begin of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk7_41to7_43withMargin

bool PndTrkCTGeometryCalculations::IsInMvdMiniDisk7_41to7_43withMargin(Double_t X,       //  X coordinate of point;
                                                                       Double_t Y,       //  Y coordinate of point;
                                                                       Double_t xmargin, //  safety margin in X coordinate;
                                                                       Double_t ymargin  //  safety margin in Y coordinate;
)
{
  //	this is a set of sensors placed vertically perpendicularly to the Z direction;
  //	they are placed between 7.41 and 7.43 in Z;
  //	look at Gianluigi's logbook on page 196 for the geometry detail;

  // this geometry is the mirror-reflected with respect of Y axis of the Z=6.97_6.99 geometry;

  if ((-X > -6.96 + xmargin && -X < -5.8 - xmargin && fabs(Y) < 2.31 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (-X > -4.64 + xmargin && -X < -3.48 - xmargin && fabs(Y) < 5.73 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (-X > -2.33 + xmargin && -X < -1.17 - xmargin && fabs(Y) < 6.87 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (-X > 0. + xmargin && -X < 1.16 - xmargin && fabs(Y) < 6.85 - ymargin && fabs(Y) > 1.19 + ymargin) ||
      (-X > 2.3 + xmargin && -X < 3.46 - xmargin && fabs(Y) < 5.73 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (-X > 4.64 + xmargin && -X < 5.8 - xmargin && fabs(Y) < 4.59 - ymargin && fabs(Y) > 0.07 + ymargin))
    return true;

  return false;
}

//----------end of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk7_41to7_43withMargin

//----------begin of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk9_97to9_99

bool PndTrkCTGeometryCalculations::IsInMvdMiniDisk9_97to9_99(Double_t X, //  X coordinate of point;
                                                             Double_t Y  //  Y coordinate of point;
)
{
  //	this is a set of sensors placed vertically perpendicularly to the Z direction;
  //	they are placed between 9.97 and 9.99 in Z;
  //	look at Gianluigi's logbook on page 197 for the geometry detail;

  // this geometry is the mirror-reflected with respect of Y axis of the Z=6.97_6.99 geometry;

  if ((-X > -6.96 && -X < -5.8 && fabs(Y) < 2.31 && fabs(Y) > 0.07) || (-X > -4.64 && -X < -3.48 && fabs(Y) < 5.73 && fabs(Y) > 0.07) ||
      (-X > -2.33 && -X < -1.17 && fabs(Y) < 6.87 && fabs(Y) > 0.07) || (-X > 0. && -X < 1.16 && fabs(Y) < 6.85 && fabs(Y) > 1.19) ||
      (-X > 2.3 && -X < 3.46 && fabs(Y) < 5.73 && fabs(Y) > 0.07) || (-X > 4.64 && -X < 5.8 && fabs(Y) < 4.59 && fabs(Y) > 0.07))
    return true;

  return false;
}

//----------end of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk9_97to9_99

//----------begin of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk9_97to9_99withMargin

bool PndTrkCTGeometryCalculations::IsInMvdMiniDisk9_97to9_99withMargin(Double_t X,       //  X coordinate of point;
                                                                       Double_t Y,       //  Y coordinate of point;
                                                                       Double_t xmargin, //  safety margin in X coordinate;
                                                                       Double_t ymargin  //  safety margin in Y coordinate;
)
{
  //	this is a set of sensors placed vertically perpendicularly to the Z direction;
  //	they are placed between 9.97 and 9.99 in Z;
  //	look at Gianluigi's logbook on page 197 for the geometry detail;

  // this geometry is the mirror-reflected with respect of Y axis of the Z=6.97_6.99 geometry;

  if ((-X > -6.96 + xmargin && -X < -5.8 - xmargin && fabs(Y) < 2.31 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (-X > -4.64 + xmargin && -X < -3.48 - xmargin && fabs(Y) < 5.73 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (-X > -2.33 + xmargin && -X < -1.17 - xmargin && fabs(Y) < 6.87 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (-X > 0. + xmargin && -X < 1.16 - xmargin && fabs(Y) < 6.85 - ymargin && fabs(Y) > 1.19 + ymargin) ||
      (-X > 2.3 + xmargin && -X < 3.46 - xmargin && fabs(Y) < 5.73 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (-X > 4.64 + xmargin && -X < 5.8 - xmargin && fabs(Y) < 4.59 - ymargin && fabs(Y) > 0.07 + ymargin))
    return true;

  return false;
}

//----------end of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk9_97to9_99withMargin

//----------begin of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk10_41to10_43

bool PndTrkCTGeometryCalculations::IsInMvdMiniDisk10_41to10_43(Double_t X, //  X coordinate of point;
                                                               Double_t Y  //  Y coordinate of point;
)
{
  //	this is a set of sensors placed vertically perpendicularly to the Z direction;
  //	they are placed between 10.41 and 10.43 in Z;
  //	look at Gianluigi's logbook on page 197 for the geometry detail;

  // this geometry is the same as the Z=6.97_6.99 geometry;

  if ((X > -6.96 && X < -5.8 && fabs(Y) < 2.31 && fabs(Y) > 0.07) || (X > -4.64 && X < -3.48 && fabs(Y) < 5.73 && fabs(Y) > 0.07) ||
      (X > -2.33 && X < -1.17 && fabs(Y) < 6.87 && fabs(Y) > 0.07) || (X > 0. && X < 1.16 && fabs(Y) < 6.85 && fabs(Y) > 1.19) ||
      (X > 2.3 && X < 3.46 && fabs(Y) < 5.73 && fabs(Y) > 0.07) || (X > 4.64 && X < 5.8 && fabs(Y) < 4.59 && fabs(Y) > 0.07))
    return true;

  return false;
}

//----------end of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk10_41to10_43

//----------begin of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk10_41to10_43withMargin

bool PndTrkCTGeometryCalculations::IsInMvdMiniDisk10_41to10_43withMargin(Double_t X,       //  X coordinate of point;
                                                                         Double_t Y,       //  Y coordinate of point;
                                                                         Double_t xmargin, //  safety margin in X coordinate;
                                                                         Double_t ymargin  //  safety margin in Y coordinate;
)
{
  //	this is a set of sensors placed vertically perpendicularly to the Z direction;
  //	they are placed between 10.41 and 10.43 in Z;
  //	look at Gianluigi's logbook on page 197 for the geometry detail;

  // this geometry is the same as the Z=6.97_6.99 geometry;

  if ((X > -6.96 + xmargin && X < -5.8 - xmargin && fabs(Y) < 2.31 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (X > -4.64 + xmargin && X < -3.48 - xmargin && fabs(Y) < 5.73 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (X > -2.33 + xmargin && X < -1.17 - xmargin && fabs(Y) < 6.87 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (X > 0. + xmargin && X < 1.16 - xmargin && fabs(Y) < 6.85 - ymargin && fabs(Y) > 1.19 + ymargin) ||
      (X > 2.3 + xmargin && X < 3.46 - xmargin && fabs(Y) < 5.73 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (X > 4.64 + xmargin && X < 5.8 - xmargin && fabs(Y) < 4.59 - ymargin && fabs(Y) > 0.07 + ymargin))
    return true;

  return false;
}

//----------end of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk10_41to10_43withMargin

//----------begin of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk14_77to14_79

bool PndTrkCTGeometryCalculations::IsInMvdMiniDisk14_77to14_79(Double_t X, //  X coordinate of point;
                                                               Double_t Y  //  Y coordinate of point;
)
{
  //	this is a set of sensors placed vertically perpendicularly to the Z direction;
  //	they are placed between 14.77 and 14.79 in Z;
  //	look at Gianluigi's logbook on page 198 for the geometry detail;

  // this geometry is the same as the Z=6.97_6.99 geometry;

  if ((X > -6.96 && X < -5.8 && fabs(Y) < 2.31 && fabs(Y) > 0.07) || (X > -4.64 && X < -3.48 && fabs(Y) < 5.73 && fabs(Y) > 0.07) ||
      (X > -2.33 && X < -1.17 && fabs(Y) < 6.87 && fabs(Y) > 0.07) || (X > 0. && X < 1.16 && fabs(Y) < 6.85 && fabs(Y) > 1.19) ||
      (X > 2.3 && X < 3.46 && fabs(Y) < 5.73 && fabs(Y) > 0.07) || (X > 4.64 && X < 5.8 && fabs(Y) < 4.59 && fabs(Y) > 0.07))
    return true;

  return false;
}

//----------end of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk14_77to14_79

//----------begin of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk14_77to14_79withMargin

bool PndTrkCTGeometryCalculations::IsInMvdMiniDisk14_77to14_79withMargin(Double_t X,       //  X coordinate of point;
                                                                         Double_t Y,       //  Y coordinate of point;
                                                                         Double_t xmargin, //  safety margin in X coordinate;
                                                                         Double_t ymargin  //  safety margin in Y coordinate;
)
{
  //	this is a set of sensors placed vertically perpendicularly to the Z direction;
  //	they are placed between 14.77 and 14.79 in Z;
  //	look at Gianluigi's logbook on page 198 for the geometry detail;

  // this geometry is the same as the Z=6.97_6.99 geometry;

  if ((X > -6.96 + xmargin && X < -5.8 - xmargin && fabs(Y) < 2.31 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (X > -4.64 + xmargin && X < -3.48 - xmargin && fabs(Y) < 5.73 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (X > -2.33 + xmargin && X < -1.17 - xmargin && fabs(Y) < 6.87 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (X > 0. + xmargin && X < 1.16 - xmargin && fabs(Y) < 6.85 - ymargin && fabs(Y) > 1.19 + ymargin) ||
      (X > 2.3 + xmargin && X < 3.46 - xmargin && fabs(Y) < 5.73 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (X > 4.64 + xmargin && X < 5.8 - xmargin && fabs(Y) < 4.59 - ymargin && fabs(Y) > 0.07 + ymargin))
    return true;

  return false;
}

//----------end of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk14_77to14_79withMargin

//----------begin of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk15_21to15_23

bool PndTrkCTGeometryCalculations::IsInMvdMiniDisk15_21to15_23(Double_t X, //  X coordinate of point;
                                                               Double_t Y  //  Y coordinate of point;
)
{
  //	this is a set of sensors placed vertically perpendicularly to the Z direction;
  //	they are placed between 15.21 and 15.23 in Z;
  //	look at Gianluigi's logbook on page 198 for the geometry detail;

  // this geometry is the same as the Z=7.41_7.43 geometry;

  if ((-X > -6.96 && -X < -5.8 && fabs(Y) < 2.31 && fabs(Y) > 0.07) || (-X > -4.64 && -X < -3.48 && fabs(Y) < 5.73 && fabs(Y) > 0.07) ||
      (-X > -2.33 && -X < -1.17 && fabs(Y) < 6.87 && fabs(Y) > 0.07) || (-X > 0. && -X < 1.16 && fabs(Y) < 6.85 && fabs(Y) > 1.19) ||
      (-X > 2.3 && -X < 3.46 && fabs(Y) < 5.73 && fabs(Y) > 0.07) || (-X > 4.64 && -X < 5.8 && fabs(Y) < 4.59 && fabs(Y) > 0.07))
    return true;

  return false;
}

//----------end of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk15_21to15_23

//----------begin of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk15_21to15_23withMargin

bool PndTrkCTGeometryCalculations::IsInMvdMiniDisk15_21to15_23withMargin(Double_t X,       //  X coordinate of point;
                                                                         Double_t Y,       //  Y coordinate of point;
                                                                         Double_t xmargin, //  safety margin in X coordinate;
                                                                         Double_t ymargin  //  safety margin in Y coordinate;
)
{
  //	this is a set of sensors placed vertically perpendicularly to the Z direction;
  //	they are placed between 15.21 and 15.23 in Z;
  //	look at Gianluigi's logbook on page 198 for the geometry detail;

  // this geometry is the same as the Z=7.41_7.43 geometry;

  if ((-X > -6.96 + xmargin && -X < -5.8 - xmargin && fabs(Y) < 2.31 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (-X > -4.64 + xmargin && -X < -3.48 - xmargin && fabs(Y) < 5.73 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (-X > -2.33 + xmargin && -X < -1.17 - xmargin && fabs(Y) < 6.87 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (-X > 0. + xmargin && -X < 1.16 - xmargin && fabs(Y) < 6.85 - ymargin && fabs(Y) > 1.19 + ymargin) ||
      (-X > 2.3 + xmargin && -X < 3.46 - xmargin && fabs(Y) < 5.73 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (-X > 4.64 + xmargin && -X < 5.8 - xmargin && fabs(Y) < 4.59 - ymargin && fabs(Y) > 0.07 + ymargin))
    return true;

  return false;
}

//----------end of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk15_21to15_23withMargin

//----------begin of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk21_77to21_79

bool PndTrkCTGeometryCalculations::IsInMvdMiniDisk21_77to21_79(Double_t X, //  X coordinate of point;
                                                               Double_t Y  //  Y coordinate of point;
)
{
  //	this is a set of sensors placed vertically perpendicularly to the Z direction;
  //	they are placed between 21.77 and 21.79 in Z;
  //	look at Gianluigi's logbook on page 198 for the geometry detail;

  // this geometry is the same as the Z=7.41_7.43 geometry;

  if ((-X > -6.96 && -X < -5.8 && fabs(Y) < 2.31 && fabs(Y) > 0.07) || (-X > -4.64 && -X < -3.48 && fabs(Y) < 5.73 && fabs(Y) > 0.07) ||
      (-X > -2.33 && -X < -1.17 && fabs(Y) < 6.87 && fabs(Y) > 0.07) || (-X > 0. && -X < 1.16 && fabs(Y) < 6.85 && fabs(Y) > 1.19) ||
      (-X > 2.3 && -X < 3.46 && fabs(Y) < 5.73 && fabs(Y) > 0.07) || (-X > 4.64 && -X < 5.8 && fabs(Y) < 4.59 && fabs(Y) > 0.07))
    return true;

  return false;
}

//----------end of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk21_77to21_79

//----------begin of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk21_77to21_79withMargin

bool PndTrkCTGeometryCalculations::IsInMvdMiniDisk21_77to21_79withMargin(Double_t X,           //  X coordinate of point;
                                                                         Double_t Y,           //  Y coordinate of point;
                                                                         Double_t /*xmargin*/, //[R.K. 9/2018] unused	//  safety margin in X coordinate;
                                                                         Double_t /*ymargin*/  //[R.K. 9/2018] unused	//  safety margin in Y coordinate;
)
{
  //	this is a set of sensors placed vertically perpendicularly to the Z direction;
  //	they are placed between 21.77 and 21.79 in Z;
  //	look at Gianluigi's logbook on page 198 for the geometry detail;

  // this geometry is the same as the Z=7.41_7.43 geometry;

  if ((-X > -6.96 && -X < -5.8 && fabs(Y) < 2.31 && fabs(Y) > 0.07) || (-X > -4.64 && -X < -3.48 && fabs(Y) < 5.73 && fabs(Y) > 0.07) ||
      (-X > -2.33 && -X < -1.17 && fabs(Y) < 6.87 && fabs(Y) > 0.07) || (-X > 0. && -X < 1.16 && fabs(Y) < 6.85 && fabs(Y) > 1.19) ||
      (-X > 2.3 && -X < 3.46 && fabs(Y) < 5.73 && fabs(Y) > 0.07) || (-X > 4.64 && -X < 5.8 && fabs(Y) < 4.59 && fabs(Y) > 0.07))
    return true;

  return false;
}

//----------end of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk21_77to21_79withMargin

//----------begin of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk22_21to22_23

bool PndTrkCTGeometryCalculations::IsInMvdMiniDisk22_21to22_23(Double_t X, //  X coordinate of point;
                                                               Double_t Y  //  Y coordinate of point;
)
{
  //	this is a set of sensors placed vertically perpendicularly to the Z direction;
  //	they are placed between 22.21 and 22.23 in Z;
  //	look at Gianluigi's logbook on page 193 for the geometry detail;

  if ((X > -6.96 && X < -5.8 && fabs(Y) < 2.31 && fabs(Y) > 0.07) || (X > -4.64 && X < -3.48 && fabs(Y) < 5.73 && fabs(Y) > 0.07) ||
      (X > -2.33 && X < -1.17 && fabs(Y) < 6.87 && fabs(Y) > 0.07) || (X > 0. && X < 1.16 && fabs(Y) < 6.85 && fabs(Y) > 1.19) ||
      (X > 2.3 && X < 3.46 && fabs(Y) < 5.73 && fabs(Y) > 0.07) || (X > 4.64 && X < 5.8 && fabs(Y) < 4.59 && fabs(Y) > 0.07))
    return true;

  return false;
}

//----------end of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk22_21to22_23

//----------begin of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk22_21to22_23withMargin

bool PndTrkCTGeometryCalculations::IsInMvdMiniDisk22_21to22_23withMargin(Double_t X,       //  X coordinate of point;
                                                                         Double_t Y,       //  Y coordinate of point;
                                                                         Double_t xmargin, //  safety margin in X coordinate;
                                                                         Double_t ymargin  //  safety margin in Y coordinate;
)
{
  //	this is a set of sensors placed vertically perpendicularly to the Z direction;
  //	they are placed between 22.21 and 22.23 in Z;
  //	look at Gianluigi's logbook on page 193 for the geometry detail;

  if ((X > -6.96 + xmargin && X < -5.8 - xmargin && fabs(Y) < 2.31 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (X > -4.64 + xmargin && X < -3.48 - xmargin && fabs(Y) < 5.73 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (X > -2.33 + xmargin && X < -1.17 - xmargin && fabs(Y) < 6.87 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (X > 0. + xmargin && X < 1.16 - xmargin && fabs(Y) < 6.85 - ymargin && fabs(Y) > 1.19 + ymargin) ||
      (X > 2.3 + xmargin && X < 3.46 - xmargin && fabs(Y) < 5.73 - ymargin && fabs(Y) > 0.07 + ymargin) ||
      (X > 4.64 + xmargin && X < 5.8 - xmargin && fabs(Y) < 4.59 - ymargin && fabs(Y) > 0.07 + ymargin))
    return true;

  return false;
}

//----------end of function PndTrkCTGeometryCalculations::IsInMvdMiniDisk22_21to22_23withMargin

//----------end of function PndTrkCTGeometryCalculations::IsInTargetPipe

bool PndTrkCTGeometryCalculations::IsInTargetPipe(Double_t Oxx, Double_t Oyy, Double_t Rr, Double_t fi0, Double_t kappa, Short_t Charge, Double_t gap)
{
  Short_t nintersections;

  Double_t delta, XintersectionList[8], YintersectionList[8], Xcross[2], Ycross[2];

  //  find if this track lays in the target pipe volume
  //  for distances < 15 (supposidly the maximum radius of the Mvd system)
  //  and consequently it cannot have Mvd hits.

  nintersections = 0;

  // intersections of trajectory with plane X = gap;
  delta = Rr * Rr - (gap - Oxx) * (gap - Oxx);
  if (delta > 0.) {
    XintersectionList[0] = gap;
    YintersectionList[0] = sqrt(delta) + Oyy;
    XintersectionList[1] = gap;
    YintersectionList[1] = -sqrt(delta) + Oyy;
    nintersections += 2;
  }

  // intersections of trajectory with plane X = -gap;
  delta = Rr * Rr - (-gap - Oxx) * (-gap - Oxx);
  if (delta > 0.) {
    XintersectionList[nintersections] = -gap;
    YintersectionList[nintersections] = sqrt(delta) + Oyy;
    XintersectionList[nintersections + 1] = -gap;
    YintersectionList[nintersections + 1] = -sqrt(delta) + Oyy;
    nintersections += 2;
  }

  // intersections of trajectory with plane Z = gap;
  XintersectionList[nintersections] = Oxx + Rr * cos(fi0 + kappa * gap);
  YintersectionList[nintersections] = Oyy + Rr * sin(fi0 + kappa * gap);
  nintersections++;

  // intersections of trajectory with plane Z = -gap;
  XintersectionList[nintersections] = Oxx + Rr * cos(fi0 - kappa * gap);
  YintersectionList[nintersections] = Oyy + Rr * sin(fi0 - kappa * gap);
  nintersections++;

  // order the found intersection point by increasing/decreasing FI
  // depending if the Charge is negative or positive.
  ChooseEntranceExitbis(Oxx, Oyy, Charge, fi0,
                        nintersections, // n. intersection in input.
                        XintersectionList, YintersectionList,
                        Xcross, // output
                        Ycross  // output
  );

  // here I suppose that the Mvd system CONSERVATIVELY ends at Rmax=5 cm.
  //---------------
  for (int ic = 0; ic < nintersections; ic++) {
  }
  //-------------
  if (fabs(Ycross[0]) > 4.)
    return true;
  else
    return false;
}
//----------end of function PndTrkCTGeometryCalculations::IsInTargetPipe

//----------star of function PndTrkCTGeometryCalculations::IsInternal

bool PndTrkCTGeometryCalculations::IsInternal(Double_t Px, // point
                                              Double_t Py, Double_t Xtraslation, Double_t Ytraslation, Double_t Theta)
{

  // for explanations see Gianluigi's logbook on page 278-280.

  if ((Xtraslation - Px) * sin(Theta) + (Py - Ytraslation) * cos(Theta) >= 0.)
    return true;
  else
    return false;
}

//----------end of function PndTrkCTGeometryCalculations::IsInternal

//----------begin of function PndTrkCTGeometryCalculations::ListAxialSectorsCrossedbyTrack_and_Hits

void PndTrkCTGeometryCalculations::ListAxialSectorsCrossedbyTrack_and_Hits(
  Double_t Ox,              // input;
  Double_t Oy,              // input;
  Double_t R,               // input;
  Double_t Charge,          // input;
  Short_t nHits,            // input;
  Short_t *ListHits,        // input;
  Double_t info[][7],       // input;
  Short_t &nArcs_populated, // output; # Arcs of trajectory populated by at least 1 axial hit; this is <= 56;
  Short_t nHitsInArc[56],   // output; number of hits in each Sector; if the maximun # of Intersected Sector is 56,
                            //  than the maximum # of Arcs is 28;

  Short_t (*ListHitsInArc)[56] // output; ordered list of hits in each Arc (from first to last
                               // according to the charge of the particle;if the maximum
                               //  # of Intersected Sector is 56, than the maximum # of Arcs is 28;
)
{

  //  nIntersection = number of sectors (entrance/exit) crossed by TRAJECTORY, namely by the entire
  //	circle projection of the helix on XY;
  //  nArcs = nIntersection/2 --> this is the number of Arcs in which the circle is segmented by the various
  //		sectors of the axial straws;
  //  nArcs_populated = the same as  nArcs, but only those Arcs in which axial STT hits are actually present;

  Short_t half, i,
    // ihit, //[R.K. 01/2017] unused variable?
    // j, //[R.K. 01/2017] unused variable?
    jArc, n, nArcs, nIntersections, oldArc, remainder;

  Double_t AllFiOrderedList[56], AllXOrderedList[56], AllYOrderedList[56], fi, FiStart, Start[3];
  // FiOrderedList12[12], //[R.K. 01/2017] unused variable?	// at most 12 intersections; ordered according to charge of the track
  // assuming it originated from (0,0,0);
  // FiOrderedList16[16]; //[R.K. 01/2017] unused variable?	// at most 16 intersections; ordered according to charge of the track
  // assuming it originated from (0,0,0);

  // Double_t
  // XintersectionList12[12],	// at most 12 intersections; //[R.K. 01/2017] unused variable?
  // XintersectionList16[16],	// at most 16 intersections; //[R.K. 01/2017] unused variable?
  // YintersectionList12[12],	// at most 12 intersections; //[R.K. 01/2017] unused variable?
  // YintersectionList16[16];	// at most 16 intersections; //[R.K. 01/2017] unused variable?

  // origin of the track assumed in (0,0,0);
  Start[0] = Start[1] = Start[2] = 0.;

  // Sector number convention:
  // 1 --> Right Axial Outer;
  // 2 --> Right Axial Inner;
  // 3 --> Left Axial Inner;
  // 4 --> Left Axial Outer;

  nArcs = 0;

  // Sector 1;
  nIntersections =
    FindTrackEntranceExitHexagonCircleRight2(Ox, Oy, R, Charge, Start, APOTEMAMINOUTERPARSTRAW, RSTRAWDETECTORMAX, VERTICALGAP, &AllXOrderedList[0], &AllYOrderedList[0],
                                             //				AllXOrderedList,
                                             //				AllYOrderedList,
                                             AllFiOrderedList);

  // nIntersections must be multiple of 2 (entrance/exit);
  // each pair entrance/exit defines an arc of the trajectory; each arc will be later analyzed for continuity;
  half = nIntersections / 2;
  remainder = nIntersections - 2 * half; // this is like fmod, only for integer arguments;
  //------

  if (remainder != 0) {
    cout << "PndTrkCTGeometryCalculations::ListAxialSectorsCrossedbyTrack_and_Hits,after FindTrackEntranceExitHexagonCircleRight2;"
         << "\n\todd number of intersections : " << nIntersections << ", cannot be, return with nArcs_populated=0!\n";
    nArcs_populated = 0;
    return;
  }
  //------

  nArcs += half;

  /*
    for(i=0;i<nIntersections;i++){
   cout<<"from PndTrkCTGeometryCalculations, 1;  Inters. so far "<<nIntersections<<", X inter "<<AllXOrderedList[i]<<", Y inter "
        <<AllYOrderedList[i]<<endl;
    };
  */

  // Sector 2;
  n = FindTrackEntranceExitbiHexagonRight2(VERTICALGAP, Ox, Oy, R, Charge, Start, APOTEMASTRAWDETECTORMIN, APOTEMAMAXINNERPARSTRAW, &AllXOrderedList[nIntersections],
                                           &AllYOrderedList[nIntersections], &AllFiOrderedList[nIntersections]);

  // n  must be multiple of 2 (entrance/exit);
  // each pair entrance/exit defines an arc of the trajectory; each arc will be later analyzed for continuity;
  half = n / 2;
  remainder = n - 2 * half; // this is like fmod, only for integer arguments;
  //------
  if (remainder != 0) {
    cout << "PndTrkCTGeometryCalculations::ListAxialSectorsCrossedbyTrack_and_Hits,after FindTrackEntranceExitbiHexagonRight2;"
         << "\n\todd number of intersections : " << n << ", cannot be, return with nArcs_populated=0!\n";
    nArcs_populated = 0;
    return;
  }
  //------

  nIntersections += n;

  nArcs += half;

  /*	for(i=0;i<nIntersections;i++){ cout<<"from PndTrkCTGeometryCalculations, 2;  Inters. so far "<<
  nIntersections<<", X inter "<<AllXOrderedList[i]<<", Y inter "
        <<AllYOrderedList[i]<<endl;};
  */

  // Sector 3;
  n = FindTrackEntranceExitbiHexagonLeft2(VERTICALGAP, Ox, Oy, R, Charge, Start, APOTEMASTRAWDETECTORMIN, APOTEMAMAXINNERPARSTRAW, &AllXOrderedList[nIntersections],
                                          &AllYOrderedList[nIntersections], &AllFiOrderedList[nIntersections]);
  // n  must be multiple of 2 (entrance/exit);
  // each pair entrance/exit defines an arc of the trajectory; each arc will be later analyzed for continuity;
  half = n / 2;
  remainder = n - 2 * half; // this is like fmod, only for integer arguments;
  //------
  if (remainder != 0) {
    cout << "PndTrkCTGeometryCalculations::ListAxialSectorsCrossedbyTrack_and_Hits,after FindTrackEntranceExitbiHexagonLeft2;"
         << "\n\todd number of intersections : " << n << ", cannot be, return with nArcs_populated=0!\n";
    nArcs_populated = 0;
    return;
  }
  //------

  nArcs += half;

  //	for(i=nIntersections;i<nIntersections+n;i++){ IntersectedSector[i]=3;};
  nIntersections += n;

  /*	for(i=0;i<nIntersections;i++){ cout<<"from PndTrkCTGeometryCalculations, 3,  Inters. so far "<<
  nIntersections<<", X inter "<<AllXOrderedList[i]<<", Y inter "
        <<AllYOrderedList[i]<<endl;};
  */

  // Sector 4;
  n = FindTrackEntranceExitHexagonCircleLeft2(Ox, Oy, R, Charge, Start, APOTEMAMINOUTERPARSTRAW, RSTRAWDETECTORMAX, VERTICALGAP, &AllXOrderedList[nIntersections],
                                              &AllYOrderedList[nIntersections], &AllFiOrderedList[nIntersections]);
  // n  must be multiple of 2 (entrance/exit);
  // each pair entrance/exit defines an arc of the trajectory; each arc will be later analyzed for continuity;
  half = n / 2;
  remainder = n - 2 * half; // this is like fmod, only for integer arguments;
  //------
  if (remainder != 0) {
    cout << "PndTrkCTGeometryCalculations::ListAxialSectorsCrossedbyTrack_and_Hits,after FindTrackEntranceExitHexagonCircleLeft2;"
         << "\n\todd number of intersections : " << n << ", cannot be, return with nArcs_populated=0!\n";
    nArcs_populated = 0;
    return;
  }
  //------

  nArcs += half;

  nIntersections += n;

  /*	for(i=0;i<nIntersections;i++){ cout<<"from PndTrkCTGeometryCalculations, 4,  Inters. so far "<<nIntersections<<
  ", X inter "<<AllXOrderedList[i]<<", Y inter "
        <<AllYOrderedList[i]<<endl;};
  */

  // now subdivide the hit list in many lists, on for each sector crossed by track;
  // since the List of Hits in a track should already be ordered from inside
  // outwards, the various ListHitsInArc[*][jArc]  should be automatically
  // ordered from inside outwards; also the various Arcs shoud be already in the right sequence
  // from inside towards outside;

  FiStart = atan2(Start[1] - Oy, Start[0] - Ox);
  if (FiStart < 0.)
    FiStart += TWO_PI;
  if (FiStart < 0.)
    FiStart = 0.;

  oldArc = -1;
  nArcs_populated = 0;

  if (Charge > 0.) { // particle runs clockwise;
    // loop over the hits in track;
    for (i = 0; i < nHits; i++) {
      fi = atan2(info[ListHits[i]][1] - Oy, info[ListHits[i]][0] - Ox);
      if (fi < 0.)
        fi += TWO_PI;
      if (fi < 0.)
        fi = 0.;
      if (fi > FiStart)
        fi -= TWO_PI;
      if (fi > FiStart)
        fi = FiStart;
      // find to which Arc this hit belongs;
      for (jArc = 0; jArc < nArcs; jArc++) {
        if (fi < AllFiOrderedList[2 * jArc] && fi > AllFiOrderedList[2 * jArc + 1]) {
          if (jArc != oldArc) { // new Arc;
            oldArc = jArc;
            nHitsInArc[nArcs_populated] = 1;
            ListHitsInArc[0][nArcs_populated] = ListHits[i];
            nArcs_populated++;
          } else {
            ListHitsInArc[nHitsInArc[nArcs_populated - 1]][nArcs_populated - 1] = ListHits[i];
            nHitsInArc[nArcs_populated - 1]++;
          } // end of if( jArc != oldArc )

          break;
        } // end of if(fi < AllFiOrderedList[2*jArc] && fi>AllFiOrderedList[2*jArc+1])
      }   // end of for(j=0;j<nIntersections;j++)
    }     // end of for(i=0;i<nHits;i++)

  } else { // continuation of if(charge > 0.) // negative particle run anticlockwise;

    // loop over the hits in track;
    for (i = 0; i < nHits; i++) {
      fi = atan2(info[ListHits[i]][1] - Oy, info[ListHits[i]][0] - Ox);
      if (fi < 0.)
        fi += TWO_PI;
      if (fi < 0.)
        fi = 0.;
      if (fi < FiStart)
        fi += TWO_PI;
      if (fi < FiStart)
        fi = FiStart;
      // find to which Arc this hit belongs;
      for (jArc = 0; jArc < nArcs; jArc++) {
        if (fi > AllFiOrderedList[2 * jArc] && fi < AllFiOrderedList[2 * jArc + 1]) {
          if (jArc != oldArc) { // new Arc;
            oldArc = jArc;
            nHitsInArc[nArcs_populated] = 1;
            ListHitsInArc[0][nArcs_populated] = ListHits[i];
            nArcs_populated++;
          } else {
            ListHitsInArc[nHitsInArc[nArcs_populated - 1]][nArcs_populated - 1] = ListHits[i];
            nHitsInArc[nArcs_populated - 1]++;
          } // end of if( jArc != oldArc )

          break;
        } // end of if(fi < AllFiOrderedList[2*jArc] && fi>AllFiOrderedList[2*jArc+1])
      }   // end of for(j=0;j<nIntersections;j++)
    }     // end of for(i=0;i<nHits;i++)

  } // end of  if(charge > 0.)

  return;
}

//----------end of function PndTrkCTGeometryCalculations::ListAxialSectorsCrossedbyTrack_and_Hits

ClassImp(PndTrkCTGeometryCalculations);
