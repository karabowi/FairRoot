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
 * @class PndHistoCombinerJSONReader
 * @brief Class to generate a PndHistoCombiner object from a JSON file already loaded into a boost property tree
 * @date 08.09.2023
 * @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 **/

#pragma once

#include <boost/property_tree/ptree.hpp>
#include <TObject.h>
#include "PndHistoCombiner.h"

//class PndHistoCombiner;

namespace pt = boost::property_tree;

class PndHistoCombinerJSONReader : public TObject {
 public:
  static PndHistoCombiner *readJSON(pt::ptree &tree);

  ClassDef(PndHistoCombinerJSONReader, 1);
};
