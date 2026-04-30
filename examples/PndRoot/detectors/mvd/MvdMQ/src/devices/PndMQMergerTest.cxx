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
 * PndMQMergerTest.cxx
 *
 * @since 2012-12-06
 * @author D. Klein, A. Rybalchenko
 */

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "baseMQtools.h"

#include "PndMQMerger.h"
#include "PndMQMergerTest.h"

using namespace std;

PndMQMergerTest::PndMQMergerTest() : PndMQMerger() {}

void PndMQMergerTest::ProcessData(std::map<std::string, BurstData> &dataToProcess)
{
  for (auto itr : dataToProcess) {
    LOG(info) << "Data in Burst: " << itr.first;
    //		for (auto eventItr : itr.second.fData)
    //			for (auto dataItr : eventItr)
    //				LOG(info) << dataItr->GetTimeStamp();
  }
  LOG(info) << "Finished!";
}
