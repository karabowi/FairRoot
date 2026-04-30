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

#ifndef RootTypesDef_h
#define RootTypesDef_h 1

#ifndef HLTCA_STANDALONE

#include "TROOT.h"

#else
typedef int Int_t;
typedef float Float_t;
typedef double Double_t;
typedef bool Bool_t;

class TObject {
};
#endif

#endif // RootTypesDef_h
