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

#ifndef BSEMCPLOTRECOHITS_HH
#define BSEMCPLOTRECOHITS_HH

#include "Rtypes.h"
#include "TString.h"

#include "BSEmcPlotProcess.h"

class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcPlotRecoHits
 * @brief Plots values of RecoHits, such as energy, time, cluster size, subcluster size, zernike moments, etc
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcPlotRecoHits : public BSEmcPlotProcess {
 public:
  BSEmcPlotRecoHits() : BSEmcPlotProcess("BSEmcPlotRecoHits") {}
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void Process() /*override*/;
  virtual void InitHistos(const TString &t_folderprefix) /*override*/;

 private:
  ClassDef(BSEmcPlotRecoHits, 1);
};

#endif /*BSEMCPLOTRECOHITS_HH*/
