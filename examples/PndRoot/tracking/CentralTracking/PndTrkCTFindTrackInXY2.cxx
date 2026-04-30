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

#include "PndTrkCTFindTrackInXY2.h"
#include "PndTrkCTGeometryCalculations.h"
//     #include "PndTrkGlpkFits.h"
#include "PndTrkLegendreFits.h"
#include "PndTrkChi2Fits.h"
#include "PndTrkCleanup.h"
#include "PndTrkCTGeometryCalculations.h"
#include "PndTrkPrintouts.h"
#include "PndTrkMergeSort.h"
#include "PndTrkVectors.h"
#include <iostream>
#include <cmath>

#define PI 3.141592654
#define two_pi 6.283185307

using namespace std;

//------------------------- begin of function  PndTrkCTFindTrackInXY2::AddMvdHitsToSttTracks

void PndTrkCTFindTrackInXY2::AddMvdHitsToSttTracks(Double_t delta,                       // input;
                                                   Double_t /*highqualitycut*/,          // input; //[R.K. 9/2018] unused
                                                   Double_t FiRangeMvdLow,               // input;
                                                   Double_t FiRangeMvdUp,                // input;
                                                   const Short_t maxmvdpixelhitsintrack, // input;
                                                   const Short_t maxmvdstriphitsintrack, // input;
                                                   Short_t nMvdPixelHit,                 // input;
                                                   Short_t nMvdStripHit,                 // input;
                                                   Double_t Ox,                          // input;
                                                   Double_t Oy,                          // input;
                                                   Double_t R,                           // input;
                                                   Double_t *XMvdPixel,                  // input;
                                                   Double_t *XMvdStrip,                  // input;
                                                   Double_t *YMvdPixel,                  // input;
                                                   Double_t *YMvdStrip,                  // input;

                                                   Short_t &nMvdPixelHitsinTrack,    // output
                                                   Short_t *ListMvdPixelHitsinTrack, // output; dimensionality : [MAXMVDSTRIPHITSINTRACK];
                                                   Short_t &nMvdStripHitsinTrack,    // output
                                                   Short_t *ListMvdStripHitsinTrack  // output; dimensionality : [MAXMVDSTRIPHITSINTRACK];
)
{
  // bool specialcase; //[R.K. 01/2017] unused variable?

  Short_t // i, //[R.K. 01/2017] unused variable?
    jmvdhit;

  Double_t angle, dist;

  //  for(i=0; i<nSttTrackCand; i++){

  // first try attach the Pixels;

  nMvdPixelHitsinTrack = 0;
  for (jmvdhit = 0; jmvdhit < nMvdPixelHit; jmvdhit++) {
    angle = atan2(YMvdPixel[jmvdhit] - Oy, XMvdPixel[jmvdhit] - Ox);
    if (angle < 0.)
      angle += 2. * PI;
    if (angle > FiRangeMvdUp) {
      angle -= 2. * PI;
      if (angle > FiRangeMvdUp)
        angle = FiRangeMvdUp;

    } else if (angle < FiRangeMvdLow) {
      angle += 2. * PI;
      if (angle < FiRangeMvdLow)
        angle = FiRangeMvdLow;
    }

    if (angle > FiRangeMvdLow && angle < FiRangeMvdUp) {
      dist = fabs(sqrt((Ox - XMvdPixel[jmvdhit]) * (Ox - XMvdPixel[jmvdhit]) + (Oy - YMvdPixel[jmvdhit]) * (Oy - YMvdPixel[jmvdhit])) - R);
      if (dist < delta) {
        ListMvdPixelHitsinTrack[nMvdPixelHitsinTrack] = jmvdhit;
        nMvdPixelHitsinTrack++;
        if (nMvdPixelHitsinTrack == maxmvdpixelhitsintrack)
          break;
      }
    } // end of  if(angle > FiRangeMvdLow)
  }   // end of  for( jmvdhit=0; jmvdhit<nMvdPixelHits; jmvdhit++)

  // then try attach the Strips;
  nMvdStripHitsinTrack = 0;
  for (jmvdhit = 0; jmvdhit < nMvdStripHit; jmvdhit++) {
    angle = atan2(YMvdStrip[jmvdhit] - Oy, XMvdStrip[jmvdhit] - Ox);
    if (angle < 0.)
      angle += 2. * PI;
    if (angle > FiRangeMvdUp) {
      angle -= 2. * PI;
      if (angle > FiRangeMvdUp)
        angle = FiRangeMvdUp;
    } else if (angle < FiRangeMvdLow) {
      angle += 2. * PI;
      if (angle < FiRangeMvdLow)
        angle = FiRangeMvdLow;
    }

    if (angle > FiRangeMvdLow && angle < FiRangeMvdUp) {
      dist = fabs(sqrt((Ox - XMvdStrip[jmvdhit]) * (Ox - XMvdStrip[jmvdhit]) + (Oy - YMvdStrip[jmvdhit]) * (Oy - YMvdStrip[jmvdhit])) - R);
      if (dist < delta) {
        ListMvdStripHitsinTrack[nMvdStripHitsinTrack] = jmvdhit;
        nMvdStripHitsinTrack++;
        if (nMvdStripHitsinTrack == maxmvdstriphitsintrack)
          break;
      }
    } // end of  if(angle > FiRangeMvdLow)
  }   // end of  for( jmvdhit=0; jmvdhit<nMvdStripHits; jmvdhit++)

  //     }	// end of for(i=0; i<nSttTrackCand; i++)

  return;
}

//------------------------- end of function  PndTrkCTFindTrackInXY2::AddMvdHitsToSttTracks

//----------begin of function PndTrkCTFindTrackInXY2::AssociateSciTilHit

Short_t PndTrkCTFindTrackInXY2::AssociateSciTilHit(Double_t /*dimensionscitil*/, //[R.K. 9/2018] unused
                                                   Double_t *esse,               // output, list of  S of the SciTil hits associated.
                                                   bool *InclusionListSciTil,
                                                   Short_t *List, // output, list of SciTil hits associated (max. 2);
                                                   Short_t maxscitilhitsintrack, Short_t nSciTilHits, Double_t Oxx, Double_t Oyy, Double_t posizSciTil[][3], Double_t Rr)
{

  bool intersect;

  Short_t igoodScit, iScitHit, Nint;

  Double_t XintersectionList[2], YintersectionList[2];

  PndTrkCTGeometryCalculations GeomCalculator;

  igoodScit = 0;

  for (iScitHit = 0; iScitHit < nSciTilHits; iScitHit++) {
    if (!InclusionListSciTil[iScitHit])
      continue;

    intersect = GeomCalculator.IntersectionSciTil_Circle(
      //	  2.*dimensionscitil,   //  dimensionscitil = true width of SciTil tiles;
      posizSciTil[iScitHit][0], posizSciTil[iScitHit][1], Oxx, Oyy, Rr,
      &Nint,             // output
      XintersectionList, // output
      YintersectionList  // output
    );

    if (intersect) {
      if (igoodScit == maxscitilhitsintrack)
        break;
      List[igoodScit] = iScitHit;

      // calculate S on the lateral face of the Helix.
      if (Nint == 1) { // the majority of the cases
        esse[igoodScit] = atan2(YintersectionList[0] - Oyy, XintersectionList[0] - Oxx);
      } else { // in this case Nint=2 (it should be a very rare case).
        // do an average of the two positions.
        esse[igoodScit] = atan2(0.5 * (YintersectionList[0] + YintersectionList[1]) - Oyy, 0.5 * (XintersectionList[0] + XintersectionList[1]) - Oxx);
      } // end of  if ( Nint==1)
      if (esse[igoodScit] < 0.)
        esse[igoodScit] += 2. * PI;
      igoodScit++;

    } // end of  if(intersect)
  }   // end of  for(iScitHit=0; iScitHit<nScitHits; iScitHit++)

  return igoodScit;
}

