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

#include "PndSdsMCPoint.h"
#include "PndSciTPoint.h"
#include "PndSciTHit.h"
#include "PndTrkComparisonMCtruth.h"
#include "PndMCTrack.h"
#include "PndTrkVectors.h"

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"

#include <iostream>
#include <cmath>

using namespace std;
//----------begin of function PndTrkComparisonMCtruth::AssociateFoundTrackstoMCquater
// inizio cambio_in_perl ;

void PndTrkComparisonMCtruth::AssociateFoundTrackstoMCquater(
  Double_t BFIELD, Double_t CVEL, Vec<Short_t> *daTrackFoundaTrackMC, TClonesArray *fMCTrackArray, Vec<Int_t> *FromPixeltoMCTrack, Vec<Int_t> *FromStriptoMCTrack,
  Vec<int> *FromSciTiltoMCTrackList, Vec<bool> *keepit, Vec<Double_t> *info, Vec<Short_t> *ListSttParHitsinTrack, Vec<Short_t> *ListMvdPixelHitsinTrack,
  Vec<Short_t> *ListSciTilHitsinTrack, Vec<Short_t> * /*ListSttSkewHitsinTrack*/, //[R.K. 9/2018] unused
  Vec<Short_t> *ListMvdStripHitsinTrack, int MAXMVDPIXELHITSINTRACK, int MAXMVDSTRIPHITSINTRACK, int MAXSCITILHITSINTRACK, int MAXSTTHITSINTRACK, Vec<int> *nFromSciTiltoMCTrack,
  Vec<Short_t> *nSttParHitsinTrack, int nMCTracks, Vec<Short_t> *nMvdPixelHitsinTrack, Short_t /*nSciTilHits*/, //[R.K. 9/2018] unused
  Vec<Short_t> *nSciTilHitsinTrack, Vec<Short_t> *nSttSkewHitsinTrack, Vec<Short_t> *nMvdStripHitsinTrack, Short_t nTracksFoundSoFar, Vec<Double_t> *Ox, Vec<Double_t> *Oy,
  Vec<Double_t> *R, Vec<Double_t> *X1, Vec<Double_t> *Y1, Vec<Double_t> *X2, Vec<Double_t> *Y2, Vec<Double_t> *X3, Vec<Double_t> *Y3, int /*IVOLTE*/ //[R.K. 9/2018] unused

)
{
  // fine cambio_in_perl ;

  int tmp_dim = MAXSTTHITSINTRACK + MAXMVDPIXELHITSINTRACK + MAXMVDSTRIPHITSINTRACK;

  int tmp_dim2;
  // protection in case nTracksFoundSoFar = 0; the dimension of vector must be
  // >= 1;
  if (nTracksFoundSoFar > 0) {
    tmp_dim2 = nTracksFoundSoFar;
  } else {
    tmp_dim2 = 1;
  }

  bool firstime, flaggo;
  Short_t TMPinclusionMC[tmp_dim2 * tmp_dim];
  Vec<Short_t> inclusionMC(TMPinclusionMC, tmp_dim2 * tmp_dim, "inclusionMC");

  Short_t TMPinclusionExp[tmp_dim2];
  Vec<Short_t> inclusionExp(TMPinclusionExp, tmp_dim2, "inclusionExp");

  Short_t TMPntoMCtrack[tmp_dim2];
  Vec<Short_t> ntoMCtrack(TMPntoMCtrack, tmp_dim2, "ntoMCtrack");

  Short_t TMPtoMCtrackfrequency[tmp_dim2 * MAXSTTHITSINTRACK];
  Vec<Short_t> toMCtrackfrequency(TMPtoMCtrackfrequency, tmp_dim2 * MAXSTTHITSINTRACK, "toMCtrackfrequency");

  Short_t i, j, jtemp, jexp; // , nmid; //[R.K. 01/2017] unused variable?

  Short_t itemp, massimo;
  Short_t TMPtoMCtracklist[tmp_dim2 * MAXSTTHITSINTRACK];
  Vec<Short_t> toMCtracklist(TMPtoMCtracklist, tmp_dim2 * MAXSTTHITSINTRACK, "toMCtracklist");

  int nindex;
  Int_t enne;
  Double_t dx, Cx, Cy, Rr, alfa, beta, gamma, minimo;
  // non cambiare senno' cambiaperl non funziona !
  Double_t TMPtanlow[tmp_dim2];
  Vec<Double_t> tanlow(TMPtanlow, tmp_dim2, "tanlow");

  Double_t TMPtanmid[tmp_dim2];
  Vec<Double_t> tanmid(TMPtanmid, tmp_dim2, "tanmid");

  Double_t TMPtanup[tmp_dim2];
  Vec<Double_t> tanup(TMPtanup, tmp_dim2, "tanup");

  Double_t TMPtoMCtrackdistance[tmp_dim2 * MAXSTTHITSINTRACK];
  Vec<Double_t> toMCtrackdistance(TMPtoMCtrackdistance, tmp_dim2 * MAXSTTHITSINTRACK, "toMCtrackdistance");

  for (i = 0; i < nTracksFoundSoFar; i++) {
    daTrackFoundaTrackMC->at(i) = -1;
    if (!keepit->at(i))
      continue;
    inclusionExp[i] = true;
    for (j = 0; j < nSttParHitsinTrack->at(i) + nSttSkewHitsinTrack->at(i) + nMvdPixelHitsinTrack->at(i) + nMvdStripHitsinTrack->at(i); j++) {
      inclusionMC[i * tmp_dim + j] = true;
    }

    //--- find the minimum, middle, end Tan(angle) of this track, for the comparison later

    tanlow[i] = 0.;
    tanmid[i] = 0.;
    tanup[i] = 0.;
    int nn = nSttParHitsinTrack->at(i) + nMvdPixelHitsinTrack->at(i) + nMvdStripHitsinTrack->at(i);
    if (nn > 2 || nn == 1) {
      dx = X1->at(i) - Ox->at(i);
      if (fabs(dx) > 1.e-10) {
        tanlow[i] = (Y1->at(i) - Oy->at(i)) / dx;
      } else {
        tanlow[i] = 999999.;
      }

      dx = X2->at(i) - Ox->at(i);
      if (fabs(dx) > 1.e-10) {
        tanmid[i] = (Y2->at(i) - Oy->at(i)) / dx;
      } else {
        tanmid[i] = 999999.;
      }

      dx = X3->at(i) - Ox->at(i);
      if (fabs(dx) > 1.e-10) {
        tanup[i] = (Y3->at(i) - Oy->at(i)) / dx;
      } else {
        tanup[i] = 999999.;
      }

    } else if (nn == 2) { // continuation  of  if( nn > 2 || nn == 1)
      dx = X1->at(i) - Ox->at(i);
      if (fabs(dx) > 1.e-10) {
        tanlow[i] = (Y1->at(i) - Oy->at(i)) / dx;
      } else {
        tanlow[i] = 999999.;
      }

      dx = X2->at(i) - Ox->at(i);
      if (fabs(dx) > 1.e-10) {
        tanmid[i] = (Y2->at(i) - Oy->at(i)) / dx;
      } else {
        tanmid[i] = 999999.;
      }

      tanup[i] = tanmid[i];
    }

    //-----------------------------

  } // end of for(i=0; i<nTracksFoundSoFar;i++)

  for (jexp = 0; jexp < nTracksFoundSoFar; jexp++) {

    if (!keepit->at(jexp))
      continue;

    firstime = true;
    ntoMCtrack[jexp] = 0;

    // prima  gli hits paralleli ---------------------

    for (i = 0; i < nSttParHitsinTrack->at(jexp); i++) {
      nindex = jexp * MAXSTTHITSINTRACK + i;
      //  enne = MC track alla quale lo hit e' associato.
      enne = (Int_t)(info->at(ListSttParHitsinTrack->at(nindex) * 7 + 6) + 0.01);

      if (enne < 0)
        continue; //  hit not associated to any MC track; noise hit.

      if (firstime) {
        toMCtracklist[jexp * MAXSTTHITSINTRACK + 0] = enne;
        toMCtrackfrequency[jexp * MAXSTTHITSINTRACK + 0] = 1;
        firstime = false;

        getMCInfo(BFIELD, CVEL, &Cx, &Cy, fMCTrackArray, enne, &Rr);
        if (Rr < 0.) {
          toMCtrackdistance[jexp * MAXSTTHITSINTRACK + 0] = -1.;
        } else {
          alfa = -2. * Cx;
          beta = -2. * Cy;
          gamma = Cx * Cx + Cy * Cy - Rr * Rr;
          toMCtrackdistance[jexp * MAXSTTHITSINTRACK + 0] = FindDistance(Ox->at(jexp), Oy->at(jexp), R->at(jexp), tanlow[jexp], tanmid[jexp], tanup[jexp], alfa, beta, gamma);
        }
        ntoMCtrack[jexp] = 1;

      } else { // continuation of  if(firstime)

        flaggo = true;
        for (j = 0; j < ntoMCtrack[jexp]; j++) {
          if (enne == toMCtracklist[jexp * MAXSTTHITSINTRACK + j]) {
            toMCtrackfrequency[jexp * MAXSTTHITSINTRACK + j]++;
            flaggo = false;
            break;
          }
        }
        if (flaggo) {
          toMCtracklist[jexp * MAXSTTHITSINTRACK + ntoMCtrack[jexp]] = enne;
          toMCtrackfrequency[jexp * MAXSTTHITSINTRACK + ntoMCtrack[jexp]] = 1;
          getMCInfo(BFIELD, CVEL, &Cx, &Cy, fMCTrackArray, enne, &Rr);
          if (Rr < 0.) {
            toMCtrackdistance[jexp * MAXSTTHITSINTRACK + ntoMCtrack[jexp]] = -1.;
          } else {
            alfa = -2. * Cx;
            beta = -2. * Cy;
            gamma = Cx * Cx + Cy * Cy - Rr * Rr;
            toMCtrackdistance[jexp * MAXSTTHITSINTRACK + ntoMCtrack[jexp]] =
              FindDistance(Ox->at(jexp), Oy->at(jexp), R->at(jexp), tanlow[jexp], tanmid[jexp], tanup[jexp], alfa, beta, gamma);
          }
          ntoMCtrack[jexp]++;
        } // end of if(flaggo)
      }

    } //  end of for(i=0; i<nSttParHitsinTrack->at(jexp); i++)

    // poi i pixel -------------------------------------------

    for (i = 0; i < nMvdPixelHitsinTrack->at(jexp); i++) {
      nindex = jexp * MAXMVDPIXELHITSINTRACK + i;

      enne = FromPixeltoMCTrack->at(ListMvdPixelHitsinTrack->at(nindex));
      if (enne < 0)
        continue; //  hit not associated to any MC track; noise hit.

      if (firstime) {
        toMCtracklist[jexp * MAXSTTHITSINTRACK + 0] = enne;
        toMCtrackfrequency[jexp * MAXSTTHITSINTRACK + 0] = 1;
        firstime = false;

        getMCInfo(BFIELD, CVEL, &Cx, &Cy, fMCTrackArray, enne, &Rr);
        if (Rr < 0.) {
          toMCtrackdistance[jexp * MAXSTTHITSINTRACK + 0] = -1.;
        } else {
          alfa = -2. * Cx;
          beta = -2. * Cy;
          gamma = Cx * Cx + Cy * Cy - Rr * Rr;
          toMCtrackdistance[jexp * MAXSTTHITSINTRACK + 0] = FindDistance(Ox->at(jexp), Oy->at(jexp), R->at(jexp), tanlow[jexp], tanmid[jexp], tanup[jexp], alfa, beta, gamma);
        }
        ntoMCtrack[jexp] = 1;

      } else { // continuation of  if(firstime)

        flaggo = true;
        for (j = 0; j < ntoMCtrack[jexp]; j++) {
          if (enne == toMCtracklist[jexp * MAXSTTHITSINTRACK + j]) {
            toMCtrackfrequency[jexp * MAXSTTHITSINTRACK + j]++;
            flaggo = false;
            break;
          }
        }
        if (flaggo) {
          toMCtracklist[jexp * MAXSTTHITSINTRACK + ntoMCtrack[jexp]] = enne;
          toMCtrackfrequency[jexp * MAXSTTHITSINTRACK + ntoMCtrack[jexp]] = 1;
          getMCInfo(BFIELD, CVEL, &Cx, &Cy, fMCTrackArray, enne, &Rr);
          if (Rr < 0.) {
            toMCtrackdistance[jexp * MAXSTTHITSINTRACK + ntoMCtrack[jexp]] = -1.;
          } else {
            alfa = -2. * Cx;
            beta = -2. * Cy;
            gamma = Cx * Cx + Cy * Cy - Rr * Rr;
            toMCtrackdistance[jexp * MAXSTTHITSINTRACK + ntoMCtrack[jexp]] =
              FindDistance(Ox->at(jexp), Oy->at(jexp), R->at(jexp), tanlow[jexp], tanmid[jexp], tanup[jexp], alfa, beta, gamma);
          }
          ntoMCtrack[jexp]++;
        } // end of if(flaggo)
      }

    } //  end of for(i=0; i<nMvdPixelHitsinTrack->at(jexp); i++)

    // le strip -------------------------------------------

    for (i = 0; i < nMvdStripHitsinTrack->at(jexp); i++) {
      nindex = jexp * MAXMVDSTRIPHITSINTRACK + i;
      enne = FromStriptoMCTrack->at(ListMvdStripHitsinTrack->at(nindex));
      if (enne < 0)
        continue; //  hit not associated to any MC track; noise hit.

      if (firstime) {
        toMCtracklist[jexp * MAXSTTHITSINTRACK + 0] = enne;
        toMCtrackfrequency[jexp * MAXSTTHITSINTRACK + 0] = 1;
        firstime = false;

        getMCInfo(BFIELD, CVEL, &Cx, &Cy, fMCTrackArray, enne, &Rr);
        if (Rr < 0.) {
          toMCtrackdistance[jexp * MAXSTTHITSINTRACK + 0] = -1.;
        } else {
          alfa = -2. * Cx;
          beta = -2. * Cy;
          gamma = Cx * Cx + Cy * Cy - Rr * Rr;
          toMCtrackdistance[jexp * MAXSTTHITSINTRACK + 0] = FindDistance(Ox->at(jexp), Oy->at(jexp), R->at(jexp), tanlow[jexp], tanmid[jexp], tanup[jexp], alfa, beta, gamma);
        }
        ntoMCtrack[jexp] = 1;

      } else { // continuation of  if(firstime)

        flaggo = true;
        for (j = 0; j < ntoMCtrack[jexp]; j++) {
          if (enne == toMCtracklist[jexp * MAXSTTHITSINTRACK + j]) {
            toMCtrackfrequency[jexp * MAXSTTHITSINTRACK + j]++;
            flaggo = false;
            break;
          }
        }
        if (flaggo) {
          toMCtracklist[jexp * MAXSTTHITSINTRACK + ntoMCtrack[jexp]] = enne;
          toMCtrackfrequency[jexp * MAXSTTHITSINTRACK + ntoMCtrack[jexp]] = 1;
          getMCInfo(BFIELD, CVEL, &Cx, &Cy, fMCTrackArray, enne, &Rr);
          if (Rr < 0.) {
            toMCtrackdistance[jexp * MAXSTTHITSINTRACK + ntoMCtrack[jexp]] = -1.;
          } else {
            alfa = -2. * Cx;
            beta = -2. * Cy;
            gamma = Cx * Cx + Cy * Cy - Rr * Rr;
            toMCtrackdistance[jexp * MAXSTTHITSINTRACK + ntoMCtrack[jexp]] =
              FindDistance(Ox->at(jexp), Oy->at(jexp), R->at(jexp), tanlow[jexp], tanmid[jexp], tanup[jexp], alfa, beta, gamma);
          }
          ntoMCtrack[jexp]++;
        } // end of if(flaggo)
      }

    } //  end of for(i=0; i<nMvdStripHitsinTrack->at(jexp); i++)

    // the SciTil hits -------------------------------------------

    for (i = 0; i < nSciTilHitsinTrack->at(jexp); i++) {
      nindex = jexp * MAXSCITILHITSINTRACK + i;
      int SciTilTilenumber = ListSciTilHitsinTrack->at(nindex);
      // nFromSciTiltoMCTrack is zero in case this SciTil hit is not associated to any
      // MC truth track. In that case effectively the SciTil hit is ignored.
      for (int iMClinks = 0; iMClinks < nFromSciTiltoMCTrack->at(SciTilTilenumber); iMClinks++) {

        enne = FromSciTiltoMCTrackList->at(SciTilTilenumber * nMCTracks + iMClinks);
        if (firstime) {
          toMCtracklist[jexp * MAXSTTHITSINTRACK + 0] = enne;
          toMCtrackfrequency[jexp * MAXSTTHITSINTRACK + 0] = 1;
          firstime = false;

          getMCInfo(BFIELD, CVEL, &Cx, &Cy, fMCTrackArray, enne, &Rr);
          if (Rr < 0.) {
            toMCtrackdistance[jexp * MAXSTTHITSINTRACK + 0] = -1.;
          } else {
            alfa = -2. * Cx;
            beta = -2. * Cy;
            gamma = Cx * Cx + Cy * Cy - Rr * Rr;
            toMCtrackdistance[jexp * MAXSTTHITSINTRACK + 0] = FindDistance(Ox->at(jexp), Oy->at(jexp), R->at(jexp), tanlow[jexp], tanmid[jexp], tanup[jexp], alfa, beta, gamma);
          }
          ntoMCtrack[jexp] = 1;

        } else { // continuation of  if(firstime)

          flaggo = true;
          for (j = 0; j < ntoMCtrack[jexp]; j++) {
            if (enne == toMCtracklist[jexp * MAXSTTHITSINTRACK + j]) {
              toMCtrackfrequency[jexp * MAXSTTHITSINTRACK + j]++;
              flaggo = false;
              break;
            }
          }
          if (flaggo) {
            toMCtracklist[jexp * MAXSTTHITSINTRACK + ntoMCtrack[jexp]] = enne;
            toMCtrackfrequency[jexp * MAXSTTHITSINTRACK + ntoMCtrack[jexp]] = 1;
            getMCInfo(BFIELD, CVEL, &Cx, &Cy, fMCTrackArray, enne, &Rr);
            if (Rr < 0.) {
              toMCtrackdistance[jexp * MAXSTTHITSINTRACK + ntoMCtrack[jexp]] = -1.;
            } else {
              alfa = -2. * Cx;
              beta = -2. * Cy;
              gamma = Cx * Cx + Cy * Cy - Rr * Rr;
              toMCtrackdistance[jexp * MAXSTTHITSINTRACK + ntoMCtrack[jexp]] =
                FindDistance(Ox->at(jexp), Oy->at(jexp), R->at(jexp), tanlow[jexp], tanmid[jexp], tanup[jexp], alfa, beta, gamma);
            }
            ntoMCtrack[jexp]++;
          } // end of if(flaggo)
        }

      } // end of  for(int iMClinks=0; iMClinks< ...
    }   //  end of for(i=0; i<nSciTilHitsinTrack->at(jexp); i++)

  } // end of  for(jexp=0; jexp< nTracksFoundSoFar ;jexp++)

  //--------------------------------------------------------------------

  itemp = 0;
  while (itemp > -1) {
    itemp = -1;
    massimo = -1;
    minimo = 999999999999.;
    for (jexp = 0; jexp < nTracksFoundSoFar; jexp++) {
      if (!keepit->at(jexp))
        continue;
      if (!inclusionExp[jexp])
        continue;
      for (i = 0; i < ntoMCtrack[jexp]; i++) {
        if (!inclusionMC[jexp * tmp_dim + i])
          continue;
        if (toMCtrackdistance[jexp * MAXSTTHITSINTRACK + i] < -0.5)
          continue;

        if (toMCtrackfrequency[jexp * MAXSTTHITSINTRACK + i] > massimo) {
          massimo = toMCtrackfrequency[jexp * MAXSTTHITSINTRACK + i];
          minimo = toMCtrackdistance[jexp * MAXSTTHITSINTRACK + i];
          itemp = toMCtracklist[jexp * MAXSTTHITSINTRACK + i];
          jtemp = jexp;
        } else if (toMCtrackfrequency[jexp * MAXSTTHITSINTRACK + i] == massimo) {
          if (toMCtrackdistance[jexp * MAXSTTHITSINTRACK + i] < minimo) {
            minimo = toMCtrackdistance[jexp * MAXSTTHITSINTRACK + i];
            itemp = toMCtracklist[jexp * MAXSTTHITSINTRACK + i];
            jtemp = jexp;
          }
        }
      } // end of for(i=0; i< ntoMCtrack[jexp]; i++)
    }   // end of   for(jexp=0; jexp< nTracksFoundSoFar ;jexp++)

    if (itemp > -1) {
      daTrackFoundaTrackMC->at(jtemp) = itemp;
      inclusionExp[jtemp] = false;
      for (jexp = 0; jexp < nTracksFoundSoFar; jexp++) {
        if (!keepit->at(jexp))
          continue;
        for (int jk = 0; jk < ntoMCtrack[jexp]; jk++) {
          if (itemp == toMCtracklist[jexp * MAXSTTHITSINTRACK + jk]) {
            inclusionMC[jexp * tmp_dim + jk] = false;
          }
        }
      } // end of  for(jexp=0; jexp<nTracksFoundSoFar;jexp++)
    }
  } //    end while ( itemp > -1)

  return;
}

