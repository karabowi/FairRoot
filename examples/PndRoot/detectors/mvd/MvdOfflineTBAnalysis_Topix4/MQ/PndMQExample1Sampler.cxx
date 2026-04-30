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
 * PndMQExample1Sampler.cpp
 *
 * @since 2014-10-10
 * @author A. Rybalchenko
 */

#include <memory> // unique_ptr

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <PndMQExample1Sampler.h>

#include "FairMQLogger.h"

using namespace std;

PndMQExample1Sampler::PndMQExample1Sampler() : fText() {}

void PndMQExample1Sampler::CustomCleanup(void *data, void *object)
{
  delete (string *)object;
}

void PndMQExample1Sampler::Run()
{
  while (CheckCurrentState(RUNNING)) {
    boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

    string *text = new string(fText);

    unique_ptr<FairMQMessage> msg(fTransportFactory->CreateMessage(const_cast<char *>(text->c_str()), text->length(), CustomCleanup, text));

    LOG(info) << "Sending \"" << fText << "\"";

    fChannels.at("data-out").at(0).Send(msg);
  }
}

PndMQExample1Sampler::~PndMQExample1Sampler() {}

void PndMQExample1Sampler::SetProperty(const int key, const string &value)
{
  switch (key) {
  case Text: fText = value; break;
  default: FairMQDevice::SetProperty(key, value); break;
  }
}

string PndMQExample1Sampler::GetProperty(const int key, const string &default_ /*= ""*/)
{
  switch (key) {
  case Text: return fText; break;
  default: return FairMQDevice::GetProperty(key, default_);
  }
}

void PndMQExample1Sampler::SetProperty(const int key, const int value)
{
  switch (key) {
  default: FairMQDevice::SetProperty(key, value); break;
  }
}

int PndMQExample1Sampler::GetProperty(const int key, const int default_ /*= 0*/)
{
  switch (key) {
  default: return FairMQDevice::GetProperty(key, default_);
  }
}
