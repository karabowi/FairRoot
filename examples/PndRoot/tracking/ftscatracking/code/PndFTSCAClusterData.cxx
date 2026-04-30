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
    Copyright (C) 2009 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) version 3.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include "PndFTSCAClusterData.h"
#include "PndFTSTPCCAMath.h"

void PndFTSCAClusterData::readEvent(const PndFTSCAGBHit *hits, int *offset, int numberOfClusters)
{
  fNumberOfClusters.reserve(PndFTSCAParameters::NumberOfRows2);
  fRowOffset.reserve(PndFTSCAParameters::NumberOfRows2);
  fData.reserve(CAMath::Min(64, numberOfClusters / 64));

  fSlice = hits[*offset].ISlice();
  fFirstRow = hits[*offset].IRow(); // the data is row sorted first in the slice, so this is our first row
  fLastRow = fFirstRow;
  int row = fFirstRow;
  for (int i = 0; i < row; ++i) {
    fNumberOfClusters.push_back(0);
    fRowOffset.push_back(0);
  }
  fRowOffset.push_back(0);
  for (int &i = *offset; i < numberOfClusters; ++i) {
    const PndFTSTPCCAGBHit &hit = hits[i];
    if (hit.ISlice() != fSlice) {
      // the data is slice sorted first so we're done gathering our data
      break;
    }
    while (row < hit.IRow()) {
      fNumberOfClusters.push_back(fData.size() - fRowOffset.back());
      fRowOffset.push_back(fData.size());
      ++row;
    }
    Data d = {hit.X(), hit.Y(), hit.Z(), hit.ID(), hit.IRow()};
    fData.push_back(d);
  }
  fNumberOfClusters.push_back(fData.size() - fRowOffset.back());
  fLastRow = row; // the last seen row is the last row in this slice
}
