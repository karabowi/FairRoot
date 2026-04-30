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
 * PndMvdPastaTorino.cxx
 *
 *  Created on: 23.09.2017
 *      Author: Stockmanns
 */

#include <PndMvdPastaTorino.h>

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

ClassImp(PndMvdPastaTorino);

PndMvdPastaTorino::PndMvdPastaTorino()
{
  // TODO Auto-generated constructor stub
}

PndMvdPastaTorino::~PndMvdPastaTorino()
{
  // TODO Auto-generated destructor stub
}

std::vector<PndMvdPastaDigi> PndMvdPastaTorino::ReadTorinoDaqFrame(std::ifstream &infile)
{
  std::vector<PndMvdPastaDigi> pastaDigis;

  std::string line;

  bool firstFrame = true;

  while (std::getline(infile, line)) {
    //		std::cout << "Line: " << line << std::endl;
    if (line.find(":") != std::string::npos) {
      fCurrentHeader = AnalyzeHeader(line);
      if (firstFrame == true)
        firstFrame = false;
      else
        return pastaDigis;
    } else {
      PndMvdPastaDigi digi = AnalyzeData(line);
      digi.SetHeader(fCurrentHeader);
      digi.CalcTot();
      digi.CalcTimeStamp();
      pastaDigis.push_back(digi);
    }
  }

  return pastaDigis;
}

FrameHeader PndMvdPastaTorino::AnalyzeHeader(std::string &line)
{
  FrameHeader header;
  std::string separator;
  std::stringstream ss(line);
  ss >> header.frameId >> separator >> header.nEvents;
  return header;
}

PndMvdPastaDigi PndMvdPastaTorino::AnalyzeData(std::string &line)
{
  PndMvdPastaDigi data;
  ThresholdDataFullMode time, energy;
  std::stringstream ss(line);
  ss >> time.t_coarse >> time.t_soc >> time.t_eoc >> time.channelId >> time.tacId >> energy.t_coarse >> energy.t_eoc;
  energy.t_soc = time.t_soc;
  energy.channelId = time.channelId;
  energy.tacId = time.tacId;

  data.SetTimeBranch(time);
  data.SetEnergyBranch(energy);

  return data;
}
