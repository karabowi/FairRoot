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
//----------------          PndSdsStripCorrelator.cxx          -----------------------
// Class to correlate top and bottom clusters on double sided strip sensors
//
// Authors: R. Kliemt (Uni Bonn)
//          H.G. Zaunick (Uni Bonn)
// Date:    June 2012
//
//------------------------------------------------------------------------------------

#define CLUSTER_MULT_CUTOFF \
  5 // 7
    // This is a hard cut on the allowed number of clusters per sensor side
    // Combinatorics take a large toll already at 6 clusters present!

#include "PndSdsStripCorrelator.h"
#include <cmath>

//------------------------------------------------------------------------------------
PndSdsStripCorrelator::PndSdsStripCorrelator(int mode, double cut, double noise, double threshold)
  : fClusterList(), fCorrelationList(), fCorrelationProbList(), fSecondProbList(), fMultProbList(), fCut(cut), fNoise(noise), fThreshold(threshold), fMode(mode), fCalculated(false)
{
}

//------------------------------------------------------------------------------------
PndSdsStripCorrelator::~PndSdsStripCorrelator() {}

//------------------------------------------------------------------------------------
void PndSdsStripCorrelator::AddCluster(int moduleId, int side, int clusterIndex, double charge)
{
  fClusterList[moduleId][side].push_back(make_pair(clusterIndex, charge));
}

//------------------------------------------------------------------------------------
void PndSdsStripCorrelator::Reset()
{
  fClusterList.clear();
  fCorrelationList.clear();
  fCorrelationProbList.clear();
  fSecondProbList.clear();
  fMultProbList.clear();
  fCalculated = false;
}

//------------------------------------------------------------------------------------
void PndSdsStripCorrelator::Setup(int mode, double cut, double noise, double threshold)
{
  fMode = mode;
  fCut = cut;
  fNoise = noise;
  fThreshold = threshold;
  fCalculated = false;
}

//------------------------------------------------------------------------------------
vector<pair<int, int>> PndSdsStripCorrelator::GetCorrelationList()
{
  if (!fCalculated) {
    switch (fMode) {
    case 0: CalcChargeDifferenceCut(); break;
    case 1: CalcLikelihoodAlgo(); break;
    default: CalcAll(); break;
    }
    fCalculated = true;
  }
  return fCorrelationList;
}

//------------------------------------------------------------------------------------
vector<double> PndSdsStripCorrelator::GetProbList()
{
  if (!fCalculated) {
    fCorrelationProbList.clear(); // empty on invalid state, just in case
    fSecondProbList.clear();      // empty on invalid state, just in case
    fMultProbList.clear();        // empty on invalid state, just in case
  }
  return fCorrelationProbList;
}

//------------------------------------------------------------------------------------
vector<double> PndSdsStripCorrelator::GetSecondProbList()
{
  if (!fCalculated) {
    fCorrelationProbList.clear(); // empty on invalid state, just in case
    fSecondProbList.clear();      // empty on invalid state, just in case
    fMultProbList.clear();        // empty on invalid state, just in case
  }
  return fSecondProbList;
}

//------------------------------------------------------------------------------------
vector<int> PndSdsStripCorrelator::GetMultProbList()
{
  if (!fCalculated) {
    fCorrelationProbList.clear(); // empty on invalid state, just in case
    fSecondProbList.clear();      // empty on invalid state, just in case
    fMultProbList.clear();        // empty on invalid state, just in case
  }
  return fMultProbList;
}

//------------------------------------------------------------------------------------
void PndSdsStripCorrelator::CalcAll()
{
  double oldcut = fCut;
  fCut = 0.;
  CalcChargeDifferenceCut();
  fCut = oldcut;
}

