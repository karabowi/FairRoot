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

//======================================================================
// Description:
//       Class BSEmcStructure
//------------------------------------------------------------------------

#ifndef BSEMCSTRUCTURE_HH
#define BSEMCSTRUCTURE_HH

#include <map>
#include <memory>
#include <string>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TObject.h"
#include "TString.h"
#include "TVector3.h"

#include "BSEmcXtal.h"

class TClonesArray;
class TGeoManager;
class TGeoTrap;
class TGeoNode;
class BSEmcCrystalPositionPar;
class BSEmcXtal;
class TBuffer;
class TClass;
class TGeoShape;
class TMemberInspector;

using DetIdToXtalMap = std::map<Int_t, std::unique_ptr<BSEmcXtal>>;

/**
 * @class BSEmcStructure
 * @brief geometry helper class to parse ROOT-Geometry Tree and create BSEmcXtal for each Emc-crystal
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcStructure : public TObject {
 public:
  static BSEmcStructure *Instance();
  static BSEmcStructure *Instance(TGeoManager *);

  virtual ~BSEmcStructure();

  const DetIdToXtalMap &GetDetIdXtalMap() const { return fDetectorIdXtalMap; };

  Bool_t IsDetIdXtalMapSet() const { return fDetectorIdXtalMap.size() != 0; }
  void PrintCrystals(std::string, Int_t t_option = 1) const;
  void CreateDetIdXtalMap()
  {
    if (fDetectorIdXtalMap.size() == 0) {
      ParseVolumeTree();
    }
  }

 protected:
  BSEmcStructure(TGeoManager *);
  TGeoTrap *CreateFromTGeoBBox(const TGeoShape *t_shape) const;
  TGeoTrap *CreateFromArb8(TGeoShape *t_shape) const;

  void ParseVolumeTree();

 private:
  static BSEmcStructure *fInstance;

  Double_t fRescaleFactor{};
  DetIdToXtalMap fDetectorIdXtalMap;
  ClassDef(BSEmcStructure, 1)
};

#endif /*BSEMCSTRUCTURE_HH*/
