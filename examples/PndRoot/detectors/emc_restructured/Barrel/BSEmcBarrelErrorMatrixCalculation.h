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
// Description:
//	Class PndEmcErrorMatrix
//      Calculate Error Matrix for the given EmcCluster
//      with parametrization defined by the given parameter PndEmcErrorMatrixPar
//
//------------------------------------------------------------------------
#ifndef BSEMCBARRELERRORMATRIXCALCULATION_HH
#define BSEMCBARRELERRORMATRIXCALCULATION_HH

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TMatrix.h"
#include "TMatrixDfwd.h"

#include "BSEmcErrorMatrixCalculation.h"
#include "BSEmcRecoHit.h"

class BSEmcRecoHit;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcBarrelErrorMatrixCalculation
 * @brief Barrel specific error matrix calculation
 * @details due to zylindrical form of Barrel the Barrel errors are parameterized using zylinder-coordinates instead of spherical
 * @author Dima Melnichuk
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-01
 * @ingroup Barrel
 */
class BSEmcBarrelErrorMatrixCalculation : public BSEmcErrorMatrixCalculation {
 public:
  BSEmcBarrelErrorMatrixCalculation();
  virtual ~BSEmcBarrelErrorMatrixCalculation();

 protected:
  virtual Double_t GetPosition2Error(Double_t t_energy) const /*override*/;
  virtual TMatrixD GetTransformationMatrix(const BSEmcRecoHit *t_recoHit) const /*override*/;

 private:
  ClassDef(BSEmcBarrelErrorMatrixCalculation, 1);
};

#endif /*BSEMCBARRELERRORMATRIXCALCULATION_HH*/