//------------------------------------------------------------------------------------
void PndSdsStripCorrelator::CalcChargeDifferenceCut()
{
  double dq = 0;
  fCorrelationList.clear();
  for (map<int, map<int, vector<pair<int, double>>>>::iterator modIt = fClusterList.begin(); modIt != fClusterList.end(); ++modIt) {
    for (vector<pair<int, double>>::iterator topIt = modIt->second[0].begin(); topIt != modIt->second[0].end(); ++topIt) {
      for (vector<pair<int, double>>::iterator botIt = modIt->second[1].begin(); botIt != modIt->second[1].end(); ++botIt) {
        dq = fabs((*topIt).second - (*botIt).second);
        if (dq < fCut) {
          fCorrelationList.push_back(make_pair((*topIt).first, (*botIt).first));
          fCorrelationProbList.push_back((dq > 5e4) ? 0 : (1 - 2e-5 * dq));
          fSecondProbList.push_back(-1);
        }
      }
    }
  }
}

//------------------------------------------------------------------------------------
void PndSdsStripCorrelator::CalcLikelihoodAlgo()
{
  // per sensor module fill a combinations matrix, calculate likelihoods, assign b

  fCorrelationList.clear();
  for (map<int, map<int, vector<pair<int, double>>>>::iterator modIt = fClusterList.begin(); modIt != fClusterList.end(); ++modIt) {
    if (modIt->second[0].size() > CLUSTER_MULT_CUTOFF)
      continue; // skip modules with too busy events
    if (modIt->second[1].size() > CLUSTER_MULT_CUTOFF)
      continue; // skip modules with too busy events
                // fill matrix
    std::map<int, std::map<int, PndSdsStripCorrelatorCand>> matrix;
    double top_total = 0., wert = 0.;
    int nTop = 0, nBot = 0, iBot = 0; //,iTop=-1,iBot=-1;
    for (vector<pair<int, double>>::iterator topIt = modIt->second[0].begin(); topIt != modIt->second[0].end(); ++topIt) {
      for (vector<pair<int, double>>::iterator botIt = modIt->second[1].begin(); botIt != modIt->second[1].end(); ++botIt) {
        wert = (*topIt).second - (*botIt).second;
        //        wert *= wert;
        //        top_total += wert;
        top_total += wert * wert;
        wert = erfc(fabs(wert) / (2. * fThreshold) / sqrt(2.)); // [R.K.31.7.'12]
        // wert = erfc(fabs(wert)/(6.*fNoise)/sqrt(2.)); // original
        matrix[nTop][iBot] = PndSdsStripCorrelatorCand((*topIt).first, (*botIt).first, (*topIt).second, (*botIt).second, wert);
        // printf("add to matrix: t=%i, b=%i, qt=%f, qb=%f, p=%f \n",(*topIt).first,(*botIt).first,(*topIt).second,(*botIt).second,wert);
        iBot++;
      }
      if (iBot > nBot)
        nBot = iBot;
      iBot = 0;
      nTop++;
    }

    // Calculate combinations
    std::vector<PndSdsStripCorrelatorCombi> combinations = getCombinations(matrix, nTop, nBot);

    if (combinations.size() == 0)
      continue; // next module

    // select the lowest probability
    double klein = -1., ganzklein = -1.;
    int who = -1, whoelse = -1;

    for (int like = 0; like < (int)combinations.size(); like++) {
      if (combinations[like].prob > klein) {
        if (combinations[like].prob > ganzklein) {
          klein = ganzklein;
          whoelse = who;
          ganzklein = combinations[like].prob;
          who = like;
        } else {
          klein = combinations[like].prob;
          whoelse = like;
        }
      }
    }
    //    if(combinations[who].prob>fCut)
    //    { // index pairs for the output
    for (int kk = 0; kk < (int)combinations[who].pairlist.size(); ++kk) {
      fCorrelationList.push_back(make_pair(combinations[who].pairlist[kk].top, combinations[who].pairlist[kk].bot));
      // printf("add correlation [%i|%i] p=%f \n",combinations[who].pairlist[kk].top,combinations[who].pairlist[kk].bot,combinations[who].prob);
      fCorrelationProbList.push_back(combinations[who].prob);
      if (whoelse >= 0)
        fSecondProbList.push_back(combinations[whoelse].prob);
      else
        fSecondProbList.push_back(-1.);
      fMultProbList.push_back(combinations.size());
    }
    //    }
  }
}

//------------------------------------------------------------------------------------

