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

/////////////////////////////////////////////////////////////
//  PndHoughApollonius
//  Calculate Apollonius Circles
/////////////////////////////////////////////////////////////////

/** PndHoughApollonius
 *@author Anna Alicke <a.alicke@fz-juelich.de>
 *@since 29.10.2018
 *@updated 23.11.2021
 *@version 3.1
 **
 ** PANDA class for calculating Apollonius circles
 ** Task level RECO
 **/

#define _USE_MATH_DEFINES

// Includes from ROOT
#include "TVector3.h"
#include "PndTrackCand.h"
#include "PndHoughApollonius.h"
#include "FairLogger.h"
// general
#include <algorithm>
#include <iostream>

ClassImp(PndHoughApollonius)

  /**
   * @brief      Calculation of Apollonius circles on CPU. (This is the same as implemented in cuda).
   *
   * @param[in]  num                number of all multiplets
   * @param[in]  hit0               An array of the x, y, r and e_r component of hit 0 (The 4 dimensions are converted to a 1d array)
   * @param[in]  hit1               An array of the x, y, r and e_r component of hit 1 (The 4 dimensions are converted to a 1d array)
   * @param[in]  hit2               An array of the x, y, r and e_r component of hit 2 (The 4 dimensions are converted to a 1d array)
   * @param[out] apolloniusCircles  An array of the resulting apollonius circles (x, y, r, e_x, e_y, e_r are stored in a 1d array as for the hits)
   */
  void PndHoughApollonius::ApolloniusCudaCalcCPU(int num, double hit0[], double hit1[], double hit2[], double apolloniusCircles[])
{
  int s[8 * 3] = {-1, -1, -1, -1, -1, 1, -1, 1, -1, -1, 1, 1, 1, -1, -1, 1, -1, 1, 1, 1, -1, 1, 1, 1};
  double counter = 0;
  double defaultval = 0;
  for (int n = 0; n < num; n++) {
    double a = 2 * (hit0[n * 4 + 0] - hit1[n * 4 + 0]);
    double b = 2 * (hit0[n * 4 + 1] - hit1[n * 4 + 1]);
    double a_dash = 2 * (hit0[n * 4 + 0] - hit2[n * 4 + 0]);
    double b_dash = 2 * (hit0[n * 4 + 1] - hit2[n * 4 + 1]);

    double N = a * b_dash - a_dash * b;

    if (TMath::Abs(N) < 0.0000001) {
      LOG(DEBUG) << "-E- ApolloniusCudaCalcCPU: Small N detected: " << n << " try streight line fit (" << hit0[n * 4 + 0] << "," << hit0[n * 4 + 1] << "," << hit0[n * 4 + 2]
                 << "), (" << hit1[n * 4 + 0] << "," << hit1[n * 4 + 1] << "," << hit1[n * 4 + 2] << "), (" << hit2[n * 4 + 0] << "," << hit2[n * 4 + 1] << "," << hit2[n * 4 + 2]
                 << ") ";
      for (int i = 0; i < 8; i++) {
        if (hit1[n * 4 + 0] == hit2[n * 4 + 0]) {
          // std::cout << "m = inf" << std::endl;
          apolloniusCircles[n * 8 * 6 + i * 6 + 0] = std::numeric_limits<double>::quiet_NaN();
          apolloniusCircles[n * 8 * 6 + i * 6 + 1] = hit1[n * 4 + 0];
          apolloniusCircles[n * 8 * 6 + i * 6 + 2] = defaultval;
          apolloniusCircles[n * 8 * 6 + i * 6 + 3] = defaultval;
          apolloniusCircles[n * 8 * 6 + i * 6 + 4] = defaultval;
          apolloniusCircles[n * 8 * 6 + i * 6 + 5] = defaultval;
        } else if (hit1[n * 4 + 1] == hit2[n * 4 + 1]) {
          // std::cout << "m = 0" << std::endl;
          apolloniusCircles[n * 8 * 6 + i * 6 + 0] = defaultval;
          apolloniusCircles[n * 8 * 6 + i * 6 + 1] = hit1[n * 4 + 1];
          apolloniusCircles[n * 8 * 6 + i * 6 + 2] = defaultval;
          apolloniusCircles[n * 8 * 6 + i * 6 + 3] = defaultval;
          apolloniusCircles[n * 8 * 6 + i * 6 + 4] = defaultval;
          apolloniusCircles[n * 8 * 6 + i * 6 + 5] = defaultval;
        } else {
          double m = (hit2[n * 4 + 1] - hit0[n * 4 + 1]) / (hit2[n * 4 + 0] - hit0[n * 4 + 0]);
          double b = hit0[n * 4 + 1] - m * hit0[n * 4 + 0];
          // std::cout << "m = " << m << ", b = " << b << std::endl;
          apolloniusCircles[n * 8 * 6 + i * 6 + 0] = m;
          apolloniusCircles[n * 8 * 6 + i * 6 + 1] = b;
          apolloniusCircles[n * 8 * 6 + i * 6 + 2] = defaultval;
          apolloniusCircles[n * 8 * 6 + i * 6 + 3] = defaultval;
          apolloniusCircles[n * 8 * 6 + i * 6 + 4] = defaultval;
          apolloniusCircles[n * 8 * 6 + i * 6 + 5] = defaultval;
        }
      }
      continue;
    }

    double d = (hit0[n * 4 + 0] * hit0[n * 4 + 0] + hit0[n * 4 + 1] * hit0[n * 4 + 1] - hit0[n * 4 + 2] * hit0[n * 4 + 2]) -
               (hit1[n * 4 + 0] * hit1[n * 4 + 0] + hit1[n * 4 + 1] * hit1[n * 4 + 1] - hit1[n * 4 + 2] * hit1[n * 4 + 2]);
    double d_dash = (hit0[n * 4 + 0] * hit0[n * 4 + 0] + hit0[n * 4 + 1] * hit0[n * 4 + 1] - hit0[n * 4 + 2] * hit0[n * 4 + 2]) -
                    (hit2[n * 4 + 0] * hit2[n * 4 + 0] + hit2[n * 4 + 1] * hit2[n * 4 + 1] - hit2[n * 4 + 2] * hit2[n * 4 + 2]);

    double Ax = b_dash * d - b * d_dash;
    double Cy = a * d_dash - a_dash * d;

    for (int i = 0; i < 8; i++) {
      double c = 2 * (s[i * 3 + 0] * hit0[n * 4 + 2] + s[i * 3 + 1] * hit1[n * 4 + 2]);
      double c_dash = 2 * (s[i * 3 + 0] * hit0[n * 4 + 2] + s[i * 3 + 2] * hit2[n * 4 + 2]);
      double Bx = -b_dash * c + b * c_dash;
      double Dy = a_dash * c - a * c_dash;

      double A = (Bx * Bx) / (N * N) + (Dy * Dy) / (N * N) - 1;
      double B = 2 * Bx / N * (Ax / N - hit0[n * 4 + 0]) + 2 * Dy / N * (Cy / N - hit0[n * 4 + 1]) - 2 * s[i * 3 + 0] * hit0[n * 4 + 2];
      double C = ((Ax / N - hit0[n * 4 + 0]) * (Ax / N - hit0[n * 4 + 0])) + ((Cy / N - hit0[n * 4 + 1]) * (Cy / N - hit0[n * 4 + 1])) -
                 ((s[i * 3 + 0] * hit0[n * 4 + 2]) * (s[i * 3 + 0] * hit0[n * 4 + 2]));
      double D = (B * B) - 4 * A * C;

      double rk = (-B + sqrt(D)) / (2.0 * A);
      double xk = (Ax + Bx * rk) / N;
      double yk = (Cy + Dy * rk) / N;

      // Sett error to 0 (currently not implemented)
      TVector3 eapol(0, 0, 0);

      // if clause to verify that in case of point-like hits the same apollonius circle is not included more than once.
      if ((hit0[n * 4 + 2] == 0 && i > 3) || (hit1[n * 4 + 2] == 0 && i > 1 && i <= 3) || (hit1[n * 4 + 2] == 0 && i > 5) || (hit2[n * 4 + 2] == 0 && i % 2 > 0)) {

        apolloniusCircles[n * 8 * 6 + i * 6 + 0] = defaultval;
        apolloniusCircles[n * 8 * 6 + i * 6 + 1] = defaultval;
        apolloniusCircles[n * 8 * 6 + i * 6 + 2] = defaultval;
        apolloniusCircles[n * 8 * 6 + i * 6 + 3] = defaultval;
        apolloniusCircles[n * 8 * 6 + i * 6 + 4] = defaultval;
        apolloniusCircles[n * 8 * 6 + i * 6 + 5] = defaultval;

      } else {
        apolloniusCircles[n * 8 * 6 + i * 6 + 0] = xk;
        apolloniusCircles[n * 8 * 6 + i * 6 + 1] = yk;
        apolloniusCircles[n * 8 * 6 + i * 6 + 2] = rk;
        apolloniusCircles[n * 8 * 6 + i * 6 + 3] = eapol.X();
        apolloniusCircles[n * 8 * 6 + i * 6 + 4] = eapol.Y();
        apolloniusCircles[n * 8 * 6 + i * 6 + 5] = eapol.Z();
      }
    }
  }
}

