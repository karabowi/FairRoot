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
 * @class PndMasterRunSimJSONReader
 * @brief Class to generate a PndMasterRunSim object from a configuration JSON file
 * @date 20.09.2022
 * @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 **/

#pragma once

#include <boost/property_tree/ptree.hpp>
namespace pt = boost::property_tree;

class PndMasterRunSim;

class PndMasterRunSimJSONReader : public TObject {
 public:
  PndMasterRunSim *createRunSim(pt::ptree &tree);
  int getNEvents() const { return fNEvents; }

 private:
  int fNEvents = 0;

  ClassDef(PndMasterRunSimJSONReader, 1);
};
