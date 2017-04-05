/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

#include "runFairMQDevice.h"

// 9-PixelDetector example
#include "FairMQEx9aFindHitsProcessor.h"

namespace bpo = boost::program_options;

void addCustomOptions(bpo::options_description& options)
{
  options.add_options()
    ("task-name",   bpo::value<std::string>()->required()                  ,  "Name of task to run")
    ("keep-data",   bpo::value<std::string>()                              ,  "Name of data to keep in stream")
    ("in-channel",  bpo::value<std::string>()->default_value("data-in")   , "input channel name")
    ("out-channel", bpo::value<std::string>()->default_value("data-out") , "output channel name")
    ("par-channel", bpo::value<std::string>()->default_value("param")    , "param channel name");
}

FairMQDevicePtr getDevice(const FairMQProgOptions& config)
{
  return new FairMQEx9aFindHitsProcessor();
}
