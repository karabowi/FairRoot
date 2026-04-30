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

// -------------------------------------------------------------------------
// -----             PndSdsStripAdvClusterFinder header file             -----
// -----          Converted 27.11.2007 from R.Jaekel by R.Kliemt       -----
// -------------------------------------------------------------------------

/** PndSdsStripAdvClusterFinder.h
 *@author R.Kliemt <r.kliemt@physik.tu-dresden.de>
 *@author L.Ackermann
 **
 ** Calculator class to find clusters in channel dimension and time dimension
 ** for the MVD strip sensors
 **/

#ifndef PNDSDSSTRIPCLUSTERFINDER_H
#define PNDSDSSTRIPCLUSTERFINDER_H

#include <vector>
#include <map>
#include <string>

#include "PndSdsDigiStrip.h"
#include "PndSdsClusterStrip.h"
#include "PndSdsStripClusterer.h"
// #include "PndSdsStripCluster.h"

/**
 @class PndSdsStripAdvClusterFinder : public PndSdsStripClusterBuilder
 @brief Find Clusters on a strip sensor in two dimensions

 Search for clusters in channel and trigger timestamp dimension. It is possible to set a whole e.g. defect strip.
 @author Lars Ackermann
 @author Hans-Georg Zaunick
 @author Ralf Kliemt
 @date 11.03.2009
*/
class PndSdsStripAdvClusterFinder : public PndSdsStripClusterer {
 public:
  /** default constructor **/
  PndSdsStripAdvClusterFinder(Int_t DigiType);

  /**
  main constructor Set number of maximum missing channels or time to get the cluster
  @param NrofmissedChannels maximum of missing channels
  @param NrofmissedTimestamps maximum of missing time
  */
  PndSdsStripAdvClusterFinder(Int_t DigiType, Int_t NrofmissedChannels, Int_t NrofmissedTimestamps);

  /** Destructor **/
  ~PndSdsStripAdvClusterFinder();

  /**
  @fn std::vector< PndSdsClusterStrip > SearchClusters()

  calculate clusters in the two dimensional way
  @return vector of clusters
  */
  std::vector<PndSdsClusterStrip *> SearchClusters();

 private:
  /**
  @fn void ClusterIterator(Int_t newCh, Int_t newFrameID, std::map<Int_t, std::map<Int_t, std::vector<Int_t> > >& hitmap, std::vector< Int_t >& onecluster)

  iterates through the hitlist and find clusters. It will stop when reaching the end of hitlist or the maximum of channels or time is reached so that this cluster has no more
  entries
  @param newCh the current channel to search for
  @param newFrameID the current time to search for
  @param hitmap the hitlist where the first element in the vector is negative for already identified hits the second member of the vector knows the hitID
  @param onecluster list of hitIDs belonging to this cluster
  @return void
  */
  void ClusterIterator(Int_t newCh, Int_t newFrameID, std::map<Int_t, std::map<Int_t, std::vector<Int_t>>> &hitmap, std::vector<Int_t> &onecluster);

  /// maximum of missing channels
  Int_t fNmCh;

  /// maximum of missing time
  Int_t fNmTS;

  ClassDef(PndSdsStripAdvClusterFinder, 1);
};

#endif // PndSdsSimpleStripClusterFinder