//----------end of function PndTrkCTFindTrackInXY2::AssociateSciTilHit

//----------begin of function PndTrkCTFindTrackInXY2::DecideWhichAngularRangeAndCharge

void PndTrkCTFindTrackInXY2::DecideWhichAngularRangeAndCharge(Double_t fiCenter,              // input, fi of the (0,0) in the Helix reference frame; it is between 0 and 2PI;
                                                              Double_t fi_low_limit[2],       // input; fi low limit of the Stt detector in the Helix frame;
                                                              Double_t fi_up_limit[2],        // input; fi up limit of the Stt detector in the Helix frame;
                                                              Double_t (*info)[7],            // input
                                                              Short_t *ListSttParHitsinTrack, // input
                                                              Short_t nSttParHitsinTrack,     // input
                                                              Double_t Oxx,                   // input
                                                              Double_t Oyy,                   // input

                                                              Short_t &charge,         // output; charge of the particle;
                                                              Double_t &FiRangeMvdLow, // output; Fi range of possible Mvd hits; FiRangeMvdLow always
                                                                                       // between 0 and 2PI; and always  FiRangeMvdLow<FiRangeMvdUp;
                                                              Double_t &FiRangeMvdUp,  // output; Fi range of possible Mvd hits;
                                                              Double_t &Fi_low_limit,  // output;
                                                              Double_t &Fi_up_limit    // output;
)
{

  Short_t ihit, Nleft, Nright;

  Double_t fi, sinus;

  //-------------------

  Nleft = 0;
  Nright = 0;
  for (ihit = 0; ihit < nSttParHitsinTrack; ihit++) {
    // using the SIGN of the cross product [in the Helix reference frame
    // in the XY projection]   (Hit_Straw_Center) CROSS (center of the Laboratory Frame)
    // to determine if
    // a hit is on the left side [looking into the beam] or in the right
    // side of the trajectory in the XY projection; if the sign is positive
    // then it is on the right side with respect to the segment joining
    // the (0,0) to the center of the Helix;
    // it turns out the the sign of such  cross product is simply the
    // sign of  sin( fi - fiCenter);

    // fi of the Straw hit :
    fi = atan2(info[ListSttParHitsinTrack[ihit]][1] - Oyy, info[ListSttParHitsinTrack[ihit]][0] - Oxx);

    // sign of the cross product :
    sinus = sin(fi - fiCenter);
    if (sinus > 0.)
      Nright++;
    else
      Nleft++;

  } // end of for(ihit=0;ihit<nSttParHitsinTrack;ihit++)

  if (Nright > Nleft) {
    // in this case the particle is travelling counterclockwise --> it is a
    // negative particle; the Stt detector range is between fi_low_limit[0]
    // and fi_up_limit[0] in ANY case (even when there is ONLY ONE entrance and exit);
    // the Mvd range is between (0,0) and Fi_low_limit;
    Fi_low_limit = fi_low_limit[0];
    Fi_up_limit = fi_up_limit[0];
    charge = -1;
    FiRangeMvdLow = fiCenter; // fiCenter is between 0. and 2PI;
    FiRangeMvdUp = Fi_low_limit;

    // put FiRangeMvdUp between 0 and 2PI;
    FiRangeMvdUp = fmod(FiRangeMvdUp, two_pi);
    if (FiRangeMvdUp < FiRangeMvdLow) {
      FiRangeMvdUp += two_pi;
      if (FiRangeMvdUp < FiRangeMvdLow)
        FiRangeMvdUp = FiRangeMvdLow;
    } // end if( FiRangeMvdUp < FiRangeMvdLow)

  } else { // continuation of if( Nright > Nleft)
           // in this case the particle is travelling clockwise --> it is a
           // positive particle; the Mvd range is between and Fi_up_limit and (0,0) ;

    if (fi_low_limit[1] < -99.) {
      // there are only 2 intersections;
      Fi_low_limit = fi_low_limit[0];
      Fi_up_limit = fi_up_limit[0];
    } else {
      // there are 4 intersections;
      Fi_low_limit = fi_low_limit[1];
      Fi_up_limit = fi_up_limit[1];
    }
    charge = 1;
    FiRangeMvdUp = fiCenter; // fiCenter is between 0. and 2PI;
    FiRangeMvdLow = Fi_up_limit;
    // put FiRangeMvdLow between 0 and 2PI;
    FiRangeMvdLow = fmod(FiRangeMvdLow, two_pi);
    if (FiRangeMvdUp < FiRangeMvdLow) {
      FiRangeMvdUp += two_pi;
      if (FiRangeMvdUp < FiRangeMvdLow)
        FiRangeMvdUp = FiRangeMvdLow;
    }
  } // end if( Nright > Nleft)

  // calculate the range for possible Mvd hits attached to this track;

  return;
}

//----------end of function PndTrkCTFindTrackInXY2::DecideWhichAngularRangeAndCharge

//----------begin of function PndTrkCTFindTrackInXY2::FindTrackInXYProjection