std::vector<PndSdsStripCorrelatorCombi> PndSdsStripCorrelator::getCombinations(std::map<int, std::map<int, PndSdsStripCorrelatorCand>> matrix, int cols, int rows)
{
  // printf("getCombinations: Matrix(%i,%i) \n",cols,rows);
  std::vector<PndSdsStripCorrelatorCombi> combinations;
  if (cols == 1 && rows == 1) {
    PndSdsStripCorrelatorCombi combi;
    combi.pairlist.push_back(PndSdsStripCorrelatorCand(matrix[0][0]));
    combi.prob = matrix[0][0].prob;
    combinations.push_back(combi);
    return combinations;
  }
  if (cols == 1) {
    PndSdsStripCorrelatorCombi combi;
    double q_t[rows];
    double bsum = 0.;
    for (int i = 0; i < rows; i++)
      bsum += matrix[0][i].q_bot;
    for (int i = 0; i < rows; i++)
      q_t[i] = matrix[0][0].q_top - bsum + matrix[0][i].q_bot;
    // double tot=0.; //unused?
    // double diag=0.; //unused?
    for (int i = 0; i < rows; i++) {
      combi.pairlist.push_back(PndSdsStripCorrelatorCand(matrix[0][i].top, matrix[0][i].bot, q_t[i], matrix[0][i].q_bot, matrix[0][i].prob));
      combi.prob *= erfc(fabs(q_t[i] - matrix[0][i].q_bot) / (2. * fThreshold * sqrt(2.))); // [R.K.31.7.'12]
      // combi.prob*=erfc(fabs(q_t[i]-matrix[0][i].q_bot)/(6.*fNoise*sqrt(2.)));//original
    }
    double prob_full = combi.prob;
    combinations.push_back(combi);
    // double _maxprob = combi.prob; //unused?
    // int _maxcombi = 0; //unused?
    for (int i = 0; i < rows; ++i) { // Recursive call
      std::vector<PndSdsStripCorrelatorCombi> combilist_reduced = getCombinations(getSubMatrix(matrix, 1, rows, -1, i), 1, rows - 1);
      for (int ii = 0; ii < (int)combilist_reduced.size(); ii++) {
        combilist_reduced[ii].prob *= (1. - prob_full);
      }
      combinations.insert(combinations.end(), combilist_reduced.begin(), combilist_reduced.end());
    }
    return combinations;
  }
  if (rows == 1) {
    PndSdsStripCorrelatorCombi combi;
    double q_b[cols];
    double tsum = 0.;
    for (int i = 0; i < cols; i++)
      tsum += matrix[i][0].q_top;
    for (int i = 0; i < cols; i++)
      q_b[i] = matrix[0][0].q_bot - tsum + matrix[i][0].q_top;
    // double tot=0.; //unused?
    // double diag=0.; //unused?
    for (int i = 0; i < cols; i++) {
      combi.pairlist.push_back(PndSdsStripCorrelatorCand(matrix[i][0].top, matrix[i][0].bot, matrix[i][0].q_top, q_b[i], matrix[i][0].prob));
      combi.prob *= erfc(fabs(matrix[i][0].q_top - q_b[i]) / (2. * fThreshold * sqrt(2.))); // [R.K.31.7.'12]
      // combi.prob*=erfc(fabs(matrix[i][0].q_top-q_b[i])/(6.*fNoise*sqrt(2.)));//original
    }
    double prob_full = combi.prob;
    combinations.push_back(combi);
    // double _maxprob = combi.prob; //unused?
    // int _maxcombi = 0; //unused?
    for (int i = 0; i < cols; ++i) { // Recursive call
      std::vector<PndSdsStripCorrelatorCombi> combilist_reduced = getCombinations(getSubMatrix(matrix, cols, 1, i, -1), cols - 1, 1);
      for (int ii = 0; ii < (int)combilist_reduced.size(); ii++) {
        combilist_reduced[ii].prob *= (1. - prob_full);
      }
      combinations.insert(combinations.end(), combilist_reduced.begin(), combilist_reduced.end());
    }
    return combinations;
  }
  // treat 2x2 matrix as separate case since the nr. of independent combinations is just half of the nr. of possible combinations
  if (rows == 2 && cols == 2) {
    PndSdsStripCorrelatorCombi combi1;
    PndSdsStripCorrelatorCombi combi2;
    combi1.prob = 1.;
    combi2.prob = 1.;
    combi1.pairlist.push_back(PndSdsStripCorrelatorCand(matrix[0][0]));
    combi1.prob *= matrix[0][0].prob;
    combi1.pairlist.push_back(PndSdsStripCorrelatorCand(matrix[1][1]));
    combi1.prob *= matrix[1][1].prob;
    combi2.pairlist.push_back(PndSdsStripCorrelatorCand(matrix[1][0]));
    combi2.prob *= matrix[1][0].prob;
    combi2.pairlist.push_back(PndSdsStripCorrelatorCand(matrix[0][1]));
    combi2.prob *= matrix[0][1].prob;
    combinations.push_back(combi1);
    combinations.push_back(combi2);
    return combinations;
  }
  for (int i = 0; i < cols; i++) {
    for (int j = 0; j < rows; j++) { // Recursive call
      std::vector<PndSdsStripCorrelatorCombi> combilist = getCombinations(getSubMatrix(matrix, cols, rows, i, j), cols - 1, rows - 1);
      for (int k = 0; k < (int)combilist.size(); k++) {
        combilist[k].pairlist.push_back(PndSdsStripCorrelatorCand(matrix[i][j]));
        combilist[k].prob *= matrix[i][j].prob;
        combinations.push_back(PndSdsStripCorrelatorCombi(combilist[k].pairlist, combilist[k].prob));
      }
    }
  }
  return combinations;
}

