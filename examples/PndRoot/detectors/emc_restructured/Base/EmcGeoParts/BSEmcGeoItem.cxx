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

#include "BSEmcGeoItem.h"

#include "fairlogger/Logger.h"

ClassImp(BSEmcGeoItem);

BSEmcGeoItem::BSEmcGeoItem() : fUniqueID(0), fNeighbours({}), fNeighbourCount(0), fDigis(), fDigiIndices(), fIsAdded(kFALSE)
{
  fNeighbours.clear();
}

BSEmcGeoItem::~BSEmcGeoItem() {}

Bool_t BSEmcGeoItem::IsNeighbour(const BSEmcGeoItem *t_item) const
{
  for (BSEmcGeoItem *neighbour : fNeighbours) {
    if (neighbour->IsSameItem(t_item)) {
      return kTRUE;
    }
  }
  return kFALSE;
}

void BSEmcGeoItem::AddNeighbour(BSEmcGeoItem *t_item)
{
  if (!IsNeighbour(t_item)) {
    fNeighbours.push_back(t_item);
    ++fNeighbourCount;
  }
}

void BSEmcGeoItem::Reset()
{
  fIsAdded = kFALSE;
  fDigis.clear();
  fDigiIndices.clear();
}

void BSEmcGeoItem::FindClusterCrystals(std::vector<BSEmcGeoItem *> &t_clusterCrystals)
{
  LOG(trace) << "void BSEmcGeoItem::FindClusterCrystals() : Looking at crystal " << GetID();

  if (IsDigiPresent() && !IsAdded()) {

    t_clusterCrystals.push_back(this);
    fIsAdded = kTRUE;
    LOG(trace) << "Crystal " << GetID() << " is being added to cluster!";

    for (BSEmcGeoItem *neighbour : fNeighbours) {
      neighbour->FindClusterCrystals(t_clusterCrystals);
    }
  }
}