bool PndTrkCTFindTrackInXY2::FindTrackInXYProjection(struct FindTrackInXYProjection2_InputOutputData *InOut, int istampa, int /*IVOLTE*/ //[R.K. 9/2018] unused
)
{

  // load the struct members in local variables;

  Double_t(*info)[7] = InOut->info;
  Double_t(*infoparalConformal)[5] = InOut->infoparalConformal;
  Short_t &nSttParHitsinTrack = (*(InOut->nHitsinTrack)), nMvdPixelHit = InOut->nMvdPixelHit, &nMvdPixelHitsinTrack = *(InOut->nMvdPixelHitsinTrack),
          nMvdStripHit = InOut->nMvdStripHit, &nMvdStripHitsinTrack = *(InOut->nMvdStripHitsinTrack), *ListSttParHitsinTrack = InOut->ListHitsinTrack,
          *ListMvdPixelHitsinTrack = InOut->ListMvdPixelHitsinTrack, *ListMvdStripHitsinTrack = InOut->ListMvdStripHitsinTrack;

  Double_t &Ox = *(InOut->Oxx), &Oy = *(InOut->Oyy), &R = *(InOut->Rr), *XMvdPixel = InOut->XMvdPixel, *XMvdStrip = InOut->XMvdStrip, *YMvdPixel = InOut->YMvdPixel,
           *YMvdStrip = InOut->YMvdStrip;

  // make a POINTER to an ARRAY[][3] of
  // Double_t and assign value present in the calling sequence of this method;

  Double_t(*posizSciTil)[3] = (Double_t(*)[3])InOut->posizSciT;

  //---------------

  // bool
  // Type; //[R.K. 01/2017] unused variable?
  // int istampa = 2;

  Short_t auxListHitsinTrack[InOut->maxstthits], flagStt, i,
    // iexcl, //[R.K. 01/2017] unused variable?
    j,
    // ListHitsinTrackinWhichToSearch[InOut->maxstthits], //[R.K. 01/2017] unused variable?
    // Naux, //[R.K. 01/2017] unused variable?
    // Nbaux, //[R.K. 01/2017] unused variable?
    nFitPoints,
    // Nint, //[R.K. 01/2017] unused variable?
    NN,
    // Nouter, //[R.K. 01/2017] unused variable?
    // OutputListHitsinTrack[InOut->maxstthits], //[R.K. 01/2017] unused variable?
    // OutputList2HitsinTrack[InOut->maxstthits], //[R.K. 01/2017] unused variable?
    status;

  Double_t aaa,
    // d, //[R.K. 01/2017] unused variable?
    // diff, //[R.K. 01/2017] unused variable?
    fiCenter, fi_low_limit[2], fi_up_limit[2], FiRangeMvdLow, FiRangeMvdUp, gamma, m, q, r2;
  // rotationangle, //[R.K. 01/2017] unused variable?
  // rotationcos, //[R.K. 01/2017] unused variable?
  // rotationsin; //[R.K. 01/2017] unused variable?

  //---------------------
  /*
   Double_t
    tDriftRadiusconformal[InOut->maxhitsinfit],
    tErrorDriftRadiusconformal[InOut->maxhitsinfit],
    tXconformal[InOut->maxhitsinfit],
    tYconformal[InOut->maxhitsinfit];
   Vec <Double_t>
    DriftRadiusconformal ( tDriftRadiusconformal, InOut->maxhitsinfit, "DriftRadiusconformal"),
    ErrorDriftRadiusconformal ( tErrorDriftRadiusconformal, InOut->maxhitsinfit, "ErrorDriftRadiusconformal"),
    Xconformal(tXconformal, InOut->maxhitsinfit, "Xconformal"),
    Yconformal(tYconformal, InOut->maxhitsinfit, "Yconformal");
  */
  Double_t DriftRadiusconformal[InOut->maxhitsinfit], ErrorDriftRadiusconformal[InOut->maxhitsinfit], Xconformal[InOut->maxhitsinfit], Yconformal[InOut->maxhitsinfit];

  //--------------------------

  PndTrkCleanup Cleanup;
  PndTrkCTGeometryCalculations GeomCalculator;
  PndTrkPrintouts Print2;

  nFitPoints = nSttParHitsinTrack;
  if (nFitPoints > InOut->maxhitsinfit)
    nFitPoints = InOut->maxhitsinfit;

  for (j = 0; j < nFitPoints; j++) {
    Xconformal[j] = infoparalConformal[ListSttParHitsinTrack[j]][0];
    Yconformal[j] = infoparalConformal[ListSttParHitsinTrack[j]][1];
    ErrorDriftRadiusconformal[j] = infoparalConformal[ListSttParHitsinTrack[j]][2];
    DriftRadiusconformal[j] = infoparalConformal[ListSttParHitsinTrack[j]][2];
  }

  // PndTrkGlpkFits fit;

  PndTrkLegendreFits fitLegiandre;
  PndTrkChi2Fits fitChi2;

  status = fitLegiandre.FitHelixCylinder2(InOut->Cosine, InOut->legiandre_nthetadiv, InOut->legiandre_nradiusdiv, nFitPoints, Xconformal, Yconformal, DriftRadiusconformal,
                                          ErrorDriftRadiusconformal,

                                          0., //  rotationangle, input;
                                          InOut->Sinus,
                                          InOut->thetamax,          // input;
                                          InOut->thetamin,          // input;
                                          InOut->trajectory_vertex, //  vertex in (X,Y) of this trajectory
                                          InOut->maxhitsinfit,      //  maximum n. of hits allowed in fast fit
                                          &m, &q, InOut->ALFA, InOut->BETA, InOut->GAMMA, InOut->TypeConf,
                                          2,              // istampa
                                          InOut->icounter //  IVOLTE
  );

  if (status < 0)
    return false;

  //  this trasformation is valid even if the equation is a straight line from the fit
  Ox = -0.5 * (*(InOut->ALFA));
  Oy = -0.5 * (*(InOut->BETA));
  R = Ox * Ox + Oy * Oy - (*(InOut->GAMMA));

  // some obvious preliminary cuts
  if (R < 0.)
    return false;
  R = sqrt(R);
  aaa = sqrt(Ox * Ox + Oy * Oy);

  // the following is because the circumference is supposed to come from (0,0);
  //   here the factor 0.9 is used in order to be conservative.
  if (aaa < 0.9 * InOut->apotemastrawdetectormin / 2.)
    return false;

  //   here the factor 0.9 is used in order to be conservative.
  if (R + aaa < InOut->apotemastrawdetectormin * 0.9)
    return false;

  //-------------- debug printout
  if (istampa >= 2) {
    bool tkeepit[10];
    tkeepit[0] = true;
    Short_t nSttSkewHitsinTrack[10], nSciTilHitsinTrack[10];
    Short_t ListSttSkewHitsinTrack[100];
    Double_t KAPPA[10];
    KAPPA[0] = 0.;
    nSttSkewHitsinTrack[0] = 0;
    nSciTilHitsinTrack[0] = 0;
    nMvdPixelHitsinTrack = 0;
    nMvdStripHitsinTrack = 0;

    cout << "\tstampa in CTFind..2.cxx, dopo FitHelixCylinder  :" << endl;
    Print2.stampetta2(tkeepit, ListMvdPixelHitsinTrack, ListMvdStripHitsinTrack, ListSttParHitsinTrack, ListSttSkewHitsinTrack, InOut->ListSciTilHitsinTrack, &nMvdPixelHitsinTrack,
                      &nMvdStripHitsinTrack, &nSttParHitsinTrack,

                      nSttSkewHitsinTrack, nSciTilHitsinTrack, 1, // questo e' nTotCand, cioe' 1
                      -1,                                         // print all candidates;
                      InOut->maxmvdpixelhitsintrack, InOut->maxmvdstriphitsintrack, InOut->maxscitilhitsintrack, InOut->maxstthitsintrack, &R, &Ox, &Oy,
                      InOut->Fi_initial_helix_referenceframe, KAPPA);
  }
  //-------------- end of debug printout
  //---------------------  find the angular range for candidate track in the Stt detector (find
  //			 the two possibilities in general);

  // fi_low_limit[0], fi_up_limit[0] is the solution (radians) corresponding to the intersection
  //	on the RIGHT side, in the XY projection of the trajectory [looking into the beam] with respect
  //	to the segment joining the Center of the Helix with the origin (0,0);
  // fi_low_limit[1], fi_up_limit[1] is the solution corresponding to the LEFT intersection; in case
  //			there is no second solution it is set at -100.;
  GeomCalculator.FindingParallelTrackAngularRange2(Ox, Oy, InOut->rstrawdetectormax, InOut->apotemastrawdetectormin, R, fi_low_limit, fi_up_limit, &flagStt);
  if (flagStt < 0)
    return false;

  // decide which fi_up_limit, fi_low_limit are good by looking at where it is one hit of the cluster;

  // the fi of the origin [= (0,0,0) ] of the trajectory;
  fiCenter = atan2(-Oy, -Ox);
  if (fiCenter < 0.)
    fiCenter += two_pi;
  *(InOut->Fi_initial_helix_referenceframe) = fiCenter;

  DecideWhichAngularRangeAndCharge(fiCenter,               // input, fi of the (0,0) in the Helix reference frame;
                                   fi_low_limit,           // input; fi low limit of the Stt detector in the Helix frame;
                                   fi_up_limit,            // input; fi up limit of the Stt detector in the Helix frame;
                                   info,                   // input
                                   InOut->ListHitsinTrack, // input
                                   nSttParHitsinTrack,     // input
                                   Ox,                     // input
                                   Oy,                     // input

                                   *(InOut->Charge),     // output; charge of the particle calculated with the
                                                         // trajectory present parameters;
                                   FiRangeMvdLow,        // output; Fi range of possible Mvd hits; FiRangeMvdLow always
                                                         // between 0 and 2PI; and always  FiRangeMvdLow<FiRangeMvdUp;
                                   FiRangeMvdUp,         // output; Fi range of possible Mvd hits;
                                   *InOut->Fi_low_limit, // output; inside DecideWhichAngulaRange this is an alias;
                                   *InOut->Fi_up_limit   // output; inside DecideWhichAngulaRange this is an alias;
  );

  // finds the intersection of the trajectory with the outer and inner STT detector radius
  // and calculates the corresponding fi_up_limit and fi_low_limit in the Helix reference
  // frame both for a positive and for a negative particle;
  // fi_up_limit[0], fi_low_limit[0] --> for a positive track (it moves clockwise when looking
  // at the beam);
  // fi_up_limit[1], fi_low_limit[1] --> for a negative track (it moves anticlockwise when looking
  // at the beam); or they are -100. when there is only one entrance and exit for the trajectory;

  //----

  NN = TrkAssociatedParallelHitsToHelix5(auxListHitsinTrack, //  this is the output
                                         InOut->InclusionListStt, *(InOut->Fi_low_limit), *(InOut->Fi_up_limit), info, InOut->ListSttParHits, InOut->nsttparhit, Ox, Oy, R,
                                         InOut->strawradius);

  // check that there are at least 2 axial STT hits (minimumhitspertrack is usually 2);
  if (NN < InOut->minimumhitspertrack)
    return false;

  if (NN > InOut->maxstthitsintrack) {
    nSttParHitsinTrack = InOut->maxstthitsintrack;
  } else {
    nSttParHitsinTrack = NN;
  }

  for (i = 0; i < nSttParHitsinTrack; i++) {
    ListSttParHitsinTrack[i] = auxListHitsinTrack[i];
  }

  //-------------- debug printout

  if (istampa >= 2) {
    bool tkeepit[10];
    tkeepit[0] = true;
    Short_t nSttSkewHitsinTrack[10], nSciTilHitsinTrack[10];
    Short_t ListSttSkewHitsinTrack[100];
    Double_t KAPPA[10];
    KAPPA[0] = 0.;
    nSttSkewHitsinTrack[0] = 0;
    nSciTilHitsinTrack[0] = 0;

    cout << "\tstampa in CTFind..2.cxx, dopo TrkAssociatedParallelHitsToHelix5 :" << endl;
    Print2.stampetta2(tkeepit, ListMvdPixelHitsinTrack, ListMvdStripHitsinTrack, ListSttParHitsinTrack, ListSttSkewHitsinTrack, InOut->ListSciTilHitsinTrack, &nMvdPixelHitsinTrack,
                      &nMvdStripHitsinTrack, &nSttParHitsinTrack,

                      nSttSkewHitsinTrack, nSciTilHitsinTrack, 1, // questo e' nTotCand, cioe' 1
                      -1,                                         // print all candidates;
                      InOut->maxmvdpixelhitsintrack, InOut->maxmvdstriphitsintrack, InOut->maxscitilhitsintrack, InOut->maxstthitsintrack, &R, &Ox, &Oy,
                      InOut->Fi_initial_helix_referenceframe, KAPPA);
  }
  //-------------- end of debug printout

  // adding the Mvd hits;

  Double_t delta = 0.5,   //  parameter of proximity for associating Mvd hits to Stt tracks;
    highqualitycut = 0.2; //  parameter of proximity for associating Mvd hits to Stt tracks; at the moment it is not used
                          // actually ;

  AddMvdHitsToSttTracks(delta,                         // input;
                        highqualitycut,                // input;
                        FiRangeMvdLow,                 // input;
                        FiRangeMvdUp,                  // input;
                        InOut->maxmvdpixelhitsintrack, // input;
                        InOut->maxmvdstriphitsintrack, // input;
                        nMvdPixelHit,                  // input;
                        nMvdStripHit,                  // input;
                        Ox,                            // input;
                        Oy,                            // input;
                        R,                             // input;
                        XMvdPixel,                     // input;
                        XMvdStrip,                     // input;
                        YMvdPixel,                     // input;
                        YMvdStrip,                     // input;

                        nMvdPixelHitsinTrack,    // output
                        ListMvdPixelHitsinTrack, // output; dimensionality : [MAXMVDPIXELHITSINTRACK];
                        nMvdStripHitsinTrack,    // output
                        ListMvdStripHitsinTrack  // output; dimensionality : [MAXMVDSTRIPHITSINTRACK];
  );

  //-------------- debug printout

  if (istampa >= 2) {
    bool tkeepit[10];
    tkeepit[0] = true;
    Short_t nSttSkewHitsinTrack[10], nSciTilHitsinTrack[10];
    Short_t ListSttSkewHitsinTrack[100];
    Double_t KAPPA[10];
    KAPPA[0] = 0.;
    nSttSkewHitsinTrack[0] = 0;
    nSciTilHitsinTrack[0] = 0;

    cout << "\tstampa in CTFind..2.cxx, dopo AddMvdHitsToSttTracks (m = " << m << ", q = " << q << ", atan(m) = " << atan(m) << "):" << endl;
    Print2.stampetta2(tkeepit, ListMvdPixelHitsinTrack, ListMvdStripHitsinTrack, ListSttParHitsinTrack, ListSttSkewHitsinTrack, InOut->ListSciTilHitsinTrack, &nMvdPixelHitsinTrack,
                      &nMvdStripHitsinTrack, &nSttParHitsinTrack,

                      nSttSkewHitsinTrack, nSciTilHitsinTrack, 1, // questo e' nTotCand, cioe' 1
                      -1,                                         // print all candidates;
                      InOut->maxmvdpixelhitsintrack, InOut->maxmvdstriphitsintrack, InOut->maxscitilhitsintrack, InOut->maxstthitsintrack, &R, &Ox, &Oy,
                      InOut->Fi_initial_helix_referenceframe, KAPPA);
  }
  //-------------- end of debug printout

  //-----------------------------------------------------------------------

  // if there are new Mvd hits attached, redo the fit;

  if (nMvdPixelHitsinTrack + nMvdStripHitsinTrack == 0) {
    // in this case no Mvd hits were added, skip and go to the SciTil section;
    *(InOut->Mvdhits) = false;
    *(InOut->ALFA) = -2. * Ox;
    *(InOut->BETA) = -2. * Oy;
    *(InOut->GAMMA) = Ox * Ox + Oy * Oy - R * R;
  }

  // in this case new Mvd hits were added;
  else {

    *(InOut->Mvdhits) = true;

    // fit again in XY projection, this time with the Chi2 fit;
    // redo the Xconformal and Yconformal arrays since the
    // hit composition of this track candidate may have changed; use first of all
    // the Mvd hits and also all the Stt hits (provided the total is <= InOut->maxstthitsintrack);
    // in the latter case the Mvd points are used first;

    nFitPoints = 0;
    // Mvd Pixels;
    for (j = 0; j < nMvdPixelHitsinTrack; j++) {
      if (nFitPoints == InOut->maxhitsinfit)
        break;
      nFitPoints++;
      r2 = XMvdPixel[ListMvdPixelHitsinTrack[j]] * XMvdPixel[ListMvdPixelHitsinTrack[j]] + YMvdPixel[ListMvdPixelHitsinTrack[j]] * YMvdPixel[ListMvdPixelHitsinTrack[j]];
      Xconformal[j] = XMvdPixel[ListMvdPixelHitsinTrack[j]] / r2;
      Yconformal[j] = YMvdPixel[ListMvdPixelHitsinTrack[j]] / r2;

      // I assume the 'drift radius' to be  the max dimension of the Pixel;
      gamma = r2 - 0.01 * 0.01;
      // 0.01 is the dimension of the pixel;
      ErrorDriftRadiusconformal[j] = 3. * delta / fabs(gamma); // 3 is an arbitrary loose safety factor;

      DriftRadiusconformal[j] = -1; // // only to signal later this is a Mvd hit;

    } // end of for(j=0; j<nMvdPixelHitsinTrack; j++)

    // Mvd Strips;
    for (j = 0; j < nMvdStripHitsinTrack; j++) {
      if (nFitPoints == InOut->maxhitsinfit)
        break;
      r2 = XMvdStrip[ListMvdStripHitsinTrack[j]] * XMvdStrip[ListMvdStripHitsinTrack[j]] + YMvdStrip[ListMvdStripHitsinTrack[j]] * YMvdStrip[ListMvdStripHitsinTrack[j]];
      Xconformal[nFitPoints] = XMvdStrip[ListMvdStripHitsinTrack[j]] / r2;
      Yconformal[nFitPoints] = YMvdStrip[ListMvdStripHitsinTrack[j]] / r2;
      // I assume the 'drift radius' to be  the max dimension of the Strip;
      gamma = r2 - 0.01 * 0.01;
      ErrorDriftRadiusconformal[nFitPoints] = 3. * 0.01 / fabs(gamma); // 3 is an arbitrary loose safety factor;
                                                                       // 0.01 is the dimension of the strip;
      DriftRadiusconformal[nFitPoints] = -1;                           // // only to signal later this is a Mvd hit;

      nFitPoints++;
    }

    // axial Stt;
    for (j = 0; j < nSttParHitsinTrack; j++) {
      if (nFitPoints == InOut->maxhitsinfit)
        break;
      Xconformal[nFitPoints] = infoparalConformal[(InOut->ListHitsinTrack)[j]][0];
      Yconformal[nFitPoints] = infoparalConformal[(InOut->ListHitsinTrack)[j]][1];

      // errors on a Stt hit are assumed to be 0.5 cm in cartesian XY variables;
      r2 = info[(InOut->ListHitsinTrack)[j]][0] * info[(InOut->ListHitsinTrack)[j]][0] + info[(InOut->ListHitsinTrack)[j]][1] * info[(InOut->ListHitsinTrack)[j]][1];
      gamma = r2 - 0.5 * 0.5;
      ErrorDriftRadiusconformal[nFitPoints] = 3. * 0.01 / fabs(gamma); // 3 is an arbitrary loose safety factor;

      DriftRadiusconformal[nFitPoints] = infoparalConformal[(InOut->ListHitsinTrack)[j]][2];

      nFitPoints++;
    }

    // the following fit in XY with the Chi2 methods requires already an existing fit (in order
    // to decide a priori on the left-right ambiguity of the Stt axial straws);
    // The calculation of a good  rotationangle is very, very useful to the success of the fitting
    //  in those cases when the trajectory is nearly perpendicular in the conformal space;

    status = fitChi2.FitHelixCylinder(nFitPoints, Xconformal, Yconformal, DriftRadiusconformal, ErrorDriftRadiusconformal,

                                      atan(m),                  //  rotationangle, in radians; m was found by the Hough transform fit;
                                                                //	0. ,  //  rotationangle, in radians; m was found by the Hough transform fit;
                                      InOut->trajectory_vertex, //  vertex in (X,Y) of this trajectory
                                      InOut->maxhitsinfit,      //  maximum n. of hits allowed in fast fit
                                      &m, &q,
                                      InOut->ALFA,  // input and output;
                                      InOut->BETA,  // input and output;
                                      InOut->GAMMA, // input and output;
                                      InOut->TypeConf,
                                      2,              // istampa
                                      InOut->icounter //  IVOLTE
    );

    if (status > 0) { // in this case the previous fit was successful, so proceed with further (better)
                      //  association of Mvd hits and STT hits;
                      // otherwise go directly to the association of the SciTil hits;
      Ox = -0.5 * (*(InOut->ALFA));
      Oy = -0.5 * (*(InOut->BETA));
      R = sqrt(Ox * Ox + Oy * Oy - (*(InOut->GAMMA)));

      //-------------- debug printout

      if (istampa >= 2) {
        bool tkeepit[10];
        tkeepit[0] = true;
        Short_t nSttSkewHitsinTrack[10], nSciTilHitsinTrack[10];
        Short_t ListSttSkewHitsinTrack[100];
        Double_t KAPPA[10];
        KAPPA[0] = 0.;
        nSttSkewHitsinTrack[0] = 0;
        nSciTilHitsinTrack[0] = 0;

        cout << "\tstampa in CTFind..2.cxx, dopo fitChi2.FitHelixCylinder(m = " << m << ", q = " << q << ", atan(m) = " << atan(m) << "):" << endl;
        Print2.stampetta2(tkeepit, ListMvdPixelHitsinTrack, ListMvdStripHitsinTrack, ListSttParHitsinTrack, ListSttSkewHitsinTrack, InOut->ListSciTilHitsinTrack,
                          &nMvdPixelHitsinTrack, &nMvdStripHitsinTrack, &nSttParHitsinTrack,

                          nSttSkewHitsinTrack, nSciTilHitsinTrack, 1, // questo e' nTotCand, cioe' 1
                          -1,                                         // print all candidates;
                          InOut->maxmvdpixelhitsintrack, InOut->maxmvdstriphitsintrack, InOut->maxscitilhitsintrack, InOut->maxstthitsintrack, &R, &Ox, &Oy,
                          InOut->Fi_initial_helix_referenceframe, KAPPA);
      }
      //-------------- end of the debug printout

      // find again the angular range for candidate track in the Stt detector (find
      //			 the two possibilities in general);

      // fi_low_limit[0], fi_up_limit[0] is the solution (radians) corresponding to the intersection
      //	on the RIGHT side, in the XY projection of the trajectory [looking into the beam] with respect
      //	to the segment joining the Center of the Helix with the origin (0,0);
      // fi_low_limit[1], fi_up_limit[1] is the solution corresponding to the LEFT intersection; in case
      //			there is no second solution it is set at -100.;
      GeomCalculator.FindingParallelTrackAngularRange2(Ox, Oy, InOut->rstrawdetectormax, InOut->apotemastrawdetectormin, R,
                                                       fi_low_limit, // output;
                                                       fi_up_limit,  // output;
                                                       &flagStt      // output;
      );

      if (flagStt < 0)
        return false;

      // decide which fi_up_limit, fi_low_limit are good by looking at where it is one hit of the cluster;

      // the fi of the origin [= (0,0,0) ] of the trajectory;
      fiCenter = atan2(-Oy, -Ox);
      if (fiCenter < 0.)
        fiCenter += two_pi;
      *(InOut->Fi_initial_helix_referenceframe) = fiCenter;

      DecideWhichAngularRangeAndCharge(fiCenter,               // input, fi of the (0,0) in the Helix reference frame;
                                       fi_low_limit,           // input; fi low limit of the Stt detector in the Helix frame;
                                       fi_up_limit,            // input; fi up limit of the Stt detector in the Helix frame;
                                       info,                   // input
                                       InOut->ListHitsinTrack, // input
                                       nSttParHitsinTrack,     // input
                                       Ox,                     // input
                                       Oy,                     // input

                                       *(InOut->Charge),     // output; charge of the particle calculated with the
                                                             // trajectory present parameters;
                                       FiRangeMvdLow,        // output; Fi range of possible Mvd hits; FiRangeMvdLow always
                                                             // between 0 and 2PI; and always  FiRangeMvdLow<FiRangeMvdUp;
                                                             // inside DecideWhichAngulaRange this is an alias;
                                       FiRangeMvdUp,         // output; Fi range of possible Mvd hits;
                                                             // inside DecideWhichAngulaRange this is an alias;
                                       *InOut->Fi_low_limit, // output; inside DecideWhichAngulaRange this is an alias;
                                       *InOut->Fi_up_limit   // output; inside DecideWhichAngulaRange this is an alias;
      );

      // ------------------------  now redo the selection of the Mvd hits belonging to this track;

      delta = 0.5;          //  parameter of proximity for associating Mvd hits to Stt tracks
                            //   highqualitycut=0.3; //  parameter of proximity for associating Mvd hits to Stt tracks
      highqualitycut = 0.5; //  parameter of proximity for associating Mvd hits to Stt tracks

      AddMvdHitsToSttTracks(delta,                         // input;
                            highqualitycut,                // input;
                            FiRangeMvdLow,                 // input;
                            FiRangeMvdUp,                  // input;
                            InOut->maxmvdpixelhitsintrack, // input;
                            InOut->maxmvdstriphitsintrack, // input;
                            nMvdPixelHit,                  // input;
                            nMvdStripHit,                  // input;
                            Ox,                            // input;
                            Oy,                            // input;
                            R,                             // input;
                            XMvdPixel,                     // input;
                            XMvdStrip,                     // input;
                            YMvdPixel,                     // input;
                            YMvdStrip,                     // input;

                            nMvdPixelHitsinTrack,    // output
                            ListMvdPixelHitsinTrack, // output; dimensionality : [MAXMVDPIXELHITSINTRACK];
                            nMvdStripHitsinTrack,    // output
                            ListMvdStripHitsinTrack  // output; dimensionality : [MAXMVDSTRIPHITSINTRACK];
      );

      // ------------------------  now redo the selection of the STT Axial hits belonging to this track;

      // try TrkAssociatedParallelHitsToHelix6 that is the same as TrkAssociatedParallelHitsToHelix5 except that
      // it takes as input the maximum distance allowed for an associated hit;
      NN = TrkAssociatedParallelHitsToHelix6(auxListHitsinTrack, //  this is the output
                                             InOut->InclusionListStt, *(InOut->Fi_low_limit), *(InOut->Fi_up_limit), info, InOut->ListSttParHits, InOut->nsttparhit, Ox, Oy, R,
                                             2. * InOut->strawradius // this is the maximum allowed distance;
      );

      if (NN < InOut->minimumhitspertrack)
        return false;
      if (NN > InOut->maxstthitsintrack) {
        nSttParHitsinTrack = InOut->maxstthitsintrack;
      } else {
        nSttParHitsinTrack = NN;
      }

      for (i = 0; i < nSttParHitsinTrack; i++) {
        ListSttParHitsinTrack[i] = auxListHitsinTrack[i];
      }

    } // end of  if(status> 0 )

  } // end of if( nMvdPixelHitsinTrack + nMvdStripHitsinTrack == 0 )

  //---------------------------

  // now the section that associates the SciTil hits to this track candidate;

  //  equation of the SciTil segment :  y0 * y + x0 * x - x0**2 - y0**2 = 0
  //  where  (x0,y0) = position of center of the SciTil.

  //  delimiting points of the SciTil segment :  define L = length of the SciTil,
  //  and RR = sqrt(x0**2+y0**2), SIGN = the sign of (-x0*y0) or SIGN=1 when y0=0,
  //  SIGN=irrelevant when x0=0;   then :
  //  P1 =  [ x0- abs{(L/2)*y0/RR}; y0-SIGN*abs{(L/2)*x0/RR} ],
  //  P2 =  [ x0+abs{(L/2)*y0/RR}; y0+SIGN*abs{(L/2)*x0/RR} ].

  *(InOut->nSciTilHitsinTrack) = AssociateSciTilHit(InOut->dimensionscitil,
                                                    InOut->S_SciTilHitsinTrack, // output; S on the lateral face of the Helix
                                                                                // of the SciTil hit (if present).
                                                    InOut->InclusionListSciTil, InOut->ListSciTilHitsinTrack, InOut->maxscitilhitsintrack, InOut->nSciTilHits, *(InOut->Oxx),
                                                    *(InOut->Oyy), posizSciTil, *(InOut->Rr));

  // even though it should be impossible in principle, EXCLUDE the possibility of having more
  // than TWO SciTil hits belonging to a track;

  if (*(InOut->nSciTilHitsinTrack) > 0) {
    // even though it should be impossible in principle, EXCLUDE
    // the possibility of having more
    // than TWO SciTil hits belonging to a track;
    if (*(InOut->nSciTilHitsinTrack) > 2)
      *(InOut->nSciTilHitsinTrack) = 2;

    //	  for(j=0;j<*(InOut->nSciTilHitsinTrack);j++){
    //		(InOut->InclusionListSciTil)[(InOut->ListSciTilHitsinTrack)[j]]
    //			=false;
    //	  }
  }

  // orderig the hits in this track cand; for a trajectory Radius not too large
  // better the ordering with conformal.

  if (R < InOut->rstrawdetectormax) {

    // the origin of the tack is assumed to be (0,0);

    // ordering the Stt axial using the conformal coordinates;
    OrderingUsingFi(*InOut->Charge,     // input;
                    info,               // input;
                    nSttParHitsinTrack, // input;
                    Ox,                 // input;
                    Oy,                 // input;

                    ListSttParHitsinTrack // input and output;
    );

  } else {                               // otherwise it is better distance from (0,0) method.
    OrderingUsingR(info,                 // input;
                   nSttParHitsinTrack,   // input;
                   ListSttParHitsinTrack // input and output;
    );
  }

  //-------------- debug printout
  if (istampa >= 2) {
    bool tkeepit[10];
    tkeepit[0] = true;
    Short_t nSttSkewHitsinTrack[10], nSciTilHitsinTrack[10];
    Short_t ListSttSkewHitsinTrack[100];
    Double_t KAPPA[10];
    KAPPA[0] = 0.;
    nSttSkewHitsinTrack[0] = 0;
    nSciTilHitsinTrack[0] = *(InOut->nSciTilHitsinTrack);

    cout << "\tstampa in CTFind..2.cxx, dopo ordering :" << endl;
    Print2.stampetta2(tkeepit, ListMvdPixelHitsinTrack, ListMvdStripHitsinTrack, ListSttParHitsinTrack, ListSttSkewHitsinTrack, InOut->ListSciTilHitsinTrack, &nMvdPixelHitsinTrack,
                      &nMvdStripHitsinTrack, &nSttParHitsinTrack,

                      nSttSkewHitsinTrack, nSciTilHitsinTrack, 1, // questo e' nTotCand, cioe' 1
                      -1,                                         // print all candidates;
                      InOut->maxmvdpixelhitsintrack, InOut->maxmvdstriphitsintrack, InOut->maxscitilhitsintrack, InOut->maxstthitsintrack, &R, &Ox, &Oy,
                      InOut->Fi_initial_helix_referenceframe, KAPPA);
  }
  //-------------- end debug printout

  return true;
};

