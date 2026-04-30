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

#include "BSEmcFwEndcap_AgThomaBonnVersion_SensorNameIdMap.h"

#include <algorithm>
#include <stdio.h>
#include <stdlib.h>

#include "TString.h"

#include "fairlogger/Logger.h"

BSEmcFwEndcap_AgThomaBonnVersion_SensorNameIdMap::BSEmcFwEndcap_AgThomaBonnVersion_SensorNameIdMap() : PndSensorNameIdMap()
{
  // These are the x and y alveole indices of the 8er alveoles being vertically mounted instead of horizontally.
  // Therefore we must identify these and do special computations to get the proper row and column number
  // for a crystal.
  f0DegRot = {
    {8, 0},   {7, 0},   {6, 0},   {5, 0},  {4, 0},

    {2, 2},   {1, 2},   {0, 2},   {-1, 2}, {-2, 2},

    {6, -7},  {5, -8},  {3, -9},  {2, -9},

    {-2, -9}, {-3, -9}, {-5, -8}, {-6, -7} // 18
  };

  f90DegRot = {
    {-9, 0},  {-9, 1},  {-9, 2},  {-8, 5}, {-7, 6},

    {-7, -6}, {-8, -5}, {-9, -2}, {-9, -1} // 27
  };

  f180DegRot = {
    {-6, 7}, {-5, 8}, {-3, 9}, {-2, 9}, {2, 9}, {3, 9}, {5, 8}, {6, 7}, {2, -2}, {1, -2}, {0, -2}, {-1, -2}, {-2, -2}, {-4, 0}, {-5, 0}, {-6, 0}, {-7, 0}, {-8, 0} // 18
  };

  f270DegRot = {
    {7, 6}, {8, 5}, {9, 2}, {9, 1}, {9, 0}, {9, -1}, {9, -2}, {8, -5}, {7, -6} // 27
  };

  fRequireOffset = {{9, -2},
                    {9, -1},
                    //{9,0},
                    {9, 1},
                    {9, 2},

                    {-9, -2},
                    {-9, -1},
                    //{-9,0},
                    {-9, 1},
                    {-9, 2}};
}

BSEmcFwEndcap_AgThomaBonnVersion_SensorNameIdMap::~BSEmcFwEndcap_AgThomaBonnVersion_SensorNameIdMap() {}

Int_t BSEmcFwEndcap_AgThomaBonnVersion_SensorNameIdMap::CreateId(TString &t_sensName) const
{
  LOG(trace) << "BSEmcFwEndcap_AgThomaBonnVersion_SensorNameIdMap::CreateId(" << t_sensName << ")";
  if (t_sensName.Contains("3_0702_3200_m_04")) {
    Int_t section = 0, detId = 0, alveolex = 0, alveoley = 0, crystal = 0, alveoleType = 0;

    sscanf(t_sensName.Data(), "/cave_%*i/FwEndcap_%*i/Alveoles_%*i/Section%i_%*i_%*i/SubUnitX%iY%i_%*i_%*i/SubUnit%iCrystals_%*i/FwEndcapCrystal_%i_%*i/3_0702_3200_m_04_%*i",
           &section, &alveolex, &alveoley, &alveoleType, &crystal);
    if (crystal > 15) {
      LOG(error) << "Crystal Value " << crystal << " larger than 15. Aborting.";
      abort();
    }
    auto xAndY = ConvertIntoAlveole(section, alveolex, alveoley);
    alveolex = xAndY.first;
    alveoley = xAndY.second;
    if (alveoleType == 8) {
      crystal = RemapCrystal(crystal, alveolex, alveoley);
    } else if (alveoleType == 16 && (section == 3 || section == 4 || section == 7)) {
      // Remap the crystal number due to rotation of alveole in quadrant 3 and 4 (consisting in CAD of section 3,4 and 7)
      crystal = 15 - crystal;
    }
    Int_t row = CalculateRow(crystal, alveolex, alveoley);
    Int_t column = CalculateColumn(crystal, alveolex, alveoley);
    LOG(trace) << "fAlveoleX: " << alveolex << ", fAlveoleY: " << alveoley << ", fCrystal: " << crystal;
    LOG(trace) << "row: " << row << " and column: " << column;

    detId = 3 * 100000000 + row * 1000000 + 0 * 10000 + column;
    LOG(trace) << "detId: " << detId;
    return detId;
  }
  return -1;
}

