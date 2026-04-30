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

#ifndef BSEMCERRORCALCULATIONPAR_HH
#define BSEMCERRORCALCULATIONPAR_HH
#include <string>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TObject.h"

#include "FairParGenericSet.h"

class FairParamList;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcErrorCalculationPar
 * @brief Container class for EMC error matrix parameter class is inherited from FairParGenericSet
 * @details Based on PndEmcErrorMatrixPar
 * Parametrization of EMC error matrix
 * functions used for parameterization
 * Energy: Delta(E)/E = (a^2/E^power) + const^2 + (quadr/E)^2
 * position: Delta(x)=(a*a/E^power) + const^2
 * There are 10 parameters (engParA, engPower, engConst, engQuadr, pos1ParA, pos1Power,
 * pos1Const, pos2ParA, pos2Power, pos2Const) for each component of EMC (barrel,
 * fwd endcap, bwd endcap, shashlyk)
 * @author Dima Melnychuk
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcErrorCalculationPar : public FairParGenericSet {
 public:
  static std::string fgParameterName;
  BSEmcErrorCalculationPar(const char *t_name = "EmcErrorCalculationPar", const char *t_title = "Emc Geometry Version Flag", const char *t_context = "TestDefaultContext");
  virtual ~BSEmcErrorCalculationPar();
  void clear() /*override*/;

  Double_t GetScaleFactor() const { return fScaleFactor; }
  Double_t GetMinEnergyCutOff() const { return fMinEnergyCutOff; }
  Double_t GetMaxEnergyCutOff() const { return fMaxEnergyCutOff; }
  Double_t GetDetectorPosition() const { return fDetectorPosition; }

  Double_t GetEnergyParA() const { return fEnergyParA; }
  Double_t GetEnergyPower() const { return fEnergyPower; }
  Double_t GetEnergyConst() const { return fEnergyConst; }
  Double_t GetEnergyQuadr() const { return fEnergyQuadr; }
  Double_t GetPosition1ParA() const { return fPosition1ParA; }
  Double_t GetPosition1Power() const { return fPosition1Power; }
  Double_t GetPosition1Const() const { return fPosition1Const; }
  Double_t GetPosition2ParA() const { return fPosition2ParA; }
  Double_t GetPosition2Power() const { return fPosition2Power; }
  Double_t GetPosition2Const() const { return fPosition2Const; }

  void putParams(FairParamList * /*unused*/) /*override*/;
  Bool_t getParams(FairParamList * /*unused*/) /*override*/;

 private:
  Double_t fScaleFactor{-1};
  Double_t fMinEnergyCutOff{-1};
  Double_t fMaxEnergyCutOff{-1};
  Double_t fDetectorPosition{-1};

  Double_t fEnergyParA{-1};
  Double_t fEnergyPower{-1};
  Double_t fEnergyConst{-1};
  Double_t fEnergyQuadr{-1};
  Double_t fPosition1ParA{-1};
  Double_t fPosition1Power{-1};
  Double_t fPosition1Const{-1};
  Double_t fPosition2ParA{-1};
  Double_t fPosition2Power{-1};
  Double_t fPosition2Const{-1};

  ClassDef(BSEmcErrorCalculationPar, 2)
};

#endif /*BSEMCERRORCALCULATIONPAR_HH*/