//----------end of function PndTrkComparisonMCtruth::AssociateFoundTrackstoMCquater

//  marker1 per cambioperl;
//----------begin of function PndTrkComparisonMCtruth::ComparisonwithMC

int PndTrkComparisonMCtruth::ComparisonwithMC(PndTrkComparisonMCtruth_io_Data ioData)
{

  int i, j,
    // dim1, //[R.K. 01/2017] unused variable?
    // dim2, //[R.K. 01/2017] unused variable?
    // dim3, //[R.K. 01/2017] unused variable?
    nele, ncand, nMCTracks, nMvdMCPoint;

  Double_t dista;

  const double PI = 3.141592654;

  Double_t BFIELD = ioData.Bfield;
  Vec<Short_t> Charge(ioData.Charge, ioData.MAXTRACKSPEREVENT, "Charge");
  Double_t CVEL = ioData.Cvel;
  Vec<Short_t> daTrackFoundaTrackMC(ioData.daTrackFoundaTrackMC, ioData.MAXTRACKSPEREVENT, "daTrackFoundaTrackMC");
  Double_t DIMENSIONSCITIL = ioData.DIMENSIONSciTil;
  Double_t ERRORSQPIXEL = ioData.Errorsqpixel;
  Double_t ERRORSQSTRIP = ioData.Errorsqstrip;
  Vec<Double_t> FI0(ioData.FI0, ioData.MAXTRACKSPEREVENT, "FI0");
  TClonesArray *fMCTrackArray = ioData.fMCTrackArray;
  TClonesArray *fMvdMCPointArray = ioData.fMvdMCPointArray;
  FILE *HANDLE = ioData.HANDLE;
  FILE *HANDLE2 = ioData.HANDLE2;
  Vec<Double_t> info(ioData.info, ioData.MAXSTTHITS * 7, "info");
  int istampa = ioData.istampa;
  int IVOLTE = ioData.IVOLTE;
  Vec<Double_t> KAPPA(ioData.KAPPA, ioData.MAXTRACKSPEREVENT, "KAPPA");
  Vec<bool> keepit(ioData.keepit, ioData.MAXTRACKSPEREVENT, "keepit");
  Vec<bool> InclusionListStt(ioData.InclusionListStt, ioData.MAXSTTHITS, "InclusionListStt");
  Vec<Short_t> ListMvdPixelHitsinTrack(ioData.ListMvdPixelHitsinTrack, ioData.MAXTRACKSPEREVENT * ioData.MAXMVDPIXELHITSINTRACK, "ListMvdPixelHitsinTrack");
  Vec<Short_t> ListMvdStripHitsinTrack(ioData.ListMvdStripHitsinTrack, ioData.MAXTRACKSPEREVENT * ioData.MAXMVDSTRIPHITSINTRACK, "ListMvdStripHitsinTrack");
  Vec<Short_t> ListSciTilHitsinTrack(ioData.ListSciTilHitsinTrack, ioData.MAXTRACKSPEREVENT * ioData.MAXSCITILHITSINTRACK, "ListSciTilHitsinTrack");
  Vec<Short_t> ListSttParHitsinTrack(ioData.ListSttParHitsinTrack, ioData.MAXTRACKSPEREVENT * ioData.maxstthitsintrack, "ListSttParHitsinTrack");
  Vec<Short_t> ListSttSkewHitsinTrack(ioData.ListSttSkewHitsinTrack, ioData.MAXTRACKSPEREVENT * ioData.maxstthitsintrack, "ListSttSkewHitsinTrack");
  Vec<Short_t> ListTrackCandHit(ioData.ListTrackCandHit,
                                ioData.MAXTRACKSPEREVENT * (ioData.maxstthitsintrack + ioData.MAXMVDPIXELHITSINTRACK + ioData.MAXMVDSTRIPHITSINTRACK + ioData.MAXSCITILHITSINTRACK),
                                "ListTrackCandHit");
  Vec<Short_t> ListTrackCandHitType(
    ioData.ListTrackCandHitType,
    ioData.MAXTRACKSPEREVENT * (ioData.maxstthitsintrack + ioData.MAXMVDPIXELHITSINTRACK + ioData.MAXMVDSTRIPHITSINTRACK + ioData.MAXSCITILHITSINTRACK), "ListTrackCandHitType");
  int MAXMCTRACKS = ioData.MAXMCTRACKS;
  int MAXMVDPIXELHITSINTRACK = ioData.MAXMVDPIXELHITSINTRACK;
  int MAXMVDMCPOINTS = ioData.Maxmvdmcpoints;
  int MAXMVDSTRIPHITSINTRACK = ioData.MAXMVDSTRIPHITSINTRACK;
  int MAXSCITILHITSINTRACK = ioData.MAXSCITILHITSINTRACK;
  int MAXSTTHITS = ioData.MAXSTTHITS;
  int MAXSTTHITSINTRACK = ioData.maxstthitsintrack;
  int MAXTRACKSPEREVENT = ioData.MAXTRACKSPEREVENT;

  //  protection against dimension 0 cases;
  /*
   ioData.nTotalCandidates >0 ? dim1 = ioData.nTotalCandidates : dim1 = 1;
   ioData.nMvdPixelHit >0 ? dim2 = ioData.nMvdPixelHit : dim2 = 1;
   ioData.nMvdStripHit >0 ? dim3 = ioData.nMvdStripHit : dim3 = 1;
   ioData.nSttHit >0 ? dim4 = ioData.nSttHit : dim4 = 1;
  */

  Vec<Short_t> MCMvdPixelAloneList(ioData.MCMvdPixelAloneList, ioData.nTotalCandidates * ioData.nMvdPixelHit, "MCMvdPixelAloneList");
  Vec<Short_t> MCMvdStripAloneList(ioData.MCMvdStripAloneList, ioData.nTotalCandidates * ioData.nMvdStripHit, "MCMvdStripAloneList");
  Vec<Short_t> MCParalAloneList(ioData.MCParalAloneList, ioData.MAXTRACKSPEREVENT * ioData.nSttHit, "MCParalAloneList");
  Vec<Short_t> MCSkewAloneList(ioData.MCSkewAloneList, ioData.MAXTRACKSPEREVENT * ioData.nSttHit, "MCSkewAloneList");
  Vec<Double_t> MCSkewAloneX(ioData.MCSkewAloneX, ioData.MAXSTTHITS, "MCSkewAloneX");

  Vec<Double_t> MCSkewAloneY(ioData.MCSkewAloneY, ioData.MAXSTTHITS, "MCSkewAloneY");

  Vec<Short_t> MvdPixelCommonList(ioData.MvdPixelCommonList, ioData.nTotalCandidates * ioData.MAXMVDPIXELHITSINTRACK, "MvdPixelCommonList");
  Vec<Short_t> MvdPixelSpuriList(ioData.MvdPixelSpuriList, ioData.nTotalCandidates * ioData.MAXMVDPIXELHITSINTRACK, "MvdPixelSpuriList");
  Vec<Short_t> MvdStripCommonList(ioData.MvdStripCommonList, ioData.nTotalCandidates * ioData.MAXMVDSTRIPHITSINTRACK, "MvdStripCommonList");
  Vec<Short_t> MvdStripSpuriList(ioData.MvdStripSpuriList, ioData.nTotalCandidates * ioData.MAXMVDSTRIPHITSINTRACK, "MvdStripSpuriList");
  Vec<Short_t> nHitsInMCTrack(ioData.nHitsInMCTrack, ioData.MAXTRACKSPEREVENT, "nHitsInMCTrack");
  Vec<Short_t> nMCMvdPixelAlone(ioData.nMCMvdPixelAlone, ioData.nTotalCandidates, "nMCMvdPixelAlone");
  Vec<Short_t> nMCMvdStripAlone(ioData.nMCMvdStripAlone, ioData.nTotalCandidates, "nMCMvdStripAlone");
  Vec<Short_t> nMCParalAlone(ioData.nMCParalAlone, ioData.MAXTRACKSPEREVENT, "nMCParalAlone");
  Vec<Short_t> nMCSkewAlone(ioData.nMCSkewAlone, ioData.MAXTRACKSPEREVENT, "nMCSkewAlone");
  Vec<Short_t> nMvdPixelCommon(ioData.nMvdPixelCommon, ioData.nTotalCandidates, "nMvdPixelCommon");
  Vec<Short_t> nMvdPixelHitsinTrack(ioData.nMvdPixelHitsinTrack, ioData.MAXTRACKSPEREVENT, "nMvdPixelHitsinTrack");
  Vec<Short_t> nMvdStripHitsinTrack(ioData.nMvdStripHitsinTrack, ioData.MAXTRACKSPEREVENT, "nMvdStripHitsinTrack");
  Short_t nMvdPixelHit = ioData.nMvdPixelHit;
  Vec<Short_t> nMvdPixelSpuriinTrack(ioData.nMvdPixelSpuriinTrack, ioData.nTotalCandidates, "nMvdPixelSpuriinTrack");
  Vec<Short_t> nMvdStripCommon(ioData.nMvdStripCommon, ioData.nTotalCandidates, "nMvdStripCommon");
  Short_t nMvdStripHit = ioData.nMvdStripHit;
  Vec<Short_t> nMvdStripSpuriinTrack(ioData.nMvdStripSpuriinTrack, ioData.nTotalCandidates, "nMvdStripSpuriinTrack");
  Vec<Short_t> nParalCommon(ioData.nParalCommon, ioData.MAXTRACKSPEREVENT, "nParalCommon");
  Short_t nSciTilHits = ioData.nSciTilHits;
  Vec<Short_t> nSciTilHitsinTrack(ioData.nSciTilHitsinTrack, ioData.MAXTRACKSPEREVENT, "nSciTilHitsinTrack");
  Vec<Short_t> nSkewCommon(ioData.nSkewCommon, ioData.MAXTRACKSPEREVENT, "nSkewCommon");
  Vec<Short_t> nSkewHitsInMCTrack(ioData.nSkewHitsInMCTrack, ioData.MAXTRACKSPEREVENT, "nSkewHitsInMCTrack");
  Vec<Short_t> nSpuriParinTrack(ioData.nSpuriParinTrack, ioData.MAXTRACKSPEREVENT, "nSpuriParinTrack");
  Vec<Short_t> nSpuriSkewinTrack(ioData.nSpuriSkewinTrack, ioData.MAXTRACKSPEREVENT, "nSpuriSkewinTrack");
  Int_t nSttHit = ioData.nSttHit;
  Vec<Short_t> nSttParHitsinTrack(ioData.nSttParHitsinTrack, ioData.MAXTRACKSPEREVENT, "nSttParHitsinTrack");
  Vec<Short_t> nSttSkewHitsinTrack(ioData.nSttSkewHitsinTrack, ioData.MAXTRACKSPEREVENT, "nSttSkewHitsinTrack");
  Short_t nTotalCandidates = ioData.nTotalCandidates;
  Vec<Double_t> Ox(ioData.Ox, ioData.MAXTRACKSPEREVENT, "Ox");
  Vec<Double_t> Oy(ioData.Oy, ioData.MAXTRACKSPEREVENT, "Oy");
  Vec<Short_t> ParalCommonList(ioData.ParalCommonList, ioData.MAXTRACKSPEREVENT * ioData.maxstthitsintrack, "ParalCommonList");
  Vec<Short_t> ParSpuriList(ioData.ParSpuriList, ioData.MAXTRACKSPEREVENT * ioData.maxstthitsintrack, "ParSpuriList");
  Vec<Double_t> R(ioData.R, ioData.MAXTRACKSPEREVENT, "R");
  Vec<Double_t> refindexMvdPixel(ioData.refindexMvdPixel, ioData.MAXMVDPIXELHITS, "refindexMvdPixel");
  Vec<Double_t> refindexMvdStrip(ioData.refindexMvdStrip, ioData.MAXMVDSTRIPHITS, "refindexMvdStrip");
  Vec<Short_t> resultFitSZagain(ioData.resultFitSZagain, ioData.MAXTRACKSPEREVENT, "resultFitSZagain");
  Vec<Short_t> SkewCommonList(ioData.SkewCommonList, ioData.MAXTRACKSPEREVENT * ioData.maxstthitsintrack, "SkewCommonList");
  Vec<Short_t> SkewSpuriList(ioData.SkewSpuriList, ioData.MAXTRACKSPEREVENT * ioData.maxstthitsintrack, "SkewSpuriList");
  Vec<bool> SttSZfit(ioData.SttSZfit, ioData.MAXTRACKSPEREVENT, "SttSZfit");
  Vec<Double_t> XMvdPixel(ioData.XMvdPixel, ioData.MAXMVDPIXELHITS, "XMvdPixel");
  Vec<Double_t> XMvdStrip(ioData.XMvdStrip, ioData.MAXMVDSTRIPHITS, "XMvdStrip");
  Vec<Double_t> XSciTilCenter(ioData.XSciTilCenter, ioData.MAXSCITILHITS, "XSciTilCenter");
  Vec<Double_t> YMvdPixel(ioData.YMvdPixel, ioData.MAXMVDPIXELHITS, "YMvdPixel");
  Vec<Double_t> YMvdStrip(ioData.YMvdStrip, ioData.MAXMVDSTRIPHITS, "YMvdStrip");
  Vec<Double_t> YSciTilCenter(ioData.YSciTilCenter, ioData.MAXSCITILHITS, "YSciTilCenter");
  Vec<Double_t> ZMvdPixel(ioData.ZMvdPixel, ioData.MAXMVDPIXELHITS, "ZMvdPixel");
  Vec<Double_t> ZMvdStrip(ioData.ZMvdStrip, ioData.MAXMVDSTRIPHITS, "ZMvdStrip");
  Vec<Double_t> ZSciTilCenter(ioData.ZSciTilCenter, ioData.MAXSCITILHITS, "ZSciTilCenter");

  nele = MAXSTTHITSINTRACK + MAXMVDPIXELHITSINTRACK + MAXMVDSTRIPHITSINTRACK + MAXSCITILHITSINTRACK;

  //  marker2 per cambioperl;

  //----------   fetching the MC truth tracks
  nMCTracks = fMCTrackArray->GetEntriesFast(); // num. tracce/evento

  if (nMCTracks == 0) {
    // cout<<"from PndTrkComparisonMCTruth  :  N. MC truth tracks = 0, return!\n"<<endl;
    return 0;
  } else if (nMCTracks > MAXMCTRACKS) {
    // cout<<"from PndTrkComparisonMCTruth  :  N. MC truth tracks = "<<nMCTracks
    // <<" and it is > MAXMCTRACKS = "<<MAXMCTRACKS
    // <<", setting it equal to MAXMCTRACKS.\n";
    nMCTracks = MAXMCTRACKS;
  }

  // inizio cambio_in_perl
  if (istampa >= 2)
    stampaMCTracks(BFIELD, CVEL, fMCTrackArray, nMCTracks);
  // fine cambio_in_perl

  // ---------------------------------------------  get MC Points of  MVD

  if (fMvdMCPointArray) {
    nMvdMCPoint = fMvdMCPointArray->GetEntriesFast();
  } else {
    nMvdMCPoint = 0;
  }

  if (nMvdMCPoint > MAXMVDMCPOINTS) {
    cout << "from PndTracking, nMvdMCPoint = " << nMvdMCPoint << " and it is > the maximum number allowed (" << MAXMVDMCPOINTS << ")"
         << ", setting nMvdMCPoint to " << MAXMVDMCPOINTS << endl;
    nMvdMCPoint = MAXMVDMCPOINTS;
  }
  if (istampa >= 2)
    cout << "N. MC Points delle Mvd = " << nMvdMCPoint << endl;

  //--------------------  comparison of the PndTrackCand from PR of the STT to the MC truth
  //    associate the tracks found with Pattern Recognition to the MC tracks

  //  the following method associates the Mvd hits to corresponding MC tracks

  // non cambiare le seguenti righe
  // perche' vengono processate da modificaPndTrkComparisonMCtruth.pl
  int dimensioneP, dimensioneS;
  if (nMvdPixelHit == 0) {
    dimensioneP = 1;
  } else {
    dimensioneP = nMvdPixelHit;
  }

  if (nMvdStripHit == 0) {
    dimensioneS = 1;
  } else {
    dimensioneS = nMvdStripHit;
  }

  Int_t FromPixeltoMCTra[dimensioneP], FromStriptoMCTra[dimensioneS];
  Vec<Int_t> FromPixeltoMCTrack(FromPixeltoMCTra, dimensioneP, "FromPixeltoMCTrack");
  Vec<Int_t> FromStriptoMCTrack(FromStriptoMCTra, dimensioneS, "FromStriptoMCTrack");
  // fine processamento;

  // inizio cambio_in_perl
  MvdMatchtoMC(ERRORSQPIXEL, ERRORSQSTRIP, fMvdMCPointArray, nMvdMCPoint, istampa, IVOLTE, nMvdPixelHit, nMvdStripHit, &refindexMvdPixel, &refindexMvdStrip, &XMvdPixel, &XMvdStrip,
               &YMvdPixel, &YMvdStrip, &ZMvdPixel, &ZMvdStrip,
               &FromPixeltoMCTrack, // output
               &FromStriptoMCTrack  // output
  );
  // fine cambio_in_perl

  if (istampa >= 3) {
    cout << "\n----------  da PndTrkComparisonMCTruth\n";
    for (int h = 0; h < nMvdPixelHit; h++) {
      cout << "\til Pixel n. " << h << "  associato a MC track n. " << FromPixeltoMCTrack[h] << endl;
    }
    for (int h = 0; h < nMvdStripHit; h++) {
      cout << "\til Strip n. " << h << "  associato a MC track n. " << FromStriptoMCTrack[h] << endl;
    }
  }

  //  the following method associates the SciTil hits to corresponding MC tracks

  // there is the possibility that one SciTil tile contained (before the purge
  // done at the beginning of the Pattern Recognition) more hits from more MC
  //  truth track. Therefore one SciTil tile has nMCTracks [in principle]
  //  maximum number of connections to MC truth. So FromSciTiltoMCTrack
  //  is meant to be equivalent to a matrix  of dimensions
  //  [nSciTilHits][nMCTracks].
  //  Correspondingly,  nFromSciTiltoMCTrack[n] is the number of
  //  connections (to the MC truth tracks) of the Sci Tile number n.

  // non cambiare le seguenti righe
  // perche' vengono processate da modificaPndTrkComparisonMCtruth.pl
  int nFromSciTiltoMCTra[nSciTilHits], FromSciTiltoMCTrackL[nSciTilHits * nMCTracks];
  Vec<int> nFromSciTiltoMCTrack(nFromSciTiltoMCTra, nSciTilHits, "nFromSciTiltoMCTrack");
  Vec<int> FromSciTiltoMCTrackList(FromSciTiltoMCTrackL, nSciTilHits * nMCTracks, "FromSciTiltoMCTrackList");
  // fine processamento;

  // inizio cambio_in_perl
  SciTilMatchtoMC(BFIELD, CVEL, DIMENSIONSCITIL, fMCTrackArray,
                  &FromSciTiltoMCTrackList, // output
                  ioData.fSciTHitArray, ioData.fSciTilMaxNumber, ioData.fSciTPointArray,
                  &nFromSciTiltoMCTrack, //  output
                  ioData.nHitsInSciTile, nMCTracks, nSciTilHits, ioData.OriginalSciTilList, &XSciTilCenter, &YSciTilCenter, &ZSciTilCenter);

  // fine cambio_in_perl

  //  this section associates the found tracks  to the
  //  MC tracks, creating a bilinear correspondence between MC tracks and PR Found tracks

  if (nMCTracks > 0 && nTotalCandidates > 0) {

    int nn; // nmid, //[R.K. 01/2017] unused variable?

    // non cambiare le seguenti righe
    // perche' vengono processate da modificaPndTrkComparisonMCtruth.pl
    Double_t xx1[MAXTRACKSPEREVENT], yy1[MAXTRACKSPEREVENT], xx2[MAXTRACKSPEREVENT], yy2[MAXTRACKSPEREVENT], xx3[MAXTRACKSPEREVENT], yy3[MAXTRACKSPEREVENT];
    Vec<Double_t> X1(xx1, MAXTRACKSPEREVENT, "X1");
    Vec<Double_t> Y1(yy1, MAXTRACKSPEREVENT, "Y1");
    Vec<Double_t> X2(xx2, MAXTRACKSPEREVENT, "X2");
    Vec<Double_t> Y2(yy2, MAXTRACKSPEREVENT, "Y2");
    Vec<Double_t> X3(xx3, MAXTRACKSPEREVENT, "X3");
    Vec<Double_t> Y3(yy3, MAXTRACKSPEREVENT, "Y3");
    // fine processamento;

    for (i = 0; i < nTotalCandidates; i++) {
      if (!keepit[i])
        continue;
      nn = nSttParHitsinTrack[i] + nSttSkewHitsinTrack[i] + nMvdPixelHitsinTrack[i] + nMvdStripHitsinTrack[i];
      // assume that the point on trajectory at Z=0 is
      // the point of closest approach to (0,0,0)
      X1[i] = Ox[i] + R[i] * cos(FI0[i]);
      Y1[i] = Oy[i] + R[i] * sin(FI0[i]);

      //  the third point on trajectory is given by the last hit

      switch (ListTrackCandHitType[i * nele + nn - 1]) {
      case 0: // Pixel
        X3[i] = XMvdPixel[ListTrackCandHit[i * nele + nn - 1]];
        Y3[i] = YMvdPixel[ListTrackCandHit[i * nele + nn - 1]];
        break;
      case 1: // Strip
        X3[i] = XMvdStrip[ListTrackCandHit[i * nele + nn - 1]];
        Y3[i] = YMvdStrip[ListTrackCandHit[i * nele + nn - 1]];
        break;
      case 2: // Straw parallel
        X3[i] = info[ListTrackCandHit[i * nele + nn - 1] * 7 + 0];
        Y3[i] = info[ListTrackCandHit[i * nele + nn - 1] * 7 + 1];
        break;
      case 3: // Straw skew
        X3[i] = info[ListTrackCandHit[i * nele + nn - 1] * 7 + 0];
        Y3[i] = info[ListTrackCandHit[i * nele + nn - 1] * 7 + 1];
        break;
      };

      //  the 2nd point on the trajectory is given by the middle between
      //  first and third point.

      Double_t angle, middle;
      angle = atan2(Y3[i] - Oy[i], X3[i] - Ox[i]);
      if (angle < 0.)
        angle += 2. * PI;
      if (angle < 0.)
        angle = 0.;

      if (Charge[i] > 0) { //  clockwise.
        if (FI0[i] < angle)
          angle -= 2. * PI;
        if (FI0[i] < angle)
          angle = 0.;
      } else { // counterclockwise.
        if (FI0[i] > angle)
          angle += 2. * PI;
        if (FI0[i] > angle)
          angle = FI0[i];
      }
      middle = (FI0[i] + angle) / 2.;

      X2[i] = Ox[i] + R[i] * cos(middle);
      Y2[i] = Oy[i] + R[i] * sin(middle);

    } // end of  for(i=0; i<nTotalCandidates;i++)

    if (nTotalCandidates > 0)
      AssociateFoundTrackstoMCquater(BFIELD, CVEL, &daTrackFoundaTrackMC, fMCTrackArray, &FromPixeltoMCTrack, &FromStriptoMCTrack, &FromSciTiltoMCTrackList, &keepit, &info,
                                     &ListSttParHitsinTrack, &ListMvdPixelHitsinTrack, &ListSciTilHitsinTrack, &ListSttSkewHitsinTrack, &ListMvdStripHitsinTrack,
                                     MAXMVDPIXELHITSINTRACK, MAXMVDSTRIPHITSINTRACK, MAXSCITILHITSINTRACK, MAXSTTHITSINTRACK, &nFromSciTiltoMCTrack, &nSttParHitsinTrack, nMCTracks,
                                     &nMvdPixelHitsinTrack, nSciTilHits, &nSciTilHitsinTrack, &nSttSkewHitsinTrack, &nMvdStripHitsinTrack, nTotalCandidates, &Ox, &Oy, &R, &X1, &Y1,
                                     &X2, &Y2, &X3, &Y3, IVOLTE);

  } // end   if( nMCTracks >0 && nTotalCandidates > 0)

  //---------- stampe.
  if (istampa >= 3) {
    for (i = 0; i < nTotalCandidates; i++) {
      // cout<<"from PndTrkComparisonMCtruth : Track candidate n. "<<i;
      if (keepit[i]) {
        // cout<<"  associated to MC Track n. "<<daTrackFoundaTrackMC[i]<<endl;
      } else {
        // cout<<"  has keepit  false!\n";
      }
    }
  }
  //--------- fine stampe.

  //  the following method counts the matched to MC and spurious hits for all STT  Found tracks and
  //  also the ghost tracks

  // inizio cambio_in_perl

  SttMatchedSpurious(&daTrackFoundaTrackMC, &InclusionListStt, &info, &keepit, MAXSTTHITS, MAXSTTHITSINTRACK, MAXTRACKSPEREVENT,
                     &ListSttParHitsinTrack,  // from P&R
                     &ListSttSkewHitsinTrack, // from P&R
                     &MCParalAloneList, &MCSkewAloneList, &nHitsInMCTrack,
                     &nSttParHitsinTrack, // n. hits PA&RALLEL from PR
                     &nMCParalAlone, &nMCSkewAlone, &nParalCommon, &nSkewCommon, &nSkewHitsInMCTrack,
                     &nSttSkewHitsinTrack, // n. hits skew, from P&R
                     &nSpuriParinTrack, &nSpuriSkewinTrack, nSttHit,
                     nTotalCandidates, // those found by P&R
                     &ParalCommonList, &ParSpuriList, &SkewCommonList, &SkewSpuriList);
  // fine cambio_in_perl

  // now, knowing    nMCSkewAlone   and  MCSkewAloneList  I can calculate  the X and Y
  // position of the corresponding  Stt  MC POINTS;

  // The original dimension of MCSkewAloneX is  MAXSTTHITS, and since MCSkewAloneList is a list
  // of Stt hits, then  MCSkewAloneList is a number always <= MAXSTTHITS;

  for (ncand = 0; ncand < nTotalCandidates; ncand++) {
    if (!keepit[ncand])
      continue;
    for (j = 0; j < nMCSkewAlone[ncand]; j++) {
      FairMCPoint *puntator = (FairMCPoint *)ioData.fSttPointArray->At(MCSkewAloneList[ncand * nSttHit + j]);
      MCSkewAloneX[MCSkewAloneList[ncand * nSttHit + j]] = puntator->GetX();
      MCSkewAloneY[MCSkewAloneList[ncand * nSttHit + j]] = puntator->GetY();
    } // end of for( j=0;j<nMCSkewAlone[ncand];j++)
  }   //  end of  for(ncand=0;ncand<nTotalCandidates;ncand++)

  //------------------------
  //	assumo che la traccia MC associata alla traccia trovata dal Pattern Recognition
  //	sia quella giusta e di
  //	conseguenza calcolo gli hits Mvd spuri e comuni

  // inizio cambio_in_perl

  MvdMatchedSpurioustoTrackCand(&daTrackFoundaTrackMC, // input

                                &FromPixeltoMCTrack, // input
                                &FromStriptoMCTrack, // input
                                &keepit,             // input

                                &ListMvdPixelHitsinTrack, // input
                                &ListMvdStripHitsinTrack, // input
                                MAXMVDPIXELHITSINTRACK, MAXMVDSTRIPHITSINTRACK, nMvdPixelHit, nMvdStripHit,
                                &nMvdPixelHitsinTrack, // input
                                &nMvdStripHitsinTrack, // input
                                nTotalCandidates,      // input

                                &nMvdPixelCommon,       // output
                                &MvdPixelCommonList,    // output
                                &nMvdPixelSpuriinTrack, // output
                                &MvdPixelSpuriList,     // output
                                &nMCMvdPixelAlone,      // output
                                &MCMvdPixelAloneList,   // output

                                &nMvdStripCommon,       // output
                                &MvdStripCommonList,    // output
                                &nMvdStripSpuriinTrack, // output
                                &MvdStripSpuriList,     // output
                                &nMCMvdStripAlone,      // output
                                &MCMvdStripAloneList    // output
  );
  //----------------------------------------------------------

  // calcolo gli hit comuni, spuri ed alone degli SciTil.

  SciTilMatchedSpurioustoTrackCand(&daTrackFoundaTrackMC,      // input
                                   &FromSciTiltoMCTrackList,   // input; equivalent to a matrix
                                                               //  of dimension  [nSciTilHits][nMCTracks]
                                   &keepit,                    // input
                                   &ListSciTilHitsinTrack,     // input; equivalent to a matrix of dimension
                                                               // [MAXTRACKSPEREVENT][MAXSCITILHITSINTRACK]
                                   MAXSCITILHITSINTRACK,       // input
                                   ioData.MCSciTilAloneList,   // output; equivalent to a matrix of dimension
                                                               // [MAXTRACKSPEREVENT][nSciTilHits]
                                   &nFromSciTiltoMCTrack,      // input
                                   ioData.nMCSciTilAlone,      // output
                                   nMCTracks,                  // input
                                   nSciTilHits,                // input
                                   &nSciTilHitsinTrack,        // input
                                   ioData.nSciTilCommon,       // output
                                   ioData.nSciTilSpuriinTrack, // output
                                   nTotalCandidates,           // input
                                   ioData.SciTilCommonList,    // output; equivalent to a matrix of dimension
                                                               // [MAXTRACKSPEREVENT][MAXSCITILHITSINTRACK]
                                   ioData.SciTilSpuriList      // output; equivalent to a matrix of dimension
                                                               // [MAXTRACKSPEREVENT][MAXSCITILHITSINTRACK].
  );

  // fine cambio_in_perl

  //----------------------------------------------------------

  // ora il confronto per il meeting di  Groningen

  //---------- conteggio delle tracce MC accettabili!!

  int cita;
  int citata;
  int nMCTracksaccettabili = 0;
  int ListaMCTracksaccettabili[nMCTracks];
  int nHasMvdHit = 0;
  int nHasSciTilHit = 0;
  PndMCTrack *pMCtr;

  for (i = 0; i < nMCTracks; i++) {
    pMCtr = (PndMCTrack *)fMCTrackArray->At(i);
    if (!pMCtr)
      continue;
    Double_t aaa, carica, Rr, Oxx, Oyy, Cx, Cy, Pxx, Pyy; // Dd, Fifi, //[R.K. 01/2017] unused variable?
    Int_t icode;
    icode = pMCtr->GetPdgCode();       //   PDG code of track
    Oxx = pMCtr->GetStartVertex().X(); //   X of starting point track
    Oyy = pMCtr->GetStartVertex().Y(); //   Y of starting point track
    Pxx = pMCtr->GetMomentum().X();
    Pyy = pMCtr->GetMomentum().Y();
    aaa = sqrt(Pxx * Pxx + Pyy * Pyy);
    Rr = aaa * 1000. / (BFIELD * CVEL); //   R (cm) of Helix of track projected in XY plane; B = 2 Tesla

    if (istampa > 2) {
      TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
      TParticlePDG *fParticle = fdbPDG->GetParticle(icode);
      if (icode > 1000000000)
        carica = 1.;
      else
        carica = fParticle->Charge() / 3.; //   charge of track
      if (fabs(carica) < 1.e-5)
        continue;
      Cx = Oxx + Pyy * 1000. / (BFIELD * CVEL * carica);
      Cy = Oyy - Pxx * 1000. / (BFIELD * CVEL * carica);
      cout << "from PndTrkComparisonMCTruth, event (starting from 0) n. " << IVOLTE << ",  track MC n. " << i << ",  R MC = " << Rr << ", X Center = " << Cx
           << ", Y Center = " << Cy << endl;
    }

    citata = 0;
    for (int ic = 0; ic < nSttHit; ic++) {
      // info[*][5] = tipe of inclination of the straw; the following is the requirement that
      // the inclination is not 99 (=skew straw) but instead 1 (= parallel straw);
      if (((int)(info[ic * 7 + 6] + 0.1)) == i && info[ic * 7 + 5] < 2.) {
        citata++;
      }
    } // end  for(int ic=0;ic<nSttHit;ic++)

    if (citata > 2 && fabs(Oxx) < 1. && fabs(Oyy) < 1.) {

      /*
          ListaMCTracksaccettabili[nMCTracksaccettabili]=i;
          nMCTracksaccettabili++;
      */

      // check if there is at least 1 MvdHit in this MC track;
      // the Pixel first;
      cita = 0;
      for (int ic = 0; ic < nMvdPixelHit; ic++) {
        if (FromPixeltoMCTrack[ic] == i) {
          cita++;
          break;
        }
      }
      // if the Pixel are 0, check the Strips;
      if (cita == 0) {
        for (int ic = 0; ic < nMvdStripHit; ic++) {
          if (FromStriptoMCTrack[ic] == i) {
            cita++;
            break;
          }
        }
      } // end of if;
      if (cita > 0) {
        nHasMvdHit++;
        // define an acceptable MC track one that has at leat 3 STT parallel hits and
        // at least 1 Mvd hit;
        ListaMCTracksaccettabili[nMCTracksaccettabili] = i;
        nMCTracksaccettabili++;
      }

      //-------------------------------

      // check if there is at least 1 SciTil hit in this MC track;
      cita = 0;
      for (int ic = 0; ic < nSciTilHits; ic++) {
        for (int jc = 0; jc < nFromSciTiltoMCTrack[ic]; jc++) {
          if (FromSciTiltoMCTrackList[ic * nMCTracks + jc] == i) {
            cita++;
            break;
          }
        } // end for(int jc=0;
        if (cita > 0)
          break;
      } // end for(int ic=0;ic<nSciTilHits;ic++)
      if (cita > 0) {
        nHasSciTilHit++;
      }

    } // end of  if( citata>2 && fabs(Oxx)<1. && fabs(Oyy) < 1. )

  } // end of  for (i=0;i<nMCTracks;i++)

  //----------- fine conteggio delle tracce MC accettabili

  fprintf(HANDLE, "\n Evento %d  NTotaleTracceMC %d ,", IVOLTE, nMCTracksaccettabili);
  fprintf(HANDLE, "\tdi cui %d con almeno 1 hit Mvd e %d con almeno 1 hit SciTil.\n", nHasMvdHit, nHasSciTilHit);
  int ibene = 0;
  if (nMCTracksaccettabili > 0) {
    for (int ii = 0; ii < nTotalCandidates; ii++) {
      for (i = 0; i < nMCTracksaccettabili; i++) {
        if (daTrackFoundaTrackMC[ii] == ListaMCTracksaccettabili[i]) {
          ibene++;
        }
      }
    }
  }
  if (ibene > 0)
    fprintf(HANDLE, "\tn. volte almeno 1 traccia MC accettabile e' ricostruita %d\n", ibene);
  bool flaggo;
  int ii, ibuone = -1;
  Double_t HoughFiii;

  Double_t perc_trueSttPar = 0., perc_trueSttSkew = 0., perc_missSttSkew = 0.,
           // spurSttSkew = 0. , //[R.K. 01/2017] unused variable?
    perc_trueStt = 0., perc_trueMvdPixel = 0., perc_trueMvdStrip = 0., perc_trueMvd = 0., perc_missSttPar = 0., perc_missStt = 0., perc_missMvdPixel = 0., perc_missMvdStrip = 0.,
           perc_missMvd = 0.;

  for (ii = 0; ii < nTotalCandidates; ii++) {
    if (!keepit[ii]) {
      if (istampa > 1) {
        cout << "\tevt. n " << IVOLTE << ", cand. " << ii << " ha keepit false." << endl;
      };
      continue;
    }
    ibuone++;
    fprintf(HANDLE, "----------------------------------------------------------\n");
    i = daTrackFoundaTrackMC[ii];

    if (i < 0) {
      fprintf(HANDLE, "   No TracciaMC associated to found track n. %d in pattern recognition, with %d Hits ||, %d skew hits, %f Radius \n ", ibuone, nSttParHitsinTrack[ii],
              nSttSkewHitsinTrack[ii], R[ii]);
      continue;
    }
    if ((!SttSZfit[ii]) && (resultFitSZagain[ii] != 1)) {
      fprintf(HANDLE, "       TracciaMC %d; sua FoundTrack associata (n. %d) NONsoddisfaRequisitiMinimi perche' in Z-S e' fallito il fit \n", i, ibuone);
      continue;
    }
    if (fabs(KAPPA[ii]) < 1.e-20) {
      fprintf(HANDLE, "       TracciaMC %d; sua FoundTrack associata (n. %d) NONsoddisfaRequisitiMinimi perche' KAPPA troppo piccolo; KAPPA = %g\n", i, ibuone, KAPPA[ii]);
      continue;
    }
    dista = sqrt(Ox[ii] * Ox[ii] + Oy[ii] * Oy[ii]);
    if (fabs(dista) < 1.e-20) {
      fprintf(HANDLE, "       TracciaMC %d; sua FoundTrack associata (n. %d) NONsoddisfaRequisitiMinimi perche' centro Helix Cilinder trovato dista solo %g da (0,0)\n", i, ibuone,
              dista);
      continue;
    }
    pMCtr = (PndMCTrack *)fMCTrackArray->At(i);
    if (!pMCtr) {
      fprintf(HANDLE, "       MC track n. %d doesn't have pointer to MC Track TClones Array\n", i);
      continue;
    }

    //   controllo che la traccia associata MC sia una delle tracce MC 'ragionevoli'.

    flaggo = true;
    for (int g = 0; g < nMCTracksaccettabili; g++) {
      if (i == ListaMCTracksaccettabili[g]) {
        flaggo = false;
        break;
      }
    }
    if (flaggo)
      continue;

    // calcolo delle % di hit di vario tipo associati a questa traccia;

    if (nHitsInMCTrack[ii] > 0) {
      perc_trueSttPar = nParalCommon[ii] / ((double)nHitsInMCTrack[ii]);
      perc_missSttPar = (nHitsInMCTrack[ii] - nParalCommon[ii]) / ((double)nHitsInMCTrack[ii]);
    } else {
      perc_trueSttPar = -0.01;
      perc_missSttPar = -0.01;
    }

    if (nSkewHitsInMCTrack[ii] > 0) {
      perc_trueSttSkew = nSkewCommon[ii] / ((double)nSkewHitsInMCTrack[ii]);
      perc_missSttSkew = (nSkewHitsInMCTrack[ii] - nSkewCommon[ii]) / ((double)nSkewHitsInMCTrack[ii]);
    } else {
      perc_trueSttSkew = -0.01;
      perc_missSttSkew = -0.01;
    }

    if ((nHitsInMCTrack[ii] + nSkewHitsInMCTrack[ii]) > 0) {
      perc_trueStt = (nParalCommon[ii] + nSkewCommon[ii]) / ((double)nHitsInMCTrack[ii] + nSkewHitsInMCTrack[ii]);
      perc_missStt = ((double)(nHitsInMCTrack[ii] - nParalCommon[ii]) + (nSkewHitsInMCTrack[ii] - nSkewCommon[ii])) / (nHitsInMCTrack[ii] + nSkewHitsInMCTrack[ii]);
    } else {
      perc_trueStt = -0.01;
      perc_missStt = -0.01;
    }

    if (nMvdPixelCommon[ii] + nMCMvdPixelAlone[ii] > 0) {
      perc_trueMvdPixel = nMvdPixelCommon[ii] / ((double)nMvdPixelCommon[ii] + nMCMvdPixelAlone[ii]);
      perc_missMvdPixel = nMCMvdPixelAlone[ii] / ((double)nMvdPixelCommon[ii] + nMCMvdPixelAlone[ii]);
    } else {
      perc_trueMvdPixel = -0.01;
      perc_missMvdPixel = -0.01;
    }

    if (nMvdStripCommon[ii] + nMCMvdStripAlone[ii] > 0) {
      perc_trueMvdStrip = nMvdStripCommon[ii] / ((double)nMvdStripCommon[ii] + nMCMvdStripAlone[ii]);
      perc_missMvdStrip = nMCMvdStripAlone[ii] / ((double)nMvdStripCommon[ii] + nMCMvdStripAlone[ii]);
    } else {
      perc_trueMvdStrip = -0.01;
      perc_missMvdStrip = -0.01;
    }

    if (nMvdPixelCommon[ii] + nMCMvdPixelAlone[ii] + nMvdStripCommon[ii] + nMCMvdStripAlone[ii] > 0) {
      perc_trueMvd = (nMvdStripCommon[ii] + nMvdPixelCommon[ii]) / ((double)nMvdPixelCommon[ii] + nMCMvdPixelAlone[ii] + nMvdStripCommon[ii] + nMCMvdStripAlone[ii]);
      perc_missMvd = (nMCMvdPixelAlone[ii] + nMCMvdStripAlone[ii]) / ((double)nMvdPixelCommon[ii] + nMCMvdPixelAlone[ii] + nMvdStripCommon[ii] + nMCMvdStripAlone[ii]);
    } else {
      perc_trueMvd = -0.01;
      perc_missMvd = -0.01;
    }

    fprintf(HANDLE, "       TracciaMC %d ParHitsMC %d ParMecc %d ParMeccSpuri %d SkewHitsMC %d  SkewMecc %d SkewMeccSpuri %d\n", i, nHitsInMCTrack[ii], nParalCommon[ii],
            nSpuriParinTrack[ii], nSkewHitsInMCTrack[ii], nSkewCommon[ii], nSpuriSkewinTrack[ii]

    );
    fprintf(HANDLE, "\t\t%d PixelHitsMC %d PixelHitsMecc %d PixelMeccSpuri ", nMvdPixelCommon[ii] + nMCMvdPixelAlone[ii], nMvdPixelCommon[ii], nMvdPixelSpuriinTrack[ii]);
    fprintf(HANDLE, "%d StripHitsMC %d StripHitsMecc %d StripMeccSpuri\n", nMvdStripCommon[ii] + nMCMvdStripAlone[ii], nMvdStripCommon[ii], nMvdStripSpuriinTrack[ii]);
    fprintf(HANDLE, "\t\t%d SciTilHitsMC %d SciTilHitsMecc %d SciTilMeccSpuri\n", ioData.nSciTilCommon[ii] + ioData.nMCSciTilAlone[ii], ioData.nSciTilCommon[ii],
            ioData.nSciTilSpuriinTrack[ii]);

    fprintf(HANDLE, "       e corrisponde a track found n. %d\n", ibuone);
    fprintf(HANDLE, "       AVENDO %d hits paralleli e %d hits skew non mecciati dalla corrisponde track found\n", nMCParalAlone[ii], nMCSkewAlone[ii]);

    HoughFiii = atan2(Oy[ii], Ox[ii]);
    if (HoughFiii < 0.)
      HoughFiii += 2. * PI;

    Double_t aaa, carica, Rr, Fifi, Oxx, Oyy, Cx, Cy, Pxx, Pyy; // Dd,  //[R.K. 01/2017] unused variable?
    Int_t icode;
    icode = pMCtr->GetPdgCode();       //   PDG code of track
    Oxx = pMCtr->GetStartVertex().X(); //   X of starting point track
    Oyy = pMCtr->GetStartVertex().Y(); //   Y of starting point track
    Pxx = pMCtr->GetMomentum().X();
    Pyy = pMCtr->GetMomentum().Y();
    aaa = sqrt(Pxx * Pxx + Pyy * Pyy);
    Rr = aaa * 1000. / (BFIELD * CVEL); //   R (cm) of Helix of track projected in XY plane; B = 2 Tesla
    TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
    TParticlePDG *fParticle = fdbPDG->GetParticle(icode);
    if (icode > 1000000000)
      carica = 1.;
    else
      carica = fParticle->Charge() / 3.; //   charge of track
    if (fabs(carica) < 1.e-5)
      fprintf(HANDLE, "       MC track n. %d e' neutra, assurdo!\n", i);
    Cx = Oxx + Pyy * 1000. / (BFIELD * CVEL * carica);
    Cy = Oyy - Pxx * 1000. / (BFIELD * CVEL * carica);
    Fifi = atan2(Cy, Cx); // MC truth Fifi angle of circle of Helix trajectory
    if (Fifi < 0.)
      Fifi += 2. * PI;
    Double_t Kakka;
    if (fabs(pMCtr->GetMomentum().Z()) < 1.e-20)
      Kakka = 99999999.;
    else
      Kakka = -carica * 0.001 * BFIELD * CVEL / pMCtr->GetMomentum().Z();

    fprintf(HANDLE, "       R_MC %g R %g Fi_MC %g Fi %g KAPPA_MC %g KAPPA %g FI0_MC %g FI0 %g\n", Rr, R[ii], Fifi, HoughFiii, Kakka, KAPPA[ii],
            fmod(Fifi + PI, 2. * PI), //  FI0  da MC truth
            FI0[ii]);

    fprintf(HANDLE, "       %%truePar %5.1f ;%%missPar %5.1f ;%%trueSkew  %5.1f ;%%missSkew  %5.1f ;%%trueStt %5.1f ;%%missStt %5.1f ;\n", perc_trueSttPar * 100.,
            perc_missSttPar * 100., perc_trueSttSkew * 100., perc_missSttSkew * 100., perc_trueStt * 100., perc_missStt * 100.);

    fprintf(HANDLE, "       %%truePix %5.1f ;%%missPix %5.1f ;%%trueStrip %5.1f ;%%missStrip %5.1f ;%%trueMvd %5.1f ;%%missMvd %5.1f ;\n", perc_trueMvdPixel * 100.,
            perc_missMvdPixel * 100., perc_trueMvdStrip * 100., perc_missMvdStrip * 100., perc_trueMvd * 100., perc_missMvd * 100.);

    //------------------------

    fprintf(HANDLE2, "Evento n. %d Found track %d messa in PndTrackCand", IVOLTE, ii);
    fprintf(HANDLE2, "       R_MC %g R %g Fi_MC %g Fi %g KAPPA_MC %g KAPPA %g FI0_MC %g FI0 %g\n", Rr, R[ii], Fifi, HoughFiii, Kakka, KAPPA[ii], fmod(Fifi + PI, 2. * PI), FI0[ii]);

    //----------------------------------

  } //   end of  for (ii=0; ii<nTotalCandidates  ;ii++)

  //--------------ghosts

  // fa il conto delle ghost solo sugli eventi che hanno almeno 1 traccia MC accettabile.
  int NParghost = 0, NParhitsghost = 0, icc;
  ibuone = -1;
  if (nMCTracksaccettabili > 0) {
    for (icc = 0; icc < nTotalCandidates; icc++) {
      if (!keepit[icc])
        continue;
      ibuone++;
      if (daTrackFoundaTrackMC[icc] == -1) {
        NParghost++;
        NParhitsghost += nSttParHitsinTrack[icc] + nSttSkewHitsinTrack[icc];

        if (NParghost == 1)
          fprintf(HANDLE, "----------------------------------------------------------\n");
        fprintf(HANDLE, "          tracce Trovata n. %d e' Ghost\n", ibuone);
      }
    }
    fprintf(HANDLE, "          tracceGhostTrovate %d TotaleHitsGhost %d  ----\n", NParghost, NParhitsghost);

    fprintf(HANDLE, "----------------------------------------------------------\n");

  } //   end of    if( nMCTracksaccettabili>0)

  return nMCTracks;
}

