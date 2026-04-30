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

/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * PndMQTopix4Sampler.h
 *
 * @since 2014-10-10
 * @author A. Rybalchenko
 */

#ifndef PNDMQTOPIX4SAMPLER_H_
#define PNDMQTOPIX4SAMPLER_H_

#include <string>

#include "FairMQDevice.h"
#include "PndMvdReadInToPix4TBData.h"

class PndMQTopix4Sampler : public FairMQDevice {
 public:
  enum { FileName = FairMQDevice::Last, Last };
  PndMQTopix4Sampler();
  virtual ~PndMQTopix4Sampler();

  static void CustomCleanup(void *data, void *hint);

  virtual void SetProperty(const int key, const std::string &value);
  virtual std::string GetProperty(const int key, const std::string &default_ = "");
  virtual void SetProperty(const int key, const int value);
  virtual int GetProperty(const int key, const int default_ = 0);

  virtual void Init();

 protected:
  std::string fFileName;
  int fFE;
  PndMvdReadInToPix4TBData fTopixDataReader;
  virtual void Run();
};

#endif /* FAIRMQEXAMPLE1SAMPLER_H_ */
