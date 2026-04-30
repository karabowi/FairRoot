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

#include "PndTrkCleanup.h"
#include "PndTrkVectors.h"
#include "PndTrkConstants.h"

#include <iostream>
#include <math.h>

// Root includes
#include "TROOT.h"

using namespace std;

//----------begin of function PndTrkCleanup::BadTrack_ParStt

bool PndTrkCleanup::BadTrack_ParStt(Double_t Oxx, Double_t Oyy, Double_t Rr, Double_t strawradius, Short_t Charge,
                                    Double_t Xcross[2], // Xcross[0]=point of entrance;
                                                        //  Xcross[1]=point of exit.
                                    Double_t Ycross[2], Short_t nHits, Short_t *ListHits, Double_t info[][7], int istampa, Double_t cut, Short_t maxnum,
                                    Short_t islack // uncertainty allowed as far as
                                                   // the n. of hits that should be present in a given section of the Stt track.
)
{
  Short_t ibad, ihit, ninside;

  Double_t cut2, length, Xprevious, Yprevious, S, Distance[nHits + 1];

  // class with all the geometry calculations :
  PndTrkCTGeometryCalculations GeometryCalculator;

  cut2 = cut * cut;
  ibad = 0;

  Xprevious = Xcross[0];
  Yprevious = Ycross[0];

  length = GeometryCalculator.CalculateArcLength(Oxx, Oyy, Rr, Charge, Xcross, Ycross);
  if (istampa > 1) {
    cout << "in BadTrack_ParStt : Xingresso " << Xcross[0] << ", Yingresso " << Ycross[0] << ", Xuscita " << Xcross[1] << ", Yuscita " << Ycross[1] << ", Lungh. arco " << length
         << ", islack " << islack << endl;
  }

  ninside = 0;
  for (ihit = 0; ihit < nHits; ihit++) {
    S = atan2(info[ListHits[ihit]][1] - Oyy, info[ListHits[ihit]][0] - Oxx);
    if (S < 0.)
      S += 2. * PI;
    if (S < 0.)
      S = 0.;
    if (!GeometryCalculator.IsInsideArc(Oxx, Oyy, Charge, Xcross, Ycross, S)) {
      continue;
    }
    ninside++;
    Distance[ihit] = (info[ListHits[ihit]][0] - Xprevious) * (info[ListHits[ihit]][0] - Xprevious) + (info[ListHits[ihit]][1] - Yprevious) * (info[ListHits[ihit]][1] - Yprevious);
    if (istampa > 1) {
      cout << "in BadTrack_ParStt :hit || n. " << ListHits[ihit] << ", X " << info[ListHits[ihit]][0] << ", Y " << info[ListHits[ihit]][1] << "\n\tX prima " << Xprevious
           << ", Y prima " << Yprevious << ", Distanza " << sqrt(Distance[ihit]) << ", cut = " << cut << endl;
    }
    Xprevious = info[ListHits[ihit]][0];
    Yprevious = info[ListHits[ihit]][1];

    if (Distance[ihit] > cut2) {
      if (Distance[ihit] > 16. * cut2) {
        return true;
      }
      ibad++;
    }

  } // end of   for(ihit=0 ;ihit<nHits;ihit++)

  // cut on the minimum (conservative) n. hits that must have fired

  int nume;
  nume = 0.5 * length / strawradius - islack;
  if (ninside < nume) {
    if (istampa > 1) {
      int icz = 0.5 * length / strawradius;
      cout << "in BadTrack_ParStt, n. Hits inside = " << ninside << " is < n. hits that should be inside at least = " << icz << "-islack (" << islack << "), track rejected!\n";
    }
    return true;
  }

  // compute the distance of last hit to point at which track leaves this detector volume.
  // In case nHits = 0 don't do this check (whether or not the track is genuine has
  // been decided already in the previous  for(ihit=0 ;ihit<nHits;ihit++) loop).

  if (nHits > 0) {

    // here S is already the fi of the last point.
    if (GeometryCalculator.IsInsideArc(Oxx, Oyy, Charge, Xcross, Ycross, S)) {

      Distance[nHits] = (info[ListHits[nHits - 1]][0] - Xcross[1]) * (info[ListHits[nHits - 1]][0] - Xcross[1]) +
                        (info[ListHits[nHits - 1]][1] - Ycross[1]) * (info[ListHits[nHits - 1]][1] - Ycross[1]);
      ;
      if (istampa > 1)
        cout << "in BadTrack_ParStt, Stt || hit n. (original notation) " << ListHits[nHits - 1] << ", Distance to boundary = " << sqrt(Distance[nHits]) << ", 4*cut " << 4. * cut
             << endl;

      if (Distance[nHits] > cut2) {
        if (Distance[nHits] > 16. * cut2)
          return true;
        ibad++;
      } // end of  if( Distance[nHits]>cut2 )
    }   // end of if( IsInsideArc

    if (istampa > 1)
      cout << "in BadTrack_ParStt, ibad " << ibad << ", max bad allowed = " << maxnum << endl;

    if (ibad > maxnum)
      return true;
    return false;

  } // end of   if(nHits>0)

  return true;
}

//----------end of function PndTrkCleanup::BadTrack_ParStt

//----------begin of function PndTrkCleanup::GoodTrack

bool PndTrkCleanup::GoodTrack(Double_t info[][7],            // input
                              bool farthest_hit_is_boundary, // input
                              Double_t Ox,                   // input; center of the current track;
                              Double_t Oy,                   // input; center of the current track;
                              Double_t R,                    // input; Radius of the current track;
                              Short_t Charge,                // input; Charge of the current track;
                              Short_t nHits,                 // input
                              Short_t *ListHits,             // input
                              Short_t *StrawCode,            // first straw boundary code (a straw can belong to 2 boundaries);
                              Short_t *StrawCode2,
                              Short_t *TubeID,                // input
                              Short_t *nParContiguous,        // input
                              Short_t ListParContiguous[][6], // input
                              Double_t *xTube,                // input
                              Double_t *yTube,                // input
                              Double_t * /*zTube*/,           // input //[R.K. 9/2018] unused
                              Double_t *xxyyTube,             // input

                              Short_t &holes // input and output
)
{

  // Convention for the Sector number used in GoodTrack :
  // Sector = 1 --> Axial Outer Right
  // Sector = 2 --> Axial Inner Right
  // Sector = 3 --> Axial Inner Left
  // Sector = 4 --> Axial Outer Left

  // holes == # of "holes" in the track;
  // no_holes == flag requiring total continuity of track (no holes) ;
  // farthest_hit_is_boundary --> if this flag is true the hit of the track furthest from the origin is
  // at the boundary of the sector;

  // nHits == # hits in this track under scrutiny;
  // info == some information on the hits;
  // ListHits == list hits in this track under scrutiny;
  // TubeID == hit number;

  bool boundary, yes;

  Short_t i, index, index_last, inner, j,
    // nIntersections, //[R.K. 01/2017] unused variable?
    outer,
    // tube_current, //[R.K. 01/2017] unused variable?
    tube_next;

  Double_t dist, dist2, fi,
    // Start[3], //[R.K. 01/2017] unused variable?
    Xend[2], Yend[2];

  PndTrkCTGeometryCalculations GeometryCalculator;

  // the first hit is the farthest from (0,0,0);
  // StrawCode convention (in the following left or right is looking to the beam from downstream) :
  //   -1 = not a boundary straw;
  //   10= inner axial boundary left;
  //   20= inner axial boundary right;
  //   12= outer VERTICAL (BUT NOT OUTERMOST) axial boundary left;
  //   22= outer VERTICAL (BUT NOT OUTERMOST)  axial boundary right;
  //   13= outermost axial boundary left;
  //   23= outermost axial boundary right;

  // the flags are 2 (StrawCode and StrawCode2) since a straw can belong to 2 boundaries;

  // first check if the first hit is required to be at the boundary; if so verify the condition;
  // only one hole is allowed;
  if (farthest_hit_is_boundary) {
    index = TubeID[ListHits[nHits - 1]]; //  number corresponding to the Straw of the last hit in the list;
    if (StrawCode[index - 1] == -1 && StrawCode2[index - 1] == -1) {
      // not at any boundary; before returning false check if one hole is still allowed and if so
      // check if any of the neighbours of the first hit, is at boundary : if so increment the
      //  number of holes and go on;
      if (holes >= MAX_NOT_CONNECTED)
        return false; // because we know already that holes becomes >= MAX_NOT_CONNECTED+1;
      yes = false;
      //  loop over the Straws contiguous to the current under scrutiny;
      for (i = 0; i < nParContiguous[index - 1]; i++) {
        if (StrawCode[ListParContiguous[index - 1][i] - 1] > -1 || StrawCode2[ListParContiguous[index - 1][i] - 1] > -1) {
          holes++;
          yes = true;
          break;
        }
      } // end of for(i=0;i<nHits-1;i++)
      if (!yes)
        return false; // none of the neighbouring Straws is at the boundary;

    } // end of  if( StrawCode[ index -1 ] == -1 && StrawCode2[ index -1 ] ==-1   )

  } // end of  if( farthest_hit_is_boundary )

  //---------------------------------------------------
  // now check if hits are contiguous going from the outermost to the innermost;
  for (i = 0; i < nHits - 1; i++) {
    outer = nHits - i - 1;
    inner = nHits - i - 2;
    //   	tube_current = TubeID[ ListHits[outer] ];
    //	tube_next = TubeID[ ListHits[inner] ];

    // distance squared between centers of the two straws;
    dist2 = (info[ListHits[outer]][0] - info[ListHits[inner]][0]) * (info[ListHits[outer]][0] - info[ListHits[inner]][0]);
    dist2 += (info[ListHits[outer]][1] - info[ListHits[inner]][1]) * (info[ListHits[outer]][1] - info[ListHits[inner]][1]);
    if (dist2 < DIAMETERSTRAWTUBE2 * 1.1)
      continue; // tubes are contiguous; the factor 1.1 just to be sure
    // against rounding errors ;

    // ----------------------------------------------------------------------------------------------------------
    //  case when tube_current and tube_next are not contiguous;

    // case in which there are only 1 entrance point and 1 exit point for the track (the most common one);
    // in this case all hits of the track are internal in this sector --> calculate the contiguity level
    // of the two hits under scrutiny;

    // next-to-contiguos hits;
    if (dist2 < 16. * STRAWRADIUS * STRAWRADIUS * 1.1) {
      // increase   holes  by 1 and then check if  holes>MAX_NOT_CONNECTED discard the track;
      holes++;
      if (holes > MAX_NOT_CONNECTED)
        return false;
      continue; // case accepted;
    } else {
      // distance between tube_current and tube_next >= 2 straws, discard the track;
      return false;
    }

  } // end of for(i=0;i<nHits;i++

  //-----------------------------------------------------------------------------------------------

  // check on the innermost hit (namely : ListHits[0]) ; it must be a boundary hit OR a next-to-boundary hit;

  index_last = TubeID[ListHits[0]] - 1;
  // it is at the boundary, therefore track is accepted;
  if (!(StrawCode[index_last] == -1 && StrawCode2[index_last] == -1)) {
    return true;
  }

  if (holes == MAX_NOT_CONNECTED) {
    // in this case the track must be rejected because the number of holes is > MAX_NOT_CONNECTED;
    return false;

  } else {
    // check if any of the neighbor straws, BELONGING TO THE TRACK AND IN THE RIGHT
    // ORDER (according to the Charge), is a boundary track;

    boundary = false;
    Xend[0] = Yend[0] = 0.; // the origin;
    Xend[1] = xTube[index_last];
    Yend[1] = yTube[index_last];
    for (j = 0; j < nParContiguous[index_last]; j++) {
      tube_next = ListParContiguous[index_last][j];

      // calculate the distance between the center of the trajectory and the center of the straw;
      // xxyyTube has been calculated (and passed trough various calling sequences) in
      // PndTrkTracking2.cxx
      dist2 = xxyyTube[tube_next - 1] - 2. * (xTube[tube_next - 1] * Ox + yTube[tube_next - 1] * Oy) + Ox * Ox + Oy * Oy;
      dist = fabs(sqrt(dist2) - R);
      if (dist > STRAWRADIUS * 1.5)
        continue; // tubes doesn't lie on trajectory; the factor 1.5 just to be sure

      // now check that the tube_next lies between (0,0) and the hit = ListHits[0] when running on the
      // trajectory according to the charge (namely : +ve --> clockwise, -ve --> anticlockwise);
      // the coordinates of the ends of the arc are given in Xend[2] and Yend[2];
      // fi is the angle (between 0. and 2 PI ) of the point under srutiny (==tube_next center);

      fi = atan2(yTube[tube_next - 1] - Oy, xTube[tube_next - 1] - Ox);
      if (fi < 0.)
        fi += 2. * PI;
      if (fi < 0.)
        fi = 0.;

      if (GeometryCalculator.IsInsideArc(Ox, Oy, Charge, Xend, Yend, fi)) {
        // check if this is at boundary;
        if (!(StrawCode[tube_next - 1] == -1 && StrawCode2[tube_next - 1] == -1)) {
          boundary = true;
          break;
        }
      } // end of  if( GeometryCalculator.IsInsideArc(Ox,Oy,Charge,Xend,Yend,f))

    } // end of  for(j=0;j<nParContiguous[ index_last ];j++)

    if (boundary) {
      holes++;
      return true;
    } else {
      return false;
    }

  } // end of   if(holes == MAX_NOT_CONNECTED)
}

//----------end of function PndTrkCleanup::GoodTrack

//----------begin of function PndTrkCleanup::IsThereMvdHitInBarrel

bool PndTrkCleanup::IsThereMvdHitInBarrel(Double_t Xintersect, // input, X position of the point of crossing as calculated from the track trajectory;
                                          Double_t Yintersect, // input, Y position of the point of crossing as calculated from the track trajectory;
                                          Double_t Zintersect, // input, Z position of the point of crossing as calculated from the track trajectory;

                                          Short_t nPixelHitsinTrack,        // number of Mvd Pixel hits in this track;
                                          Short_t *ListMvdPixelHitsinTrack, // ... and their list;
                                          Double_t *XMvdPixel,              // list of the X positions of ALL Mvd hits of the event;
                                          Double_t *YMvdPixel,              // list of the Y positions of ALL Mvd hits of the event;
                                          Double_t *ZMvdPixel,              // list of the Z positions of ALL Mvd hits of the event;
                                          Short_t nStripHitsinTrack,        // number of Mvd Strip hits in this track;
                                          Short_t *ListMvdStripHitsinTrack, // ... and their list;
                                          Double_t *XMvdStrip,              // list of the X positions of ALL Mvd hits of the event;
                                          Double_t *YMvdStrip,              // list of the Y positions of ALL Mvd hits of the event;
                                          Double_t *ZMvdStrip               // list of the Z positions of ALL Mvd hits of the event;
)
{
  // bool	at_least_one_good_hit; //[R.K. 01/2017] unused variable?

  Short_t // i, //[R.K. 01/2017] unused variable?
    j;

  const Double_t Ximprecision = 1., Yimprecision = 1., Zimprecision = 1.5;

  for (j = 0; j < nPixelHitsinTrack; j++) {

    if (fabs(XMvdPixel[ListMvdPixelHitsinTrack[j]] - Xintersect) < Ximprecision && fabs(YMvdPixel[ListMvdPixelHitsinTrack[j]] - Yintersect) < Yimprecision &&
        fabs(ZMvdPixel[ListMvdPixelHitsinTrack[j]] - Zintersect) < Zimprecision)
      return true;
  } // end of for(j=0;j<nPixelHitsinTrack;j++)

  for (j = 0; j < nStripHitsinTrack; j++) {

    if (fabs(XMvdStrip[ListMvdStripHitsinTrack[j]] - Xintersect) < Ximprecision && fabs(YMvdStrip[ListMvdStripHitsinTrack[j]] - Yintersect) < Yimprecision &&
        fabs(ZMvdStrip[ListMvdStripHitsinTrack[j]] - Zintersect) < Zimprecision)
      return true;
  } // end of for(j=0;j<nStripHitsinTrack;j++)

  return false;
}

//----------end of function PndTrkCleanup::IsThereMvdHitInBarrel

//----------begin of function PndTrkCleanup::IsThereMvdHitMiniDisk1_97to1_99