/**
 * @brief      This functions brings the FairLinks in a simpler structure for using cuda
 *
 * @param[in]  multiplets         All possible multiplets for a tracklet
 * @param[out] apolloniusCircles  Result array for the apollonius circles
 */
void PndHoughApollonius::ApolloniusCuda(std::vector<std::vector<FairLink>> &multiplets, double apolloniusCircles[])
{
  /** Here a pointer is needed, because the number of multiplets can get very large (> 10000).
   *   The memory of the stack (where usual arrays are stored) is not large enough. So the array has to be dynamically allocated.
   *   Also a different structure like std::array<> was tested.
   *   But it was not possible to pass it per reference to another function (ApolloniusCudaCalcCPU), because the size of the array can not be passed as a parameter.
   */
  double *hit0 = new double[multiplets.size() * 4];
  double *hit1 = new double[multiplets.size() * 4];
  double *hit2 = new double[multiplets.size() * 4];

  for (int n = 0; n < multiplets.size(); n++) {
    std::vector<FairLink> multiplet = multiplets[n];

    FairHit *k0 = fMapFairLinktoFairHit[multiplet[0]];
    Double_t isochrone0 = fMapFairLinktoIsochrone[multiplet[0]];
    Double_t isochroneError0 = fMapFairLinktoIsochroneError[multiplet[0]];
    hit0[n * 4 + 0] = k0->GetX();
    hit0[n * 4 + 1] = k0->GetY();
    hit0[n * 4 + 2] = isochrone0;
    hit0[n * 4 + 3] = isochroneError0;

    FairHit *k1 = fMapFairLinktoFairHit[multiplet[1]];
    Double_t isochrone1 = fMapFairLinktoIsochrone[multiplet[1]];
    Double_t isochroneError1 = fMapFairLinktoIsochroneError[multiplet[1]];
    hit1[n * 4 + 0] = k1->GetX();
    hit1[n * 4 + 1] = k1->GetY();
    hit1[n * 4 + 2] = isochrone1;
    hit1[n * 4 + 3] = isochroneError1;

    if (multiplet.size() == 3) {
      FairHit *k2 = fMapFairLinktoFairHit[multiplet[2]];
      Double_t isochrone2 = fMapFairLinktoIsochrone[multiplet[2]];
      Double_t isochroneError2 = fMapFairLinktoIsochroneError[multiplet[2]];
      hit2[n * 4 + 0] = k2->GetX();
      hit2[n * 4 + 1] = k2->GetY();
      hit2[n * 4 + 2] = isochrone2;
      hit2[n * 4 + 3] = isochroneError2;
    } else {
      hit2[n * 4 + 0] = 0.;
      hit2[n * 4 + 1] = 0.;
      hit2[n * 4 + 2] = 0.;
      hit2[n * 4 + 3] = 0.;
    }
  }

  ApolloniusCudaCalcCPU(multiplets.size(), hit0, hit1, hit2, apolloniusCircles);
  delete[] hit0, hit1, hit2;
}
