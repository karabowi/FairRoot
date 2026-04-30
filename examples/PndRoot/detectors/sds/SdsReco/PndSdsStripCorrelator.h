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

//------------------------------------------------------------------------------------
//----------------          PndSdsStripCorrelator.h          -------------------------
// Class to correlate top and bottom clusters on double sided strip sensors
//
// Authors: R. Kliemt (Uni Bonn)
//          H.G. Zaunick (Uni Bonn)
// Date:    June 2012
//
//------------------------------------------------------------------------------------

#ifndef PNDSDSSTRIPCLUSTECORRELATOR_H
#define PNDSDSSTRIPCLUSTECORRELATOR_H 1

#include <map>
#include <vector>
#include <utility>
#include <iostream>

using namespace std;

// Data types

struct PndSdsStripCorrelatorCand {
  PndSdsStripCorrelatorCand() : top(), bot(), q_top(), q_bot(), prob(1.) {}
  PndSdsStripCorrelatorCand(int t, int b, double qt, double qb, double p) : top(t), bot(b), q_top(qt), q_bot(qb), prob(p) {}
  int top, bot;
  double q_top, q_bot;
  double prob;
};

struct PndSdsStripCorrelatorCombi {
 public:
  PndSdsStripCorrelatorCombi() : pairlist(), prob(1.) {}
  PndSdsStripCorrelatorCombi(const std::vector<PndSdsStripCorrelatorCand> &list, double p) : pairlist(), prob(p) { pairlist.insert(pairlist.end(), list.begin(), list.end()); }
  PndSdsStripCorrelatorCombi(const PndSdsStripCorrelatorCombi &a_combi) : pairlist(), prob(a_combi.prob)
  {
    pairlist.insert(pairlist.end(), a_combi.pairlist.begin(), a_combi.pairlist.end());
  }
  std::vector<PndSdsStripCorrelatorCand> pairlist;
  double prob;
};

// Now the "real" class

class PndSdsStripCorrelator {

 public:
  PndSdsStripCorrelator(int mode = 0, double cut = 0., double noise = 0., double threshold = 0.);
  ~PndSdsStripCorrelator();

  void Reset();

  void Setup(int mode = 0, double cut = 0., double noise = 0., double threshold = 0.);

  void AddCluster(int moduleId, int side, int clusterIndex, double charge);

  vector<pair<int, int>> GetCorrelationList();

  vector<double> GetProbList();
  vector<double> GetSecondProbList();
  vector<int> GetMultProbList();

 private:
  void CalcChargeDifferenceCut();
  void CalcLikelihoodAlgo();
  void CalcAll();
  std::vector<PndSdsStripCorrelatorCombi> getCombinations(std::map<int, std::map<int, PndSdsStripCorrelatorCand>> matrix, int cols, int rows);
  std::map<int, std::map<int, PndSdsStripCorrelatorCand>>
  getSubMatrix(std::map<int, std::map<int, PndSdsStripCorrelatorCand>> matrix, int cols, int rows, int pivotCol, int pivotRow);

  //      Module            Side                    clusterindex charge
  map<int, map<int, vector<pair<int, double>>>> fClusterList; // List of the cluster indices and their charges
  vector<pair<int, int>> fCorrelationList;                    // List of matching top/bottom clusters
  vector<double> fCorrelationProbList;                        // List of associated prob values for each module
  vector<double> fSecondProbList;                             // List of the second prob values for each module
  vector<int> fMultProbList;                                  // List of number of correlations

  double fCut;       // Cut parameter: ChargeDifference or minimum Likelihood
  double fNoise;     // Noise sigma[e-]
  double fThreshold; // Discrimination threshold [e-]
  int fMode;         // which mode to use 0:ChargeDifferenceCut 1:Liklelihood
  bool fCalculated;  // flag for automatic calculation calls
};

#endif