bool PndTrkCleanup::IsThereHitInMvdMiniDisk(Double_t ZLayerBegin,             // Z of the beginning of the layer (end of layer = + 0.02);
                                            Short_t nPixelHitsinTrack,        // number of Mvd Pixel hits in this track;
                                            Short_t *ListMvdPixelHitsinTrack, // ... and their list;
                                            Double_t *XMvdPixel, Double_t *YMvdPixel, Double_t *ZMvdPixel,

                                            Short_t nStripHitsinTrack,        // number of Mvd Strip hits in this track;
                                            Short_t *ListMvdStripHitsinTrack, // ... and their list;
                                            Double_t *XMvdStrip, Double_t *YMvdStrip, Double_t *ZMvdStrip,

                                            PndTrkCTGeometryCalculations *GeometryCalculator // pointer to
                                                                                             // the class doing the geometrical calculations;
)
{

  int i;

  // all the MvdMiniDisks sensitive layers begin at Z position ZLayerBegin and ends at ZLayerBegin+0.02;

  if (ZLayerBegin == 1.97) {

    // first the Mvd Pixel hits;

    for (i = 0; i < nPixelHitsinTrack; i++) {
      if (ZMvdPixel[ListMvdPixelHitsinTrack[i]] <= ZLayerBegin + 0.02 && ZMvdPixel[ListMvdPixelHitsinTrack[i]] >= ZLayerBegin) {

        if (GeometryCalculator->IsInMvdMiniDisk1_97to1_99(XMvdPixel[ListMvdPixelHitsinTrack[i]], YMvdPixel[ListMvdPixelHitsinTrack[i]]))
          return true;
      }
    } // end of  for(i=0;i<nPixelHitsinTrack;i++)

    // then the Mvd Strip hits;

    for (i = 0; i < nStripHitsinTrack; i++) {
      if (ZMvdStrip[ListMvdStripHitsinTrack[i]] <= ZLayerBegin + 0.02 && ZMvdStrip[ListMvdStripHitsinTrack[i]] >= ZLayerBegin) {

        if (GeometryCalculator->IsInMvdMiniDisk1_97to1_99(XMvdStrip[ListMvdStripHitsinTrack[i]], YMvdStrip[ListMvdStripHitsinTrack[i]]))
          return true;
      }
    } // end of  for(i=0;i<nPixelHitsinTrack;i++)

    return false;

  } else if (ZLayerBegin == 2.41) {

    // first the Mvd Pixel hits;

    for (i = 0; i < nPixelHitsinTrack; i++) {
      if (ZMvdPixel[ListMvdPixelHitsinTrack[i]] <= ZLayerBegin + 0.02 && ZMvdPixel[ListMvdPixelHitsinTrack[i]] >= ZLayerBegin) {

        if (GeometryCalculator->IsInMvdMiniDisk2_41to2_43(XMvdPixel[ListMvdPixelHitsinTrack[i]], YMvdPixel[ListMvdPixelHitsinTrack[i]]))
          return true;
      }
    } // end of  for(i=0;i<nPixelHitsinTrack;i++)

    // then the Mvd Strip hits;

    for (i = 0; i < nStripHitsinTrack; i++) {
      if (ZMvdStrip[ListMvdStripHitsinTrack[i]] <= ZLayerBegin + 0.02 && ZMvdStrip[ListMvdStripHitsinTrack[i]] >= ZLayerBegin) {

        if (GeometryCalculator->IsInMvdMiniDisk2_41to2_43(XMvdStrip[ListMvdStripHitsinTrack[i]], YMvdStrip[ListMvdStripHitsinTrack[i]]))
          return true;
      }
    } // end of  for(i=0;i<nPixelHitsinTrack;i++)

    return false;

  } else if (ZLayerBegin == 3.97) {

    // first the Mvd Pixel hits;

    for (i = 0; i < nPixelHitsinTrack; i++) {
      if (ZMvdPixel[ListMvdPixelHitsinTrack[i]] <= ZLayerBegin + 0.02 && ZMvdPixel[ListMvdPixelHitsinTrack[i]] >= ZLayerBegin) {

        if (GeometryCalculator->IsInMvdMiniDisk3_97to3_99(XMvdPixel[ListMvdPixelHitsinTrack[i]], YMvdPixel[ListMvdPixelHitsinTrack[i]]))
          return true;
      }
    } // end of  for(i=0;i<nPixelHitsinTrack;i++)

    // then the Mvd Strip hits;

    for (i = 0; i < nStripHitsinTrack; i++) {
      if (ZMvdStrip[ListMvdStripHitsinTrack[i]] <= ZLayerBegin + 0.02 && ZMvdStrip[ListMvdStripHitsinTrack[i]] >= ZLayerBegin) {

        if (GeometryCalculator->IsInMvdMiniDisk3_97to3_99(XMvdStrip[ListMvdStripHitsinTrack[i]], YMvdStrip[ListMvdStripHitsinTrack[i]]))
          return true;
      }
    } // end of  for(i=0;i<nPixelHitsinTrack;i++)

    return false;

  } else if (ZLayerBegin == 4.41) {

    // first the Mvd Pixel hits;

    for (i = 0; i < nPixelHitsinTrack; i++) {
      if (ZMvdPixel[ListMvdPixelHitsinTrack[i]] <= ZLayerBegin + 0.02 && ZMvdPixel[ListMvdPixelHitsinTrack[i]] >= ZLayerBegin) {

        if (GeometryCalculator->IsInMvdMiniDisk4_41to4_43(XMvdPixel[ListMvdPixelHitsinTrack[i]], YMvdPixel[ListMvdPixelHitsinTrack[i]]))
          return true;
      }
    } // end of  for(i=0;i<nPixelHitsinTrack;i++)

    // then the Mvd Strip hits;

    for (i = 0; i < nStripHitsinTrack; i++) {
      if (ZMvdStrip[ListMvdStripHitsinTrack[i]] <= ZLayerBegin + 0.02 && ZMvdStrip[ListMvdStripHitsinTrack[i]] >= ZLayerBegin) {

        if (GeometryCalculator->IsInMvdMiniDisk4_41to4_43(XMvdStrip[ListMvdStripHitsinTrack[i]], YMvdStrip[ListMvdStripHitsinTrack[i]]))
          return true;
      }
    } // end of  for(i=0;i<nPixelHitsinTrack;i++)

    return false;

  } else if (ZLayerBegin == 6.97) {

    // first the Mvd Pixel hits;

    for (i = 0; i < nPixelHitsinTrack; i++) {
      if (ZMvdPixel[ListMvdPixelHitsinTrack[i]] <= ZLayerBegin + 0.02 && ZMvdPixel[ListMvdPixelHitsinTrack[i]] >= ZLayerBegin) {

        if (GeometryCalculator->IsInMvdMiniDisk6_97to6_99(XMvdPixel[ListMvdPixelHitsinTrack[i]], YMvdPixel[ListMvdPixelHitsinTrack[i]]))
          return true;
      }
    } // end of  for(i=0;i<nPixelHitsinTrack;i++)

    // then the Mvd Strip hits;

    for (i = 0; i < nStripHitsinTrack; i++) {
      if (ZMvdStrip[ListMvdStripHitsinTrack[i]] <= ZLayerBegin + 0.02 && ZMvdStrip[ListMvdStripHitsinTrack[i]] >= ZLayerBegin) {

        if (GeometryCalculator->IsInMvdMiniDisk6_97to6_99(XMvdStrip[ListMvdStripHitsinTrack[i]], YMvdStrip[ListMvdStripHitsinTrack[i]]))
          return true;
      }
    } // end of  for(i=0;i<nPixelHitsinTrack;i++)

    return false;

  } else if (ZLayerBegin == 7.41) {

    // first the Mvd Pixel hits;

    for (i = 0; i < nPixelHitsinTrack; i++) {
      if (ZMvdPixel[ListMvdPixelHitsinTrack[i]] <= ZLayerBegin + 0.02 && ZMvdPixel[ListMvdPixelHitsinTrack[i]] >= ZLayerBegin) {

        if (GeometryCalculator->IsInMvdMiniDisk7_41to7_43(XMvdPixel[ListMvdPixelHitsinTrack[i]], YMvdPixel[ListMvdPixelHitsinTrack[i]]))
          return true;
      }
    } // end of  for(i=0;i<nPixelHitsinTrack;i++)

    // then the Mvd Strip hits;

    for (i = 0; i < nStripHitsinTrack; i++) {
      if (ZMvdStrip[ListMvdStripHitsinTrack[i]] <= ZLayerBegin + 0.02 && ZMvdStrip[ListMvdStripHitsinTrack[i]] >= ZLayerBegin) {

        if (GeometryCalculator->IsInMvdMiniDisk7_41to7_43(XMvdStrip[ListMvdStripHitsinTrack[i]], YMvdStrip[ListMvdStripHitsinTrack[i]]))
          return true;
      }
    } // end of  for(i=0;i<nPixelHitsinTrack;i++)

    return false;

  } else if (ZLayerBegin == 9.97) {

    // first the Mvd Pixel hits;

    for (i = 0; i < nPixelHitsinTrack; i++) {
      if (ZMvdPixel[ListMvdPixelHitsinTrack[i]] <= ZLayerBegin + 0.02 && ZMvdPixel[ListMvdPixelHitsinTrack[i]] >= ZLayerBegin) {

        if (GeometryCalculator->IsInMvdMiniDisk9_97to9_99(XMvdPixel[ListMvdPixelHitsinTrack[i]], YMvdPixel[ListMvdPixelHitsinTrack[i]]))
          return true;
      }
    } // end of  for(i=0;i<nPixelHitsinTrack;i++)

    // then the Mvd Strip hits;

    for (i = 0; i < nStripHitsinTrack; i++) {
      if (ZMvdStrip[ListMvdStripHitsinTrack[i]] <= ZLayerBegin + 0.02 && ZMvdStrip[ListMvdStripHitsinTrack[i]] >= ZLayerBegin) {

        if (GeometryCalculator->IsInMvdMiniDisk9_97to9_99(XMvdStrip[ListMvdStripHitsinTrack[i]], YMvdStrip[ListMvdStripHitsinTrack[i]]))
          return true;
      }
    } // end of  for(i=0;i<nPixelHitsinTrack;i++)

    return false;

  } else if (ZLayerBegin == 10.41) {

    // first the Mvd Pixel hits;

    for (i = 0; i < nPixelHitsinTrack; i++) {
      if (ZMvdPixel[ListMvdPixelHitsinTrack[i]] <= ZLayerBegin + 0.02 && ZMvdPixel[ListMvdPixelHitsinTrack[i]] >= ZLayerBegin) {

        if (GeometryCalculator->IsInMvdMiniDisk10_41to10_43(XMvdPixel[ListMvdPixelHitsinTrack[i]], YMvdPixel[ListMvdPixelHitsinTrack[i]]))
          return true;
      }
    } // end of  for(i=0;i<nPixelHitsinTrack;i++)

    // then the Mvd Strip hits;

    for (i = 0; i < nStripHitsinTrack; i++) {
      if (ZMvdStrip[ListMvdStripHitsinTrack[i]] <= ZLayerBegin + 0.02 && ZMvdStrip[ListMvdStripHitsinTrack[i]] >= ZLayerBegin) {

        if (GeometryCalculator->IsInMvdMiniDisk10_41to10_43(XMvdStrip[ListMvdStripHitsinTrack[i]], YMvdStrip[ListMvdStripHitsinTrack[i]]))
          return true;
      }
    } // end of  for(i=0;i<nPixelHitsinTrack;i++)

    return false;

  } else if (ZLayerBegin == 14.77) {

    // first the Mvd Pixel hits;

    for (i = 0; i < nPixelHitsinTrack; i++) {
      if (ZMvdPixel[ListMvdPixelHitsinTrack[i]] <= ZLayerBegin + 0.02 && ZMvdPixel[ListMvdPixelHitsinTrack[i]] >= ZLayerBegin) {

        if (GeometryCalculator->IsInMvdMiniDisk14_77to14_79(XMvdPixel[ListMvdPixelHitsinTrack[i]], YMvdPixel[ListMvdPixelHitsinTrack[i]]))
          return true;
      }
    } // end of  for(i=0;i<nPixelHitsinTrack;i++)

    // then the Mvd Strip hits;

    for (i = 0; i < nStripHitsinTrack; i++) {
      if (ZMvdStrip[ListMvdStripHitsinTrack[i]] <= ZLayerBegin + 0.02 && ZMvdStrip[ListMvdStripHitsinTrack[i]] >= ZLayerBegin) {

        if (GeometryCalculator->IsInMvdMiniDisk14_77to14_79(XMvdStrip[ListMvdStripHitsinTrack[i]], YMvdStrip[ListMvdStripHitsinTrack[i]]))
          return true;
      }
    } // end of  for(i=0;i<nPixelHitsinTrack;i++)

    return false;

  } else if (ZLayerBegin == 15.21) {

    // first the Mvd Pixel hits;

    for (i = 0; i < nPixelHitsinTrack; i++) {
      if (ZMvdPixel[ListMvdPixelHitsinTrack[i]] <= ZLayerBegin + 0.02 && ZMvdPixel[ListMvdPixelHitsinTrack[i]] >= ZLayerBegin) {

        if (GeometryCalculator->IsInMvdMiniDisk15_21to15_23(XMvdPixel[ListMvdPixelHitsinTrack[i]], YMvdPixel[ListMvdPixelHitsinTrack[i]]))
          return true;
      }
    } // end of  for(i=0;i<nPixelHitsinTrack;i++)

    // then the Mvd Strip hits;

    for (i = 0; i < nStripHitsinTrack; i++) {
      if (ZMvdStrip[ListMvdStripHitsinTrack[i]] <= ZLayerBegin + 0.02 && ZMvdStrip[ListMvdStripHitsinTrack[i]] >= ZLayerBegin) {

        if (GeometryCalculator->IsInMvdMiniDisk15_21to15_23(XMvdStrip[ListMvdStripHitsinTrack[i]], YMvdStrip[ListMvdStripHitsinTrack[i]]))
          return true;
      }
    } // end of  for(i=0;i<nPixelHitsinTrack;i++)

    return false;

  } else if (ZLayerBegin == 21.77) {

    // first the Mvd Pixel hits;

    for (i = 0; i < nPixelHitsinTrack; i++) {
      if (ZMvdPixel[ListMvdPixelHitsinTrack[i]] <= ZLayerBegin + 0.02 && ZMvdPixel[ListMvdPixelHitsinTrack[i]] >= ZLayerBegin) {

        if (GeometryCalculator->IsInMvdMiniDisk21_77to21_79(XMvdPixel[ListMvdPixelHitsinTrack[i]], YMvdPixel[ListMvdPixelHitsinTrack[i]]))
          return true;
      }
    } // end of  for(i=0;i<nPixelHitsinTrack;i++)

    // then the Mvd Strip hits;

    for (i = 0; i < nStripHitsinTrack; i++) {
      if (ZMvdStrip[ListMvdStripHitsinTrack[i]] <= ZLayerBegin + 0.02 && ZMvdStrip[ListMvdStripHitsinTrack[i]] >= ZLayerBegin) {

        if (GeometryCalculator->IsInMvdMiniDisk21_77to21_79(XMvdStrip[ListMvdStripHitsinTrack[i]], YMvdStrip[ListMvdStripHitsinTrack[i]]))
          return true;
      }
    } // end of  for(i=0;i<nPixelHitsinTrack;i++)

    return false;

  } else if (ZLayerBegin == 22.21) {

    // first the Mvd Pixel hits;

    for (i = 0; i < nPixelHitsinTrack; i++) {
      if (ZMvdPixel[ListMvdPixelHitsinTrack[i]] <= ZLayerBegin + 0.02 && ZMvdPixel[ListMvdPixelHitsinTrack[i]] >= ZLayerBegin) {

        if (GeometryCalculator->IsInMvdMiniDisk22_21to22_23(XMvdPixel[ListMvdPixelHitsinTrack[i]], YMvdPixel[ListMvdPixelHitsinTrack[i]]))
          return true;
      }
    } // end of  for(i=0;i<nPixelHitsinTrack;i++)

    // then the Mvd Strip hits;

    for (i = 0; i < nStripHitsinTrack; i++) {
      if (ZMvdStrip[ListMvdStripHitsinTrack[i]] <= ZLayerBegin + 0.02 && ZMvdStrip[ListMvdStripHitsinTrack[i]] >= ZLayerBegin) {

        if (GeometryCalculator->IsInMvdMiniDisk22_21to22_23(XMvdStrip[ListMvdStripHitsinTrack[i]], YMvdStrip[ListMvdStripHitsinTrack[i]]))
          return true;
      }
    } // end of  for(i=0;i<nPixelHitsinTrack;i++)

    return false;

  } else {
    cout << "PndTrkCleanup.cxx::IsThereHitInMvdMiniDisk    WARNING, this Mvd MiniDisk apparently"
         << " is not in the list of known Mvd MiniDisks !";
  }

  return false; // FIXME Is this logically correct?
}

