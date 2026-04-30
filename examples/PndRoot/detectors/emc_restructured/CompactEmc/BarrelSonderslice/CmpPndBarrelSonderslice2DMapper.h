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

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	EMC Digi.
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Author List:
//	Xiaorong Shi            Lawrence Livermore National Lab
//	Steve Playfer           University of Edinburgh
//	Stephen Gowdy           University of Edinburgh
// Dima Melnichuk - adaption for PANDA
//
// Copyright Information:
//	Copyright (C) 1994	Lawrence Livermore National Lab
//
///////////////////////////////////////////////////////////////
#ifndef CMPPNDBarrelSonderslice2DMAPPER_HH
#define CMPPNDBarrelSonderslice2DMAPPER_HH

#include <utility>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "BSEmcBarrel2DMapper.h"

class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class CmpPndBarrelSonderslice2DMapper
 * @brief Decodes the Barrel Detector Ids into a X-Y-Index pair for Plotting purposes
 * @details Based on PndEmc.cxx
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2024-01
 * @ingroup Barrel
 */
class CmpPndBarrelSonderslice2DMapper : public BSEmcBarrel2DMapper {
 public:
  CmpPndBarrelSonderslice2DMapper();
  virtual ~CmpPndBarrelSonderslice2DMapper();

  virtual std::pair<Int_t, Int_t> GetXYIndices(Int_t t_detectorId) const /*override*/;

 private:
  ClassDef(CmpPndBarrelSonderslice2DMapper, 1);
};

#endif /*CMPPNDBarrelSonderslice2DMAPPER_HH*/
