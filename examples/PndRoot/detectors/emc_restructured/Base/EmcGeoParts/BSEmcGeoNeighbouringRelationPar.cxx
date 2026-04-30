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

#include "BSEmcGeoNeighbouringRelationPar.h"

#include <fstream>
#include <memory>
#include <stdlib.h>
#include <string>
#include <utility>

#include "TObjArray.h"
#include "TObjString.h"
#include "TObject.h"
#include "TString.h"

#include "FairParamList.h"
#include "fairlogger/Logger.h"

std::string BSEmcGeoNeighbouringRelationPar::fgParameterName = "EmcGeoNeighbouringRelationPar";
BSEmcGeoNeighbouringRelationPar::BSEmcGeoNeighbouringRelationPar(const char *t_name, const char *t_title, const char *t_context)
  : FairParGenericSet(t_name, t_title, t_context), fRelations()
{
  clear();
  fRelations.reset(new TObjArray());
  fRelations->SetOwner(kTRUE);
}

BSEmcGeoNeighbouringRelationPar::BSEmcGeoNeighbouringRelationPar(const BSEmcGeoNeighbouringRelationPar &t_l) : FairParGenericSet(t_l)
{
  fRelations.reset(new TObjArray(*(t_l.GetRelations())));
}

BSEmcGeoNeighbouringRelationPar::~BSEmcGeoNeighbouringRelationPar(void) {}

BSEmcGeoNeighbouringRelationPar &BSEmcGeoNeighbouringRelationPar::operator=(const BSEmcGeoNeighbouringRelationPar &t_l)
{
  fRelations.reset(new TObjArray(*(t_l.GetRelations())));

  return *this;
}

void BSEmcGeoNeighbouringRelationPar::putParams(FairParamList *t_list)
{
  LOG(debug) << "BSEmcGeoNeighbouringRelationPar::putParams(FairParamList* list)";
  if (t_list == nullptr) {
    LOG(warn) << "BSEmcGeoNeighbouringRelationPar::putParams(FairParamList* list) returning early as list is null";
    return;
  }
  LOG(debug) << "BSEmcGeoNeighbouringRelationPar::putParams(FairParamList* list) adding fRlations";
  UpdateRelationArray();

  t_list->addObject("Relations", fRelations.get());
}

Bool_t BSEmcGeoNeighbouringRelationPar::getParams(FairParamList *t_list)
{
  LOG(debug) << "BSEmcGeoNeighbouringRelationPar::getParams(FairParamList* list)";

  if (t_list == nullptr) {
    return kFALSE;
  }
  if (!t_list->fillObject("Relations", fRelations.get())) {
    return kFALSE;
  }

  FillMap();
  return kTRUE;
}

void BSEmcGeoNeighbouringRelationPar::FillMap()
{
  LOG(debug) << "BSEmcGeoNeighbouringRelationPar::FillMap()";
  fRelationMap.clear();
  if (fRelations == nullptr) {
    LOG(error) << "BSEmcGeoNeighbouringRelationPar::FillMap() - fRelations is nullptr! Exiting!";
    exit(-1);
  }
  if (fRelations->GetEntriesFast() == 0) {
    LOG(error) << "BSEmcGeoNeighbouringRelationPar::FillMap() - Failed, no relations list found.";
    return;
  }
  LOG(debug) << "BSEmcGeoNeighbouringRelationPar::FillMap() - There are " << fRelations->GetEntriesFast() << " Relation entries";
  TString tmpstr;
  for (Int_t i = 0; i < fRelations->GetEntriesFast(); i++) {
    TObjString *relation = dynamic_cast<TObjString *>(fRelations->At(i));
    TString relationStr = relation->GetString();
    std::unique_ptr<TObjArray> tokens{relationStr.Tokenize(":")};
    Int_t crystalId = 0;
    std::vector<Int_t> neighbourIds;
    if (tokens->GetEntriesFast() > 1) {
      tmpstr = (dynamic_cast<TObjString *>(tokens->At(0)))->GetString();
      crystalId = tmpstr.Atoi();
      for (Int_t tokenId = 1; tokenId < tokens->GetEntriesFast(); ++tokenId) {
        tmpstr = (dynamic_cast<TObjString *>(tokens->At(tokenId)))->GetString();
        neighbourIds.push_back(tmpstr.Atoi());
      }
      fRelationMap[crystalId] = neighbourIds;
    }
    tokens->Delete();
  }
  fFilledMap = kTRUE;
}

void BSEmcGeoNeighbouringRelationPar::UpdateRelationArray()
{
  LOG(trace) << "BSEmcGeoNeighbouringRelationPar::UpdateRelationArray()";
  UInt_t counter = 0;
  fRelations->Delete();
  fRelations->Clear();
  fRelations->Compress();
  for (auto &relation : fRelationMap) {
    Int_t main = relation.first;
    TString entry = std::to_string(main);
    for (Int_t neighbourId : relation.second) {
      entry += ":" + std::to_string(neighbourId);
    }
    fRelations->AddLast(new TObjString(entry));
    ++counter;
  }
  LOG(trace) << "BSEmcGeoNeighbouringRelationPar::UpdateRelationArray() - " << counter << " Crystals found.";
}

void BSEmcGeoNeighbouringRelationPar::RegisterIdAndNeighbours(Int_t id, std::vector<Int_t> &t_neighbours)
{
  setChanged(kTRUE);
  fRelationMap[id] = t_neighbours;
}

void BSEmcGeoNeighbouringRelationPar::AddNeighbourToId(Int_t t_neighbourid, Int_t t_id)
{
  LOG(trace) << "void BSEmcGeoNeighbouringRelationPar::AddNeighbourToId(Int_t neighbourid= " << t_neighbourid << ", Int_t id = " << t_id << ")";

  setChanged(kTRUE);
  auto pos = fRelationMap.find(t_id);
  if (pos == fRelationMap.end()) {
    LOG(trace) << "Created new empty vector for " << t_id;
    fRelationMap[t_id] = std::vector<Int_t>{};
  }
  LOG(trace) << "Added " << t_neighbourid << " to " << t_id;
  fRelationMap[t_id].push_back(t_neighbourid);
}

const std::vector<Int_t> &BSEmcGeoNeighbouringRelationPar::GetNeighbourIds(Int_t t_detectorid) const
{
  auto pos = fRelationMap.find(t_detectorid);
  if (pos == fRelationMap.end()) {
    LOG(error) << "BSEmcGeoNeighbouringRelationPar::GetNeighbourIds(detectorid)"
               << " - no neighbour info found for detector: " << t_detectorid << "! Exiting!";
    exit(-1);
  }
  return (*pos).second;
}

void BSEmcGeoNeighbouringRelationPar::InitFromTextFile(const TString &t_filename)
{
  LOG(debug) << "BSEmcGeoNeighbouringRelationPar::InitFromTextFile(const TString& filename: " << t_filename << ").";

  fRelations->Delete();
  std::string line;
  std::ifstream txtfile(t_filename.Data());
  if (txtfile.is_open()) {
    while (std::getline(txtfile, line)) {
      TString entry{line};
      fRelations->AddLast(new TObjString(entry));
    }
    txtfile.close();
  } else {
    LOG(error) << "BSEmcGeoNeighbouringRelationPar::InitFromTextFile(const TString& filename: " << t_filename << ") - Unable to open file!";
  }
  FillMap();
  setChanged(kTRUE);
}

ClassImp(BSEmcGeoNeighbouringRelationPar);
