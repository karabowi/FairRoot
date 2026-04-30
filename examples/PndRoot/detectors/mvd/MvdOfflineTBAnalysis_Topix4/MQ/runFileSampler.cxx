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


/// std
#include <PndMvdMQFileSampler.h>
#include <csignal>

/// FairRoot - FairMQ - base/MQ
#include "FairMQLogger.h"
#include "runSimpleMQStateMachine.h"

// 9-PixelDetector example

// ////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
  try {
    std::vector<std::string> filename;
    std::string branchname;
    int64_t maxindex;

    namespace po = boost::program_options;
    po::options_description sampler_options("Sampler options");
    sampler_options.add_options()("file-name", po::value<std::vector<std::string>>(&filename), "Path to the input file")(
      "max-index", po::value<int64_t>(&maxindex)->default_value(-1), "number of events to read")("branch-name", po::value<std::string>(&branchname), "branch name");

    FairMQProgOptions config;
    config.AddToCmdLineOptions(sampler_options);

    config.ParseAll(argc, argv);

    PndMvdMQFileSampler sampler;

    for (unsigned int ielem = 0; ielem < filename.size(); ielem++) {
      sampler.AddInputFileName(filename.at(ielem));
    }

    sampler.SetMaxIndex(maxindex);

    sampler.AddInputBranchName(branchname);

    sampler.AddInputBranchName("EventHeader.");

    runStateMachine(sampler, config);

  } catch (std::exception &e) {
    LOG(error) << "Unhandled Exception reached the top of main: " << e.what() << ", application will now exit";
    return 1;
  }

  return 0;
}
