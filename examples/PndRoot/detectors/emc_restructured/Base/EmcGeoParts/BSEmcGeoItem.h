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

#ifndef BSEMCGEOITEM_HH
#define BSEMCGEOITEM_HH

#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TObject.h"

#include "BSEmcDigi.h"

class BSEmcDigi;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcGeoItem
 * @brief Convenience object for neighbouring relations used by BSEmcRecursiveClustering
 * @details Represents a crystal and contains ptr to neighbouring crystal representations to allow for simple recursive clustering of neighbours
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-01
 * @ingroup EmcBase
 */
class BSEmcGeoItem : public TObject {
 public:
  BSEmcGeoItem();
  virtual ~BSEmcGeoItem();

  Bool_t IsSameItem(const BSEmcGeoItem *t_item) const { return GetID() == t_item->GetID(); }

  Bool_t IsNeighbour(const BSEmcGeoItem *t_item) const;
  Bool_t HasNeighbours() const { return !fNeighbours.empty(); }
  Bool_t IsAdded() const { return fIsAdded; }
  Bool_t IsDigiPresent() const { return fDigis.size() > 0; }
  Int_t NumberOfNeighbours() const { return fNeighbourCount; }

  void AddNeighbour(BSEmcGeoItem *t_item);
  void SetID(Int_t t_uniqueId) { fUniqueID = t_uniqueId; }
  Int_t GetID() const { return fUniqueID; }
  const std::vector<BSEmcGeoItem *> &GetNeighbours() const { return fNeighbours; }
  const BSEmcGeoItem *GetNeighbour(Int_t t_index) const { return fNeighbours.at(t_index); }
  BSEmcGeoItem *GetNeighbour(Int_t t_index) { return fNeighbours.at(t_index); }
  void ClearNeighbourList() { fNeighbours.clear(); }
  void FindClusterCrystals(std::vector<BSEmcGeoItem *> &t_clusterCrystals);

  void Reset();
  void AddDigi(const BSEmcDigi *t_digi) { fDigis.push_back(t_digi); }
  const std::vector<const BSEmcDigi *> &GetDigis() const { return fDigis; }
  void AddDigiIndex(Int_t t_digiindex) { fDigiIndices.push_back(t_digiindex); }
  const std::vector<Int_t> &GetDigiIndices() const { return fDigiIndices; }

 private:
  Int_t fUniqueID{-1};                       //!
  std::vector<BSEmcGeoItem *> fNeighbours{}; //!
  Int_t fNeighbourCount{-1};                 //!
  std::vector<const BSEmcDigi *> fDigis{};   //!
  std::vector<Int_t> fDigiIndices{};         //!
  Bool_t fIsAdded{kFALSE};                   //!
  ClassDef(BSEmcGeoItem, 1);
};

#endif /*BSEMCGEOITEM_HH*/
