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

#ifndef CbmL1Def_h
#define CbmL1Def_h 1

#define HAVE_SSE

#ifdef HAVE_SSE
#include "vectors/P4_F32vec4.h"
#else
#include "vectors/PSEUDO_F32vec4.h"
#error NoSseFound
#endif // HAVE_SSE

#endif // CbmL1Def_h