// Geometry consists of 7 sections. Section 0 and 5 are the middle column (alveoles here belong to quadrant 1 and 2 or 3 and 4).
// Section 1 to 4 correspond to alveoles that only belong to quadrant 1 to 4. Section 6 and 7 consists of the horizontal 8er alveoles that belong to 1 and 4 or 2 and 3.
std::pair<Int_t, Int_t> BSEmcFwEndcap_AgThomaBonnVersion_SensorNameIdMap::ConvertIntoAlveole(Int_t t_section, Int_t t_alveolex, Int_t t_alveoley) const
{
  if (t_section == 1) {
    return {std::abs(t_alveolex), std::abs(t_alveoley)};
  } else if (t_section == 2) {
    return {-1 * std::abs(t_alveolex), std::abs(t_alveoley)};
  } else if (t_section == 3) {
    return {-1 * std::abs(t_alveolex), -1 * std::abs(t_alveoley)};
  } else if (t_section == 4) {
    return {std::abs(t_alveolex), -1 * std::abs(t_alveoley)};
  } else if (t_section == 5) {
    return {std::abs(t_alveolex), std::abs(t_alveoley)};
  } else if (t_section == 6) {
    return {-1 * std::abs(t_alveolex), std::abs(t_alveoley)};
  } else if (t_section == 7) {
    return {std::abs(t_alveolex), -1 * std::abs(t_alveoley)};
  } else if (t_section == 8) {
    return {std::abs(t_alveolex), std::abs(t_alveoley)};
  }
  LOG(error) << "Section " << t_section << " unknown! Aborting!";
  exit(-1);
  return {0, 0};
}

// remap the crystals so that they all are labeled in the following order.
// overall row                          overall col    6 5 4 3     2 1-1-2    -3-4
//    row                                      col   1 2 3 4
//                                                  ----------- ----------- -------
// 1   4                                            | 0 1 2 3 | | 0 1 2 3 | | 0 1 |
// 2   3                                            | 4 5 6 7 | | 4 5 6 7 | | 4 5 |
// 3   2                                            | 8 9 A B | | 8 9 A B | | 8 9 |
// 4   1                                            | C D E F | | C D E F | | C D |
//                                                  ----------- ----------- -------

Int_t BSEmcFwEndcap_AgThomaBonnVersion_SensorNameIdMap::RemapCrystal(Int_t t_crystal, Int_t t_alveolex, Int_t t_alveoley) const
{
  if (IsIncludedIn(t_alveolex, t_alveoley, f0DegRot)) {
    return t_crystal;
  } else if (IsIncludedIn(t_alveolex, t_alveoley, f90DegRot)) {
    Int_t row = t_crystal % 4;
    Int_t col = t_crystal / 4;
    return (3 - row) * 4 + col;
  } else if (IsIncludedIn(t_alveolex, t_alveoley, f180DegRot)) {
    return 15 - t_crystal;
  } else if (IsIncludedIn(t_alveolex, t_alveoley, f270DegRot)) {
    Int_t row = 3 - t_crystal % 4;
    Int_t col = 1 - t_crystal / 4;
    return (3 - row) * 4 + col + 2;
  }
  return t_crystal;
}

Bool_t BSEmcFwEndcap_AgThomaBonnVersion_SensorNameIdMap::IsIncludedIn(Int_t t_xIndex, Int_t t_yIndex, const std::vector<std::pair<Int_t, Int_t>> &t_indices) const
{
  auto pos =
    std::find_if(t_indices.begin(), t_indices.end(), [t_xIndex, t_yIndex](const std::pair<Int_t, Int_t> &apair) { return apair.first == t_xIndex && apair.second == t_yIndex; });

  return pos != t_indices.end();
}