//----------end of function PndTrkCleanup::IsThereMvdHitInMiniDisk1_97to1_99

//----------begin of function PndTrkCleanup::MvdCleanup

bool PndTrkCleanup::MvdCleanup(Double_t Ox, Double_t Oy, Double_t R, Double_t fi0, Double_t kappa, Double_t charge,
                               Double_t *XMvdPixel,       // list of the X positions of ALL Mvd hits of the event;
                               Double_t *XMvdStrip,       // list of the X positions of ALL Mvd hits of the event;
                               Double_t *YMvdPixel,       // list of the Y positions of ALL Mvd hits of the event;
                               Double_t *YMvdStrip,       // list of the Y positions of ALL Mvd hits of the event;
                               Double_t *ZMvdPixel,       // list of the Z positions of ALL Mvd hits of the event;
                               Double_t *ZMvdStrip,       // list of the Z positions of ALL Mvd hits of the event;
                               Short_t nPixelHitsinTrack, // number of Mvd Pixel hits in this track;
                               Short_t *ListMvdPixelHitsinTrack,
                               Short_t nStripHitsinTrack, // number of Mvd Strip hits in this track;
                               Short_t *ListMvdStripHitsinTrack, Double_t extra_distance, Double_t extra_distance_Z, PndTrkCTGeometryCalculations *GeomCalculator)
{

  /*
    Double_t Ox  -->  X of center of the Helix of the particle trajectory;
    Double_t Oy  -->  Y of center of the Helix of the particle trajectory;
    Double_t R,  -->  radius of the Helix of the particle trajectory;
    Double_t fi0 -->  FI0 of the Helix of the particle trajectory;
    Double_t kappa  -->  KAPPA of the Helix of the particle trajectory;
    Double_t charge -->  charge of the particle;
    Double_t semiverticalgap -->  half dimension of the gap between the two STT sectors;
    Short_t nMvdHits -->  number of Mvd hits in this track;
    Double_t extra_distance --> in cm; extra distance (in X and Y) allowed during the process to decide whether there
          should be an hit in a Mvd sensitive layer;
    Double_t extra_distance_Z --> in cm; extra distance in Z allowed during the process to decide whether there
          should be an hit in a Mvd sensitive layer;
    PndTrkCTGeometryCalculations* GeomCalculator  -->  class that makes the geometrical calculations;
  */

  bool yes_hit;

  Short_t i, j, nFaults, n_forseen_hits, n_present_hits,
    type_of_intersection_in_disk[MVD_DISK_LAYERS] // = +1 --> track completely in the sensors;
                                                  //  = 0 --> uncertain; = -1 --> out of the sensor;
    //,yes_intersect //[R.K. 01/2017] unused variable?
    ;
  Double_t
    // FiOrderedList[2], //[R.K. 01/2017] unused variable?
    phase,
    r, rmax, rmin,
    // r2, //[R.K. 01/2017] unused variable?
    // Xcross[2], //[R.K. 01/2017] unused variable?
    X_disk, Xintersect[2],
    // Xlow, //[R.K.02/2017] Unused variable?
    // Xup, //[R.K.02/2017] Unused variable?
    // Ycross[2], //[R.K. 01/2017] unused variable?
    Y_disk, Yintersect[2],
    // Ylow, //[R.K. 01/2017] unused variable?
    // Yup, //[R.K. 01/2017] unused variable?
    // Z_disk, //[R.K. 01/2017] unused variable?
    Zintersect[2];

  PndTrkCTGeometryCalculations GeometryCalculator;

  // total Mvd hits that are supposed to be in this track (if the parameter of the track were totally right);
  n_forseen_hits = 0;
  // number of 'right' hits that are present in this track (belonging to the predicted Mvd layers);
  n_present_hits = 0;

  //------------------------------------------------------- MVD BARREL ----------------------------------------------------------

  //  check of the barrels with full azimuthal coverage ; ----------------------------------------------------

  // loop over the number of barrel Mvd layers with full azimuthal coverage;
  // calculate if trajectory intersects such Mvd barrel layers;

  for (i = 0; i < MVD_BARREL_LAYERS_FULL_AZIMUTH; i++) {

    // yes_hit = true --> at least one Mvd hit from this barrel; yes_hit = false --> no Mvd hits from this barrel;

    yes_hit = Track_Crosses_MvdBarrelFullAzimuthalCoverage(Ox,     // track trajectory center;
                                                           Oy,     // track trajectory center;
                                                           R,      // track trajectory radius;
                                                           fi0,    // track trajectory starting FI;
                                                           kappa,  // track trajectory Kappa parameter;
                                                           charge, // track charge;

                                                           MVD_BARREL_FULL_AZIMUTH_Z_LOW[i],      // Z low limit of this barrel;
                                                           MVD_BARREL_FULL_AZIMUTH_Z_UP[i],       // Z upper limit of this barrel;
                                                           MVD_BARREL_FULL_AZIMUTH_MAX_RADIUS[i], // R maximum of this barrel;
                                                           GeomCalculator,                        // pointer to the class making useful geometry
                                                                                                  // calculation;
                                                           extra_distance_Z,                      // in cm; extra distance allowed during decision
                                                                                                  // if there should be an hit in a Mvd sensitive layer;
                                                           Xintersect[0],                         // output, X position of the point of crossing;
                                                           Yintersect[0],                         // output, Y position of the point of crossing;
                                                           Zintersect[0]                          // output, Z position of the point of crossing;

    );
    // verify if actually there is a hit in this MVD barrel;

    if (yes_hit) {
      n_forseen_hits++;
      if (IsThereMvdHitInBarrel(Xintersect[0], Yintersect[0], Zintersect[0],

                                nPixelHitsinTrack, ListMvdPixelHitsinTrack,
                                ZMvdPixel,         // list of the X positions of ALL Mvd hits of the event;
                                YMvdPixel,         // list of the Y positions of ALL Mvd hits of the event;
                                ZMvdPixel,         // list of the Z positions of ALL Mvd hits of the event;
                                nStripHitsinTrack, // number of Mvd Strip hits in this track;
                                ListMvdStripHitsinTrack,
                                XMvdStrip, // list of the X positions of ALL Mvd hits of the event;
                                YMvdStrip, // list of the Y positions of ALL Mvd hits of the event;
                                ZMvdStrip  // list of the Z positions of ALL Mvd hits of the event;
                                ))
        n_present_hits++;

    } // end of if( yes_hit)

  } // end of  for(i=0;i<MVD_BARREL_LAYERS_FULL_AZIMUTH;i++)

  // check on the Barrel Mvd sector; allow only for one hit mismatch;

  nFaults = n_forseen_hits - n_present_hits;
  if (nFaults > 1)
    return false;

  //  end of check of the barrels with full azimuthal coverage ; ---------------------------------------------

  //-------------------------------------------start the check of the barrels with partial azimuthal coverage ;

  // loop over the number of barrel Mvd layers with partial azimuthal coverage;
  // calculate if trajectory intersects such Mvd barrel layers;
  for (i = 0; i < MVD_BARREL_LAYERS_PARTIAL_AZIMUTH; i++) {

    // yes_hit = true --> at least one Mvd hit from this barrel; yes_hit = false --> no Mvd hits from this barrel;
    yes_hit = Track_Crosses_MvdBarrelPartialAzimuthalCoverage(Ox,     // track trajectory center;
                                                              Oy,     // track trajectory center;
                                                              R,      // track trajectory radius;
                                                              fi0,    // track trajectory starting FI;
                                                              kappa,  // track trajectory Kappa parameter;
                                                              charge, // track charge;

                                                              MVD_BARREL_PARTIAL_AZIMUTH_Z_LOW[i], // Z low limit of this barrel;
                                                              MVD_BARREL_PARTIAL_AZIMUTH_Z_UP[i],  // Z upper limit of this barrel;

                                                              MVD_BARREL_PARTIAL_AZIMUTH_RADIUS_INNER[i],      // R of this barrel;
                                                              MVD_BARREL_PARTIAL_AZIMUTH_NGAP[i][0],           // number of gaps in azimuthal angle coverage of INNER;
                                                              &MVD_BARREL_PARTIAL_AZIMUTH_GAP_LOW_INNER[i][0], // low limit of the azimuthal gap range;
                                                              &MVD_BARREL_PARTIAL_AZIMUTH_GAP_UP_INNER[i][0],  // upper limit of the azimuthal gap range;

                                                              MVD_BARREL_PARTIAL_AZIMUTH_RADIUS_OUTER[i],      // R of this barrel;
                                                              MVD_BARREL_PARTIAL_AZIMUTH_NGAP[i][1],           // number of gaps in azimuthal angle coverage og OUTER;
                                                              &MVD_BARREL_PARTIAL_AZIMUTH_GAP_LOW_OUTER[i][0], // low limit of the azimuthal gap range;
                                                              &MVD_BARREL_PARTIAL_AZIMUTH_GAP_UP_OUTER[i][0],  // upper limit of the azimuthal gap range;

                                                              GeomCalculator,   // pointer to the class making useful geometry
                                                                                // calculation;
                                                              extra_distance_Z, // in cm; extra distance allowed during decision
                                                                                // if there should be an hit in a Mvd sensitive layer;
                                                              Xintersect,       // output, X position of the point of crossing track-Inner Barrel
                                                                                // and track-Outer Barrel;
                                                              Yintersect,       // output, Y position of the point of crossing;
                                                              Zintersect        // output, Z position of the point of crossing;
    );

    if (yes_hit) {
      // loop over the at most 2 intersections (one with the Inner Barrel, one with the Outer Barrel);
      for (j = 0; j < 2; j++) {

        // when there is no intersection between track and Barrel
        // Xintersect[j] is set at -99999. ;
        if (Xintersect[j] < -99998.)
          continue;

        // there was intersection between this track and this Barrel;
        n_forseen_hits++;
        // check if there is actually a hit in this Mvd Barrel (Inner or Outer);
        if (IsThereMvdHitInBarrel(Xintersect[j], Yintersect[j], Zintersect[j],

                                  nPixelHitsinTrack, ListMvdPixelHitsinTrack,
                                  XMvdPixel,         // list of the X positions of ALL Mvd hits of the event;
                                  YMvdPixel,         // list of the Y positions of ALL Mvd hits of the event;
                                  ZMvdPixel,         // list of the Z positions of ALL Mvd hits of the event;
                                  nStripHitsinTrack, // number of Mvd Strip hits in this track;
                                  ListMvdStripHitsinTrack,
                                  XMvdStrip, // list of the X positions of ALL Mvd hits of the event;
                                  YMvdStrip, // list of the Y positions of ALL Mvd hits of the event;
                                  ZMvdStrip  // list of the Z positions of ALL Mvd hits of the event;
                                  ))
          n_present_hits++;

      } //   end of for(j=0; j<2; j++)

    } // end of if(yes_hit){

  } // end of  for(i=0;i<MVD_BARREL_LAYERS_PARTIAL_AZIMUTH;i++)

  // check on the Barrel Mvd sector; allow only for one hit mismatch;
  nFaults = n_forseen_hits - n_present_hits;
  if (nFaults > 1)
    return false;

  //------------------------------------------------------- END OF MVD BARREL --------------------------------------------------

  //------------------------------------------------------- MVD MINIDISK LAYERS ------------------------------------------------

  //  loop over all Mvd Minidisks ;

  for (i = 0; i < MVD_MINIDISK_LAYERS; i++) {

    if (Track_Crosses_MvdMiniDisk_withMargin(MVD_Z_LAYER_BEGIN[i], // Z of the beginning of the layer (end of layer = + 0.02);
                                             0.5,                  // xmargin;
                                             0.5,                  // ymargin;
                                             Ox,                   // track trajectory center;
                                             Oy,                   // track trajectory center;
                                             R,                    // track trajectory radius;
                                             fi0,                  // FI0 of the Helix of the particle trajectory;
                                             kappa,                // KAPPA of the Helix of the particle trajectory;
                                             charge,               // charge of the particle;
                                             GeomCalculator        // pointer to the class making useful geometry
                                                                   // calculation;
                                             )) {
      yes_hit = IsThereHitInMvdMiniDisk(MVD_Z_LAYER_BEGIN[i], // Z of the beginning of the layer (end of layer = + 0.02);
                                        nPixelHitsinTrack, ListMvdPixelHitsinTrack, XMvdPixel, YMvdPixel, ZMvdPixel,

                                        nStripHitsinTrack, ListMvdStripHitsinTrack, XMvdStrip, YMvdStrip, ZMvdStrip,

                                        GeomCalculator);

      if (!yes_hit) {
        nFaults++;
      }
    }
    if (nFaults > 1)
      return false;

  } // end of  for(i=0;i<MVD_MINIDISK_LAYERS;i++)

  //------------------------------------------------------- END OF MVD MINIDISK LAYERS -----------------------------------------

  //------------------------------------------------------- MVD DISKS ----------------------------------------------------------

  // loop over the number of Disks Mvd;
  // calculate if trajectory intersects the Mvd Disk layers;
  for (i = 0; i < MVD_DISK_LAYERS; i++) {

    // calculate the intersections on the Mvd Disks;
    phase = fi0 + kappa * MVD_DISK_Z[i];
    X_disk = Ox + R * cos(phase);
    // Xup = X_disk + extra_distance; //[R.K.02/2017] Unused variable?
    // Xlow = X_disk - extra_distance; //[R.K.02/2017] Unused variable?
    Y_disk = Oy + R * sin(phase);
    // Ylow = Y_disk - extra_distance; //[R.K.02/2017] Unused variable?
    // Yup = Y_disk + extra_distance; //[R.K.02/2017] Unused variable?

    // now calculate if the intersection falls in the sensor active region of the Mvd Disk;
    // theoretical radius**2 of the intersection point;
    r = sqrt(X_disk * X_disk + Y_disk * Y_disk);
    // conservative maximum possible radius**2 of the intersection point;
    rmax = r + extra_distance;
    // conservative minimum possible radius**2 of the intersection point;
    rmin = r - extra_distance;
    if (rmin < 0.)
      rmin = 0.;

    if (rmax < MVD_DISK_MAX_RADIUS[i] && rmin > MVD_DISK_MIN_RADIUS[i]) { // certainly in;
      type_of_intersection_in_disk[i] = 1;
    } else if (rmin > MVD_DISK_MAX_RADIUS[i] || rmax < MVD_DISK_MIN_RADIUS[i]) {
      type_of_intersection_in_disk[i] = -1; // certainly out;
    } else {
      type_of_intersection_in_disk[i] = 0; // partly in;
    }

  }; // end of  for(i=0;i<MVD_DISKS_LAYERS;i++)

  //  check if there are the hits in the layer predicted by the previous extrapolation of the track;
  // nFaults can be 0 or 1, depending on the analysis of the Barrel layers and Minidisk layers;

  for (i = 0; i < MVD_DISK_LAYERS; i++) {
    if (type_of_intersection_in_disk[i] == 1) {

      yes_hit = false;

      // loop over all Mvd hits of the track;
      for (j = 0; j < nPixelHitsinTrack; j++) {
        if (fabs(ZMvdPixel[ListMvdPixelHitsinTrack[j]] - MVD_DISK_Z[i]) < 1.) {
          yes_hit = true;
          break;
        }
      } // end of for(j=0;j< nPixelHitsinTrack; j++)

      if (!yes_hit) {
        for (j = 0; j < nStripHitsinTrack; j++) {
          if (fabs(ZMvdStrip[ListMvdStripHitsinTrack[j]] - MVD_DISK_Z[i]) < 1.) {
            yes_hit = true;
            break;
          }
        } // end of for(j=0;j< nStripHitsinTrack; j++)
      }   // end of if(!yes_hit)

      // if( yes_hit) cout<<" true "<<endl ; else cout<<" false "<<endl ;

      if (!yes_hit) {
        nFaults++;
        if (nFaults > 1)
          return false;
      }

    } // end of if (type_of_intersection_in_disk[i]==1)

  }; // end of  for(i=0;i<MVD_DISKS_LAYERS;i++)

  return true;
}
//----------end of function PndTrkCleanup::MvdCleanup