//----------end of function PndTrkCTFindTrackInXY2::FindTrackInXYProjection

//----------begin of function PndTrkCTFindTrackInXY2::OrderingUsingConformal

void PndTrkCTFindTrackInXY2::OrderingUsingConformal(Short_t Charge,     // input;
                                                    Double_t info[][7], // input;
                                                    Int_t nHits,        // input;
                                                    Double_t oX,        // input;
                                                    Double_t oY,        // input;
                                                    Short_t *ListHits   // input and output (ordered);
)
{

  Short_t i, j, tmp[nHits];
  Double_t aaa, b1, U[nHits], V[nHits];

  PndTrkMergeSort MergeSort;

  //  here there is the ordering of the hits, NOT under the assumption that the circumference
  //  in XY goes through  Trajectory_Start.
  //  Moreover, the code before is supposed to have selected trajectories in XY with (fOx,fOy)
  //  farther from (0,0) by > 0.9 * RminStrawDetector/2 and consequently fOx and fOy are not both 0.
  //  The scheme for the ordering of the hit is as follows :
  //  1)  order hits by increasing U or V of the conformal mapping; see Gianluigi's Logbook page 283;
  //  2)  find the charge of the track by checking if it is closest to the center in XY
  //	the first or the last of the ordered hits.
  //  3)  in case, invert the ordering of U, V and ListHits such that the first hits in the
  //	list are always those closer to the Trajectory_Start.

  //   ordering of the hits

  aaa = atan2(oY, oX); // atan2 defined between -PI and PI.

  // the following statement is necessary since for unknown reason the root interpreter
  // gives a weird error when using PI directly in the if statement below!!!!!!! I lost
  // 2 hours trying to figure this out!
  b1 = PI / 4.;

  if ((aaa > b1 && aaa < 3. * b1) || (aaa > -3. * b1 && aaa < -b1)) { // use U as ordering variable;
    //[case 1 or 3 Gianluigi's Logbook page 285].
    for (j = 0; j < nHits; j++) {
      U[j] = info[ListHits[j]][0] / (info[ListHits[j]][0] * info[ListHits[j]][0] + info[ListHits[j]][1] * info[ListHits[j]][1]);
    }
    MergeSort.Merge_Sort2(nHits, U, ListHits);

    if ((aaa > b1 && aaa < 3. * b1)) { //  case #1;
      if (Charge == -1) {
        // inverting the order of the hits.
        for (i = 0; i < nHits; i++) {
          tmp[i] = ListHits[nHits - 1 - i];
        }
        for (i = 0; i < nHits; i++) {
          ListHits[i] = tmp[i];
        }
      }
    } else { //  case # 3.
      if (Charge == 1) {
        // inverting the order of the hits.
        for (i = 0; i < nHits; i++) {
          tmp[i] = ListHits[nHits - 1 - i];
        }
        for (i = 0; i < nHits; i++) {
          ListHits[i] = tmp[i];
        }
      } // end of  if( Charge ==1)
    }   // end of  if((aaa>b1&&aaa<3.*b1))

  } else { // use V as ordering variable [case 2 or 4 Gianluigi's Logbook page 285].
    for (j = 0; j < nHits; j++) {
      V[j] = info[ListHits[j]][1] / (info[ListHits[j]][0] * info[ListHits[j]][0] + info[ListHits[j]][1] * info[ListHits[j]][1]);
    }
    MergeSort.Merge_Sort2(nHits, V, ListHits);

    if ((aaa <= -3. * b1 || aaa >= 3. * b1)) { //  case #2;
      if (Charge == -1) {
        // inverting the order of the hits.
        for (i = 0; i < nHits; i++) {
          tmp[i] = ListHits[nHits - 1 - i];
        }
        for (i = 0; i < nHits; i++) {
          ListHits[i] = tmp[i];
        }
      }
    } else { //  case # 4.
      if (Charge == 1) {
        // inverting the order of the hits.
        for (i = 0; i < nHits; i++) {
          tmp[i] = ListHits[nHits - 1 - i];
        }
        for (i = 0; i < nHits; i++) {
          ListHits[i] = tmp[i];
        }
      }
    }

  } //  end of   if((aaa>b1&& ....

  return;
}
//----------end of function PndTrkCTFindTrackInXY2::OrderingUsingConformal

