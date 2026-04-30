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

/*
 * PndSttSkewStrawPzFinderData.h
 *
 *  Created on: Jun 29, 2016
 *      Author: walan603
 */

#ifndef PNDSTTSKEWSTRAWPZFINDERDATA_H
#define PNDSTTSKEWSTRAWPZFINDERDATA_H

// Base Class Headers
#include "TObject.h"
#include "TVector2.h"
#include "FairTimeStamp.h"
#include "PndSttHit.h"

#include <iostream>

using namespace std;

class PndSttSkewStrawPzFinderData : public FairTimeStamp {
 public:
  PndSttSkewStrawPzFinderData();
  virtual ~PndSttSkewStrawPzFinderData();

  unsigned int getMethod() const { return fMethod; }
  void setMethod(unsigned int input) { fMethod = input; }

  vector<PndSttHit> getSttHits() { return skewHits; }
  void setSttHits(const vector<PndSttHit> &input) { skewHits = input; }

  double getLineIntercept() const { return LineIntercept; }
  void setLineIntercept(double input) { LineIntercept = input; }

  double getLineSlope() const { return LineSlope; }
  void setLineSlope(double input) { LineSlope = input; }

  const TVector2 &getCenter() const { return center; }
  void setCenter(const TVector2 &input) { center = input; }

  double getRadius() const { return radius; }
  void setRadius(double input) { radius = input; }

  double getS0() const { return Snot; }
  void setS0(double input) { Snot = input; }

  const vector<vector<TVector2>> &getSZPairVector() const { return SZPairVector; }
  void setSZPairVector(const vector<vector<TVector2>> &input) { SZPairVector = input; }

  const vector<pair<unsigned int, unsigned int>> &getTrueSZindex() const { return TrueSZindex; }
  void setTrueSZindex(const vector<pair<unsigned int, unsigned int>> &input) { TrueSZindex = input; }

  const vector<vector<TVector2>> &getTrueSZ() const { return TrueSZ; }
  void setTrueSZ(const vector<vector<TVector2>> &input) { TrueSZ = input; }

  const vector<TVector2> &getSZPairVectorMvdPixel() const { return SZPairVectorMvdPixel; }
  void setSZPairVectorMvdPixel(const vector<TVector2> &input) { SZPairVectorMvdPixel = input; }

  const vector<TVector2> &getTrueSZMvdPixel() const { return TrueSZMvdPixel; }
  void setTrueSZMvdPixel(const vector<TVector2> &trueSZMvdPixel) { TrueSZMvdPixel = trueSZMvdPixel; }

  const vector<TVector2> &getSZPairVectorMvdStrip() const { return SZPairVectorMvdStrip; }
  void setSZPairVectorMvdStrip(const vector<TVector2> &input) { SZPairVectorMvdStrip = input; }

  const vector<TVector2> &getTrueSZMvdStrip() const { return TrueSZMvdStrip; }
  void setTrueSZMvdStrip(const vector<TVector2> &input) { TrueSZMvdStrip = input; }

  const vector<TVector2> &getSZPairVectorGem() const { return SZPairVectorGem; }
  void setSZPairVectorGem(const vector<TVector2> &input) { SZPairVectorGem = input; }

  const vector<TVector2> &getTrueSZGem() const { return TrueSZGem; }
  void setTrueSZGem(const vector<TVector2> &input) { TrueSZGem = input; }

  const vector<vector<PndSttHit>> &getAlignedSkewStt() const { return AlignedSkewStt; }
  void setAlignedSkewStt(const vector<vector<PndSttHit>> &input) { AlignedSkewStt = input; }

 private:
  unsigned int fMethod; // 0 = use all hits, 1 = Recursive Line Fit, 2 = Combi line Finder, 3 = Hough Transform
  double LineSlope;
  double LineIntercept;
  TVector2 center;
  double radius;
  double Snot;
  vector<PndSttHit> skewHits;

  vector<vector<TVector2>> SZPairVector;
  vector<pair<unsigned int, unsigned int>> TrueSZindex; // indices of selected SZ positions in SZPairVector.
  vector<vector<TVector2>> TrueSZ;
  vector<TVector2> SZPairVectorMvdPixel;
  vector<TVector2> TrueSZMvdPixel;
  vector<TVector2> SZPairVectorMvdStrip;
  vector<TVector2> TrueSZMvdStrip;
  vector<TVector2> SZPairVectorGem;
  vector<TVector2> TrueSZGem;

  vector<vector<PndSttHit>> AlignedSkewStt;

  ClassDef(PndSttSkewStrawPzFinderData, 1)
};

#endif /* PNDTOOLS_STTCELLTRACKFINDER_PNDSTTSKEWSTRAWPZFINDERDATA_H_ */
