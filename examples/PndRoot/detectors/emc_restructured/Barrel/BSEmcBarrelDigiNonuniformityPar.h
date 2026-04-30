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

#ifndef BSEMCBARRELDIGINONUNIFORMITYPAR_HH
#define BSEMCBARRELDIGINONUNIFORMITYPAR_HH
#include <map>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TObject.h"

#include "FairParGenericSet.h"
#include "FairParamList.h"

class FairParamList;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcBarrelDigiNonuniParObject
 * @brief Parameter object containing Values relevant for the non-uniform lightyield of Barrel crystals.
 * @details Based on PndEmcDigiNonuniformityPar.cxx
 * @author Christian Hammann <chammann@hiskp.uni-bonn.de>, HISKP Bonn
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-01
 * @ingroup Barrel
 */
class BSEmcBarrelDigiNonuniParObject : public TObject {
 public:
  void SetNonuniformity(Int_t t_crystaltype, Double_t *t_pars);
  void GetNonuniformity(Int_t t_crystaltype, Double_t *t_pars);
  const std::map<Int_t, std::vector<Double_t>> &GetMap() const { return fNonUniPars; }
  BSEmcBarrelDigiNonuniParObject() : fNonUniPars() { fNonUniPars.clear(); };
  ~BSEmcBarrelDigiNonuniParObject(){};

 private:
  std::map<Int_t, std::vector<Double_t>> fNonUniPars{};
  ClassDef(BSEmcBarrelDigiNonuniParObject, 1);
};

/**
 * @class BSEmcBarrelDigiNonuniformityPar
 * @brief Parameter to load Values relevant for the non-uniform lightyield of Barrel crystals.
 * @details Based on PndEmcDigiNonuniformityPar.cxx
 * @author Christian Hammann <chammann@hiskp.uni-bonn.de>, HISKP Bonn
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-01
 * @ingroup Barrel
 */
class BSEmcBarrelDigiNonuniformityPar : public FairParGenericSet {
 public:
  void GetNonuniformityParameters(Int_t t_detId, Double_t *t_pars);
  void SetNonuniParObject(BSEmcBarrelDigiNonuniParObject *t_parObject) { fParObject = t_parObject; };
  BSEmcBarrelDigiNonuniformityPar(const char *t_name = "PndEmcDigiPar", const char *t_title = "Emc digi parameter", const char *t_context = "TestDefaultContext");
  ~BSEmcBarrelDigiNonuniformityPar(void){};

  virtual void clear(void) /*override*/ {};

  virtual void putParams(FairParamList *t_list) /*override*/;
  virtual Bool_t getParams(FairParamList *t_list) /*override*/;

 private:
  BSEmcBarrelDigiNonuniParObject *fParObject{nullptr};

  BSEmcBarrelDigiNonuniformityPar(const BSEmcBarrelDigiNonuniformityPar &t_l);
  BSEmcBarrelDigiNonuniformityPar &operator=(const BSEmcBarrelDigiNonuniformityPar & /*unused*/) { return *this; };

  ClassDef(BSEmcBarrelDigiNonuniformityPar, 1);
};

#endif /*BSEMCBARRELDIGINONUNIFORMITYPAR_HH*/