// marker3 per cambioperl;
//----------end of function PndTrkComparisonMCtruth::ComparisonwithMC

//------------------------- begin of function  PndTrkComparisonMCtruth::FindDistance

//  inizio cambio_in_perl.

Double_t PndTrkComparisonMCtruth::FindDistance(Double_t Oxx, //  center from wich distance is calculated
                                               Double_t Oyy, //  center from wich distance is calculated
                                               Double_t Rr, Double_t tanlow, Double_t tanmid, Double_t tanup,
                                               Double_t alfa, //  intersection circumference parameter
                                               Double_t beta, //  intersection circumference parameter
                                               Double_t gamma //  intersection circumference parameter
)
{
  //  fine cambio_in_perl.
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

//------------------------- end of function  PndTrkComparisonMCtruth::FindDistance

//------------------------- begin of function  PndTrkComparisonMCtruth::getMCInfo

//  inizio cambio_in_perl.
void PndTrkComparisonMCtruth::getMCInfo(Double_t BFIELD, Double_t CVEL, Double_t *Cx, Double_t *Cy, TClonesArray *fMCTrackArray, Int_t MCTrack, Double_t *Rr)
{
  //  fine cambio_in_perl.
  Int_t icode;
  Double_t aaa, Oxx, Oyy, Pxx, Pyy, carica; // Dd,Fifi,  //[R.K. 01/2017] unused variable?
  PndMCTrack *pMC;

  if (MCTrack < 0) {
    *Rr = -2.;
    return;
  }

  pMC = (PndMCTrack *)fMCTrackArray->At(MCTrack);
  if (pMC) {
    icode = pMC->GetPdgCode();       //   PDG code of track
    Oxx = pMC->GetStartVertex().X(); //   X of starting point track
    Oyy = pMC->GetStartVertex().Y(); //   Y of starting point track
    Pxx = pMC->GetMomentum().X();
    Pyy = pMC->GetMomentum().Y();
    aaa = sqrt(Pxx * Pxx + Pyy * Pyy);
    *Rr = aaa * 1000. / (BFIELD * CVEL); //   R (cm) of Helix of track
                                         //   projected in XY plane; B = 2 Tesla
    TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
    TParticlePDG *fParticle = fdbPDG->GetParticle(icode);
    if (icode > 1000000000)
      carica = 1.;
    else
      carica = fParticle->Charge() / 3.; //   charge of track
    if (fabs(carica) < 1.e-5) {
      *Rr = -3.;
      return;
    }
    *Cx = Oxx + Pyy * 1000. / (BFIELD * CVEL * carica);
    *Cy = Oyy - Pxx * 1000. / (BFIELD * CVEL * carica);
  } else {
    *Rr = -1.;
  }

  return;
}

//------------------------- end of function  PndTrkComparisonMCtruth::getMCInfo

//  inizio cambio_in_perl.
//------------------ begin function  PndTrkComparisonMCtruth::MvdMatchedSpurioustoTrackCand
void PndTrkComparisonMCtruth::MvdMatchedSpurioustoTrackCand(Vec<Short_t> *daTrackFoundaTrackMC, Vec<Int_t> *FromPixeltoMCTrack, Vec<Int_t> *FromStriptoMCTrack, Vec<bool> *keepit,
                                                            Vec<Short_t> *ListMvdPixelHitsinTrack, Vec<Short_t> *ListMvdStripHitsinTrack, int MAXMVDPIXELHITSINTRACK,
                                                            int MAXMVDSTRIPHITSINTRACK, Short_t nMvdPixelHit, Short_t nMvdStripHit, Vec<Short_t> *nMvdPixelHitsinTrack,
                                                            Vec<Short_t> *nMvdStripHitsinTrack,
                                                            Short_t nSttTrackCand, // input

                                                            Vec<Short_t> *nMvdPixelCommon, Vec<Short_t> *MvdPixelCommonList, Vec<Short_t> *nMvdPixelSpuriinTrack,
                                                            Vec<Short_t> *MvdPixelSpuriList, Vec<Short_t> *nMCMvdPixelAlone, Vec<Short_t> *MCMvdPixelAloneList,

                                                            Vec<Short_t> *nMvdStripCommon, Vec<Short_t> *MvdStripCommonList, Vec<Short_t> *nMvdStripSpuriinTrack,
                                                            Vec<Short_t> *MvdStripSpuriList, Vec<Short_t> *nMCMvdStripAlone, Vec<Short_t> *MCMvdStripAloneList)
{
  //  fine cambio_in_perl.

  int tmp_dim1, tmp_dim2, tmp_dim3;

  if (nSttTrackCand > 0) {
    tmp_dim1 = nSttTrackCand;
  } else {
    tmp_dim1 = 1;
  }
  if (nMvdPixelHit > 0) {
    tmp_dim2 = nMvdPixelHit;
  } else {
    tmp_dim2 = 1;
  }
  if (nMvdStripHit > 0) {
    tmp_dim3 = nMvdStripHit;
  } else {
    tmp_dim3 = 1;
  }

  // non modificare; modificaperl processa i seguenti statements;
  bool TMPincludePixel[tmp_dim1 * tmp_dim2];
  Vec<bool> includePixel(TMPincludePixel, tmp_dim1 * tmp_dim2, "includePixel");

  bool TMPincludeStrip[tmp_dim1 * tmp_dim3];
  Vec<bool> includeStrip(TMPincludeStrip, tmp_dim1 * tmp_dim3, "includeStrip");

  // fine di non modificare; modificaperl processa i seguenti statements;

  Short_t i, j;

  int index, index2;

  for (i = 0; i < nSttTrackCand; i++) {
    if (!keepit->at(i))
      continue;
    nMvdPixelCommon->at(i) = 0;
    nMvdPixelSpuriinTrack->at(i) = 0;
    nMCMvdPixelAlone->at(i) = 0;
    nMvdStripCommon->at(i) = 0;
    nMvdStripSpuriinTrack->at(i) = 0;
    nMCMvdStripAlone->at(i) = 0;
  }

  for (i = 0; i < nSttTrackCand; i++) {
    if (!keepit->at(i))
      continue;
    for (j = 0; j < nMvdPixelHit; j++) {
      includePixel[i * tmp_dim2 + j] = true;
    }
    for (j = 0; j < nMvdStripHit; j++) {
      includeStrip[i * tmp_dim3 + j] = true;
    }

    for (j = 0; j < nMvdPixelHitsinTrack->at(i); j++) {
      index = i * MAXMVDPIXELHITSINTRACK + j;
      includePixel[i * tmp_dim2 + ListMvdPixelHitsinTrack->at(index)] = false;

      if (daTrackFoundaTrackMC->at(i) >= 0 && daTrackFoundaTrackMC->at(i) == FromPixeltoMCTrack->at(ListMvdPixelHitsinTrack->at(index))) {
        index2 = i * MAXMVDPIXELHITSINTRACK + nMvdPixelCommon->at(i);
        MvdPixelCommonList->at(index2) = ListMvdPixelHitsinTrack->at(index);
        nMvdPixelCommon->at(i)++;
      } else {
        index2 = i * MAXMVDPIXELHITSINTRACK + nMvdPixelSpuriinTrack->at(i);
        MvdPixelSpuriList->at(index2) = ListMvdPixelHitsinTrack->at(index);
        nMvdPixelSpuriinTrack->at(i)++;
      }
    }

    for (j = 0; j < nMvdStripHitsinTrack->at(i); j++) {
      index = i * MAXMVDSTRIPHITSINTRACK + j;
      includeStrip[i * tmp_dim3 + ListMvdStripHitsinTrack->at(index)] = false;
      if (daTrackFoundaTrackMC->at(i) >= 0 && daTrackFoundaTrackMC->at(i) == FromStriptoMCTrack->at(ListMvdStripHitsinTrack->at(index))) {
        index2 = i * MAXMVDSTRIPHITSINTRACK + nMvdStripCommon->at(i);
        MvdStripCommonList->at(index2) = ListMvdStripHitsinTrack->at(index);
        nMvdStripCommon->at(i)++;
      } else {
        index2 = i * MAXMVDSTRIPHITSINTRACK + nMvdStripSpuriinTrack->at(i);
        MvdStripSpuriList->at(index2) = ListMvdStripHitsinTrack->at(index);
        nMvdStripSpuriinTrack->at(i)++;
      }
    }
  } // end of for(i=0; i<nSttTrackCand;i++)

  for (j = 0; j < nSttTrackCand; j++) {
    if (!keepit->at(j))
      continue;
    if (daTrackFoundaTrackMC->at(j) > -1) {
      for (i = 0; i < nMvdPixelHit; i++) {
        if (!includePixel[j * tmp_dim2 + i])
          continue;

        if (daTrackFoundaTrackMC->at(j) == FromPixeltoMCTrack->at(i)) {
          index = j * nMvdPixelHit + nMCMvdPixelAlone->at(j);
          MCMvdPixelAloneList->at(index) = i;
          nMCMvdPixelAlone->at(j)++;
        }
      }
    }
  }

  for (j = 0; j < nSttTrackCand; j++) {
    if (!keepit->at(j))
      continue;
    if (daTrackFoundaTrackMC->at(j) > -1) {
      for (i = 0; i < nMvdStripHit; i++) {
        if (!includeStrip[j * tmp_dim3 + i])
          continue;
        if (daTrackFoundaTrackMC->at(j) == FromStriptoMCTrack->at(i)) {
          index = j * nMvdStripHit + nMCMvdStripAlone->at(j);
          MCMvdStripAloneList->at(index) = i;
          nMCMvdStripAlone->at(j)++;
        }
      }
    }
  }

  return;
}

//------------------ end function  PndTrkComparisonMCtruth::MvdMatchedSpurioustoTrackCand

//----------begin of function PndTrkComparisonMCtruth::MvdMatchtoMC

//  inizio cambio_in_perl.

void PndTrkComparisonMCtruth::MvdMatchtoMC(Double_t /*ERRORSQPIXEL*/, //[R.K. 9/2018] unused
                                           Double_t ERRORSQSTRIP, TClonesArray *fMvdMCPointArray, Short_t nMvdMCPoint, int istampa, int IVOLTE, Short_t nMvdPixelHit,
                                           Short_t nMvdStripHit, Vec<Double_t> *refindexMvdPixel, Vec<Double_t> *refindexMvdStrip, Vec<Double_t> *XMvdPixel,
                                           Vec<Double_t> *XMvdStrip, Vec<Double_t> *YMvdPixel, Vec<Double_t> *YMvdStrip, Vec<Double_t> *ZMvdPixel, Vec<Double_t> *ZMvdStrip,
                                           Vec<Int_t> *FromPixeltoMCTrack, Vec<Int_t> *FromStriptoMCTrack)
{
  //  fine cambio_in_perl.

  int tmp_dim;
  if (nMvdMCPoint > 0) {
    tmp_dim = nMvdMCPoint;
  } else {
    tmp_dim = 1;
  }
  // non modificare la seguente linea; modificaperl la cambia.
  bool TMPinclusionMCPoint[tmp_dim];
  Vec<bool> inclusionMCPoint(TMPinclusionMCPoint, tmp_dim, "inclusionMCPoint");

  Short_t i, j, jmcpoint;
  Double_t dist, distance;

  Int_t MCPointtoMCTrackID;

  Double_t XMvdMCPoint, YMvdMCPoint, ZMvdMCPoint;

  PndSdsMCPoint *pMvdMCPoint;

  //----  initializations

  for (i = 0; i < nMvdMCPoint; i++) {
    inclusionMCPoint[i] = true;
  }
  for (i = 0; i < nMvdPixelHit; i++) {
    FromPixeltoMCTrack->at(i) = -1;
  }
  for (i = 0; i < nMvdStripHit; i++) {
    FromStriptoMCTrack->at(i) = -1;
  }

  //----------

  for (i = 0; i < nMvdPixelHit; i++) {
    if (refindexMvdPixel->at(i) < 0.)
      continue;
    // multiply by an arbitrary factor of 2. to take into account the possibility
    // that the Mvd hit is rather far (due to cluster analysis by Tobias & Co.)
    // than the MC Mvd point;
    dist = 9999999.;
    for (j = 0; j < nMvdMCPoint; j++) {
      // get the MC info.
      pMvdMCPoint = (PndSdsMCPoint *)fMvdMCPointArray->At(j);
      TVector3 position;
      pMvdMCPoint->Position(position);
      XMvdMCPoint = position.X();
      YMvdMCPoint = position.Y();
      ZMvdMCPoint = position.Z();
      MCPointtoMCTrackID = pMvdMCPoint->GetTrackID();

      if (!inclusionMCPoint[j])
        continue;
      distance = (XMvdMCPoint - XMvdPixel->at(i)) * (XMvdMCPoint - XMvdPixel->at(i)) + (YMvdMCPoint - YMvdPixel->at(i)) * (YMvdMCPoint - YMvdPixel->at(i)) +
                 (ZMvdMCPoint - ZMvdPixel->at(i)) * (ZMvdMCPoint - ZMvdPixel->at(i));
      if (distance < dist) {
        FromPixeltoMCTrack->at(i) = MCPointtoMCTrackID;
        jmcpoint = j;
        dist = distance;
      }
    } // end of for(j=0;j<nMvdMCPoint;j++)

    if (FromPixeltoMCTrack->at(i) >= 0) {
      inclusionMCPoint[jmcpoint] = false;
    }

  } // end of for(i=0; i<nMvdPixelHit;i++)

  for (i = 0; i < nMvdStripHit; i++) {
    if (refindexMvdStrip->at(i) < 0.)
      continue;
    // multiply by an arbitrary factor of 2. to take into account the possibility
    // that the Mvd hit is rather far (due to cluster analysis by Tobias & Co.)
    // than the MC Mvd point;
    dist = 9999999.;
    for (j = 0; j < nMvdMCPoint; j++) {
      // get the MC info.
      pMvdMCPoint = (PndSdsMCPoint *)fMvdMCPointArray->At(j);
      TVector3 position;
      pMvdMCPoint->Position(position);
      XMvdMCPoint = position.X();
      YMvdMCPoint = position.Y();
      ZMvdMCPoint = position.Z();
      MCPointtoMCTrackID = pMvdMCPoint->GetTrackID();

      if (!inclusionMCPoint[j])
        continue;
      distance = (XMvdMCPoint - XMvdStrip->at(i)) * (XMvdMCPoint - XMvdStrip->at(i)) + (YMvdMCPoint - YMvdStrip->at(i)) * (YMvdMCPoint - YMvdStrip->at(i)) +
                 (ZMvdMCPoint - ZMvdStrip->at(i)) * (ZMvdMCPoint - ZMvdStrip->at(i));
      if (istampa >= 2)
        cout << "distanza**2 di Strip hit n. " << i << " da MC Mvd Point n. " << j << " = " << distance << ", distanza precedente " << dist << ", 2*ERRORSQSTRIP "
             << 2. * ERRORSQSTRIP << endl;
      if (distance < dist) {
        FromStriptoMCTrack->at(i) = MCPointtoMCTrackID;
        jmcpoint = j;
        dist = distance;
      }
    } // end of for(j=0;j<nMvdMCPoint;j++)
    if (FromStriptoMCTrack->at(i) >= 0) {
      inclusionMCPoint[jmcpoint] = false;
    }
    if (istampa >= 2) {
      if (FromStriptoMCTrack->at(i) < 0) {
        cout << "Evento n. " << IVOLTE << ",  lo strip hit n. " << i << " non e' associato ad alcun Mvd Point (FromStriptoMCTrack=-1).\n";
      } else {
        cout << "Evento n. " << IVOLTE << ";  associato strip hit n. " << i << " a  MC Mvd Point n. " << jmcpoint << " e di conseguenza alla traccia MC n. "
             << FromStriptoMCTrack->at(i) << endl;
      }
    }

  } // end of for(i=0; i<nMvdStripHit;i++)

  return;
}

//----------end of function PndTrkComparisonMCtruth::MvdMatchtoMC

//----------begin of function PndTrkComparisonMCtruth::SciTilMatchtoMC
//  inizio cambio_in_perl.
void PndTrkComparisonMCtruth::SciTilMatchtoMC(Double_t /*BFIELD*/,              //[R.K. 9/2018] unused
                                              Double_t /*CVEL*/,                //[R.K. 9/2018] unused
                                              Double_t /*DIMENSIONSCITIL*/,     //[R.K. 9/2018] unused
                                              TClonesArray * /*fMCTrackArray*/, //[R.K. 9/2018] unused
                                              Vec<int> *FromSciTiltoMCTrackList, TClonesArray *fSciTHitArray, Short_t fSciTilMaxNumber, TClonesArray *fSciTPointArray,
                                              Vec<int> *nFromSciTiltoMCTrack, Short_t *nHitsInSciTile, int nMCTracks, Short_t nSciTilHits, Short_t *OriginalSciTilList,
                                              Vec<Double_t> * /*XSciTilCenter*/, //[R.K. 9/2018] unused
                                              Vec<Double_t> * /*YSciTilCenter*/, //[R.K. 9/2018] unused
                                              Vec<Double_t> * /*ZSciTilCenter*/  //[R.K. 9/2018] unused

)
{
  //  fine cambio_in_perl.

  // initialization;

  for (int nsc = 0; nsc < nSciTilHits; nsc++) {

    nFromSciTiltoMCTrack->at(nsc) = 0;
    // in a SciTil there may be more than 1 MC hit, here
    // it is the loop over those.
    // nHitsInSciTile[n] is the number of SciTil hits in the
    // tile indentified by the number   n, which in actuality is
    // the number of the first hit [in the SciTil hit list]
    // belonging to that tile;
    // OriginalSciTilList[n][nnn]  is their list; the dimension of
    // OriginalSciTilList  is fSciTilMaxNumber*fSciTilMaxNumber when nSciTilHits>0
    // (otherwise it is 1*1 ).

    for (int h = 0; h < nHitsInSciTile[nsc]; h++) {
      int m = OriginalSciTilList[nsc * fSciTilMaxNumber + h];
      PndSciTHit *hit = (PndSciTHit *)fSciTHitArray->At(m);
      PndSciTPoint *point = (PndSciTPoint *)fSciTPointArray->At(hit->GetRefIndex());
      if (point->GetTrackID() >= 0) {

        // controllo se questa traccia MC non sia gia' stata inserita
        // prima da uno hit della stessa SciTil;
        bool accetto = true;
        for (int kk = 0; kk < nFromSciTiltoMCTrack->at(nsc); kk++) { // loop sulle tracce MC finora associate;
          if (point->GetTrackID() == FromSciTiltoMCTrackList->at(nsc * nMCTracks + kk)) {
            accetto = false;
            break;
          }
        } // end of  for(int kk=0;kk<nFromSciTiltoMCTrack->at(nsc);kk++)

        if (accetto) {
          FromSciTiltoMCTrackList->at(nsc * nMCTracks + nFromSciTiltoMCTrack->at(nsc)) = point->GetTrackID();
          nFromSciTiltoMCTrack->at(nsc)++;
        }
      } //  end of  if( point->GetTrackID()>=0)
    }   // end of for(int h=0;h<nHitsInSciTile[nsc];h++)

  } // end of  for(int nsc=0; nsc<nSciTilHits; nsc++)

  return;
}
//----------end of function PndTrkComparisonMCtruth::SciTilMatchtoMC

//----------begin function PndTrkComparisonMCtruth::SciTilMatchedSpurioustoTrackCand

//  inizio cambio_in_perl.
void PndTrkComparisonMCtruth::SciTilMatchedSpurioustoTrackCand(Vec<Short_t> *daTrackFoundaTrackMC, Vec<int> *FromSciTiltoMCTrackList,
                                                               //  of dimension  [nSciTilHits][nMCTracks]
                                                               Vec<bool> *keepit, Vec<Short_t> *ListSciTilHitsinTrack,
                                                               // [MAXTRACKSPEREVENT][MAXSCITILHITSINTRACK]
                                                               int MAXSCITILHITSINTRACK,   // input
                                                               Short_t *MCSciTilAloneList, // output; equivalent to a matrix of dimension
                                                                                           // [MAXTRACKSPEREVENT][nSciTilHits]
                                                               Vec<int> *nFromSciTiltoMCTrack,
                                                               Short_t *nMCSciTilAlone, // output
                                                               int nMCTracks,           // input
                                                               Short_t nSciTilHits,     // input
                                                               Vec<Short_t> *nSciTilHitsinTrack,
                                                               Short_t *nSciTilCommon,       // output
                                                               Short_t *nSciTilSpuriinTrack, // output
                                                               Short_t nSttTrackCand,        // input
                                                               Short_t *SciTilCommonList,    // output; equivalent to a matrix of dimension
                                                                                             // [MAXTRACKSPEREVENT][MAXSCITILHITSINTRACK]
                                                               Short_t *SciTilSpuriList      // output; equivalent to a matrix of dimension
                                                                                             // [MAXTRACKSPEREVENT][MAXSCITILHITSINTRACK].
)
{
  //  fine cambio_in_perl.

  // calculate the SciTil hits matched, spurious, alone to all tracks found
  // by Pattern Recognition.

  // The SciTil hit number is the 'purged' one already.

  int tmp_dim1, tmp_dim2;
  if (nSttTrackCand > 0) {
    tmp_dim1 = nSttTrackCand;
  } else {
    tmp_dim1 = 1;
  }
  if (nSciTilHits > 0) {
    tmp_dim2 = nSciTilHits;
  } else {
    tmp_dim2 = 1;
  }

  bool flag_Common;

  // non modificare la seguente linea (viene cambiata da modificaperl);
  bool TMPincludeSciTil[tmp_dim1 * tmp_dim2];
  Vec<bool> includeSciTil(TMPincludeSciTil, tmp_dim1 * tmp_dim2, "includeSciTil");

  int i, index2, j, SciTHn;

  for (i = 0; i < nSttTrackCand; i++) {
    if (!keepit->at(i))
      continue;
    nSciTilSpuriinTrack[i] = 0;
    nMCSciTilAlone[i] = 0;
  }

  for (i = 0; i < nSttTrackCand; i++) {
    if (!keepit->at(i))
      continue;
    nSciTilCommon[i] = 0;
    for (j = 0; j < nSciTilHits; j++) {
      includeSciTil[i * tmp_dim2 + j] = true;
    }

    for (j = 0; j < nSciTilHitsinTrack->at(i); j++) {
      SciTHn = ListSciTilHitsinTrack->at(i * MAXSCITILHITSINTRACK + j);
      includeSciTil[i * tmp_dim2 + SciTHn] = false;
      flag_Common = false;

      // loop over all MC tracks associated to SciTilHit n. SciTHn;
      for (int h = 0; h < nFromSciTiltoMCTrack->at(SciTHn); h++) {
        if (daTrackFoundaTrackMC->at(i) >= 0 && daTrackFoundaTrackMC->at(i) == FromSciTiltoMCTrackList->at(SciTHn * nMCTracks + h)) {
          flag_Common = true;
          index2 = i * MAXSCITILHITSINTRACK + nSciTilCommon[i];
          SciTilCommonList[index2] = SciTHn;
          // increment the n. of common SciTils of Track i
          nSciTilCommon[i]++;
        } // end of if( daTrackFoundaTrackMC->at(i)>= 0 && ......
      }   // end of  for(int h=0;h<nFromSciTiltoMCTrack;h++)

      // if  flag_Common  is still false, this SciTil Hit is spurious;
      if (!flag_Common) {
        index2 = i * MAXSCITILHITSINTRACK + nSciTilSpuriinTrack[i];
        SciTilSpuriList[index2] = SciTHn;
        nSciTilSpuriinTrack[i]++;
      }

    } // end of for(j=0;j<nSciTilHitsinTrack->at(i);j++)

  } // end of for(i=0; i<nSttTrackCand;i++)

  // find the SciTil hits 'alone';
  for (j = 0; j < nSttTrackCand; j++) {
    if (!keepit->at(j))
      continue;
    if (daTrackFoundaTrackMC->at(j) > -1) {
      for (i = 0; i < nSciTilHits; i++) {
        if (!includeSciTil[j * tmp_dim2 + i])
          continue;
        for (int h = 0; h < nFromSciTiltoMCTrack->at(i); h++) {
          if (daTrackFoundaTrackMC->at(j) == FromSciTiltoMCTrackList->at(i * nMCTracks + h)) {
            index2 = j * nSciTilHits + nMCSciTilAlone[j];
            MCSciTilAloneList[index2] = i;
            nMCSciTilAlone[j]++;
          } // end of if( daTrackFoundaTrackMC->at(j) ....)
        }   // end of  for(int h=0;h<nFromSciTiltoMCTrack->at(SciTHn);h++)
      }     // end of for(i=0;i<nSciTilHit;i++)
    }       // end of if(daTrackFoundaTrackMC->at(j)> -1)
  }         // end of  for(j=0; j<nSttTrackCand;j++)

  return;
}

//----------end of function PndTrkComparisonMCtruth::SciTilMatchedSpurioustoTrackCand

//----------begin of function PndTrkComparisonMCtruth::StampaMCTracks
//  inizio cambio_in_perl.
void PndTrkComparisonMCtruth::stampaMCTracks(Double_t /*BFIELD*/, //[R.K. 9/2018] unused
                                             Double_t /*CVEL*/,   //[R.K. 9/2018] unused
                                             TClonesArray *fMCTrackArray, int nMCTracks)
{
  // cout<<"from PndTrkComparisonMCtruth::StampaMCTracks  n. MC Tracks "
  //	<<nMCTracks<<" e lista solo di quelle che vengono da (0,0,0) :\n";
  for (int ic = 0; ic < nMCTracks; ic++) {
    PndMCTrack *pMC = (PndMCTrack *)fMCTrackArray->At(ic);
    if (!(fabs(pMC->GetStartVertex().X()) < 0.5 && fabs(pMC->GetStartVertex().Y()) < 0.5 && fabs(pMC->GetStartVertex().Z()) < 0.5))
      continue;
    // double carica; //[R.K. 9/2018] unused
    // int icode  = pMC->GetPdgCode() ;    //   PDG code of track //[R.K. 9/2018] unused
    // double Pxx = pMC->GetMomentum().X(); //[R.K. 9/2018] unused
    // double Pyy = pMC->GetMomentum().Y(); //[R.K. 9/2018] unused
    // double aaa = sqrt( Pxx*Pxx + Pyy*Pyy); //[R.K. 9/2018] unused
    // double Rr =   aaa*1000./(BFIELD*CVEL);// R (cm) of Helix of track projected //[R.K. 9/2018] unused
    //  in XY plane; B = 2 Tesla
    // TDatabasePDG *fdbPDG= TDatabasePDG::Instance(); //[R.K. 9/2018] unused
    // TParticlePDG *fParticle= fdbPDG->GetParticle(icode); //[R.K. 9/2018] unused
    // if (icode>1000000000) carica = 1.; //[R.K. 9/2018] unused
    // else  carica = fParticle->Charge()/3. ;    //   charge of track //[R.K. 9/2018] unused

    // cout<<"\tTraccia n. "<<ic<<", Px "<<pMC->GetMomentum().X()
    //<<", Py "<<pMC->GetMomentum().Y()
    //<<", Pz "<<pMC->GetMomentum().Z()
    //<<", carica = "<<carica
    //<<"\n\t\tRaggio "<<Rr<<", Xvert "<<pMC->GetStartVertex().X()
    //<<", Yvert "<<pMC->GetStartVertex().Y()
    //<<", Zvert "<<pMC->GetStartVertex().Z()<<endl;
  }
}

//----------end of function PndTrkComparisonMCtruth::StampaMCTracks

//------------------------- begin of function  PndTrkComparisonMCtruth::SttMatchedSpurious

//  inizio cambio_in_perl.

void PndTrkComparisonMCtruth::SttMatchedSpurious(Vec<Short_t> *daTrackFoundaTrackMC, Vec<bool> *InclusionListStt, Vec<Double_t> *info, Vec<bool> *keepit,
                                                 int /*MAXSTTHITS*/,                               //[R.K. 9/2018] unused
                                                 int MAXSTTHITSINTRACK, int /*MAXTRACKSPEREVENT*/, //[R.K. 9/2018] unused
                                                 Vec<Short_t> *ListSttParHitsinTrack, Vec<Short_t> *ListSttSkewHitsinTrack, Vec<Short_t> *MCParalAloneList,
                                                 Vec<Short_t> *MCSkewAloneList, Vec<Short_t> *nHitsInMCTrack, Vec<Short_t> *nSttParHitsinTrack, Vec<Short_t> *nMCParalAlone,
                                                 Vec<Short_t> *nMCSkewAlone, Vec<Short_t> *nParalCommon, Vec<Short_t> *nSkewCommon, Vec<Short_t> *nSkewHitsInMCTrack,
                                                 Vec<Short_t> *nSttSkewHitsinTrack, Vec<Short_t> *nSpuriParinTrack, Vec<Short_t> *nSpuriSkewinTrack, Short_t nSttHit,
                                                 Short_t nTracksFoundSoFar, // those found by PR
                                                 Vec<Short_t> *ParalCommonList, Vec<Short_t> *ParSpuriList, Vec<Short_t> *SkewCommonList, Vec<Short_t> *SkewSpuriList

)
{
  // fine cambio_in_perl.
  bool flaggo;
  Short_t i, jexp, exphit, iHit, enne;
  Short_t emme;

  int index1, index2;

  for (jexp = 0; jexp < nTracksFoundSoFar; jexp++) {
    if (!keepit->at(jexp))
      continue;
    nParalCommon->at(jexp) = 0;
    nSkewCommon->at(jexp) = 0;
    nMCParalAlone->at(jexp) = 0;
    nMCSkewAlone->at(jexp) = 0;
    nSpuriParinTrack->at(jexp) = 0;
    nSpuriSkewinTrack->at(jexp) = 0;

    // --- parallel hits
    for (exphit = 0; exphit < nSttParHitsinTrack->at(jexp); exphit++) {
      index1 = jexp * MAXSTTHITSINTRACK + exphit;
      iHit = ListSttParHitsinTrack->at(index1);
      enne = (Short_t)(info->at(iHit * 7 + 6) + 0.01);
      if (enne == daTrackFoundaTrackMC->at(jexp)) {
        index2 = jexp * MAXSTTHITSINTRACK + nParalCommon->at(jexp);
        ParalCommonList->at(index2) = iHit;
        nParalCommon->at(jexp)++;
      } else {
        index2 = jexp * MAXSTTHITSINTRACK + nSpuriParinTrack->at(jexp);
        ParSpuriList->at(index2) = iHit;
        nSpuriParinTrack->at(jexp)++;
      }
    }
    //--- ricerca degli hits non mecciati, della traccia MC associata a questa traccia trovata.
    for (i = 0; i < nSttHit; i++) {
      emme = (Short_t)(info->at(i * 7 + 6) + 0.01);
      // escludo gli hits non paralleli oppure che non appartengono alla giusta
      // traccia MC
      if (info->at(i * 7 + 5) > 2. || (emme != daTrackFoundaTrackMC->at(jexp)))
        continue;
      if (!InclusionListStt->at(i))
        continue; // escludo gli hits con multiple hits
      flaggo = true;
      for (exphit = 0; exphit < nSttParHitsinTrack->at(jexp); exphit++) {
        index1 = jexp * MAXSTTHITSINTRACK + exphit;
        if (ListSttParHitsinTrack->at(index1) == i) {
          flaggo = false;
          break;
        }
      }
      if (flaggo) {
        index2 = jexp * nSttHit + nMCParalAlone->at(jexp);
        MCParalAloneList->at(index2) = i;
        nMCParalAlone->at(jexp)++;
      } // end of  if(flaggo)
    }   //  end of  for(i=0; i<nSttHit; i++)

    nHitsInMCTrack->at(jexp) = nMCParalAlone->at(jexp) + nParalCommon->at(jexp);
    // --- skew hits

    for (exphit = 0; exphit < nSttSkewHitsinTrack->at(jexp); exphit++) {
      index1 = jexp * MAXSTTHITSINTRACK + exphit;
      iHit = ListSttSkewHitsinTrack->at(index1);
      enne = (Short_t)(info->at(iHit * 7 + 6) + 0.01);
      if (enne == daTrackFoundaTrackMC->at(jexp)) {
        index2 = jexp * MAXSTTHITSINTRACK + nSkewCommon->at(jexp);
        SkewCommonList->at(index2) = iHit;
        nSkewCommon->at(jexp)++;
      } else {
        index2 = jexp * MAXSTTHITSINTRACK + nSpuriSkewinTrack->at(jexp);
        SkewSpuriList->at(index2) = iHit;
        nSpuriSkewinTrack->at(jexp)++;
      }
    }
    //--- ricerca degli hits non mecciati, della traccia MC associata a questa traccia trovata.
    for (i = 0; i < nSttHit; i++) {
      emme = (Short_t)(info->at(i * 7 + 6) + 0.01);

      // considero solo le skew ( info->at(i*7+5)=99.) ed escludo quelle che
      //  non appartengono alla giusta traccia MC
      if (info->at(i * 7 + 5) < 98. || (emme != daTrackFoundaTrackMC->at(jexp)))
        continue;
      if (!InclusionListStt->at(i))
        continue; // escludo gli hits con multiple hits
      flaggo = true;
      for (exphit = 0; exphit < nSttSkewHitsinTrack->at(jexp); exphit++) {
        index1 = jexp * MAXSTTHITSINTRACK + exphit;
        if (i == ListSttSkewHitsinTrack->at(index1)) {
          flaggo = false;
          break;
        }
      }
      if (flaggo) {
        index2 = jexp * nSttHit + nMCSkewAlone->at(jexp);
        MCSkewAloneList->at(index2) = i;
        nMCSkewAlone->at(jexp)++;
      }
    }

    nSkewHitsInMCTrack->at(jexp) = nMCSkewAlone->at(jexp) + nSkewCommon->at(jexp);

  } //   end of  for(jexp=0; jexp<nTracksFoundSoFar;jexp++)

  return;
}

//------------------------- end of function  PndTrkComparisonMCtruth::SttMatchedSpurious

ClassImp(PndTrkComparisonMCtruth);
