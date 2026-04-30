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
/* mrfbase.h                                                  */
/* MVD Readout Framework Base Functions                       */
/*                                               M.C. Mertens */
/*============================================================*/

#ifndef __MRFBASE_H__
#define __MRFBASE_H__

//#include <machine/types.h>
#include "Rtypes.h"

//#include <stdint.h>

//! Basic register address and content types used within MRF Suite.
namespace mrf {
//! Register content data type.
typedef UInt_t registertype;
//! Register address data type.
typedef UInt_t addresstype;
//	typedef UInt_t devicetype;
} // namespace mrf

#endif // __MRFBASE_H__