//----------begin of function PndTrkCleanup::MvdCleanup_prova

bool PndTrkCleanup::MvdCleanup_prova(Double_t Ox, Double_t Oy, Double_t R, Double_t fi0, Double_t kappa, Double_t charge, Double_t semiverticalgap, Short_t nMvdHits,
                                     PndTrkCTGeometryCalculations *GeomCalculator)
{

  if (!GeomCalculator->IsInTargetPipe(Ox, Oy, R, fi0, kappa, charge, semiverticalgap) && nMvdHits == 0)
    return false;
  return true;
}
//----------end of function PndTrkCleanup::MvdCleanup_prova

//----------begin of function PndTrkCleanup::SeparateInnerOuterParallel

void PndTrkCleanup::SeparateInnerOuterParallel(

  // input
  Short_t nHits, Short_t *ListHits, Double_t info[][7], Double_t R_STT_INNER_PAR_MAX,

  // output
  Short_t *nInnerHits, Short_t *ListInnerHits, Short_t *nOuterHits, Short_t *ListOuterHits,

  Short_t *nInnerHitsLeft, Short_t *ListInnerHitsLeft, Short_t *nInnerHitsRight, Short_t *ListInnerHitsRight,

  Short_t *nOuterHitsLeft, Short_t *ListOuterHitsLeft, Short_t *nOuterHitsRight, Short_t *ListOuterHitsRight)
{

  Short_t ihit;

  Double_t r;

  //   separation of inner Parallel Stt hits from outer Parallel Stt hits.

  *nInnerHits = 0;
  *nInnerHitsLeft = 0;
  *nInnerHitsRight = 0;
  *nOuterHits = 0;
  *nOuterHitsLeft = 0;
  *nOuterHitsRight = 0;
  for (ihit = 0; ihit < nHits; ihit++) {
    r = sqrt(info[ListHits[ihit]][0] * info[ListHits[ihit]][0] + info[ListHits[ihit]][1] * info[ListHits[ihit]][1]);
    // the value 2.*RStrawDetectorParMax/sqrt(3.) is because RStrawDetectorParMax
    // is an Apotema !
    if (r > R_STT_INNER_PAR_MAX) { // outer Parallel hit.
      ListOuterHits[*nOuterHits] = ListHits[ihit];
      (*nOuterHits)++;
      if (info[ListHits[ihit]][0] < 0.) {
        ListOuterHitsLeft[*nOuterHitsLeft] = ListHits[ihit];
        (*nOuterHitsLeft)++;
      } else {
        ListOuterHitsRight[*nOuterHitsRight] = ListHits[ihit];
        (*nOuterHitsRight)++;
      }
    } else {
      ListInnerHits[*nInnerHits] = ListHits[ihit];
      (*nInnerHits)++;
      if (info[ListHits[ihit]][0] < 0.) {
        ListInnerHitsLeft[*nInnerHitsLeft] = ListHits[ihit];
        (*nInnerHitsLeft)++;
      } else {
        ListInnerHitsRight[*nInnerHitsRight] = ListHits[ihit];
        (*nInnerHitsRight)++;
      }
    }
  }
}

//----------end of function PndTrkCleanup::SeparateInnerOuterParallel

//----------begin of function PndTrkCleanup::SeparateInnerOuterRightLeftAxialStt

void PndTrkCleanup::SeparateInnerOuterRightLeftAxialStt(

  // input
  Double_t info[][7], Short_t *ListHits, Short_t nHits, Double_t R_STT_INNER_PAR_MAX,

  // output

  Short_t *ListInnerHitsLeft, Short_t *ListInnerHitsRight, Short_t *ListOuterHitsLeft, Short_t *ListOuterHitsRight, Short_t *nInnerHitsLeft, Short_t *nInnerHitsRight,
  Short_t *nOuterHitsLeft, Short_t *nOuterHitsRight)
{

  Short_t ihit;

  Double_t r;

  //   separation of inner Parallel Stt hits from outer Parallel Stt hits.

  *nInnerHitsLeft = 0;
  *nInnerHitsRight = 0;
  *nOuterHitsLeft = 0;
  *nOuterHitsRight = 0;
  for (ihit = 0; ihit < nHits; ihit++) {
    r = sqrt(info[ListHits[ihit]][0] * info[ListHits[ihit]][0] + info[ListHits[ihit]][1] * info[ListHits[ihit]][1]);
    // the value 2.*RStrawDetectorParMax/sqrt(3.) is because RStrawDetectorParMax
    // is an Apotema !
    if (r > R_STT_INNER_PAR_MAX) { // outer Parallel hit.
      if (info[ListHits[ihit]][0] < 0.) {
        ListOuterHitsLeft[*nOuterHitsLeft] = ListHits[ihit];
        (*nOuterHitsLeft)++;
      } else {
        ListOuterHitsRight[*nOuterHitsRight] = ListHits[ihit];
        (*nOuterHitsRight)++;
      }
    } else {
      if (info[ListHits[ihit]][0] < 0.) {
        ListInnerHitsLeft[*nInnerHitsLeft] = ListHits[ihit];
        (*nInnerHitsLeft)++;
      } else {
        ListInnerHitsRight[*nInnerHitsRight] = ListHits[ihit];
        (*nInnerHitsRight)++;
      }
    } // end of if(r>R_STT_INNER_PAR_MAX )
  }   // end of for(ihit=0  ;ihit<nHits;ihit++)

  return;
}

//----------end of function PndTrkCleanup::SeparateInnerOuterRightLeftAxialStt

//----------begin of function PndTrkCleanup::ParalCleanup

