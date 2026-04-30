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

/*============================================================*/
/* mrf_confitem.cpp                                           */
/* MVD Readout Framework Configuration Item                   */
/*                                               M.C. Mertens */
/*============================================================*/

#include "mrf_confitem.h"

TConfItem::TConfItem() : value(0), position(0), length(0), min(0), max(0), flags(0) {}

/*
TConfItem::TConfItem(const mrf::registertype value, const UInt_t position, const UInt_t length)
: value(value), position(position), length(length), user1(0), user2(0)
{
}
*/

TConfItem::TConfItem(const mrf::registertype value, const UInt_t position, const UInt_t length)
  : value(value), position(position), length(length), min(0), max((1 << length) - 1), flags(0)
{
}

TConfItem::TConfItem(const mrf::registertype value, const UInt_t position, const UInt_t length, const UInt_t min, const UInt_t max, const UInt_t flags)
  : value(value), position(position), length(length), min(min), max(max), flags(flags)
{
}