//----------begin of function PndTrkCTFindTrackInXY2::OrderingUsingFi

void PndTrkCTFindTrackInXY2::OrderingUsingFi(Short_t Charge,     // input;
                                             Double_t info[][7], // input;
                                             Int_t nHits,        // input;
                                             Double_t oX,        // input;
                                             Double_t oY,        // input;
                                             Short_t *ListHits   // input and output (ordered);
)
{

  Short_t iaux[nHits], j;
  Double_t fi[nHits], Fi0;

  PndTrkMergeSort MergeSort;

  //  here there is the ordering of the hits under the assumption that the circumference
  //  in XY goes through  (0,0).

  //   ordering of the hits

  Fi0 = atan2(-oY, -oX); // atan2 defined between -PI and PI.

  // the following statement is necessary since for unknown reason the root interpreter
  if (Charge < 0) { // particle rotates counterclockwise (beam direction along Z);
    for (j = 0; j < nHits; j++) {
      fi[j] = atan2(info[ListHits[j]][1] - oY, info[ListHits[j]][0] - oX);
      if (fi[j] < Fi0)
        fi[j] += 2. * PI;
      if (fi[j] < Fi0)
        fi[j] = Fi0;
    }
    MergeSort.Merge_Sort2(nHits, fi, ListHits);

  } else { // particle rotates clockwise;
    for (j = 0; j < nHits; j++) {
      fi[j] = atan2(info[ListHits[j]][1] - oY, info[ListHits[j]][0] - oX);
      if (fi[j] > Fi0)
        fi[j] -= 2. * PI;
      if (fi[j] > Fi0)
        fi[j] = Fi0;
    }
    MergeSort.Merge_Sort2(nHits, fi, ListHits);
    // it is necessary now to invert the order;
    for (j = 0; j < nHits; j++) {
      iaux[j] = ListHits[nHits - j - 1];
    }

    for (j = 0; j < nHits; j++) {
      ListHits[j] = iaux[j];
    }
  } // end of if( Charge <0 )

  return;
}
//----------end of function PndTrkCTFindTrackInXY2::OrderingUsingFi

