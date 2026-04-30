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
//  PndHoughSpace
//  Defines a data structure for a hough space
/////////////////////////////////////////////////////////////////

/** PndHoughSpace
 *@author Anna Alicke <a.alicke@fz-juelich.de>
 *@since 02.09.2020
 *@updated 02.02.2021
 *@version 2.0
 **
 ** PANDA class that defines a data structure for a hough space
 ** Task level RECO
 **/

#include "PndHoughSpace.h"
#include <cmath>
#include <stdio.h>
#include <iostream>
#include "FairLogger.h"
using std::cout;
using std::endl;

PndHoughSpace::PndHoughSpace() : fNbins1(450), fNbins2(450) {}

/**
 * @brief      Returns the circle (x, y, r) from a bin in the Hough space. The Hough space is in (r, phi)
 *
 * @param[in]  bin   The bin
 *
 * @return     The corresponding circle (x, y, r)
 */
TVector3 PndHoughSpace::GetCircleFromBin(Int_t bin)
{
  Int_t nx = bordersX.size();
  Int_t ny = bordersY.size();
  Int_t binx = bin % nx;
  Int_t biny = ((bin - binx) / nx) % ny;

  double x = (bordersX[binx - 1] + bordersX[binx]) / 2.;
  double y = (bordersY[biny - 1] + bordersY[biny]) / 2.;

  double r = x;
  double phi = y;
  TVector3 vec(r * TMath::Cos(phi * TMath::Pi() / 180.), r * TMath::Sin(phi * TMath::Pi() / 180.), r);
  return vec;
}

/**
 * @brief      Returns a global bin corresponding to the 2d bin set (binX, binY) of the Hough space.
 *
 * @param[in]  binX  The bin in x direction
 * @param[in]  binY  The bin in y direction
 *
 * @return     The global bin.
 */
Int_t PndHoughSpace::GetBin(Int_t binX, Int_t binY)
{
  return binX + (bordersX.size()) * binY;
}

/**
 * @brief      Fills the Hough space with the hit (x, y)
 *
 * @param[in]  x     x value of hit (if Hough space is in (r, phi) x correspond to r)
 * @param[in]  y     y value of hit (if Hough space is in (r, phi) y correspond to phi)
 */
void PndHoughSpace::Fill(double x, double y)
{
  LOG(debug2) << "filled values x/r: " << x << " y/phi " << y << endl;
  // overflow/ underflow
  if (std::isnan(x) || std::isnan(y))
    fCounterNan += 1;
  else if (x < *(bordersX.begin())) {
    if (y < *(bordersY.begin()))
      fCounterUnderflowXY += 1;
    else if (y > *((bordersY.end() - 1)))
      fCounterUnderflowXOverflowY += 1;
    else
      fCounterUnderflowX += 1;
  } else if (x > *(bordersX.end() - 1)) {
    if (y < *(bordersY.begin()))
      fCounterOverflowXUnderflowY += 1;
    else if (y > *((bordersY.end() - 1)))
      fCounterOveflowXY += 1;
    else
      fCounterOverflowX += 1;
  } else {
    if (y < *(bordersY.begin()))
      fCounterUnerflowY += 1;
    else if (y > *((bordersY.end() - 1)))
      fCounterOverflowY += 1;
    else {

      std::vector<double>::iterator itx = std::lower_bound(bordersX.begin(), bordersX.end(), x);
      std::vector<double>::iterator ity = std::lower_bound(bordersY.begin(), bordersY.end(), y);

      Int_t binX = std::distance(bordersX.begin(), itx);
      Int_t binY = std::distance(bordersY.begin(), ity);
      Int_t bin = GetBin(binX, binY);

      auto it = HoughSpaceMapBinToEntries.find(bin);
      if (it == HoughSpaceMapBinToEntries.end())
        HoughSpaceMapBinToEntries[bin] = 1;
      else
        HoughSpaceMapBinToEntries[bin] += 1;

      LOG(debug2) << "filled in bin: " << bin << " with " << HoughSpaceMapBinToEntries[bin] << " entries" << endl;
    }
  }
}

/**
 * @brief      Initializes the Hough space.
 *             This means all vales and counter are set to zero, all maps and vectors are cleared.
 */
void PndHoughSpace::Init()
{
  fCounterNan = 0;
  fCounterUnderflowXY = 0;
  fCounterUnderflowXOverflowY = 0;
  fCounterUnderflowX = 0;
  fCounterOverflowXUnderflowY = 0;
  fCounterOveflowXY = 0;
  fCounterOverflowX = 0;
  fCounterUnerflowY = 0;
  fCounterOverflowY = 0;
  bordersX.clear();
  bordersY.clear();

  for (int i = 0; i < fNbins1; i++) {
    double border = -0.5 + i * ((300.5 + 0.5) / fNbins1);
    bordersX.push_back(border);
  }
  for (int i = 0; i < fNbins2; i++) {
    double border = -180.5 + i * ((180.5 + 180.5) / fNbins2);
    bordersY.push_back(border);
  }
}

/**
 * @brief      Finds all maxima in the Hough space.
 *             They are found by refilling the map in a set, which is by definition sorted. In this case it is sorted by the second value which is the number of entries in the
 * hough space.
 *
 * @param[out] FoundMaxima  A vector of found maxima.
 */
void PndHoughSpace::FindMaxima(std::vector<TVector3> &FoundMaxima)
{
  std::set<std::pair<int, int>, PairCmp> SortedHoughSpaceSet;
  for (auto iterator = HoughSpaceMapBinToEntries.begin(); iterator != HoughSpaceMapBinToEntries.end(); iterator++) {
    std::pair<int, int> p(iterator->first, iterator->second);
    SortedHoughSpaceSet.insert(p);
  }

  // sort map
  // Declaring a set that will store the pairs
  std::set<std::pair<int, int>, PairCmp>::iterator it = SortedHoughSpaceSet.begin();
  Int_t elementBefore = it->second;
  for (std::pair<int, int> element : SortedHoughSpaceSet) {
    if (element.second < elementBefore)
      break;
    else {
      elementBefore = element.second;
      TVector3 maximum = GetCircleFromBin(element.first);
      FoundMaxima.push_back(maximum);
      LOG(debug2) << "maximum bin: " << element.first << " number of entries in maximum bin: " << element.second << " maximum: (" << maximum.X() << "," << maximum.Y() << ","
                  << maximum.Z() << ")" << endl;
    }
  }
}

ClassImp(PndHoughSpace);
