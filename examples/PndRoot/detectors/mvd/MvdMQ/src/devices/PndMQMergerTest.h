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
 * PndMQMergerTest.h
 *
 * @since 2012-12-06
 * @author D. Klein, A. Rybalchenko
 */

#ifndef PndMQMergerTest_H_
#define PndMQMergerTest_H_

#include "PndMQMerger.h"

#include <PndMvdMQFileSamplerBursts.h>

#include <boost/serialization/access.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>

class PndMQMergerTest : public PndMQMerger {
 public:
  PndMQMergerTest();

 protected:
  virtual void ProcessData(std::map<std::string, BurstData> &dataToProcess);
};

#endif /* PndMQMergerTest_H_ */
