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

#include "PndTrkSttConformalFilling.h"
#include "PndTrkVectors.h"
#include <cmath>
#include <iostream>
// Root includes
#include "TROOT.h"

using namespace std;

#define PI 3.141592654

//----------begin of function PndTrkSttConformalFilling::BoxConformalFilling

void PndTrkSttConformalFilling::BoxConformalFilling(Vec<Short_t> &FiConformalIndex, Vec<Short_t> &HitsinBoxConformal, bool *InclusionListStt, Double_t infoparalConformal[][5],
                                                    Short_t *ListSttParHits, Short_t MAXHITSINCELL, Vec<Short_t> &nBoxConformal, Short_t NFIDIVCONFORMAL, Int_t Nparal,
                                                    Short_t NRDIVCONFORMAL, Double_t *radiaConf, Vec<Short_t> &RConformalIndex)
{

  Short_t bi_index, i, iFi, iR, j, tri_index;

  Double_t Fi;

  // initialize nBoxConformal (each event);
  for (j = 0; j < NFIDIVCONFORMAL * NRDIVCONFORMAL; j++) {
    nBoxConformal[j] = 0;
  }

  // fill the nBoxConformal and the list contained in HitsinBoxConformal with
  // all the axial straw hits;
  for (i = 0; i < Nparal; i++) {

    if (!InclusionListStt[ListSttParHits[i]])
      continue;
    Fi = atan2(infoparalConformal[ListSttParHits[i]][1], infoparalConformal[ListSttParHits[i]][0]);
    if (Fi < 0.)
      Fi += 2. * PI;
    iFi = (Short_t)(0.5 * NFIDIVCONFORMAL * Fi / PI);
    if (iFi > NFIDIVCONFORMAL) {
      iFi = NFIDIVCONFORMAL;
    } else if (iFi < 0) {
      iFi = 0;
    }

    Double_t RRR = sqrt(infoparalConformal[ListSttParHits[i]][0] * infoparalConformal[ListSttParHits[i]][0] +
                        infoparalConformal[ListSttParHits[i]][1] * infoparalConformal[ListSttParHits[i]][1]);

    for (j = NRDIVCONFORMAL - 1, iR = 0; j > 0; j--) {
      if (RRR > radiaConf[j]) {
        iR = j;
        break;
      }
    }

    bi_index = iR * NFIDIVCONFORMAL + iFi;
    tri_index = nBoxConformal[bi_index] * NRDIVCONFORMAL * NFIDIVCONFORMAL + bi_index;
    if (nBoxConformal[bi_index] >= MAXHITSINCELL) {
      cout << "Warning from PndTrkSttConformalFilling::BoxConformalFilling\t:"
           << "\n\tcontent in nBoxConformal[" << iR << "][" << iFi << "] has reached the Max allowed value = " << MAXHITSINCELL << endl;
      continue;
    }
    HitsinBoxConformal[tri_index] = (Short_t)ListSttParHits[i];
    nBoxConformal[bi_index]++;
    RConformalIndex[ListSttParHits[i]] = iR;
    FiConformalIndex[ListSttParHits[i]] = iFi;

  } // end of for(i = 0; i< Nparal ; i++)

  return;
}
//----------end of function PndTrkSttConformalFilling::BoxConformalFilling

//----------begin of function PndTrkSttConformalFilling::FromXYtoConformal

void PndTrkSttConformalFilling::FromXYtoConformal(Double_t trajectory_vertex[2], Double_t info[][7], Short_t *ListSttParHits, Int_t Nparal, Double_t infoparalConformal[][5],
                                                  Double_t STRAWRADIUS)
{

  //   do the transformation in the conformal space :  u= x/(x**2+y**2), v= y/(x**2+y**2) for each hit from parallel
  //   straws;  also the equidrift radius changes.

  //

  Short_t i;

  Double_t gamma, r, x, y;

  for (i = 0; i < Nparal; i++) {
    x = info[ListSttParHits[i]][0] - trajectory_vertex[0];
    y = info[ListSttParHits[i]][1] - trajectory_vertex[1];
    r = info[ListSttParHits[i]][3];
    gamma = x * x + y * y - r * r;
    infoparalConformal[ListSttParHits[i]][0] = x / gamma;
    infoparalConformal[ListSttParHits[i]][1] = y / gamma;
    infoparalConformal[ListSttParHits[i]][2] = r / fabs(gamma);
    //  n. of the Hit (in the original order)
    infoparalConformal[ListSttParHits[i]][3] = ListSttParHits[i];
    infoparalConformal[ListSttParHits[i]][4] = STRAWRADIUS / fabs(gamma);
  }

  return;
}

//----------end of function PndTrkSttConformalFilling::FromXYtoConformal

ClassImp(PndTrkSttConformalFilling)
