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

#ifndef BSEMCCRYSTALPOSITIONPAR_HH
#define BSEMCCRYSTALPOSITIONPAR_HH
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TObjArray.h"
#include "TString.h"
#include "TVector3.h"

#include "FairParGenericSet.h"
#include "FairParamList.h"
#include "FairRun.h"

class FairParamList;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcCrystalPositionData
 * @brief Struct containing crystal position data
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
struct BSEmcCrystalPositionData {
  TVector3 Center{0, 0, 0};
  TVector3 FrontCenter{0, 0, 0};
  TVector3 FrontFaceNormal{0, 0, 0};
  TVector3 Axis{0, 0, 0};
  BSEmcCrystalPositionData() = default;
  BSEmcCrystalPositionData(const TVector3 &center, const TVector3 &fcenter, const TVector3 &frontfacenormal, const TVector3 &axis)
    : Center(center), FrontCenter(fcenter), FrontFaceNormal(frontfacenormal), Axis(axis)
  {
  }
  const TVector3 &GetCentre() const { return Center; }
  const TVector3 &GetFrontCentre() const { return FrontCenter; }
  const TVector3 &GetNormalToFrontFace() const { return FrontFaceNormal; }
  const TVector3 &GetAxisVector() const { return Axis; }
};

/**
 * @class BSEmcCrystalPositionPar
 * @brief Parameter for crystal positions
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcCrystalPositionPar : public FairParGenericSet {
 public:
  static std::string fgParameterName;
  BSEmcCrystalPositionPar(const char *t_name = "BSEmcCrystalPositionPar", const char *t_title = "Center front face position of BSEmc crystals",
                          const char *t_context = "TestDefaultContext");
  ~BSEmcCrystalPositionPar(void);

  void clear(void) /*override*/ {}
  void putParams(FairParamList * /*unused*/) /*override*/;
  Bool_t getParams(FairParamList * /*unused*/) /*override*/;

  void SetPositionDataForCrystal(Int_t t_id, const BSEmcCrystalPositionData &t_position);
  const BSEmcCrystalPositionData &GetPositionData(Int_t t_detectorid) const;
  TObjArray *GetPositions() const { return fPositions.get(); }

  void SetPositionMethod(const TString &t_positionMethod);
  void SetPositionDepth(Double_t t_depth) { fCrystalPositionDepth = t_depth; }
  void SetRescaleFactor(Double_t t_factor) { fRescaleFactor = t_factor; }
  Double_t GetRescaleFactor() const { return fRescaleFactor; }
  TVector3 GetPosition(Int_t t_detectorId) const;

  BSEmcCrystalPositionPar(const BSEmcCrystalPositionPar &t_posPar);
  BSEmcCrystalPositionPar &operator=(const BSEmcCrystalPositionPar &t_posPar);

  void InitFromTextFile(const TString &t_filename);
  void WriteToTextFile(const TString &t_filename) const;
  void FillMap();
  void UpdatePositionArray();

  Bool_t IsSet() { return fPositions->GetEntriesFast() != 0; }

 private:
  std::unique_ptr<TObjArray> fPositions{nullptr};
  std::map<Int_t, BSEmcCrystalPositionData> fPositionMap{}; //!
  Double_t fRescaleFactor{-1};
  Double_t fCrystalPositionDepth{-1};
  std::function<TVector3(const BSEmcCrystalPositionData &, Double_t)> fDigiPosMethod{}; //!
  Bool_t fDigiPosMethodIsSet{kFALSE};

  ClassDef(BSEmcCrystalPositionPar, 2)
};

#endif /*BSEMCCRYSTALPOSITIONPAR_HH*/
