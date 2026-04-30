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

/////////////////////////////////////////////////////////////
// PndSttRootGeometryInitTask
//
// Class to initialize the geometry handling of the ROOT geometry of the STT
//
// authors: Tobias Stockmanns - FZ Jülich
//
//
/////////////////////////////////////////////////////////////

#include "PndSttRootGeometryInitTask.h"
#include "PndSttTubeIdMapCreatorRoot.h"
#include "PndSttTubeMapCreatorRoot.h"
#include "FairLogger.h"

PndSttRootGeometryInitTask::PndSttRootGeometryInitTask() : fRunOnce(true), fGeoH(nullptr) {}

PndSttRootGeometryInitTask::~PndSttRootGeometryInitTask() {}

InitStatus PndSttRootGeometryInitTask::Init()
{
  return kSUCCESS;
}

InitStatus PndSttRootGeometryInitTask::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

void PndSttRootGeometryInitTask::SetParContainers()
{
  if (fGeoH == nullptr)
    fGeoH = PndGeoHandling::Instance();

  fGeoH->SetParContainers();
  FairRuntimeDb *rtdb = FairRun::Instance()->GetRuntimeDb();
  PndGeoHandling::Instance()->SetParContainers();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
}

void PndSttRootGeometryInitTask::Exec(Option_t *opt)
{
  if (fSttParameters->GetGeometryType() != 2)
    fRunOnce = false;
  if (fRunOnce) {
    PndSttTubeIdMapCreatorRoot idmapCreator;
    idmapCreator.CreateTubeMap();
    PndSttTubeMapCreatorRoot mapCreator;
    mapCreator.CreateTubeMap();
    fRunOnce = false;
    LOG(info) << " PndSttRootGeometryInitTask::Exec PndSttTubeIdMap and PndSttTubeMap created";
  }
}

ClassImp(PndSttRootGeometryInitTask)