//----------begin of function PndTrkCTFindTrackInXY2::OrderingUsingR

void PndTrkCTFindTrackInXY2::OrderingUsingR(Double_t info[][7], // input;
                                            Int_t nHits,        // input;
                                            Short_t *ListHits   // input and output (ordered);
)
{
  Short_t i, iaux;
  // j, //[R.K. 01/2017] unused variable?
  // ipar, //[R.K. 01/2017] unused variable?
  // iskew; //[R.K. 01/2017] unused variable?
  Double_t aux, auxR2[nHits], distq1, distq2;

  PndTrkMergeSort MergeSort;

  //     ordering all the hits belonging to the candidate track, by increasing fR;
  //     forming the new track with Mvd+Stt hits

  for (i = 0; i < nHits; i++) {
    auxR2[i] = info[ListHits[i]][0] * info[ListHits[i]][0] + info[ListHits[i]][1] * info[ListHits[i]][1];
  }

  //  ordering the Stt Hits
  MergeSort.Merge_Sort2(nHits, auxR2, ListHits);

  // take care of the case when 2 hits ave the same distance from (0,0). this case can
  //  happen (as a difference with the Ordering with the Conformal method) typically
  // when the last 2 hits lie at the boundary of the outer axial layer OR the first 2 hits lie at the boundary
  // of the inner axial layer ;

  // case of the two first hits at the same R : decide order based on proximity to the 3 hit;
  if (fabs(auxR2[0] - auxR2[1]) < 0.1) {
    distq1 = (info[ListHits[0]][0] - info[ListHits[2]][0]) * (info[ListHits[0]][0] - info[ListHits[2]][0]) +
             (info[ListHits[0]][1] - info[ListHits[2]][1]) * (info[ListHits[0]][1] - info[ListHits[2]][1]);
    distq2 = (info[ListHits[1]][0] - info[ListHits[2]][0]) * (info[ListHits[1]][0] - info[ListHits[2]][0]) +
             (info[ListHits[1]][1] - info[ListHits[2]][1]) * (info[ListHits[1]][1] - info[ListHits[2]][1]);

    if (distq1 < distq2) { // exchange place of hits;
      iaux = ListHits[0];
      ListHits[0] = ListHits[1];
      ListHits[1] = iaux;
      aux = auxR2[0];
      auxR2[0] = auxR2[1];
      auxR2[1] = aux;
    } // end of if(distq1 < distq2 )
  }

  for (i = 2; i < nHits; i++) {
    if (auxR2[i] == auxR2[i - 1]) {
      // ambiguity case; decide the order based on which of the two hits (hit # i and # i-1 )is closer the
      // hit # i-2;
      distq1 = (info[ListHits[i]][0] - info[ListHits[i - 2]][0]) * (info[ListHits[i]][0] - info[ListHits[i - 2]][0]) +
               (info[ListHits[i]][1] - info[ListHits[i - 2]][1]) * (info[ListHits[i]][1] - info[ListHits[i - 2]][1]);

      distq2 = (info[ListHits[i - 1]][0] - info[ListHits[i - 2]][0]) * (info[ListHits[i - 1]][0] - info[ListHits[i - 2]][0]) +
               (info[ListHits[i - 1]][1] - info[ListHits[i - 2]][1]) * (info[ListHits[i - 1]][1] - info[ListHits[i - 2]][1]);

      if (distq1 < distq2) {
        // exchange the order;
        iaux = ListHits[i];
        ListHits[i] = ListHits[i - 1];
        ListHits[i - 1] = iaux;
        aux = auxR2[i];
        auxR2[i] = auxR2[i - 1];
        auxR2[i - 1] = aux;
      }
    }
  } // end of for(i=0; i<nHits; i++)

  return;
}