Int_t BSEmcFwEndcap_AgThomaBonnVersion_SensorNameIdMap::CalculateRow(Int_t t_crystal, Int_t t_xAlveoleIndex, Int_t t_yAlveoleIndex) const
{
  Int_t sign = (t_yAlveoleIndex > 0) ? 1 : -1;
  Int_t crystalRow = 3 - t_crystal / 4 + 1; // +1 offset by one
  Int_t rowOfLastAlveole = 0;
  const Int_t rowOffset = 37;
  if (t_yAlveoleIndex != 0) {
    rowOfLastAlveole = sign * (4 * (std::abs(t_yAlveoleIndex) - 1) + 1); // +1 due to Y0 alveoles
  } else if (t_yAlveoleIndex == 0) {
    // Handle the horizontal and vertical 8er alveoles on the middle row.
    if (IsIncludedIn(t_xAlveoleIndex, t_yAlveoleIndex, f0DegRot) || IsIncludedIn(t_xAlveoleIndex, t_yAlveoleIndex, f180DegRot)) {
      if (crystalRow == 4) {
        return 1 + rowOffset;
      } else if (crystalRow == 3) {
        return -1 + rowOffset;
      } else if (crystalRow == 2) {
        return 1 + rowOffset;
      } else if (crystalRow == 1) {
        return -1 + rowOffset;
      } else {
        LOG(error) << "BSEmcFwEndcap_AgThomaBonnVersion_SensorNameIdMap::CalculateRow() - Unexpected row number " << crystalRow << "! Aborting!";
        exit(-1);
      }
    } else {
      if (crystalRow == 4) {
        return 2 + rowOffset;
      } else if (crystalRow == 3) {
        return 1 + rowOffset;
      } else if (crystalRow == 2) {
        return -1 + rowOffset;
      } else if (crystalRow == 1) {
        return -2 + rowOffset;
      } else {
        LOG(error) << "BSEmcFwEndcap_AgThomaBonnVersion_SensorNameIdMap::CalculateRow() - Unexpected row number " << crystalRow << "! Aborting!";
        exit(-1);
      }
    }
  }

  Int_t row = 0;

  if (sign < 0) {
    row = rowOfLastAlveole + (crystalRow - 5); // 5-crystalRow remaps 1,2,3,4 to 4,3,2,1
  } else {
    row = rowOfLastAlveole + crystalRow;
  }
  if (IsIncludedIn(t_xAlveoleIndex, t_yAlveoleIndex, fRequireOffset)) {
    row += sign;
  }
  row += rowOffset;
  return row;
}

Int_t BSEmcFwEndcap_AgThomaBonnVersion_SensorNameIdMap::CalculateColumn(Int_t t_crystal, Int_t t_xAlveoleIndex, Int_t /*t_yAlveoleIndex*/) const
{
  Int_t sign = (t_xAlveoleIndex > 0) ? 1 : -1; // X0 has sign -1. That is ok.
  Int_t column = 0;
  const Int_t columnOffset = 36;
  Int_t crystalColumn = t_crystal % 4 + 1;

  Int_t columnOfLastAlveole = 0;
  if (t_xAlveoleIndex != 0) {
    columnOfLastAlveole = 4 * (std::abs(t_xAlveoleIndex) - 1) + 2; // due to X0 alveoles
  } else {
    if (crystalColumn == 1) {
      return -2 + columnOffset;
    } // --2 = +2
    else if (crystalColumn == 2) {
      return -1 + columnOffset;
    } else if (crystalColumn == 3) {
      return 1 + columnOffset;
    } else if (crystalColumn == 4) {
      return 2 + columnOffset;
    }
  }

  if (sign < 0) {
    column = columnOfLastAlveole + crystalColumn; // In case of X0 -crystalColumn is column
  } else {
    columnOfLastAlveole *= -1;
    column = columnOfLastAlveole - (5 - crystalColumn);
  }

  column += columnOffset;
  return column;
}
