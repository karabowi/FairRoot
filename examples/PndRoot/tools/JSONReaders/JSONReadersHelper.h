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

/**
 * JSONReadersHelper.h
 * @brief collection of helper methods to load in pairs, triplets or vectors of doubles from JSON files
 * @date 21.09.2022
 * @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 **/

#pragma once

namespace pt = boost::property_tree;

#include <FairLogger.h>
#include <stdexcept>
#include <string>

namespace JSONReaders {
inline std::pair<double, double> readPair(pt::ptree &tree)
{
  if (tree.size() != 2) {
    throw std::range_error("Expected two values e.g min/max not " + std::to_string(tree.size()));
    //    LOG(error) << "Expected two values eg. min/max not " << tree.size();
  }
  auto it = tree.begin();
  double min = it->second.get_value<double>();
  it++;
  double max = it->second.get_value<double>();
  return std::make_pair(min, max);
}

inline std::pair<std::string, std::string> readStringPair(pt::ptree &tree)
{
  if (tree.size() != 2) {
    throw std::range_error("Expected two values e.g min/max not " + std::to_string(tree.size()));
    //    LOG(error) << "Expected two values eg. min/max not " << tree.size();
  }
  auto it = tree.begin();
  std::string first = it->second.data();
  it++;
  std::string second = it->second.data();
  return std::make_pair(first, second);
}

inline std::array<double, 3> readTriplet(pt::ptree &tree)
{
  if (tree.size() != 3) {
    throw std::range_error("Expected three values e.g x/y/z not " + std::to_string(tree.size()));
    //    LOG(error) <<  << tree.size();
  }
  std::array<double, 3> xyz;
  int index = 0;
  for (auto val : tree) {
    xyz[index++] = val.second.get_value<double>();
  }
  return xyz;
}

inline std::vector<double> readVector(pt::ptree &tree)
{
  std::vector<double> result;
  for (auto val : tree) {
    result.push_back(val.second.get_value<double>());
  }
  return result;
}
} // namespace JSONReaders
