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

#ifndef BSEMCPLOTPROCESS_HH
#define BSEMCPLOTPROCESS_HH

#include <string>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TString.h"
#include "TVector3.h"

#include "PndContainerI.h"
#include "PndProcess.h"

#include "BSEmcCrystalPositionPar.h"
#include "BSEmcGeoNeighbouringRelationPar.h"
#include "BSEmcHistogrammer.h"

class PndMCTrack;
class BSEmcMCPoint;
class BSEmcMCDeposit;
class BSEmcMultiWaveform;
class BSEmcDigi;
class BSEmcCluster;
class BSEmcSubCluster;
class BSEmcRecoHit;
class BSEmcCrystalPositionPar;
class BSEmcGeoNeighbouringRelationPar;
class BSEmcHistogrammer;
class PndParameterRegister;
class TBuffer;
class TClass;
class TMemberInspector;
struct BSEmcPlotData;
template <class T>
class PndContainerI;

/**
 * @class BSEmcPlotProcess
 * @brief Base Processclass for plotting
 * @details Fetches and sets pointers to Emc-Detector Data Containers and fetches often used Emc Parameters
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcPlotProcess : public PndProcess {
 public:
  BSEmcPlotProcess();
  BSEmcPlotProcess(const std::string &t_processname);
  virtual ~BSEmcPlotProcess();

  virtual void SetDetectorName(const std::string &t_detectorName);
  virtual void RequestDataContainer(PndContainerRegister * /*t_register*/) /*override*/ {};
  virtual void GetDataContainer(PndContainerRegister * /*t_register*/) /*override*/ {};
  virtual void SetupParameters(const PndParameterRegister * /*t_parameterRegister*/) /*override*/;
  void SetFolderPrefix(TString t_prefix) { fFolderprefix = t_prefix; }
  void SetHistogrammer(BSEmcHistogrammer *t_histogrammer) { fHistogrammer = t_histogrammer; }
  virtual void InitHistos(const TString &t_folderprefix) = 0;
  virtual void Process() /*override*/ = 0;

 protected:
  Double_t GetPhiOfInDeg(const TVector3 &t_pos) const;
  Double_t GetThetaOfInDeg(const TVector3 &t_pos) const;

  std::string fPositionParName;
  std::string fNeighbouringRelationParName;
  PndContainerI<PndMCTrack> *fMCTrackArray{nullptr};
  PndContainerI<BSEmcMCPoint> *fMCPointArray{nullptr};
  PndContainerI<BSEmcMCDeposit> *fMCDepositArray{nullptr};
  PndContainerI<BSEmcMultiWaveform> *fWaveformArray{nullptr};
  PndContainerI<BSEmcDigi> *fDigiArray{nullptr};
  PndContainerI<BSEmcCluster> *fClusterArray{nullptr};
  PndContainerI<BSEmcSubCluster> *fSubClusterArray{nullptr};
  PndContainerI<BSEmcRecoHit> *fRecoHitArray{nullptr};

  BSEmcGeoNeighbouringRelationPar *fNeighbouringRelationPar{nullptr};
  BSEmcCrystalPositionPar *fPositionPar{nullptr};
  BSEmcHistogrammer *fHistogrammer{nullptr};
  TString fFolderprefix{""};

 private:
  ClassDef(BSEmcPlotProcess, 1);
};

#endif /*BSEMCPLOTPROCESS_HH*/