//----------end of function PndTrkCTFindTrackInXY2::OrderingUsingR

//----------begin of function PndTrkCTFindTrackInXY2::TrkAssociatedParallelHitsToHelix5

Short_t PndTrkCTFindTrackInXY2::TrkAssociatedParallelHitsToHelix5(Short_t *auxListHitsinTrack, bool *InclusionListStt, Double_t Fi_low, Double_t Fi_up, Double_t info[][7],
                                                                  Short_t *ListSttParHits, Int_t NhitsParallel, Double_t Oxx, Double_t Oyy, Double_t Rr, Double_t strawradius)
{

  Short_t i;

  Short_t nAssociatedHits;

  Double_t angle, dx, dy, distance,
    //           NTIMES=1.5;   //   number of Straw radia allowed in association.
    NTIMES = 2.; //   number of Straw radia allowed in association.

  nAssociatedHits = 0;
  //   find the Hits belonging to this Track.

  for (i = 0; i < NhitsParallel; i++) {
    if (!InclusionListStt[ListSttParHits[i]])
      continue;
    // check if the hit position is near the circle of the Helix found by the fit
    dx = -Oxx + info[ListSttParHits[i]][0];
    dy = -Oyy + info[ListSttParHits[i]][1];
    angle = atan2(dy, dx);
    if (angle < 0.)
      angle += 2. * PI;
    if (angle < 0.)
      angle = 0.;
    distance = sqrt(dx * dx + dy * dy);

    if (fabs(Rr - distance) > NTIMES * strawradius)
      continue;
    if (angle < Fi_low)
      angle += 2. * PI;
    if (angle > Fi_up)
      continue;
    auxListHitsinTrack[nAssociatedHits] = ListSttParHits[i];
    nAssociatedHits++;
  } // end for(i=0; i<NhitsParallel;i++)

  return nAssociatedHits;
};

