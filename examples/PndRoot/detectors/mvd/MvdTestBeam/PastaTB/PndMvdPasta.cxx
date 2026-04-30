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
 * PndMvdPasta.cxx
 *
 *  Created on: 23.09.2017
 *      Author: Stockmanns
 */

#include <PndMvdPasta.h>
#include "mrftools.h"

ClassImp(PndMvdPasta);

PndMvdPasta::PndMvdPasta()
{
  // TODO Auto-generated constructor stub
}

PndMvdPasta::~PndMvdPasta()
{
  // TODO Auto-generated destructor stub
}

ThresholdDataFullMode PndMvdPasta::AnalyzeThresholdWordFull(ULong64_t word)
{
  ULong_t tempTime = word;
  ThresholdDataFullMode result;
  result.tacId = tempTime & 0x3;
  tempTime = tempTime >> 2;
  result.channelId = tempTime & 0x3f;
  tempTime = tempTime >> 8; // two bits are empty
  result.t_eoc = mrftools::grayToBin(tempTime & 0x3ff);
  tempTime = tempTime >> 10;
  result.t_soc = mrftools::grayToBin(tempTime & 0x3ff);
  tempTime = tempTime >> 10;
  result.t_coarse = mrftools::grayToBin(tempTime & 0x3ff);

  return result;
}

FrameHeader PndMvdPasta::AnalyzeHeader(ULong64_t word)
{
  FrameHeader tempHeader;
  ULong_t temp = word;
  tempHeader.frameId = temp & 0xffffffff;
  temp = temp >> 32;
  tempHeader.nEvents = temp & 0xff;

  return tempHeader;
}

std::vector<char> PndMvdPasta::ConvertData(std::vector<ULong64_t> frame)
{
  std::vector<char> result;
  for (int wordInFrame = 0; wordInFrame < frame.size() - 1; wordInFrame++) {
    //		std::cout << "WordInFrame: " << std::hex << frame[wordInFrame] << std::endl;
    for (int charInWord = 0; charInWord != 5; charInWord++) {
      char value = (frame[wordInFrame] >> ((4 - charInWord) * 8)) & 0xff;
      // std::cout << charInWord << " charInWord: " << std::hex << (int)value << std::endl;
      result.push_back(value);
    }
  }
  if (frame.size() % 2 == 0)
    result.push_back(0x00);
  //	for (int charInWord = 0; charInWord != 2; charInWord++){
  //		char value = ((frame[frame.size()-1]) >> (1 - charInWord)*8);
  //		std::cout << charInWord << " charInWord: " << std::hex << (int)value << std::endl;
  //		result.push_back(value);
  //	}

  return result;
}
