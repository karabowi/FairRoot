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

#ifndef BSEMCPLOTSUBCLUSTER_HH
#define BSEMCPLOTSUBCLUSTER_HH

#include "Rtypes.h"
#include "TString.h"

#include "BSEmcPlotProcess.h"

class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcPlotSubCluster
 * @brief Creates plots showing the subcluster energy, size, etc
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcPlotSubCluster : public BSEmcPlotProcess {
 public:
  BSEmcPlotSubCluster() : BSEmcPlotProcess("BSEmcPlotSubCluster") {}
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void Process() /*override*/;
  virtual void InitHistos(const TString &t_folderprefix) /*override*/;

 private:
  ClassDef(BSEmcPlotSubCluster, 1);
};

#endif /*BSEMCPLOTSUBCLUSTER_HH*/
