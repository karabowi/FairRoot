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
 * PndMvdPastaTorino.h
 *
 *  Created on: 23.09.2017
 *      Author: Stockmanns
 */

#ifndef MVD_MVDTESTBEAM_PASTATB_PNDMVDPASTATORINO_H_
#define MVD_MVDTESTBEAM_PASTATB_PNDMVDPASTATORINO_H_

#include "PndMvdPastaDigi.h"
#include "TObject.h"

class PndMvdPastaTorino : public TObject {
 public:
  PndMvdPastaTorino();
  virtual ~PndMvdPastaTorino();

  std::vector<PndMvdPastaDigi> ReadTorinoDaqFrame(std::ifstream &infile);

 protected:
  FrameHeader AnalyzeHeader(std::string &line);
  PndMvdPastaDigi AnalyzeData(std::string &line);

 private:
  FrameHeader fCurrentHeader;

  ClassDef(PndMvdPastaTorino, 1);
};

#endif /* MVD_MVDTESTBEAM_PASTATB_PNDMVDPASTATORINO_H_ */