bool PndTrkCleanup::SttParalCleanup(Double_t ApotemaInnerParMax, Double_t ApotemaMinOuterPar, Short_t Charge, Double_t FI0, Double_t FiLimitAdmissible, Double_t GAP,
                                    Double_t info[][7], int istampa, int IVOLTE, Short_t *Listofhits, Short_t nHits, Double_t Oxx, Double_t Oyy, Double_t Rr,
                                    Double_t RStrawDetMax, // radius of circle encompassing ALL
                                                           // the straw detector;
                                    Double_t RStrawDetMin, Double_t Start[3], Double_t strawradius)
{

  // this method does 3 things :
  //
  //	1)  finds the entrance and exit points in the STT parallel volumes of the current track;
  //	2)  eliminates from the track hit list possible spurious hits that are not encompassed
  //		by the entrance and exit point;
  //	3)  eliminates the tracks if the hit sequence is not continuous enough.

  bool flaggo = true; // when flaggo is true it means that the track CROSSED the
                      // STT axial layer(s); flaggo can be falsified later in this method;

  Short_t flagInnerSttL, flagInnerSttR, flagOuterSttL, flagOuterSttR, flagOutStt, i, ipurged, islack, nintersections, nConsideredHits, nInnerHits, nInnerHitsLeft, nInnerHitsRight,
    nOuterHits, nOuterHitsLeft, nOuterHitsRight, ListHits[nHits], ListInnerHits[nHits], ListInnerHitsLeft[nHits], ListInnerHitsRight[nHits], ListOuterHits[nHits],
    ListOuterHitsLeft[nHits], ListOuterHitsRight[nHits];

  Double_t epsilonTheta, fi, LimitCoord[2], Xcross[2], Ycross[2], XcrossL[2], YcrossL[2], XcrossR[2], YcrossR[2], XcrossOut[2], YcrossOut[2],
    XintersectionList[7], // there is also the last boundary FiLimitAdmissible
    YintersectionList[7]; // take into account and the two possible
                          // intersections with the external circle.

  islack = 1; // uncertainty allowed in the # of straws that should be hit in a given part
              // of the Stt detector.

  // calculation of the Limit X Y coordinates, corresponding to the Limiting angle FiLimitAdmissible,
  // used later in some circumstances;

  LimitCoord[0] = Oxx + Rr * cos(FiLimitAdmissible);
  LimitCoord[1] = Oyy + Rr * sin(FiLimitAdmissible);

  //-------------------------------------------------------------------------------------------
  //  elimination of hits outside the physical FI range (FiLimitAdmissible). The physical
  //  FI range depends on Pz and Pt of the track : given the Pz of the track, the azimuthal angle
  //  covered by te Helix of the trajectory may very well be less than 2 pi radians.

  epsilonTheta = strawradius / Rr; // some extra slac for being conservative.

  for (i = 0, ipurged = 0; i < nHits; i++) {

    fi = atan2(info[Listofhits[i]][1] - Oyy, info[Listofhits[i]][0] - Oxx);
    if (fi < 0.)
      fi += 2. * PI;

    if (Charge < 0) {
      if (fi > FI0) {
        if (fi > FiLimitAdmissible + epsilonTheta)
          continue;
      } else {
        fi += 2. * PI;
        if (fi > FiLimitAdmissible + epsilonTheta)
          continue;
      }      // end of  if( fi > FI0)
    } else { // continuation of  if(Charge <0)
      if (fi > FI0) {
        fi -= 2. * PI;
      } // end of  if( fi > FI0)
      if (fi < FiLimitAdmissible - epsilonTheta)
        continue;
    } // end of if(Charge <0)

    ListHits[ipurged] = Listofhits[i];
    ipurged++;
  } // end of    for(i=0, ipurged=0; i< nHits; i++)

  // nHits is the final outcome of the elimination; it is the # of remaining hits;
  nHits = ipurged;

  if (nHits == 0) {
    // if the remaining hits is 0, don't discard track yet : maybe that
    // its particular trajectory is such that it doesn't cross any axial
    // straws (this doesn't prevent the Pattern Recognition - which requires
    // at least
    nInnerHits = 0;
    nInnerHitsRight = 0;
    nInnerHitsLeft = 0;
    nOuterHits = 0;
    nOuterHitsRight = 0;
    nOuterHitsLeft = 0;
    // don't discard track yet, see if it should have parallel hits.

  } else {

    //------------------
    //   separation of inner Parallel Stt hits from outer Parallel Stt hits.

    SeparateInnerOuterParallel(

      // input
      nHits, ListHits, info, 2. * ApotemaInnerParMax / sqrt(3.),

      // output
      &nInnerHits, ListInnerHits, &nOuterHits, ListOuterHits,

      &nInnerHitsLeft, ListInnerHitsLeft, &nInnerHitsRight, ListInnerHitsRight,

      &nOuterHitsLeft, ListOuterHitsLeft, &nOuterHitsRight, ListOuterHitsRight);

  } // end of if(nHits==0)

  //--------------------------------------------------------------------------
  //  class with all the geometry calculations:
  PndTrkCTGeometryCalculations GeometryCalculator;

  //-------------------------- intersection points with outer circle encompassing
  //	the Stt system.

  // flag meaning :
  // -1 -->  track does NOT intersect the Outer Circle (which has radius RStrawDetMax);
  // 0 -->  2 intersections;

  flagOutStt = GeometryCalculator.FindIntersectionsOuterCircle(Oxx, Oyy, Rr, RStrawDetMax, XcrossOut, YcrossOut);

  //-----------------------------------------intersection with Inner Section.
  // flag meaning :
  // -1 -->  track outside outer perimeter OR less than 2 intersections;
  // 0 -->  at least 2 intersection with polygon, therefore a possible entry and an exit;
  // 1 -->  track contained completely between the two polygons : it should be
  //  impossible for a track coming really from (0,0,0);

  flagInnerSttL = GeometryCalculator.FindTrackEntranceExitbiHexagonLeft(GAP, Oxx, Oyy, Rr, Charge, Start, RStrawDetMin, ApotemaInnerParMax, XcrossL, YcrossL);
  // find the entrance and exit of the track in the Inner Right Parallel Straw region.
  // This region is bounded by two Hexagons, and it has the target gap in the middle.

  flagInnerSttR = GeometryCalculator.FindTrackEntranceExitbiHexagonRight(GAP, Oxx, Oyy, Rr, Charge, Start, RStrawDetMin, ApotemaInnerParMax, XcrossR, YcrossR);

  //-----------------

  //	working in the hypothesis that his is a track coming from Vertex at (0,0).

  // case when track is completely contained either in Left of Right Inner Stt Parallel
  // sections; that should not be possible if the track comes from (0,0,0) !
  if (flagInnerSttL == 1 || flagInnerSttR == 1) {

    return false;
  }

  // if a track enters only marginally in the volumes, define the track
  // as non-entering and the corresponding flag to -1.

  if (flagInnerSttR == 0 && (XcrossR[0] - XcrossR[1]) * (XcrossR[0] - XcrossR[1]) + (YcrossR[0] - YcrossR[1]) * (YcrossR[0] - YcrossR[1]) < 9. * strawradius * strawradius)
    flagInnerSttR = -1;

  if (flagInnerSttL == 0 && (XcrossL[0] - XcrossL[1]) * (XcrossL[0] - XcrossL[1]) + (YcrossL[0] - YcrossL[1]) * (YcrossL[0] - YcrossL[1]) < 9. * strawradius * strawradius)
    flagInnerSttR = -1;

  // case when track is outside both Inner Stt Parallel sections.

  if (flagInnerSttL == -1 && flagInnerSttR == -1) {

  } else {
    // here at least one of flagInnerSttL or flagInnerSttR is 0;
    // namely the track has at least 2 intersections with an Inner
    // section, Left or Right, or both;
    if (flagInnerSttL == 0 && flagInnerSttR == 0) {
      // case when the track crosses both InnerLeft and InnerRight.
      // Decide what was crossed first and ignore the other part.
      // This may be changed in the future.
      XintersectionList[0] = XcrossL[0];
      YintersectionList[0] = YcrossL[0];
      XintersectionList[1] = XcrossL[1];
      YintersectionList[1] = YcrossL[1];
      XintersectionList[2] = XcrossR[0];
      YintersectionList[2] = YcrossR[0];
      XintersectionList[3] = XcrossR[1];
      YintersectionList[3] = YcrossR[1];
      nintersections = 4;
      GeometryCalculator.ChooseEntranceExitbis(Oxx, Oyy, Charge, FI0,
                                               nintersections, // n. intersection in input.
                                               XintersectionList, YintersectionList,
                                               Xcross, // output
                                               Ycross  // output
      );
      // now decide which sector was crossed first.
      if ((fabs(XcrossL[0] - Xcross[0]) < 1.e-5 && fabs(YcrossL[0] - Ycross[0]) < 1.e-5) ||
          (fabs(XcrossL[1] - Xcross[0]) < 1.e-5 && fabs(YcrossL[1] - Ycross[0]) < 1.e-5)) { // the Left part was entered first.
        flagInnerSttR = -1;
      } else { // the Right part was entered first.
        flagInnerSttL = -1;
      } // end of  if( (fabs(XcrossL[0]-Xcross.....
    }   // end of if( (flagInnerSttL == 0 && flagInnerSttR = 0 )

    //---------  the other 2 possible cases.

    if (flagInnerSttL == 0) {
      nConsideredHits = nInnerHitsLeft;
      for (i = 0; i < 2; i++) {
        XintersectionList[i] = XcrossL[i];
        YintersectionList[i] = YcrossL[i];
      }
    } else { // continuation of if( (flagInnerSttL == 0), case in which
      nConsideredHits = nInnerHitsRight;

      for (i = 0; i < 2; i++) {
        XintersectionList[i] = XcrossR[i];
        YintersectionList[i] = YcrossR[i];
      }
    } // end of   if( (flagInnerSttL == 0)

    nintersections = 2;
    if (fabs(FiLimitAdmissible - FI0) < 2. * PI) { // in this case the point
      // corresponding to FiLimitAdmissible can play a role in the
      // determination of the limiting points of the hits.
      XintersectionList[2] = LimitCoord[0];
      YintersectionList[2] = LimitCoord[1];
      nintersections++;
    } // end of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)

    if (flagOutStt == 0) { // 2 intersections with outer Stt circle.
      XintersectionList[nintersections] = XcrossOut[0];
      XintersectionList[nintersections + 1] = XcrossOut[1];
      YintersectionList[nintersections] = YcrossOut[0];
      YintersectionList[nintersections + 1] = YcrossOut[1];
      nintersections += 2;
    }

    // the method ChooseEntranceExitbis works under the hypothesis that there are
    // at least 2 intersections.
    // It orders the nintersections  intersections using as order parameter the azimuthal
    // angle fi (in the reference frame of the trajectory helix); then it returns the FIRST
    // TWO INTERSECTIONS assuming that the track was originated from (0,0,0) and taking into
    // account its charge;
    GeometryCalculator.ChooseEntranceExitbis(Oxx, Oyy, Charge, FI0,
                                             nintersections, // n. intersection in input.
                                             XintersectionList, YintersectionList,
                                             Xcross, // output
                                             Ycross  // output
    );

    if (fabs(FiLimitAdmissible - FI0) < 2. * PI) {
      // case when this track exits in Z before having the possibility
      // of hitting the Stt parallel inner section.
      if (fabs(LimitCoord[0] - Xcross[0]) < 1.e-5 && fabs(LimitCoord[1] - Ycross[0]) < 1.e-5) {
        return true;
      }

      // case when this track exits in Z AFTER having the possibility
      // of hitting the Stt parallel inner section.

      if (flagOutStt == 0 // 2 intersections with outer Stt circle;
                          //  those intersections were calculated in this method before,
                          // they are called     XcrossOut    and    YcrossOut ;
                          // case when this track exits the Stt outer circle without
                          // hitting the Stt parallel inner section;
                          // this is achieved by comparing the coordinates Xcross[0]
                          // and Ycross[0] (i.e. the FIRST crossing point of this track)
                          // to the crossing point with the outer Stt circle (i.e.
                          // XcrossOut and YcrossOut );

          && ((fabs(XcrossOut[0] - Xcross[0]) < 1.e-5 && fabs(YcrossOut[0] - Ycross[0]) < 1.e-5) ||
              (fabs(XcrossOut[1] - Xcross[0]) < 1.e-5 && fabs(YcrossOut[1] - Ycross[0]) < 1.e-5))) {
        flaggo = false; // this track DOES NOT intersect any STT axial layer;
      } else {          // continuation of  if( flagOutStt ==0)

        // most usual case when track crossed the Inner parallel Stt;
        if (nConsideredHits == 0) {
          return false;
        }

        // if the exit point is actually given by FiLimitAdmissible, then allow
        // an extra uncertainty in the # Stt hit that must be present;
        // this is done because FiLimitAdmissible is not a very precise number.
        if (fabs(LimitCoord[0] - Xcross[1]) < 1.e-5 && fabs(LimitCoord[1] - Ycross[1]) < 1.e-5) {
          islack = 3;
        }
      } //  end of  if( flagOutStt ==0)

    } else {                 // continuation of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)
      if (flagOutStt == 0) { // 2 intersections with outer Stt circle.
        // case when this track exits the Stt outer circle without
        // hitting the Stt parallel inner section
        if ((fabs(XcrossOut[0] - Xcross[0]) < 1.e-5 && fabs(YcrossOut[0] - Ycross[0]) < 1.e-5) ||
            (fabs(XcrossOut[1] - Xcross[0]) < 1.e-5 && fabs(YcrossOut[1] - Ycross[0]) < 1.e-5)) {
          flaggo = false; // this track DOES NOT intersect any STT axial layer;
        }
      } else { // continuation of  if( flagOutStt ==0)

        // most usual case when track crossed the Inner parallel Stt.
        if (nConsideredHits == 0) {
          return false;
        }
      } // end of  if( flagOutStt ==0)
    }   // end of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)

    if (flaggo) {
      //-------------  cleanup of the spurious tracks first using the inner parallel straws.

      if (istampa >= 1) {
        cout << "SttParalCleanup, evento n. " << IVOLTE << ", prima di BadTrack_ParStt; Xin Inner " << Xcross[0] << ", Yin Inner " << Ycross[0] << ",  Xout Inner " << Xcross[1]
             << ", Yout Inner " << Ycross[1] << endl;
      }

      // at this point the n. of inner hits cannot be 0 for a true track.
      if (BadTrack_ParStt(Oxx, Oyy, Rr, strawradius, Charge,
                          Xcross, // Xcross[0]=point of entrance; Xcross[1]=point of exit.
                          Ycross, nInnerHits, ListInnerHits, info, istampa,
                          2. * 2 * strawradius, //  cut of proximity between hits.
                          1,                    // maximum allowed # consecutive hits with distance > cut.
                          islack                // uncertainty allowed as far as the n. of hits that should be present.
                          )) {

        return false;
      }

    } // end of if(flaggo)
  }   // end of if( flagInnerSttL == -1 && flagInnerSttR == -1 )

  islack = 1; // reset the extra uncertainty in the # Stt.

  //------------------------------------------------------------------------------------------------

  //------------ Outer Parallel Stt hits section.
  // find the entrance and exit of the track in the Outer Parallel Straw region, Left side.
  // This region is bounded by a Hexagon (inner), a Circle (outer) and it has
  // the target gap in the middle.

  //  It returns -1 if there are 0 or 1 intersections, or it returns 0
  //  if they are at least 2.
  flagOuterSttL = GeometryCalculator.FindTrackEntranceExitHexagonCircleLeft(Oxx, Oyy, Rr, Charge, Start, ApotemaMinOuterPar, RStrawDetMax, GAP, XcrossL, YcrossL);
  //------------
  // find the entrance and exit of the track in the Outer Parallel Straw region, Right side.
  // This region is bounded by a Hexagon (inner), a Circle (outer) and it has
  // the target gap in the middle.
  flagOuterSttR = GeometryCalculator.FindTrackEntranceExitHexagonCircleRight(Oxx, Oyy, Rr, Charge, Start, ApotemaMinOuterPar, RStrawDetMax, GAP, XcrossR, YcrossR);

  if (istampa > 0) {
    cout << "SttParalCleanup, evento n. " << IVOLTE << ", flagOuterSttR (-1,or 0 --> 2 or more inter.) = " << flagOuterSttR << ", flagOuterSttL " << flagOuterSttL << endl;
  }

  //--------------------------------

  // if a track enters only marginally in the volumes, define the track
  // as non-entering and the corresponding flag to -1.

  if (flagOuterSttR == 0 && (XcrossR[0] - XcrossR[1]) * (XcrossR[0] - XcrossR[1]) + (YcrossR[0] - YcrossR[1]) * (YcrossR[0] - YcrossR[1]) < 9. * strawradius * strawradius)
    flagOuterSttR = -1;

  if (flagOuterSttL == 0 && (XcrossL[0] - XcrossL[1]) * (XcrossL[0] - XcrossL[1]) + (YcrossL[0] - YcrossL[1]) * (YcrossL[0] - YcrossL[1]) < 9. * strawradius * strawradius)
    flagOuterSttR = -1;

  // case when track is outside both Outer Stt Parallel sections; in such case
  // the tracks doesn't have to have Stt Parallel hits in a continuos fashion
  // in the Outer Sections;
  if (flagOuterSttL == -1 && flagOuterSttR == -1) {
    return true;
  }

  // case when the track crosses both OuterLeft and OuterRight.
  // Decide what was crossed first and ignore the other part.
  // This may be changed in the future.
  if (flagOuterSttL == 0 && flagOuterSttR == 0) {

    XintersectionList[0] = XcrossL[0];
    YintersectionList[0] = YcrossL[0];
    XintersectionList[1] = XcrossL[1];
    YintersectionList[1] = YcrossL[1];
    XintersectionList[2] = XcrossR[0];
    YintersectionList[2] = YcrossR[0];
    XintersectionList[3] = XcrossR[1];
    YintersectionList[3] = YcrossR[1];
    nintersections = 4;
    GeometryCalculator.ChooseEntranceExitbis(Oxx, Oyy, Charge, FI0,
                                             nintersections, // n. intersection in input.
                                             XintersectionList, YintersectionList,
                                             Xcross, // output
                                             Ycross  // output
    );
    // now decide which sector was crossed first.
    if ((fabs(XcrossL[0] - Xcross[0]) < 1.e-5 && fabs(YcrossL[0] - Ycross[0]) < 1.e-5) ||
        (fabs(XcrossL[1] - Xcross[0]) < 1.e-5 && fabs(YcrossL[1] - Ycross[0]) < 1.e-5)) { // the Left part was entered first.
      flagOuterSttR = -1;
    } else { // the Right part was entered first.
      flagOuterSttL = -1;
    } // end of  if( (fabs(XcrossL[0]-Xcross.....
  }   // end of if( (flagInnerSttL == 0 && flagInnerSttR = 0 )

  if (flagOuterSttL == 0) {
    nConsideredHits = nOuterHitsLeft;

    for (i = 0; i < 2; i++) {
      XintersectionList[i] = XcrossL[i];
      YintersectionList[i] = YcrossL[i];
    }
  } else { // continuation of if( (flagOuterSttL == 0), case in which
    // flagOuterSttR == 0.
    nConsideredHits = nOuterHitsRight;
    for (i = 0; i < 2; i++) {
      XintersectionList[i] = XcrossR[i];
      YintersectionList[i] = YcrossR[i];
    }
  } // end of   if( (flagOuterSttL == 0)

  nintersections = 2;
  if (fabs(FiLimitAdmissible - FI0) < 2. * PI) { // in this case the point
    // corresponding to FiLimitAdmissible can play a role in the
    // determination of the limiting points of the hits.
    XintersectionList[2] = LimitCoord[0];
    YintersectionList[2] = LimitCoord[1];
    nintersections++;
    //-------------stampe.
    if (istampa > 1) {
      cout << "in SttParalCleanup Outer, caso in cui  FiLimitAdmissible = " << FiLimitAdmissible << "  conta!" << endl;
    }
    //-------------fine stampe.
  } // end of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)

  if (flagOutStt == 0) { // 2 intersections with outer Stt circle.
    XintersectionList[nintersections] = XcrossOut[0];
    XintersectionList[nintersections + 1] = XcrossOut[1];
    YintersectionList[nintersections] = YcrossOut[0];
    YintersectionList[nintersections + 1] = YcrossOut[1];
    nintersections += 2;
  }
  //-------------stampe.
  if (istampa > 1) {
    cout << "in SttParalCleanup Outer, prima di  ChooseEntranceExitbis, nintersections " << nintersections << " e loro lista :" << endl;
    for (int ic = 0; ic < nintersections; ic++) {
      cout << "\tX[" << ic << "] = " << XintersectionList[ic] << ", Y[" << ic << "] = " << YintersectionList[ic] << endl;
    }
  }

  //-------------fine stampe.

  GeometryCalculator.ChooseEntranceExitbis(Oxx, Oyy, Charge, FI0,
                                           nintersections, // n. intersection in input.
                                           XintersectionList, YintersectionList,
                                           Xcross, // output
                                           Ycross  // output
  );
  //-------------stampe.
  if (istampa >= 1) {
    cout << "in SttParalCleanup Outer, dopo di  ChooseEntranceExitbis, Xin" << Xcross[0] << ", Yin " << Ycross[0] << ", Xout " << Xcross[1] << ", Yout " << Ycross[1] << endl;
  }
  //-------------fine stampe.

  if (fabs(FiLimitAdmissible - FI0) < 2. * PI) {
    // case when this track exit in Z before having the possibility
    // of hitting the Stt parallel inner section.

    if (fabs(LimitCoord[0] - Xcross[0]) < 1.e-5 && fabs(LimitCoord[1] - Ycross[0]) < 1.e-5) {
      return true;
    }

    if (flagOutStt == 0) { // 2 intersections with outer Stt circle.
      // case when this track exits the Stt outer circle without
      // hitting the Stt parallel Outer section (for instance the track
      if ((fabs(XcrossOut[0] - Xcross[0]) < 1.e-5 && fabs(YcrossOut[0] - Ycross[0]) < 1.e-5) ||
          (fabs(XcrossOut[1] - Xcross[0]) < 1.e-5 && fabs(YcrossOut[1] - Ycross[0]) < 1.e-5)) {
        // nOuterHits=0; // eliminate all the hits from hit list.
        // nOuterHitsRight=0;
        // nOuterHitsLeft=0;
        return true;
      }
    } // end of  if( flagOutStt ==0)

    // most usual case when track crossed the Outer parallel Stt.
    if (nConsideredHits == 0) {
      return false;
    }

    // if the exit point is actually given by FiLimitAdmissible, then allow
    // an extra uncertainty in the # Stt hit that must be present;
    // this is done because FiLimitAdmissible is not a very precise number.
    if (fabs(LimitCoord[0] - Xcross[1]) < 1.e-5 && fabs(LimitCoord[1] - Ycross[1]) < 1.e-5) {
      islack = 3;
    }

  } else { // continuation of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)

    if (flagOutStt == 0) { // 2 intersections with outer Stt circle.
      // case when this track exits the Stt outer circle without
      // hitting the Stt parallel Outer section (for instance the track
      if ((fabs(XcrossOut[0] - Xcross[0]) < 1.e-5 && fabs(YcrossOut[0] - Ycross[0]) < 1.e-5) ||
          (fabs(XcrossOut[1] - Xcross[0]) < 1.e-5 && fabs(YcrossOut[1] - Ycross[0]) < 1.e-5)) {
        // nOuterHits=0; // eliminate all the hits from hit list.
        // nOuterHitsRight=0;
        // nOuterHitsLeft=0;
        return true;
      }
    } // end of  if( flagOutStt ==0)

    // most usual case when track crossed the Outer parallel Stt.
    if (nConsideredHits == 0) {
      return false;
    }

  } // end of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)

  //-------------------- stampe
  if (istampa >= 2) {
    cout << "SttParalCleanup, OUTER, caso R || L true, IVOLTE = " << IVOLTE << "\n\t Xcross[0] " << Xcross[0] << ", Ycross[0] " << Ycross[0] << "\n\t Xcross[1] " << Xcross[1]
         << ", Ycross[1] " << Ycross[1] << " e charge = " << Charge << ", FiLimitAdmissible " << FiLimitAdmissible << " (X=" << Oxx + Rr * cos(FiLimitAdmissible)
         << ", Y=" << Oyy + Rr * sin(FiLimitAdmissible) << ")." << endl;
  }
  //-------------------fine stampe

  //  cleanup of the spurious tracks now using the outer parallel straws.

  // at this point nOuterHits cannot be 0 for a real track.
  if (BadTrack_ParStt(Oxx, Oyy, Rr, strawradius, Charge,
                      Xcross, // Xcross[0]=point of entrance; Xcross[1]=point of exit.
                      Ycross, nOuterHits, ListOuterHits, info, istampa,
                      2. * 2. * strawradius, //  cut of proximity between hits.
                      1,                     // maximum allowed # consecutive hits with distance > cut.
                      islack                 // uncertainty allowed as far as the n. of hits that should be present.
                      )) {
    return false;
  }

  //      }  // end of  if(nOuterHits==0)

  //----------------------------------------------------------------------------

  // if the code comes here it means that the track is acceptable.

  //	nHits = nOuterHits+nInnerHits;

  return true;
};

//----------end of function PndTrkCleanup::SttParalCleanup

//----------begin of function PndTrkCleanup::SttSkewCleanup
bool PndTrkCleanup::SttSkewCleanup(Double_t ApotemaMaxSkew, Double_t ApotemaMinSkew, Short_t Charge,
                                   Double_t cut, // cut distance (in cm).
                                   Double_t FI0, Double_t FiLimitAdmissible, Double_t GAP, Double_t info[][7], int istampa, int IVOLTE, Short_t *Listofhits,
                                   Short_t maxnum, // max number allowed of failures to pass the cut.
                                   int MAXSTTHITS, Short_t nHits, Double_t Oxx, Double_t Oyy, Double_t Rr, Double_t RStrawDetMax, Double_t *S, Double_t Start[3],
                                   Double_t strawradius)