//----------end of function PndTrkCTFindTrackInXY2::TrkAssociatedParallelHitsToHelix5

//----------begin of function PndTrkCTFindTrackInXY2::TrkAssociatedParallelHitsToHelix6

Short_t PndTrkCTFindTrackInXY2::TrkAssociatedParallelHitsToHelix6(Short_t *auxListHitsinTrack, bool *InclusionListStt, Double_t Fi_low, Double_t Fi_up, Double_t info[][7],
                                                                  Short_t *ListSttParHits, Int_t NhitsParallel, Double_t Oxx, Double_t Oyy, Double_t Rr, Double_t maximum_distance)
{

  Short_t i;

  Short_t nAssociatedHits;

  Double_t angle, dx, dy, distance;

  nAssociatedHits = 0;
  //   find the Hits belonging to this Track.

  for (i = 0; i < NhitsParallel; i++) {
    if (!InclusionListStt[ListSttParHits[i]])
      continue;
    // check if the hit position is near the circle of the Helix found by the fit
    dx = -Oxx + info[ListSttParHits[i]][0];
    dy = -Oyy + info[ListSttParHits[i]][1];
    angle = atan2(dy, dx);
    if (angle < 0.)
      angle += 2. * PI;
    if (angle < 0.)
      angle = 0.;
    distance = sqrt(dx * dx + dy * dy);

    if (fabs(Rr - distance) > maximum_distance)
      continue;
    if (angle < Fi_low)
      angle += 2. * PI;
    if (angle > Fi_up)
      continue;
    auxListHitsinTrack[nAssociatedHits] = ListSttParHits[i];
    nAssociatedHits++;
  } // end for(i=0; i<NhitsParallel;i++)

  return nAssociatedHits;
};

//----------end of function PndTrkCTFindTrackInXY2::TrkAssociatedParallelHitsToHelix6
ClassImp(PndTrkCTFindTrackInXY2);
