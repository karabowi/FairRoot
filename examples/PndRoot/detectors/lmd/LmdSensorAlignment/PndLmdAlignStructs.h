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
 * PndLmdAlignStructs.h
 *
 *	Header-only file to collect all structs used during SensorAlignment
 *
 *  Created on: Jul 20, 2017
 *      Author: Roman Klasen, roklasen@uni-mainz.de or klasen@kph.uni-mainz.de
 */

#ifndef LMD_LMDSENSORALIGNMENT_PNDLMDALIGNSTRUCTS_H_
#define LMD_LMDSENSORALIGNMENT_PNDLMDALIGNSTRUCTS_H_

#include "PndLmdHitPair.h"

#include <TH1D.h>
#include <TCanvas.h>

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

using std::cout;
using std::max;
using std::min;
using std::vector;

// simple pixel hit, maybe not even necessary
struct pixelHit {
  int _sensorId;
  double _col;
  double _row;

  double x() const { return _col; }

  pixelHit(int idVal, double colVal, double rowVal)
  {
    _sensorId = idVal;
    _col = colVal;
    _row = rowVal;
  }

  pixelHit()
  {
    _col = -1;
    _row = -1;
    _sensorId = -1;
  }
};

/*
 * contains multiple pixelHits that form a cluster. most routines are for checking,
 * if two separate pixelHits belong to the same cluster
 */
struct pixelCluster {
  int _sensorId;
  double centerCol, centerRow; //,centerZ;
  double clusterSize;
  vector<pixelHit> pixelHits;
  bool clusterReady;

  pixelCluster()
  {
    _sensorId = -1;
    centerCol = -1;
    centerRow = -1; // centerZ=-1;
    clusterSize = -1;
    clusterReady = false;
  }

  pixelCluster(const pixelHit &hit)
  {
    _sensorId = hit._sensorId;
    pixelHits.push_back(hit);

    centerCol = -1;
    centerRow = -1; // centerZ=-1;
    clusterSize = -1;
    clusterReady = false;
  }

  pixelCluster(const pixelCluster &copy)
  {
    _sensorId = copy._sensorId;
    for (size_t i = 0; i < copy.pixelHits.size(); i++) {
      pixelHits.push_back(copy.pixelHits[i]);
    }

    centerCol = -1;
    centerRow = -1; // centerZ=-1;
    clusterSize = -1;
    clusterReady = false;
  }

  // checks, if two clusters lie DIRECTLY next to each other, that means any two pixels
  // must be directly next to each other
  // TODO: inefficient code, may be improved
  bool isNeighbour(pixelCluster &other)
  {
    // first, they must be on same sensor
    if (_sensorId != other._sensorId) {
      return false;
    }
    double _col1, _col2, _row1, _row2;
    for (size_t i = 0; i < this->pixelHits.size(); i++) {
      _col1 = this->pixelHits[i]._col;
      _row1 = this->pixelHits[i]._row;
      for (size_t j = 0; j < other.pixelHits.size(); j++) {
        _col2 = other.pixelHits[j]._col;
        _row2 = other.pixelHits[j]._row;
        // check if neighboring, that means distance of pixels is smaller than 1.5 pixels
        if ((_col2 - _col1) * (_col2 - _col1) + (_row2 - _row1) * (_row2 - _row1) < 2.25) {
          return true;
        }
      }
    }
    return false;
  }

  // merges other to this one
  void merge(pixelCluster &other)
  {
    for (size_t i = 0; i < other.pixelHits.size(); i++) {
      pixelHits.push_back(other.pixelHits[i]);
    }
  }

  void calculateCenter()
  {
    centerCol = 0;
    centerRow = 0;
    for (size_t i = 0; i < pixelHits.size(); i++) {
      centerCol += pixelHits[i]._col;
      centerRow += pixelHits[i]._row;
    }
    centerCol /= pixelHits.size();
    centerRow /= pixelHits.size();
    double tempDistance;
    // calculate size, go from corner to corner for clusters larger than 2 pixels
    if (pixelHits.size() == 1) {
      clusterSize = 1;
    } else {
      for (size_t i = 0; i < pixelHits.size(); i++) {
        for (size_t j = i + 1; j < pixelHits.size(); j++) {
          double deltax = (pixelHits[i]._col - pixelHits[j]._col);
          if (deltax > 0) {
            deltax = deltax + 1;
          }
          if (deltax < 0) {
            deltax = deltax - 1;
          }
          double deltay = (pixelHits[i]._row - pixelHits[j]._row);
          if (deltay > 0) {
            deltay = deltay + 1;
          }
          if (deltay < 0) {
            deltay = deltay - 1;
          }
          tempDistance = sqrt(deltax * deltax + deltay * deltay);
          clusterSize = max(clusterSize, tempDistance);
        }
      }
    }
    clusterReady = true;
  }

  void printPixels()
  {
    for (size_t i = 0; i < pixelHits.size(); i++) {
      cout << "pixelHit x:" << pixelHits[i]._col << ", y:" << pixelHits[i]._row << " on sensor " << pixelHits[i]._sensorId << "\n";
    }
  }
  void printCenter()
  {
    cout << "clusterCenter x:" << centerCol << ", y:" << centerRow << " on sensor " << _sensorId << ", contains " << pixelHits.size() << " pixels and is " << clusterSize
         << " pixels in diameter."
         << "\n";
  }
};

#endif /* LMD_LMDSENSORALIGNMENT_PNDLMDALIGNSTRUCTS_H_ */