{

  // bool ConsiderLastHit; //[R.K. 01/2017] unused variable?

  Short_t flagSttL, flagSttR, flagOutStt;

  Short_t i, ipurged, ibad, islack, nHitsLeft, nHitsRight, nintersections, ninside, nnn,
    // nIntersections[2], //[R.K. 01/2017] unused variable?
    ListHits[nHits];
  // ListHitsRight[nHits], //[R.K. 01/2017] unused variable?
  // ListHitsLeft[nHits]; //[R.K. 01/2017] unused variable?

  Double_t cut2, epsilonTheta, fi,
    // FiStart, //[R.K. 01/2017] unused variable?
    length,
    // r, //[R.K. 01/2017] unused variable?
    Sprevious, aux[2], Distance[MAXSTTHITS + 1], Xcross[2], Ycross[2], XcrossL[2], YcrossL[2], XcrossR[2], YcrossR[2], XcrossOut[2], YcrossOut[2],
    XintersectionList[5], // second index =0 --> inner Hexagon, =1 --> outer.
    YintersectionList[5]; // first index : all the possible intersections
                          // (up to 12 intersections).

  //  class with all the geometry calculations :
  PndTrkCTGeometryCalculations GeometryCalculator;

  cut2 = cut * cut;
  islack = 1; // uncertainty allowed as far as
              // the n. of hits that should be present in a given section of the Stt track.

  //------------------------
  //  elimination of hits outside the physical FI range (FiLimitAdmissible) due to finite length of
  //  Straws.

  epsilonTheta = strawradius / Rr; // some extra slac for being conservative.

  if (istampa > 1)
    cout << "\n\nevt " << IVOLTE << ", FI0 " << FI0 << ", Filimit " << FiLimitAdmissible + epsilonTheta << ", Ox " << Oxx << ", Oy " << Oyy << ", R " << Rr << endl;

  for (i = 0, ipurged = 0; i < nHits; i++) {
    fi = S[i];

    if (istampa > 1)
      cout << "\thit // n. " << Listofhits[i] << ", fi " << fi << endl;

    if (Charge < 0) {
      if (fi > FI0) {
        if (fi > FiLimitAdmissible + epsilonTheta)
          continue;
      } else {
        fi += 2. * PI;
        if (fi > FiLimitAdmissible + epsilonTheta)
          continue;
      }      // end of  if( fi > FI0)
    } else { // continuation of  if(Charge <0)
      if (fi > FI0) {
        fi -= 2. * PI;
      } // end of  if( fi > FI0)
      if (fi < FiLimitAdmissible - epsilonTheta)
        continue;
    } // end of if(Charge <0)
    if (istampa > 1)
      cout << "in SttSkewCleanup : hit preso!" << endl;

    ListHits[ipurged] = Listofhits[i];
    S[ipurged] = S[i];
    ipurged++;
  } // end of    for(i=0, ipurged=0; i< nHits; i++)

  if (istampa > 1)
    cout << "in SttSkewCleanup : hit skew prima di purga = " << nHits << ", dopo purga " << ipurged << endl;

  nHits = ipurged;
  if (nHits == 0) { // don't discard track yet, see if it should have
    // skew hits or not.
    nHitsRight = nHitsLeft = 0;
    //		goto jampa;
  } else {

    // separation of Right and Left Skew hits.

    nHitsRight = nHitsLeft = 0;
    for (i = 0; i < nHits; i++) {
      if (info[ListHits[i]][0] < 0.) {
        // ListHitsLeft[nHitsLeft]=ListHits[i]; //[R.K. 01/2017] unused variable?
        nHitsLeft++;
      } else {
        // ListHitsRight[nHitsRight]=ListHits[i]; //[R.K. 01/2017] unused variable?
        nHitsRight++;
      }
    }

    if (istampa > 1)
      cout << "in SttSkewCleanup : n. hit skew Left = " << nHitsLeft << ", right " << nHitsRight << endl;

  } // end of if(nHits==0)
    // jampa: ;
  //	first of all, find possible intersection points with outer circle encompassing
  //	the Stt system.

  flagOutStt = GeometryCalculator.FindIntersectionsOuterCircle(Oxx, Oyy, Rr, RStrawDetMax, XcrossOut, YcrossOut);

  //------------------------------------------
  // find the entrance and exit of the track in the Skew Straw region.
  // This region is bounded by two Hexagons, and it has the target gap
  // in the middle. So Left is the left looking from downstream.

  flagSttL = GeometryCalculator.FindTrackEntranceExitbiHexagonLeft(GAP, Oxx, Oyy, Rr, Charge, Start, ApotemaMinSkew,
                                                                   ApotemaMaxSkew, // Apotema is the distance of a Hexagonal side from (0,0)
                                                                   XcrossL, YcrossL);

  flagSttR = GeometryCalculator.FindTrackEntranceExitbiHexagonRight(GAP, Oxx, Oyy, Rr, Charge, Start, ApotemaMinSkew,
                                                                    ApotemaMaxSkew, // Apotema is the distance of a Hexagonal side from (0,0)
                                                                    XcrossR, YcrossR);

  // find the entrance and exit of the track in the Skew Straw region.
  // This region is bounded by two Hexagons, and it has the target gap in the middle.

  if (istampa > 1)
    cout << "in SttSkewCleanup : flagLeft (-1,0,1) = " << flagSttL << ", right " << flagSttR << endl;
  if (flagSttR == 1 || flagSttL == 1) { // the trajectory is contained completely
                                        // in  the Right or Left Skew section, reject!
    return false;
  }

  // if a track enters only marginally in the skew volumes, define the track
  // as non-entering and the corresponding flag to -1.

  if (flagSttR == 0 && (XcrossR[0] - XcrossR[1]) * (XcrossR[0] - XcrossR[1]) + (YcrossR[0] - YcrossR[1]) * (YcrossR[0] - YcrossR[1]) < 16. * strawradius * strawradius) {
    flagSttR = -1;
    if (istampa > 1)
      cout << "in SttSkewCleanup : distanza entrata-uscita<4*strawradius,flagSttR set at -1!\n";
  }

  if (flagSttL == 0 && (XcrossL[0] - XcrossL[1]) * (XcrossL[0] - XcrossL[1]) + (YcrossL[0] - YcrossL[1]) * (YcrossL[0] - YcrossL[1]) < 16. * strawradius * strawradius) {
    flagSttR = -1;
    if (istampa > 1)
      cout << "in SttSkewCleanup : distanza entrata-uscita<4*strawradius,flagSttL set at -1!\n";
  }

  if (flagSttR != 0 && flagSttL != 0) {
    // nHits=0;
    if (istampa > 1)
      cout << "in SttSkewCleanup : flagSttR = " << flagSttR << ", e  flagSttL = " << flagSttL << ", exit con true!\n";
    return true; // don't discard track because it may have Mvd hits anyway
                 // and/or they can have Inner Parallel hits.
  }

  // case when the track crosses both SkewLeft and SkewRight.
  // Decide what was crossed first and ignore the other part.
  // This may be changed in the future.

  if (flagSttL == 0 && flagSttR == 0) { // crosses both right and left sections.
    XintersectionList[0] = XcrossL[0];
    YintersectionList[0] = YcrossL[0];
    XintersectionList[1] = XcrossL[1];
    YintersectionList[1] = YcrossL[1];
    XintersectionList[2] = XcrossR[0];
    YintersectionList[2] = YcrossR[0];
    XintersectionList[3] = XcrossR[1];
    YintersectionList[3] = YcrossR[1];
    nintersections = 4;
    GeometryCalculator.ChooseEntranceExitbis(Oxx, Oyy, Charge, FI0,
                                             nintersections, // n. intersection in input.
                                             XintersectionList, YintersectionList,
                                             Xcross, // output
                                             Ycross  // output
    );
    // now decide which sector was crossed first.
    if ((fabs(XcrossL[0] - Xcross[0]) < 1.e-5 && fabs(YcrossL[0] - Ycross[0]) < 1.e-5) ||
        (fabs(XcrossL[1] - Xcross[0]) < 1.e-5 && fabs(YcrossL[1] - Ycross[0]) < 1.e-5)) { // the Left part was entered first.
      flagSttR = -1;
    } else { // the Right part was entered first.
      flagSttL = -1;
    } // end of  if( (fabs(XcrossL[0]-Xcross.....
  }   // end of if( (flagInnerSttL == 0 && flagInnerSttR = 0 )

  //---------  the other 2 possible cases.

  if (flagSttL == 0) {
    nnn = nHitsLeft;
    for (i = 0; i < 2; i++) {
      XintersectionList[i] = XcrossL[i];
      YintersectionList[i] = YcrossL[i];
    }
  } else { // continuation of if( (flagSttL == 0), case in which
    // flagSttR == 0.
    nnn = nHitsRight;
    for (i = 0; i < 2; i++) {
      XintersectionList[i] = XcrossR[i];
      YintersectionList[i] = YcrossR[i];
    }
  } // end of   if( (flagInnerSttL == 0)

  nintersections = 2;
  if (fabs(FiLimitAdmissible - FI0) < 2. * PI) { // in this case the point
    // corresponding to FiLimitAdmissible can play a role in the
    // determination of the limiting points of the hits.
    aux[0] = Oxx + Rr * cos(FiLimitAdmissible);
    aux[1] = Oyy + Rr * sin(FiLimitAdmissible);
    XintersectionList[2] = aux[0];
    YintersectionList[2] = aux[1];
    nintersections++;
  } // end of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)

  if (flagOutStt == 0) { // 2 intersections with outer Stt circle.
    XintersectionList[nintersections] = XcrossOut[0];
    XintersectionList[nintersections + 1] = XcrossOut[1];
    YintersectionList[nintersections] = YcrossOut[0];
    YintersectionList[nintersections + 1] = YcrossOut[1];
    nintersections += 2;
  }

  GeometryCalculator.ChooseEntranceExitbis(Oxx, Oyy, Charge, FI0,
                                           nintersections, // n. intersection in input.
                                           XintersectionList, YintersectionList,
                                           Xcross, // output
                                           Ycross  // output
  );

  if (fabs(FiLimitAdmissible - FI0) < 2. * PI) {
    // case when this track exit in Z before having the possibility
    // of hitting the Stt parallel inner section.
    if (fabs(aux[0] - Xcross[0]) < 1.e-5 && fabs(aux[1] - Ycross[0]) < 1.e-5) {
      return true;
    }
    if (flagOutStt == 0) { // 2 intersections with outer Stt circle.
      // case when this track exits the Stt outer circle without
      // hitting the Stt parallel inner section (for instance the track
      if ((fabs(XcrossOut[0] - Xcross[0]) < 1.e-5 && fabs(YcrossOut[0] - Ycross[0]) < 1.e-5) ||
          (fabs(XcrossOut[1] - Xcross[0]) < 1.e-5 && fabs(YcrossOut[1] - Ycross[0]) < 1.e-5)) {
        return true;
      }
    } // end of  if( flagOutStt ==0)

    // most usual case when track crossed the Inner parallel Stt.
    if (nnn == 0)
      return false;

    // if the exit point is actually given by FiLimitAdmissible, then allow
    // an extra uncertainty in the # Stt hit that must be present;
    // this is done because FiLimitAdmissible is not a very precise number.
    if (fabs(aux[0] - Xcross[1]) < 1.e-5 && fabs(aux[1] - Ycross[1]) < 1.e-5) {
      islack = 3;
    }

  } else {                 // continuation of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)
    if (flagOutStt == 0) { // 2 intersections with outer Stt circle.
      // case when this track exits the Stt outer circle without
      // hitting the Stt parallel inner section (for instance the track
      if ((fabs(XcrossOut[0] - Xcross[0]) < 1.e-5 && fabs(YcrossOut[0] - Ycross[0]) < 1.e-5) ||
          (fabs(XcrossOut[1] - Xcross[0]) < 1.e-5 && fabs(YcrossOut[1] - Ycross[0]) < 1.e-5)) {
        return true;
      }
    } // end of  if( flagOutStt ==0)

    // most usual case when track crossed the Skew Stt.
    if (nnn == 0)
      return false;

  } // end of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)

  length = GeometryCalculator.CalculateArcLength(Oxx, Oyy, Rr, Charge, Xcross, Ycross);
  //-------------------- stampe
  if (istampa >= 2) {
    cout << "in SttSkewCleanup,  IVOLTE = " << IVOLTE << "\n\t Xcross[0] " << Xcross[0] << ", Ycross[0] " << Ycross[0] << "\n\t Xcross[1] " << Xcross[1] << ", Ycross[1] "
         << Ycross[1] << ", R = " << Rr << ", Lungh. arco " << length << endl;
  }
  //-------------------fine stampe

  //-------------------------------------------------------------------------

  Sprevious = atan2(Ycross[0] - Oyy, Xcross[0] - Oxx);
  ibad = 0;
  ninside = 0;

  for (i = 0; i < nHits; i++) {
    if (!GeometryCalculator.IsInsideArc(Oxx, Oyy, Charge, Xcross, Ycross, S[i])) {
      continue;
      if (istampa > 1)
        cout << "in SttSkewCleanup :hit n. " << ListHits[i] << " is NOT inside the arc between entrance and exit; hit excluded!\n";
    }

    ninside++;

    Distance[i] = 2. * Rr * Rr * (1. - cos(S[i] - Sprevious)); // this is the usual
                                                               //  distance**2 formula: (x1-x2)**2+(y1-y2)**2;
                                                               // it is already 'protected' against S[i] jumps
                                                               // around 2PI/0.
    Sprevious = S[i];
    if (Distance[i] < 0.)
      Distance[i] = 0.; // rounding errors protection.
    if (istampa >= 2)
      cout << "in SttSkewCleanup, Hit n. " << ListHits[i] << " has Distance " << sqrt(Distance[i]) << endl;
    if (Distance[i] > cut2) {
      if (Distance[i] > 16. * cut2) {
        if (istampa >= 2)
          cout << "in SttSkewCleanup, Hit n. " << ListHits[i] << " has Distance " << sqrt(Distance[i]) << " which is >4.*cut [=" << cut << "], discard the track!" << endl;
        return false;
      }
      if (istampa >= 2)
        cout << "in SttSkewCleanup, Hit n. " << ListHits[i] << " has Distance " << sqrt(Distance[i]) << " which is > cut [=" << cut << "]." << endl;
      ibad++;
    }
  } // end of do (i=1; i<nHits;i++)

  // cut on the minimum (conservative) n. hits that must have fired
  if (ninside < ((int)0.5 * length / strawradius) - islack) {
    if (istampa > 1) {
      cout << "in SttSkewCleanup, n. Hits inside = " << ninside << " is < n. hits that should be inside at least = " << ((int)0.5 * length / strawradius) << "-islack (" << islack
           << "), track rejected!\n";
      return false;
    }
  }

  // compute the distance of last hit to point at which track leaves this detector volume
  // or the last physical possible Fi (given the length of the  straw).

  if (GeometryCalculator.IsInsideArc(Oxx, Oyy, Charge, Xcross, Ycross, S[nHits - 1])) {
    Distance[nHits] = (Oxx + Rr * cos(S[nHits - 1]) - Xcross[1]) * (Oxx + Rr * cos(S[nHits - 1]) - Xcross[1]) +
                      (Oyy + Rr * sin(S[nHits - 1]) - Ycross[1]) * (Oyy + Rr * sin(S[nHits - 1]) - Ycross[1]);

    if (istampa >= 2)
      cout << "in SttSkewCleanup, last Hit n. " << ListHits[nHits - 1] << " has Distance from boundary " << sqrt(Distance[nHits]) << endl;
    if (Distance[nHits] > cut2) {
      if (Distance[nHits] > 16. * cut2) {
        if (istampa >= 2)
          cout << "in SttSkewCleanup, last Hit n. " << ListHits[nHits - 1] << " has Distance from boundary " << sqrt(Distance[nHits]) << " which is >4 .*cut [=" << cut
               << "], discard the track!" << endl;
        return false;
      }
      if (istampa >= 2)
        cout << "in SttSkewCleanup, last Hit n. " << ListHits[nHits - 1] << " has Distance from boundary " << sqrt(Distance[nHits]) << " and it is > cut [=" << cut << "]." << endl;
      ibad++;
    }
  } // end of if(IsInsideArc(

  if (ibad > maxnum) {
    if (istampa >= 2)
      cout << "in SttSkewCleanup, reject this track because ibad = " << ibad << " and it is >  maxnum [=" << maxnum << "].\n";
    return false;
  }

  return true;
};

