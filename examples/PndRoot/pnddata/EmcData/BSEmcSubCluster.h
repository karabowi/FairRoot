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

#ifndef BSEMCSUBCLUSTER_HH
#define BSEMCSUBCLUSTER_HH

#include <string> // for string

#include "Rtypes.h"     // for BSEmcSubCluster::Class, BSEmcSubCluster::S...
#include "RtypesCore.h" // for Int_t

#include "BSEmcCluster.h" // for BSEmcCluster
#include "BSEmcDataBranchNames.h"

class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @brief represents a reconstructed (splitted) emc cluster
 * @author
 * @ingroup EmcData
 */
class BSEmcSubCluster : public BSEmcCluster {

 public:
  // Constructors
  BSEmcSubCluster();
  BSEmcSubCluster(const BSEmcSubCluster &t_other);

  // Destructor
  virtual ~BSEmcSubCluster();

  Int_t GetParentClusterIndex() const { return fClusterIndex; }
  void SetParentClusterIndex(Int_t clusterIndex) { fClusterIndex = clusterIndex; }
  Int_t GetCentralCrystalId() const { return fCentralCrystalId; }
  void SetCentralCrystalId(Int_t centralCrystal) { fCentralCrystalId = centralCrystal; }

 protected:
  // Data members
  Int_t fClusterIndex{-1}; // Index of cluster the subcluster is made off in TClonesArray
  Int_t fCentralCrystalId{-1};

 private:
  ClassDef(BSEmcSubCluster, 2)
};
#endif /*BSEMCSUBCLUSTER_HH*/
