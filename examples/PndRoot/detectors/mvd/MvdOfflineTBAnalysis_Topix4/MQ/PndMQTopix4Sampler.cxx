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
 * PndMQTopix4Sampler.cpp
 *
 * @since 2014-10-10
 * @author A. Rybalchenko
 */

#include <memory> // unique_ptr

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <PndMQTopix4Sampler.h>

#include "FairMQLogger.h"
#include "mrfdata_8b.h"
#include "PndMQStatus.h"

using namespace std;

PndMQTopix4Sampler::PndMQTopix4Sampler() : fFileName() {}

void PndMQTopix4Sampler::CustomCleanup(void *data, void *object)
{
  delete (TMrfData_8b *)object;
}

void PndMQTopix4Sampler::Init()
{
  fTopixDataReader.Init();
}

void PndMQTopix4Sampler::Run()
{
  bool stop = false;

  while (CheckCurrentState(RUNNING) && stop == false) {
    // boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

    TMrfData_8b *data = 0;
    stop = fTopixDataReader.ReadInDataFromFile(data);

    unique_ptr<FairMQMessage> header(fTransportFactory->CreateMessage(sizeof(int)));

    int flag = -1;
    if (stop == false) {
      flag = PndMQStatus::RUNNING;
      memcpy(header->GetData(), &flag, sizeof(int));
      unique_ptr<FairMQMessage> msg(fTransportFactory->CreateMessage(reinterpret_cast<u_int8_t *>(&data->regdata[0]), data->getNumWords(), CustomCleanup, data));
      //       LOG(info) << "Sending Words\"" << data->getNumWords() << "\"" << " Bits: " << data->getNumBits();
      fChannels.at("data-out").at(0).SendPart(header);
      fChannels.at("data-out").at(0).Send(msg);
    } else {
      flag = PndMQStatus::STOP;
      memcpy(header->GetData(), &flag, sizeof(int));
      fChannels.at("data-out").at(0).Send(header);
    }
  }
}

PndMQTopix4Sampler::~PndMQTopix4Sampler() {}

void PndMQTopix4Sampler::SetProperty(const int key, const string &value)
{
  switch (key) {
  case FileName:
    fFileName = value;
    fTopixDataReader.SetFileName(fFileName);
    break;
  default: FairMQDevice::SetProperty(key, value); break;
  }
}

string PndMQTopix4Sampler::GetProperty(const int key, const string &default_ /*= ""*/)
{
  switch (key) {
  case FileName: return fFileName; break;
  default: return FairMQDevice::GetProperty(key, default_);
  }
}

void PndMQTopix4Sampler::SetProperty(const int key, const int value)
{
  switch (key) {
  default: FairMQDevice::SetProperty(key, value); break;
  }
}

int PndMQTopix4Sampler::GetProperty(const int key, const int default_ /*= 0*/)
{
  switch (key) {
  default: return FairMQDevice::GetProperty(key, default_);
  }
}
