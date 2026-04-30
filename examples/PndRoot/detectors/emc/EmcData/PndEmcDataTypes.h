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

#ifndef PNDEMCDATATYPES_HH
#define PNDEMCDATATYPES_HH

#include <map>
#include <set>

class PndEmcTwoCoordIndex;
class PndEmcDigi;
class PndEmcXtal;
typedef std::map<PndEmcTwoCoordIndex *, PndEmcDigi *> PndEmcDigiPtrDict;
typedef std::map<PndEmcTwoCoordIndex *, PndEmcXtal *> PndEmcTciXtalMap;
typedef std::set<PndEmcTwoCoordIndex *> PndEmcCoordIndexSet;
#endif