//------------------------------------------------------------------------------------

std::map<int, std::map<int, PndSdsStripCorrelatorCand>>
PndSdsStripCorrelator::getSubMatrix(std::map<int, std::map<int, PndSdsStripCorrelatorCand>> matrix, int cols, int rows, int pivotCol, int pivotRow)
{
  std::map<int, std::map<int, PndSdsStripCorrelatorCand>> result;
  int ii = 0;
  for (int i = 0; i < cols; i++) {
    if (i == pivotCol)
      continue;
    int jj = 0;
    for (int j = 0; j < rows; j++) {
      result[ii][jj] = matrix[i][j];
      if (j == pivotRow)
        continue;
      jj++;
    }
    ii++;
  }
  return result;
}

//------------------------------------------------------------------------------------

//#define CLUSTER_MULT_CUTOFF 7
//
// std::vector<RecoHitPair> StripHitReco::GetHitPoints(double sigmaNoise, double minProbability)
//{
//	std::vector<RecoHitPair> result;
//	if(!fTopClusterList.size() || !fBottomClusterList.size()) return result;
//	if (
//      (fTopClusterList.size()>=CLUSTER_MULT_CUTOFF || fBottomClusterList.size()>=CLUSTER_MULT_CUTOFF))
//	{
//		if (fVerbose) cout<<"-W- too many clusters ("<<fTopClusterList.size()<<","<<fBottomClusterList.size()<<"). skipping event "<<endl;
//		return result;
//	}
//
//	if (fVerbose>2) cout<<"-I- *** clusters (t/b): ("<<fTopClusterList.size()<<"/"<<fBottomClusterList.size()<<")"<<endl;
//
//	std::map<int,std::map<int,PndSdsStripCorrelatorCand> > matrix;
//	double top_total=0.;
//  //fill matrix
//	for(int first=0;first<fTopClusterList.size();++first)
//	{
//		for(int second=0;second<fBottomClusterList.size();++second)
//		{
//			double wert=fTopClusterList[first].GetChargeSum()-fBottomClusterList[second].GetChargeSum();
//			wert*=wert;
//			top_total+=wert;
//			matrix[first][second]=PndSdsStripCorrelatorCand(first,second,fTopClusterList[first].GetChargeSum(),fBottomClusterList[second].GetChargeSum(),wert);
//		}
//	}
//	if (fVerbose>2)   if (fTopClusterList.size()>=1 || fBottomClusterList.size()>=1) cout<<"-I- total sum of charge differences="<<sqrt(top_total)<<endl;
//
//
//	if (fVerbose>1) if (fTopClusterList.size()>=1 || fBottomClusterList.size()>=1) cout<<"-I- matrix of charge differences:"<<endl;
//	if (fTopClusterList.size()>1 || fBottomClusterList.size()>1)
//	{
//		for(int second=0;second<fBottomClusterList.size();++second)
//		{
//			for(int first=0;first<fTopClusterList.size();++first)
//			{
//				double _diff=sqrt(matrix[first][second].prob);
//				if (fVerbose>1) if (fTopClusterList.size()>1 || fBottomClusterList.size()>1) cout<<" "<<_diff;
//				// combinatorial probability
//				matrix[first][second].prob=1.;
//				// probability due to charge difference
//				matrix[first][second].prob*=erfc(fabs(_diff)/(6.*fNoise)/sqrt(2.));
//			}
//			if (fVerbose>1) if (fTopClusterList.size()>1 || fBottomClusterList.size()>1) cout<<endl;
//		}
//	} else if (fTopClusterList.size()==1 && fBottomClusterList.size()==1) {
//		double _diff=sqrt(matrix[0][0].prob);
//		if (fVerbose>1) cout<<_diff<<endl;
//		matrix[0][0].prob=erfc(fabs(_diff)/(6.*fNoise)/sqrt(2.));
//	}
//
//	std::vector<PndSdsStripCorrelatorCombi> combinations=getCombinations(matrix,fTopClusterList.size(),fBottomClusterList.size());
//
//	if (fVerbose) if (fTopClusterList.size()>1 || fBottomClusterList.size()>1) cout<<"-I- nr of combinations="<<combinations.size()<<endl;
//
//
//	if (fVerbose>2)
//		if (combinations.size()>1)
//			for (int i=0; i<combinations.size(); i++)
//			{
//				cout<<"-I- "<<i<<": prob="<<combinations[i].prob<<"  ";
//				for (int j=0; j<combinations[i].pairlist.size(); j++)
//					cout<<"("<<combinations[i].pairlist[j].top<<","<<combinations[i].pairlist[j].bot<<") ";
//        cout<<endl;
//			}
//
//	double klein=-1.;
//	int who=-1;
//	for(int like=0;like<combinations.size();like++)
//	{
//		if(combinations[like].prob>klein)
//		{
//			klein=combinations[like].prob;
//			who=like;
//		}
//	}
//
//	if (fVerbose)
//		if (fTopClusterList.size()>=1 || fBottomClusterList.size()>=1) {
//			cout<<"-I- most probable combination (prob="<<combinations[who].prob<<"): ";
//			for (int j=0; j<combinations[who].pairlist.size(); j++)
//				cout<<"("<<combinations[who].pairlist[j].top<<","<<combinations[who].pairlist[j].bot<<") ";
//			cout<<endl;
//			cout<<"-I- charges: ";
//			for (int j=0; j<combinations[who].pairlist.size(); j++)
//				cout<<"("<<combinations[who].pairlist[j].q_top<<","<<combinations[who].pairlist[j].q_bot<<") ";
//			cout<<endl;
//		}
//
//
//	if(combinations.size()!=0 && combinations[who].prob>minProbability)
//	{
//		for(int i=0;i<combinations[who].pairlist.size();++i)
//		{
//			double top=calcCoordinate(fTopClusterList[combinations[who].pairlist[i].top]);
//			double toperror=falgorith.GetError();
//			double bottom=calcCoordinate(fBottomClusterList[combinations[who].pairlist[i].bot]);
//			double bottomerror=falgorith.GetError();
//
//      RecoHitPair Punkt(fTopClusterList[combinations[who].pairlist[i].top].GetEventID(),
//                        fTopClusterList[combinations[who].pairlist[i].top].GetTimestamp(),
//                        fTopClusterList[combinations[who].pairlist[i].top].GetModuleID(),
//                        combinations[who].pairlist[i].q_top,
//                        combinations[who].pairlist[i].q_bot,
//                        top,
//                        bottom,
//                        fTopClusterList[combinations[who].pairlist[i].top].NrHits(),
//                        fBottomClusterList[combinations[who].pairlist[i].bot].NrHits(),
//                        combinations[who].prob,
//                        toperror,
//                        bottomerror);
//
//			result.push_back(Punkt);
//		}
//	}
//	return result;
//}
