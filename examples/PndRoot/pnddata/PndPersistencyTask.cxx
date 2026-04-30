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

// -------------------------------------------------------------------------
// -----                  PndPersistencyTask source file                    -----
// -----             Created 28/02/18  by T.Stockmanns                 -----
// -------------------------------------------------------------------------
#include "PndPersistencyTask.h"

// -----   Default constructor   -------------------------------------------
PndPersistencyTask::PndPersistencyTask() : fPersistency(kFALSE) {}

// -------------------------------------------------------------------------

PndPersistencyTask::PndPersistencyTask(const char *name, Int_t iVerbose) : FairTask(name, iVerbose), fPersistency(kFALSE) {}

// -----   Destructor   ----------------------------------------------------
PndPersistencyTask::~PndPersistencyTask() {}
// -------------------------------------------------------------------------

ClassImp(PndPersistencyTask)
