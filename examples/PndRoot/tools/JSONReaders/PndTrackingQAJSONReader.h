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
 * @class PndMasterRunSimReaderJSON
 * @brief Class to generate a PndBoxGenerator object from a JSON file already loaded into a boost property tree
 * @date 19.09.2022
 * @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 **/

#pragma once

#include <boost/property_tree/ptree.hpp>
#include "PndTaskJSONReader.h"

class FairTask;

namespace pt = boost::property_tree;

class PndTrackingQAJSONReader : public PndTaskJSONReader {
 public:
  void AddTask(FairRun *run, pt::ptree &tree) override;

 protected:
  FairTask *createTask(pt::ptree &tree);

  ClassDef(PndTrackingQAJSONReader, 1);
};