//----------end of function PndTrkCleanup::SttSkewCleanup

//----------begin of function PndTrkCleanup::TrackCleanup

bool PndTrkCleanup::TrackCleanup(Double_t ApotemaMaxInnerPar, Double_t /*ApotemaMaxSkew*/, //[R.K. 9/2018] unused
                                 Double_t ApotemaMinOuterPar, Double_t /*ApotemaMinSkew*/, //[R.K. 9/2018] unused
                                 Double_t * /*auxS*/,                                      //[R.K. 9/2018] unused
                                 Short_t Charge, Double_t FI0, Double_t GAP, Double_t info[][7], int istampa, int IVOLTE, Double_t KAPPA, Short_t *ListHitsPar,
                                 Short_t * /*ListHitsSkew*/, //[R.K. 9/2018] unused
                                 int /*MAXSTTHITS*/,         //[R.K. 9/2018] unused
                                 Short_t nHitsPar,           // n. hits parall Stt
                                 Short_t /*nHitsSkew*/,      // n. hits parall Stt //[R.K. 9/2018] unused
                                 Double_t Oxx, Double_t Oyy, Double_t Rr, Double_t RStrawDetMax, Double_t RStrawDetMin, Double_t Start[3], Double_t strawradius)
{

  // this method is called by PdnTrkTracking2::Exec for each track found (inside a loop over the
  // number of tracks found by the pattern recognition);

  // this method does 3 things :
  //
  //	1)  finds the entrance and exit points in the STT parallel and skew volumes of the current track;
  //	2)  eliminates from the track hit list possible spurious hits that are not encompassed
  //		by the entrance and exit point;
  //	3)  eliminates the tracks if the hit sequence is not continuous enough.

  // bool ConsiderLastHit; //[R.K. 01/2017] unused variable?

  // Short_t flagInnerStt, //[R.K. 01/2017] unused variable?
  // flagOuterStt; //[R.K. 01/2017] unused variable?

  // Short_t	ihit, //[R.K. 01/2017] unused variable?
  // nInnerHits, //[R.K. 01/2017] unused variable?
  // nOuterHits, //[R.K. 01/2017] unused variable?
  // nIntersections[2], //[R.K. 01/2017] unused variable?
  // ListInnerHits[MAXSTTHITS], //[R.K. 01/2017] unused variable?
  // ListOuterHits[MAXSTTHITS]; //[R.K. 01/2017] unused variable?

  Double_t FiLimitAdmissible;
  // r, //[R.K. 01/2017] unused variable?
  // Xcross[2], //[R.K. 01/2017] unused variable?
  // Ycross[2], //[R.K. 01/2017] unused variable?
  // XintersectionList[12][2],  //[R.K. 01/2017] unused variable?// second index =0 --> inner Hexagon, =1 --> outer.
  // YintersectionList[12][2];  //[R.K. 01/2017] unused variable?// first index : all the possible intersections
  // (up to 12 intersections).

  //------------------------
  // calculation of the Maximum FI angle possible (if it is a +ve charge) of the Minimum
  // for this track, taking into account
  // that the maximum possible Z of a hit is ZCENTER_STRAIGHT + SEMILENGTH_STRAIGHT; the minimum
  // Z of a hit is ZCENTER_STRAIGHT - SEMILENGTH_STRAIGHT.
  if (Charge < 0) {
    if (KAPPA > 0.) {
      FiLimitAdmissible = FI0 + KAPPA * (ZCENTER_STRAIGHT + SEMILENGTH_STRAIGHT);
    } else {
      FiLimitAdmissible = FI0 + KAPPA * (ZCENTER_STRAIGHT - SEMILENGTH_STRAIGHT);
    }
  } else {
    if (KAPPA > 0.) {
      FiLimitAdmissible = FI0 + KAPPA * (ZCENTER_STRAIGHT - SEMILENGTH_STRAIGHT);
    } else {
      FiLimitAdmissible = FI0 + KAPPA * (ZCENTER_STRAIGHT + SEMILENGTH_STRAIGHT);
    }
  } // end of    if(Charge<0)
  //-----------------------------------------------------------------------------------------------------

  // parallel cleanup.

  //----------------stampe
  if (istampa >= 2) {
    cout << " IVOLTE = " << IVOLTE << ", prima di paral cleanup, nHitsPar " << nHitsPar << ", KAPPA = " << KAPPA << ", charge " << Charge << ", FI0 " << FI0
         << "\n\tFiLimitAdmissible " << FiLimitAdmissible << ", X limit " << Oxx + Rr * cos(FiLimitAdmissible) << ", Y limit " << Oyy + Rr * sin(FiLimitAdmissible) << ", Ox "
         << Oxx << ", Oy " << Oyy << ", R " << Rr << endl;
  }

  //-------------------- fine stampe

  if (!SttParalCleanup(ApotemaMaxInnerPar, ApotemaMinOuterPar, Charge, FI0, FiLimitAdmissible, GAP, info, istampa, IVOLTE,
                       ListHitsPar, // input only for now.
                       nHitsPar,    // it doesn't get modify for now.
                       Oxx, Oyy, Rr, RStrawDetMax, RStrawDetMin, Start, strawradius)) {

    return false;
  }

  return true;
};

//----------end of function PndTrkCleanup::TrackCleanup

//----------begin of function PndTrkCleanup::Track_Crosses_MvdBarrelFullAzimuthalCoverage

bool PndTrkCleanup::Track_Crosses_MvdBarrelFullAzimuthalCoverage(Double_t Ox,     // track trajectory center;
                                                                 Double_t Oy,     // track trajectory center;
                                                                 Double_t R,      // track trajectory radius;
                                                                 Double_t fi0,    // FI0 of the Helix of the particle trajectory;
                                                                 Double_t kappa,  // KAPPA of the Helix of the particle trajectory;
                                                                 Double_t charge, // charge of the particle;

                                                                 const Double_t Zlow,                              // Z low limit of this barrel;
                                                                 const Double_t Zup,                               // Z upper limit of this barrel;
                                                                 Double_t RBarrel,                                 // R of this barrel at whicazch the intersection of the particle
                                                                                                                   // trajectory is calculated;
                                                                 PndTrkCTGeometryCalculations *GeometryCalculator, // pointer
                                                                                                                   // to the class making useful geometry calculations;
                                                                 Double_t extra_distance_Z,                        // in cm; extra distance allowed during decision
                                                                                                                   // if there should be an hit in a Mvd sensitive layer;
                                                                 Double_t &Xintersect,                             // output, X position of the point of crossing;
                                                                 Double_t &Yintersect,                             // output, Y position of the point of crossing;
                                                                 Double_t &Zintersect                              // output, Z position of the point of crossing;
)
{
  Short_t yes_intersect;

  Double_t FiOrderedList[2], Xcross[2], Ycross[2];

  // since Pz of the track is given by  -charge*0.003*BField/kappa, the sign of
  // Pz is the opposite of   charge/kappa;
  // the first check is the check that Pz of the track is consistent with the Z limits of
  // this Mvd Barrel;
  // yes_intersect = 0 --> 2 intersections
  // otherwise yes_intersect = -1;
  // I use the method FindIntersectionsOuterCircle that calculates the intersection between
  // two circles;
  yes_intersect = GeometryCalculator->FindIntersectionsOuterCircle(Ox, Oy, R,
                                                                   RBarrel, //  AVERAGE radius of the i-th Mvd barrel layer;
                                                                   Xcross, Ycross);

  if (yes_intersect == 0) { // there are 2 intersections of the trajectory with this barrel;
    // calculate the entrance point based on the rotation direction of the particle
    // (positive --> clockwise);
    GeometryCalculator->ChooseEntranceExit3(Ox, Oy, charge, fi0, 2,
                                            Xcross,       // input and output;
                                            Ycross,       // input and output;
                                            FiOrderedList // output; Fi in the Helix reference frame;
    );

    Xintersect = Xcross[0];
    Yintersect = Ycross[0];
    // calculate the Z coordinate of the intersection; since kappa is necessarily
    //  > 1.e-10 by construction, then Z is always well defined;
    Zintersect = (FiOrderedList[0] - fi0) / kappa;

    // condition for having Mvd hits in the Mvd Barrel layers certainly, namely taking
    // into account also possible errors in the Z caused by the uncertainty of the
    // trajectory; such an error is called extra_distance_Z (in cm);

    // condition by which the trajectory surely had to cross the barrel layer;
    if (Zintersect <= Zup - extra_distance_Z && Zintersect >= Zlow + extra_distance_Z) { // case in which there should be Mvd hits;
      return true;
    }
  } // end of if(yes_intersect>0 )

  return false;
}

//----------end of function PndTrkCleanup::Track_Crosses_MvdBarrelFullAzimuthalCoverage

//----------begin of function PndTrkCleanup::Track_Crosses_MvdBarrelPartialAzimuthalCoverage

bool PndTrkCleanup::Track_Crosses_MvdBarrelPartialAzimuthalCoverage(

  // in this function it is assumed to deal with an Mvd Barrel section composed of an Inner Barrel with
  // RMin radius and an Outer Barrel with RMax radius.
  // Both the Inner and Outer Barrel have their own azimuthal (partial) coverage defined by a number of
  // azimuthal gaps ( ngapInner and ngapOuter respectively, maximum 4 gaps) with a certain range in Fi
  // defined in the arrays   :    gap_lowInner - gap_upInner  and   gap_lowOuter - gap_upOuter respectively;

  Double_t Ox,     // track trajectory center;
  Double_t Oy,     // track trajectory center;
  Double_t R,      // track trajectory radius;
  Double_t fi0,    // FI0 of the Helix of the particle trajectory;
  Double_t kappa,  // KAPPA of the Helix of the particle trajectory;
  Double_t charge, // charge of the particle;

  const Double_t Zlow, // Z low limit of this barrel;
  const Double_t Zup,  // Z upper limit of this barrel;

  Double_t RInnerBarrel,        // R Minimum of this barrel at which the intersection of the particle
                                // trajectory is calculated;
  int ngapInner,                // number of gaps in the azimuthal coverage;
  const Double_t *gap_lowInner, // array of low limits of the range of the azimuthal gaps (radians);
  const Double_t *gap_upInner,  // array of upper limits of the range of the azimuthal gaps (radians);

  Double_t ROuterBarrel,        // R Minimum of this barrel at which the intersection of the particle
                                // trajectory is calculated;
  int ngapOuter,                // number of gaps in the azimuthal coverage;
  const Double_t *gap_lowOuter, // array of low limits of the range of the azimuthal gaps (radians);
  const Double_t *gap_upOuter,  // array of upper limits of the range of the azimuthal gaps (radians);

  PndTrkCTGeometryCalculations *GeometryCalculator, // pointer
                                                    // to the class making useful geometry calculations;
  Double_t extra_distance_Z,                        // in cm; extra distance allowed during decision
                                                    // if there should be an hit in a Mvd sensitive layer;
  Double_t *Xintersect,                             // output, X position of the point of crossing track-Inner Barrel
                                                    // and track-Outer Barrel;
  Double_t *Yintersect,                             // output, Y position of the point of crossing;
  Double_t *Zintersect                              // output, Z position of the point of crossing;
)
{
  bool cross;

  Short_t i, yes_intersect;

  Double_t fi, FiOrderedList[2], Xcross[2], Ycross[2];

  // yes_intersect = 0 --> 2 intersections
  // otherwise yes_intersect = -1;
  // I use the method FindIntersectionsOuterCircle that calculates the intersection between
  // two circles;

  cross = false; // at the end, if there is a crossing point in this Inner or Outer Barrel then
                 // cross will be true, otherwise it will be false;

  // first check if the trajectory crosses this Inner Barrel;
  // to check that, the function GeometryCalculator->FindIntersectionsOuterCircle is
  // used (despite the name, it is only a fuction that checks if a circular trajectory
  // of center Ox and Oy and radius R crosses (in the XY view) another circle with center in 0,0
  // and radius RInnerBarrel);

  yes_intersect = GeometryCalculator->FindIntersectionsOuterCircle(Ox, Oy, R,
                                                                   RInnerBarrel, //  radius of this Inner Mvd Barrel layer;
                                                                   Xcross, Ycross);
  if (yes_intersect == 0) { // there are 2 intersections of the trajectory with this barrel;
    // calculate the entrance point based on the rotation direction of the particle
    // (positive --> clockwise);
    GeometryCalculator->ChooseEntranceExit3(Ox, Oy, charge, fi0, 2,
                                            Xcross,       // input and output;
                                            Ycross,       // input and output;
                                            FiOrderedList // output; Fi of the intersections in the Helix frame;
    );

    // calculate the Z coordinate of the intersection; since |kappa| is necessarily
    //  > 1.e-10 by construction, so Z is always well defined;
    Zintersect[0] = (FiOrderedList[0] - fi0) / kappa;

    // condition for having Mvd hits in the Mvd Barrel layers certainly, namely taking
    // into account also possible errors in the Z caused by the uncertainty of the
    // trajectory; such an error is called extra_distance_Z (in cm);

    // condition by which the trajectory surely had to cross the barrel layer;
    if (Zintersect[0] <= Zup - extra_distance_Z && Zintersect[0] >= Zlow + extra_distance_Z) { // case in which there should be Mvd hits;
      // loop to check that the track doesn't fall in the gap region;
      // fi must be between 0. and 2Pi;
      fi = atan2(Ycross[0], Xcross[0]);
      if (fi < 0.)
        fi += TWO_PI;
      if (fi < 0.)
        fi = 0.;
      if (fi > TWO_PI)
        fi = TWO_PI;

      cross = true;
      for (i = 0; i < ngapInner; i++) {
        if (fi < gap_upInner[i] && fi > gap_lowInner[i]) {
          cross = false;
          break;
        }
      } // end of  for(i=0; i<ngapInner; i++)
    }   //  end of  if( Zintersect[0]<= Zup - extra_distance...
  }     // end of if(yes_intersect==0 )

  // in case the trajectory crosses the Inner Barrel then there must be a hit in this Barrel section ;
  // now check this Mvd Outer Barrel; at any rate if now cross is true it will remain so no matter the result
  // of the nalysis of the Outer Barrel;

  // if there was no intersection Xintersect[0] is conventionally set at -99999.

  if (cross) {
    Xintersect[0] = Xcross[0];
    Yintersect[0] = Ycross[0];

  } else {
    Xintersect[0] = -99999.;
  }

  // then check if the trajectory crosses this Outer Barrel;
  // first of all check if there is an Outer Barrel; if it doesn't its radius is conventionally set at -1.;

  if (ROuterBarrel < 0.) {
    // since there was no intersection Xintersect[1] is conventionally set at -99999.
    Xintersect[1] = -99999.;
    //  at this point cross can be either true or false;
    return cross;
  }

  yes_intersect = GeometryCalculator->FindIntersectionsOuterCircle(Ox, Oy, R,
                                                                   ROuterBarrel, //  radius of this Outer Mvd Barrel layer;
                                                                   Xcross, Ycross);

  if (yes_intersect == 0) { // there are 2 intersections of the trajectory with this barrel;
    // calculate the entrance point based on the rotation direction of the particle
    // (positive --> clockwise);
    GeometryCalculator->ChooseEntranceExit3(Ox, Oy, charge, fi0, 2,
                                            Xcross,       // input and output;
                                            Ycross,       // input and output;
                                            FiOrderedList // output; Fi of the intersections in the Helix frame;
    );

    // calculate the Z coordinate of the intersection; since kappa is necessarily
    //  > 1.e-10 by construction, then Z is always well defined;
    Zintersect[1] = (FiOrderedList[0] - fi0) / kappa;

    // condition for having Mvd hits in the Mvd Barrel layers certainly, namely taking
    // into account also possible errors in the Z caused by the uncertainty of the
    // trajectory; such an error is called extra_distance_Z (in cm);

    // condition by which the trajectory surely had to cross the barrel layer;
    if (Zintersect[1] <= Zup - extra_distance_Z && Zintersect[1] >= Zlow + extra_distance_Z) { // case in which there should be Mvd hits;
      // loop to check that the track doesn't fall in the gap region; fi must be between 0. and 2Pi;
      fi = atan2(Ycross[0], Xcross[0]);
      if (fi < 0.)
        fi += TWO_PI;
      if (fi < 0.)
        fi = 0.;
      if (fi > TWO_PI)
        fi = TWO_PI;

      Xintersect[1] = Xcross[0];
      for (i = 0; i < ngapOuter; i++) {
        if (fi < gap_upOuter[i] && fi > gap_lowOuter[i]) {
          Xintersect[1] = -99999.;
          break;
        }
      } // end of  for(i=0; i<ngapOuter; i++)
    } else {
      Xintersect[1] = -99999.;
    }      //  end of if( Zintersect[1]<= Zup - extra....
  } else { // continuation of if(yes_intersect==0 )

    Xintersect[1] = -99999.;

  } // end of if(yes_intersect==0 )

  if (Xintersect[1] > -99998.) // in this case there was intersection between this track and the Barrel;
  {
    Yintersect[1] = Ycross[0];
    return true; // because there is at least one intersection : the one with the Outer Barrel;
  } else {
    return cross; // this is the result of the analysis of the Inner Barrel;
  }
}

