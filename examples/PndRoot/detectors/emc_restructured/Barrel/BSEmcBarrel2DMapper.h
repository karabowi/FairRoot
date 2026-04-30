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
#ifndef BSEMCBARREL2DMAPPER_HH
#define BSEMCBARREL2DMAPPER_HH

#include <utility>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "BSEmc2DMapper.h"

class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcBarrel2DMapper
 * @brief Decodes the Barrel Detector Ids into a X-Y-Index pair for Plotting purposes
 * @details Based on PndEmc.cxx
 * @author Xiaorong Shi            Lawrence Livermore National Lab
 * @author Steve Playfer           University of Edinburgh
 * @author Stephen Gowdy           University of Edinburgh
 * @author Dima Melnichuk - adaption for PANDA
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-01
 * @ingroup Barrel
 */
class BSEmcBarrel2DMapper : public BSEmc2DMapper {
 public:
  BSEmcBarrel2DMapper();
  virtual ~BSEmcBarrel2DMapper();

  virtual std::pair<Int_t, Int_t> GetXYIndices(Int_t t_detectorId) const /*override*/;

 private:
  ClassDef(BSEmcBarrel2DMapper, 1);
};

#endif /*BSEMCBARREL2DMAPPER_HH*/
