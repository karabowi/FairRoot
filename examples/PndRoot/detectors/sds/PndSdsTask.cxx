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
 * PndSdsTask.cxx
 *
 *  Created on: May 18, 2010
 *      Author: stockman
 */

#include "PndSdsTask.h"

PndSdsTask::PndSdsTask() : PndPersistencyTask("SdsTask", 0), fInBranchName(""), fOutBranchName(""), fFolderName(""), fInBranchId(-1), fOutBranchId(-1)
{
  // TODO Auto-generated constructor stub
}

PndSdsTask::~PndSdsTask()
{
  // TODO Auto-generated destructor stub
}

ClassImp(PndSdsTask);
