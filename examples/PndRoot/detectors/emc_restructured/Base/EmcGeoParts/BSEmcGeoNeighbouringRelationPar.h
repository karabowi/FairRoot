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

#ifndef BSEMCNEIGHBOURINGRELATIONPAR_HH
#define BSEMCNEIGHBOURINGRELATIONPAR_HH
#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TObjArray.h"
#include "TString.h"

#include "FairParGenericSet.h"
#include "FairParamList.h"
#include "FairRun.h"

class FairParamList;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcGeoNeighbouringRelationPar
 * @brief Neigbouring Relations based on a list of detectorIds
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcGeoNeighbouringRelationPar : public FairParGenericSet {
 public:
  static std::string fgParameterName;
  BSEmcGeoNeighbouringRelationPar(const char *t_name = "EmcGeoNeighbouringRelationPar", const char *t_title = "Neighbouring relation of Emc crystals",
                                  const char *t_context = "TestDefaultContext");
  ~BSEmcGeoNeighbouringRelationPar(void);

  void clear(void) /*override*/ {}
  void putParams(FairParamList * /*unused*/) /*override*/;
  Bool_t getParams(FairParamList * /*unused*/) /*override*/;

  void RegisterIdAndNeighbours(Int_t t_id, std::vector<Int_t> &t_neighbours);
  void AddNeighbourToId(Int_t t_neighbourid, Int_t t_id);

  const std::vector<Int_t> &GetNeighbourIds(Int_t t_detectorid) const;
  Bool_t AreNeighbours(Int_t t_a, Int_t t_b) const
  {
    const std::vector<Int_t> &ids = GetNeighbourIds(t_a);
    return std::find(ids.begin(), ids.end(), t_b) != ids.end();
  }
  const TObjArray *GetRelations() const { return fRelations.get(); }

  BSEmcGeoNeighbouringRelationPar(const BSEmcGeoNeighbouringRelationPar &t_l);
  BSEmcGeoNeighbouringRelationPar &operator=(const BSEmcGeoNeighbouringRelationPar &t_l);

  void InitFromTextFile(const TString &t_filename);
  void FillMap();
  void UpdateRelationArray();

  Bool_t IsSet() { return fRelations->GetEntriesFast() != 0; }
  const std::map<Int_t, std::vector<Int_t>> &GetRelationMap() const { return fRelationMap; }

 private:
  std::unique_ptr<TObjArray> fRelations{nullptr};
  std::map<Int_t, std::vector<Int_t>> fRelationMap{}; //!
  Bool_t fFilledMap{kFALSE};
  ClassDef(BSEmcGeoNeighbouringRelationPar, 2)
};

#endif /*BSEMCNEIGHBOURINGRELATIONPAR_HH*/
