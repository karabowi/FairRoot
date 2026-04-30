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

#include "TMath.h"

#define SKEW_ANGLE 3. // CHECK

#define STTPARALDISTANCE 1.255
#define STTTOMVDSTRIPDISTANCE 15

// STT - geometry
#define TUBERADIUS 0.5
#define CTOUTRADIUS 41.
#define CTINRADIUS 15.
#define SKEWANGLE 3. * TMath::DegToRad()
#define SKEWLIMIT 30. // CHECK

// GEM - geometry
#define NOFSTATIONS 3
#define NOFSENSORS 2
#define NOFLAYERS NOFSTATIONS *NOFSENSORS

// PIPE
#define PIPEDIAMETER 4.08 // CHECK

// IREGIONS ....................................
// MVD
#define MVDPIXEL 0
#define MVDSTRIP 1
// STT
#define INNER_LEFT 3
#define SKEW_LEFT 5
#define OUTER_LEFT 7
#define INNER_RIGHT 2
#define SKEW_RIGHT 4
#define OUTER_RIGHT 6
#define INDIVISIBLE 10
// SCIT
#define SCIT 8
// GEM
#define GEM 9

// MVD + STT
#define MVD_INNER_LEFT 13  // mvd + stt inner lf
#define MVD_INNER_RIGHT 12 // mvd + stt inner rg

// COMBINATION
#define INNER_OUTER_LEFT 37
#define INNER_OUTER_RIGHT 26

#define INNER_LEFT_OUTER_RIGTH 36
#define INNER_RIGHT_OUTER_LEFT 27

#define INNER_LEFT_LEFT 33
#define INNER_RIGHT_RIGHT 22

#define OUTER_LEFT_LEFT 77
#define OUTER_RIGHT_RIGHT 66

#define INNER_LEFT_RIGHT 32
#define OUTER_LEFT_RIGHT 76
#define INNER_RIGHT_LEFT 23
#define OUTER_RIGHT_LEFT 67

// SECTORS
#define STT_0 2000
#define STT_1 2001
#define STT_2 2002
#define STT_3 2003
#define STT_4 2004
#define STT_5 2005
