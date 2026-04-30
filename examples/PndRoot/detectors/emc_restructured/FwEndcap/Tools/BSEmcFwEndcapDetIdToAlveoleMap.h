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

#ifndef BSEMCFWENDCAPDETIDTOALVEOLEMAP_HH
#define BSEMCFWENDCAPDETIDTOALVEOLEMAP_HH

#include <map>
#include <vector>

#include "TString.h"

class BSEmcFwEndcapDetIdToAlveoleMap {
 public:
  struct BSEmcFwEndcapCrystal_t {
    Int_t fX{0}, fY{0}, fCrystal{-1};
  };

  BSEmcFwEndcapDetIdToAlveoleMap() { LoadMap(); };
  virtual ~BSEmcFwEndcapDetIdToAlveoleMap(){};
  void SetFileName(const TString &t_filename)
  {
    fFilename = t_filename;
    LoadMap();
  }
  BSEmcFwEndcapCrystal_t GetAlveoleId(const Int_t t_detectorId) const { return fDetIdToAlveole.at(t_detectorId); }
  std::vector<Int_t> GetAlveoleCrystals(const Int_t t_xindex, const Int_t t_yindex) const { return GetAlveoleCrystals(ConvertToKey(t_xindex, t_yindex)); };
  std::vector<Int_t> GetAlveoleCrystals(const TString t_key) const { return fAlveoleDetIds.at(t_key); }
  std::vector<Int_t> GetAlveoleCrystals(const Int_t t_detectorId) const;

  TString ConvertToKey(const Int_t t_xindex, const Int_t t_yindex) const { return "X" + TString{std::to_string(t_xindex)} + "Y" + TString{std::to_string(t_yindex)}; }

 private:
  void LoadMap();
  std::map<Int_t, BSEmcFwEndcapCrystal_t> fDetIdToAlveole{};
  std::map<TString, std::vector<Int_t>> fAlveoleDetIds{};
  TString fFilename{"FwEndcapDetIdToAlveoleMap.txt"};
  ClassDef(BSEmcFwEndcapDetIdToAlveoleMap, 1);
};

#endif /*BSEMCFWENDCAPDETIDTOALVEOLEMAP_HH*/