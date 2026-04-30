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
#include <csignal>

/// FairRoot - FairMQ - base/MQ
#include "FairMQLogger.h"
#include "runSimpleMQStateMachine.h"

// 9-PixelDetector example
#include "PndMvdMQFileSamplerBursts.h"

// ////////////////////////////////////////////////////////////////////////
namespace po = boost::program_options;

using channelBranches = std::vector<std::pair<std::string, std::string>>;

namespace std {
static inline std::istream &operator>>(std::istream &is, std::pair<std::string, std::string> &into)
{
  char ch;
  while (is >> ch && ch != '=')
    into.first += ch;
  return is >> into.second;
}
} // namespace std

int main(int argc, char **argv)
{
  try {
    std::vector<std::string> filename;
    channelBranches branchnames;
    int64_t maxindex;

    po::options_description sampler_options("Sampler options");
    sampler_options.add_options()("file-name", po::value<std::vector<std::string>>(&filename),
                                  "Path to the input file")("max-index", po::value<int64_t>(&maxindex)->default_value(-1),
                                                            "number of events to read")("branch-name", po::value<channelBranches>(&branchnames)->multitoken(), "branch name");

    FairMQProgOptions config;
    config.AddToCmdLineOptions(sampler_options);

    config.ParseAll(argc, argv);

    PndMvdMQFileSamplerBursts sampler;

    for (unsigned int ielem = 0; ielem < filename.size(); ielem++) {
      sampler.AddInputFileName(filename.at(ielem));
    }

    sampler.SetMaxIndex(maxindex);

    for (int i = 0; i < branchnames.size(); i++) {
      LOG(info) << "BranchNames: " << branchnames[i].first << "/" << branchnames[i].second;
      sampler.AddInputChannelBranchName(branchnames[i]);
    }

    //	sampler.AddInputBranchName("EventHeader.");

    runStateMachine(sampler, config);

  } catch (std::exception &e) {
    LOG(error) << "Unhandled Exception reached the top of main: " << e.what() << ", application will now exit";
    return 1;
  }

  return 0;
}
