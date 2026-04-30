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
 *@author Anna Scholl <a.scholl@fz-juelich.de>
 *@since 02.09.2020
 *@updated 02.02.2021
 *@version 1.0
 **
 ** PANDA class that defines a data structure for a hough space
 ** Task level RECO
 **/

#ifndef PndHoughSpace_H_
#define PndHoughSpace_H_

#include <map>
#include <set>
#include "TVector3.h"

struct PairCmp {
  bool operator()(const std::pair<int, int> &lhs, const std::pair<int, int> &rhs) const { return lhs.second >= rhs.second; }
};

class PndHoughSpace {

 public:
  PndHoughSpace();

  virtual ~PndHoughSpace() {}

  /** @brief Sets the number of bins in x direction of the Hough space. */
  virtual void SetNBins1(double n) { fNbins1 = n; }
  /** @brief Sets the number of bins in y direction of the Hough space. */
  virtual void SetNBins2(double n) { fNbins2 = n; }
  /** @brief Initializes the Hough space. */
  void Init();
  /** @brief Returns the circle (x, y, r) from a bin in the Hough space. The Hough space is in (r, phi) */
  TVector3 GetCircleFromBin(Int_t bin);
  /** @brief Returns a global bin corresponding to the 2d bin set (binX, binY) of the Hough space. */
  Int_t GetBin(Int_t binX, Int_t binY);
  /** @brief Fills the Hough space with the hit (x, y) */
  void Fill(double x, double y);
  /** @brief Finds all maxima in the Hough space. */
  void FindMaxima(std::vector<TVector3> &FoundMaxima);
  /** @brief Clears the Hough space. */
  void clear() { HoughSpaceMapBinToEntries.clear(); }
  /** @brief Returns the number of filled bins in the Hough space. */
  Int_t GetEntries() { return HoughSpaceMapBinToEntries.size(); }
  /** @brief Returns a vector containing the borders of each bin of the x axis. */
  std::vector<double> GetXBorder() { return bordersX; }
  /** @brief Returns a vector containing the borders of each bin of the y axis. */
  std::vector<double> GetYBorder() { return bordersY; }

 private:
  std::map<int, int> HoughSpaceMapBinToEntries;

  std::vector<double> bordersX;
  std::vector<double> bordersY;

  double fNbins1;
  double fNbins2;

  Int_t fCounterNan;
  Int_t fCounterUnderflowXY;
  Int_t fCounterUnderflowXOverflowY;
  Int_t fCounterUnderflowX;
  Int_t fCounterOverflowXUnderflowY;
  Int_t fCounterOveflowXY;
  Int_t fCounterOverflowX;
  Int_t fCounterUnerflowY;
  Int_t fCounterOverflowY;

  ClassDef(PndHoughSpace, 1);
};

#endif /* PndHoughSpace_H_ */