//----------end of function PndTrkCleanup::Track_Crosses_MvdBarrelPartialAzimuthalCoverage

//----------begin of function PndTrkCleanup::Track_Crosses_MvdMiniDisk_withMargin

bool PndTrkCleanup::Track_Crosses_MvdMiniDisk_withMargin(Double_t ZLayerBegin, // Z of the beginning of the layer (end of layer = + 0.02);
                                                         Double_t xmargin,     //  safety margin in X coordinate;
                                                         Double_t ymargin,     //  safety margin in Y coordinate;

                                                         Double_t Ox,     // track trajectory center;
                                                         Double_t Oy,     // track trajectory center;
                                                         Double_t R,      // track trajectory radius;
                                                         Double_t fi0,    // FI0 of the Helix of the particle trajectory;
                                                         Double_t kappa,  // KAPPA of the Helix of the particle trajectory;
                                                         Double_t charge, // charge of the particle;

                                                         PndTrkCTGeometryCalculations *GeometryCalculator // pointer to
                                                                                                          // the class doing the geometrical calculations;
)
{

  Double_t angle, X, Y;

  // since Pz of the track is given by  -charge*0.003*BField/kappa, the sign of
  // Pz is the opposite of   charge/kappa (or charge*kappa) ;
  // so when the sign of Pz is > 0, ZLayerBegin must be positive otherwise
  // return false; viceversa when Pz is negative;

  if (-charge * kappa * ZLayerBegin <= 0.) {
    // this is a track travelling in the Z direction opposite to where the
    // ZLayerBegin is, consequently it cannot cross this MiniDisk;

    return false;
  }

  angle = fi0 + ZLayerBegin * kappa;

  X = Ox + R * cos(angle); // X position reached by the track at Z = 1.98, the
                           // middle of this MiniDisk;

  Y = Oy + R * sin(angle); // Y position reached by the track at Z = 1.98, the
                           // middle of this MiniDisk;

  // the list of Mvd MiniDisks, listed according to the Z position of the silicon sensitive layer;
  if (ZLayerBegin == 1.97) {

    if (GeometryCalculator->IsInMvdMiniDisk1_97to1_99withMargin(X, Y, xmargin, ymargin))
      return true;
    else
      return false;

  } else if (ZLayerBegin == 2.41) {

    if (GeometryCalculator->IsInMvdMiniDisk2_41to2_43withMargin(X, Y, xmargin, ymargin))
      return true;
    else
      return false;

  } else if (ZLayerBegin == 3.97) {

    if (GeometryCalculator->IsInMvdMiniDisk3_97to3_99withMargin(X, Y, xmargin, ymargin))
      return true;
    else
      return false;

  } else if (ZLayerBegin == 4.41) {

    if (GeometryCalculator->IsInMvdMiniDisk4_41to4_43withMargin(X, Y, xmargin, ymargin))
      return true;
    else
      return false;

  } else if (ZLayerBegin == 6.97) {

    if (GeometryCalculator->IsInMvdMiniDisk6_97to6_99withMargin(X, Y, xmargin, ymargin))
      return true;
    else
      return false;

  } else if (ZLayerBegin == 7.41) {

    if (GeometryCalculator->IsInMvdMiniDisk7_41to7_43withMargin(X, Y, xmargin, ymargin))
      return true;
    else
      return false;

  } else if (ZLayerBegin == 9.97) {

    if (GeometryCalculator->IsInMvdMiniDisk9_97to9_99withMargin(X, Y, xmargin, ymargin))
      return true;
    else
      return false;

  } else if (ZLayerBegin == 10.41) {

    if (GeometryCalculator->IsInMvdMiniDisk10_41to10_43withMargin(X, Y, xmargin, ymargin))
      return true;
    else
      return false;

  } else if (ZLayerBegin == 14.77) {

    if (GeometryCalculator->IsInMvdMiniDisk14_77to14_79withMargin(X, Y, xmargin, ymargin))
      return true;
    else
      return false;

  } else if (ZLayerBegin == 15.21) {

    if (GeometryCalculator->IsInMvdMiniDisk15_21to15_23withMargin(X, Y, xmargin, ymargin))
      return true;
    else
      return false;

  } else if (ZLayerBegin == 21.77) {

    if (GeometryCalculator->IsInMvdMiniDisk21_77to21_79withMargin(X, Y, xmargin, ymargin))
      return true;
    else
      return false;

  } else if (ZLayerBegin == 22.21) {

    if (GeometryCalculator->IsInMvdMiniDisk22_21to22_23withMargin(X, Y, xmargin, ymargin))
      return true;
    else
      return false;

  } else {
    cout << "PndTrkCleanup.cxx::Track_Crosses_MvdMiniDisk_withMargin WARNING, this Mvd MiniDisk apparently"
         << " is not in the list of known Mvd MiniDisks !";
  }
  return false; // FIXME Is this logically correct?
}

//----------end of function PndTrkCleanup::Track_Crosses_MvdMiniDisk_withMargin

//----------begin of function PndTrkCleanup::XYCleanup
bool PndTrkCleanup::XYCleanup(
  // general infos about the axial Straws;
  int istampa, Double_t info[][7], Short_t (*ListParContiguous)[6], Short_t *nParContiguous, Short_t *StrawCode, Short_t *StrawCode2, Short_t *TubeID, Double_t *xTube,
  Double_t *yTube, Double_t *zTube, Double_t *xxyyTube,
  // the following are the info of the track under scrutiny;
  Double_t Ox, Double_t Oy, Double_t R, Short_t Charge, Short_t *ListHits, Short_t nHits, Double_t /*R_STT_INNER_PAR_MAX*/, //[R.K. 9/2018] unused
  Short_t nScitilHitsInTrack,                                                                                               // input, # of SciTil hits in the current track;
  Short_t *ListSciTilHitsinTrack,                                                                                           // input, list of SciTil hits in the current track;
  Double_t posizSciTil[][3]                                                                                                 // input, info on all the SciTil position;
)
{

  bool // connected, //[R.K. 01/2017] unused variable?
    farthest_hit_is_boundary,
    good;

  Short_t auxListHits[MAXSTTHITSINTRACK], holes, i, j,
    // k, //[R.K. 01/2017] unused variable?
    nArcs_populated,

    tListInnerHitsLeft[nHits], tListInnerHitsRight[nHits], tListOuterHitsLeft[nHits], tListOuterHitsRight[nHits],
    // tube_adjacent, //[R.K. 01/2017] unused variable?
    // tube_current, //[R.K. 01/2017] unused variable?
    // tube_next, //[R.K. 01/2017] unused variable?
    // tube_near, //[R.K. 01/2017] unused variable?
    ListHitsInArc[MAXSTTHITSINTRACK][56], // ordered list of hits in each Arc (from first to last
                                          // according to the charge of the particle;if the maximum
                                          //  # of Intersected Sector is 56, than the maximum # of Arcs is 28;

    nHitsInArc[56]; // number of hits in each Arc; the maximum # of Arcs is 56;
                    // nInnerHitsLeft, //[R.K. 01/2017] unused variable?
  // nInnerHitsRight, //[R.K. 01/2017] unused variable?
  // nOuterHitsLeft, //[R.K. 01/2017] unused variable?
  // nOuterHitsRight; //[R.K. 01/2017] unused variable?
  //	OrderedSectorList[56];	// ordered list of Sectors crossed (from first to last); each
  // Sector number is the Sector where the Arc lies;

  Double_t dist2, FiOrderedList[2], FiStart, Xcross[2], Ycross[2];

  Vec<Short_t> ListInnerHitsLeft(tListInnerHitsLeft, nHits, "ListInnerHitsLeft"), ListInnerHitsRight(tListInnerHitsRight, nHits, "ListInnerHitsRight"),
    ListOuterHitsLeft(tListOuterHitsLeft, nHits, "ListOuterHitsLeft"), ListOuterHitsRight(tListOuterHitsRight, nHits, "ListOuterHitsRight");

  //  class with all the geometry calculations:
  PndTrkCTGeometryCalculations GeometryCalculator;

  //------------------------------------------------------------------------------------------------------------
  // first of all, eliminate spurious with SciTil's since it is faster;

  // now check if there is an intersection with the SciTil;

  // calculate the intersection points in XY of the track trajectory with a circle tangent to the SciTil in
  // the middle of each SciTil tile; these can be 0 or 2;
  // if there are no intersections don't do anything; if there are 2 intersections check that there is a SciTil
  // hit in the proper place;
  //  FindIntersectionsOuterCircle returns -1 or 0;
  // if FindIntersectionsOuterCircle returns -1 there are no intersections, if it returns 0 there are 2;
  if (GeometryCalculator.FindIntersectionsOuterCircle(Ox, Oy, R, RADIUSSCITIL, Xcross, Ycross) >= 0) {

    // there are 2 intersections;
    //  choose the first entrance point according to the charge of the particle;
    // ChooseEntranceExit3 works under the hypothesis that there are at least 2 intersections.

    FiStart = atan2(-Oy, -Ox);
    if (FiStart < 0.)
      FiStart += TWO_PI;
    if (FiStart < 0.)
      FiStart = 0.;

    GeometryCalculator.ChooseEntranceExit3(Ox, Oy, Charge, FiStart,
                                           2,            // # of Intersections between track and Circle;
                                           Xcross,       // input and output; these are the intersections;
                                           Ycross,       // input and output; these are the intersections;
                                           FiOrderedList // output
    );

    // the intersection point must be close enough to at least 1 SciTil hit;

    good = false;
    for (i = 0; i < nScitilHitsInTrack; i++) {
      dist2 = (posizSciTil[ListSciTilHitsinTrack[i]][0] - Xcross[0]) * (posizSciTil[ListSciTilHitsinTrack[i]][0] - Xcross[0]) +
              (posizSciTil[ListSciTilHitsinTrack[i]][1] - Ycross[0]) * (posizSciTil[ListSciTilHitsinTrack[i]][1] - Ycross[0]);

      if (dist2 < 2.25 * DIMENSIONSCITIL * DIMENSIONSCITIL) {
        good = true;
        break;
      } // oversizing; in principle
      // dist2 should be < DIMENSIONSCITIL*DIMENSIONSCITIL/4 ;

    } // end of for(i=0;i<nScitilHitsInTrack;i++)
    // failed to find a hit SciTil close to trajectory;
    if (!good)
      return false;
  } // end of  if( GeometryCalculator.FindIntersectionsOuterCircle(

  // ----------------------------------------------------------------------------- end of check with SciTil's;

  // now use the STT hits for cleaning;

  // Convention for the Sector number used in GoodTrack :
  // Sector = 1 --> Axial Outer Right
  // Sector = 2 --> Axial Inner Right
  // Sector = 3 --> Axial Inner Left
  // Sector = 4 --> Axial Outer Left

  // --------------------------------------------------------------------------------------------------

  // the intersections of the current track with the boundaries of each Axial Sector (Inner Left, Outer Left,
  // Inner Right, Outer Right) determine the number of Arcs in which the trajectory is subdivided;
  // for each Arc the number of ordered (according to the charge of the particle) axial STT hits are
  // found and listed;
  // nArcs_populated = # of Arcs (maximum possible  28 in the Left side + 28 in the Right Side of STT axial detector
  //  --->  56 maximum) populated by axial STT hits;
  // OrderedSectorList = ordered list, from last to first, of the Sectors corresponding to each Arc ;
  // nHitsInArc[56] = # of hits of the track belonging to each Arc (in order corresponding to the Sector order);
  // ListHitsInArc[MAXSTTHITSINTRACK][56] = list of hits in each Arc; this list is ordered clockwise or anticlockwise
  //						according to the charge;

  GeometryCalculator.ListAxialSectorsCrossedbyTrack_and_Hits(Ox,              // input;
                                                             Oy,              // input;
                                                             R,               // input;
                                                             Charge,          // input;
                                                             nHits,           // input;
                                                             ListHits,        // input;
                                                             info,            // input;
                                                             nArcs_populated, // output; # Arcs of trajectory populated by at least 1 axial hit; this is <= 28;
                                                                              //		OrderedSectorList,	// output; ordered list of Sectors crossed (from first to last); each
                                                                              //					// Sector number correspond to the Sector where the Arc lies;
                                                             nHitsInArc,      // output; number of hits in each Sector; if the maximun # of Intersected Sector is 56,
                                                                              //  than the maximum # of Arcs is 28;

                                                             ListHitsInArc // output; ordered list of hits in each Arc (from first to last
                                                                           // according to the charge of the particle;if the maximum
                                                                           //  # of Intersected Sector is 56, than the maximum # of Arcs is 28;
  );

  //-------------------------------------------------
  if (istampa > 0) {
    cout << "from XYCleanup,after ListAxialSectorsCrossedbyTrack_and_Hits :" << endl << "\tnArcs_populated " << nArcs_populated << ", ordered list of Sectors crossed :" << endl;
    for (int kg = 0; kg < nArcs_populated; kg++) {
      cout << "\tSector  populated with " << nHitsInArc[kg] << "  hits; loro lista :" << endl;
      for (int nn = 0; nn < nHitsInArc[kg]; nn++) {
        cout << "\t\taxial hit n. " << ListHitsInArc[nn][kg] << ", R**2 "
             << info[ListHitsInArc[nn][kg]][0] * info[ListHitsInArc[nn][kg]][0] + info[ListHitsInArc[nn][kg]][1] * info[ListHitsInArc[nn][kg]][1] << endl;
      }
    };
  };
  //-------------------------------------------------

  // ------------------- starts analysis of the track; loop over the number of Arcs in which the track has been
  //   subdivided and allow a maximum total number of holes of 1 per track;

  // Sector number convention:
  // 1 --> Right Axial Outer;
  // 2 --> Right Axial Inner;
  // 3 --> Left Axial Inner;
  // 4 --> Left Axial Outer;

  // # of "holes" in the track;
  holes = 0;

  //   no_holes = false;

  // if the following flag is true the hit of the track furthest from the origin is at the boundary of
  // this sector;
  farthest_hit_is_boundary = false;

  // loop from the last Arc (the farthest according to the charge of the track) to the first one;
  for (i = nArcs_populated - 1; i >= 0; i--) {
    for (j = 0; j < nHitsInArc[i]; j++) {
      auxListHits[j] = ListHitsInArc[j][i];
    }

    if (!GoodTrack(info,                     // input
                   farthest_hit_is_boundary, // input
                   Ox,                       // input; center of the current track;
                   Oy,                       // input; center of the current track;
                   R,                        // input; Radius of the current track;
                   Charge,                   // input; charge of the current track;
                   nHitsInArc[i],            // input
                   auxListHits,              // input
                   StrawCode,                // input
                   StrawCode2,               // input
                   TubeID,                   // input
                   nParContiguous,           // input
                   ListParContiguous,        // input
                   xTube,                    // input
                   yTube,                    // input
                   zTube,                    // input
                   xxyyTube,                 // input
                   holes                     // input and output
                   )) {
      return false;
    }

    // now the first farthest hit in the next arc must be boundary;
    farthest_hit_is_boundary = true;
  } // end of  for(i=nArcs_populated;i>0; i--)

  // -----------------------------------------------------------------------

  return true;
}
//----------end of function PndTrkCleanup::XYCleanup

ClassImp(PndTrkCleanup);
