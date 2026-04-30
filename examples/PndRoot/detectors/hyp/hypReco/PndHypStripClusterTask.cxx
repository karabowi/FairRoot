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
// -----                PndMvdStripClusterTask source file             -----
// -------------------------------------------------------------------------

#include <cmath>

#include "TClonesArray.h"
#include "TArrayD.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairGeoNode.h"
#include "FairGeoVector.h"
#include "FairLogger.h"

//#include "PndStringVector.h"
#include "PndGeoHypPar.h"
#include "PndHypStripDigiPar.h"
#include "PndHypStripClusterTask.h"
#include "PndHypPoint.h"
#include "PndHypCalcStrip.h"
#include "PndHypDigiStrip.h"
//#include "PndHypStripCluster.h"
//#include "PndHypClusterCand.h"
#include "PndHypCluster.h"
#include "PndHypGeoHandling.h"
#include "FairHit.h"
#include "PndHypStripClusterBuilder.h"

#include <map>

// enum SensorSide { SensorSide::kTOP, SensorSide::kBOTTOM };

// -----   Default constructor   -------------------------------------------
PndHypStripClusterTask::PndHypStripClusterTask() : FairTask("HYP Strip Clustertisation Task")
{
  fChargeCut = 1.e6; // this ist really large and shall have no effect
  fGeoFile = "";
}
// -------------------------------------------------------------------------

// -----           constructor   -------------------------------------------
PndHypStripClusterTask::PndHypStripClusterTask(Double_t chargecut, TString geoFile) : FairTask("HYP Strip Clustertisation Task")
{
  fChargeCut = chargecut;
  fGeoFile = geoFile;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndHypStripClusterTask::~PndHypStripClusterTask()
{
  if (0 != fGeoH)
    delete fGeoH;
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndHypStripClusterTask::SetParContainers()
{
  // Get Base Container

  FairRunAna *ana = FairRunAna::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  // fGeoPar = (PndGeoHypPar*)(rtdb->getContainer("PndGeoHypPar"));
  fDigiPar = (PndHypStripDigiPar *)(rtdb->getContainer("PndHypStripDigiPar"));
}

InitStatus PndHypStripClusterTask::ReInit()
{

  InitStatus stat = kERROR;
  return stat;

  /*FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
 fGeoPar=(PndGeoHypPar*)(rtdb->getContainer("PndGeoHypPar"));
  return kSUCCESS;*/

  /*
  return kSUCCESS;
  */
}

// -----   Public method Init   --------------------------------------------
InitStatus PndHypStripClusterTask::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    std::cout << "-E- PndHypStripClusterTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input array
  fDigiArray = (TClonesArray *)ioman->GetObject("HypStripDigis");

  if (!fDigiArray) {
    std::cout << "-W- PndHypStripClusterTask::Init: "
              << "No HYPDigi array!" << std::endl;
    return kERROR;
  }

  // set output arrays
  fHitArray = new TClonesArray("PndHypHit");
  // ioman->Register("HYPStripClusterHit", "HYP", fHitArray, kTRUE);
  ioman->Register("HYPHit", "HYP", fHitArray, kTRUE);

  fClusterArray = new TClonesArray("PndHypCluster");
  ioman->Register("HYPStripClusterCand", "HYP", fClusterArray, kTRUE);

  SetParContainers();

  // geo name handling
  if (fGeoFile == "")
    fGeoFile = ioman->GetInFile()->GetName();
  fGeoH = new PndHypGeoHandling(fGeoFile.Data());

  if (!fDigiPar) {
    std::cout << "-W- PndMvdStripClusterTask::Init: "
              << "No fDigiParTrap!" << std::endl;
    return kERROR;
  }
  fstripcalcTOP = new PndHypCalcStrip(fDigiPar, SensorSide::kTOP);
  fstripcalcBOT = new PndHypCalcStrip(fDigiPar, SensorSide::kBOTTOM);

  LOG(info) << " PndHypStripClusterTask: Initialisation successfull";
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndHypStripClusterTask::Exec(Option_t *)
{
  if (fVerbose > 2)
    std::cout << "Sarting PndHypStripClusterTask::Exec()" << std::endl;
  std::vector<PndHypDigiStrip> digiStripArray;
  //   std::vector<PndHypClusterCand> clusters;

  // Reset output array
  if (!fClusterArray)
    Fatal("Exec", "No ClusterArray");
  fClusterArray->Clear();

  if (!fHitArray)
    Fatal("Exec", "No HitArray");
  fHitArray->Clear();

  // Int_t nPoints = fDigiArray->GetEntriesFast(); //[R.K. 01/2017] unused variable

  // load the Clusterfinder
  PndHypStripClusterBuilder clusterbuilder;
  //  PndHypCalcStrip stripcalcTOP(fDigiPar, SensorSide::kTOP);
  //   PndHypCalcStrip stripcalcBOT(fDigiPar, SensorSide::kBOTTOM);
  if (fVerbose > 1)
    fDigiPar->Print();

  TVector2 topDirection, botDirection;
  // TGeoVolume* actVolume; //[R.K. 01/2017] unused variable
  // TGeoBBox* actBox; //[R.K. 01/2017] unused variable
  TVector3 sensorDim, localpos, localDpos;
  TString detName;                // std::string detName;
  Int_t detID, clindex, botIndex; //;iDigi,  mcindex,topIndex, //[R.K. 01/2017] unused variable
  Double_t mycharge;
  TVector2 meantopPoint, meanbotPoint, onsensorPoint;
  TVector3 hitPos, hitErr;
  Double_t t, b; // local[3], master[3], //[R.K. 01/2017] unused variable

  // std::map<std::string,Int_t> firedSensors;
  Int_t strip;
  SensorSide side;
  PndHypDigiStrip *myDigi = 0;
  // PndHypCluster* myCandTop=0; //[R.K. 01/2017] unused variable
  // PndHypCluster* myCandBot=0; //[R.K. 01/2017] unused variable

  for (Int_t iPoint = 0; iPoint < fDigiArray->GetEntriesFast(); iPoint++) { // sort digis by sensor name and stripnumber

    myDigi = (PndHypDigiStrip *)(fDigiArray->At(iPoint));

    detName = myDigi->GetDetName().Data();

    fstripcalcTOP->CalcFeChToStrip(myDigi->GetFE(), myDigi->GetChannel(), strip, side);

    std::cout << " strip " << strip << " side " << std::endl;

    // stripcalcBOT.CalcFeChToStrip(myDigi->GetFE(), myDigi->GetChannel(), stripB, sideB);

    clusterbuilder.AddDigi(detName.Data(), side, strip, iPoint);

    // if(firedSensors[detName.Data()]) firedSensors[detName.Data()] = myDigi->GetDetID();
  }

  std::vector<PndHypCluster> clusters;
  std::vector<Int_t> topclusters; // contains index to fClusterArray
  std::vector<Int_t> botclusters; // contains index to fClusterArray
  std::vector<Int_t> oneclustertop;
  std::vector<Int_t> oneclusterbot;

  //  -----  search for clusters  -----
  clusters = clusterbuilder.SearchClusters();

  topclusters = clusterbuilder.GetTopClusterIDs();
  botclusters = clusterbuilder.GetBotClusterIDs();

  for (std::vector<PndHypCluster>::iterator clit = clusters.begin(); clit != clusters.end(); ++clit) {
    /// Muss ich den copy construktor selbst schreiben?
    /// fehlt was im linkdef?
    clindex = fClusterArray->GetEntriesFast();
    // std::cout<<" clindex "<<clindex<<std::endl;
    new ((*fClusterArray)[clindex]) PndHypCluster(*clit);
  }

  if (fVerbose > 2) {
    std::cout << "Top Clusters: ";
    for (std::vector<Int_t>::iterator itTop = topclusters.begin(); itTop != topclusters.end(); ++itTop) {
      std::cout << *itTop << " | ";
    }
    std::cout << std::endl;
    std::cout << "Bot Clusters: ";
    for (std::vector<Int_t>::iterator itBot = botclusters.begin(); itBot != botclusters.end(); ++itBot) {
      std::cout << *itBot << " | ";
    }
    std::cout << std::endl;
  }
  //  -----  calculate mean strip numbers  -----

  // -----  merge top/bot clusters to hits  -----
  // loop on clusters from the top side
  for (std::vector<Int_t>::iterator itTop = topclusters.begin(); itTop != topclusters.end(); ++itTop) {
    // prepare values for the top side
    Double_t topcharge = 0., meantopstrip = 0.;

    oneclustertop = clusters[*itTop].GetClusterList();

    TString detnametop = ((PndHypDigiStrip *)fDigiArray->At(oneclustertop[0]))->GetDetName();

    // std::cout<<"det id top "<<detnametop<<std::endl;
    topDirection = fstripcalcTOP->GetStripDirection();
    botDirection = fstripcalcBOT->GetStripDirection();

    detID = ((PndHypDigiStrip *)fDigiArray->At(oneclustertop[0]))->GetDetID();

    // Double_t lastcharge=0; //[R.K. 01/2017] unused variable
    for (std::vector<Int_t>::iterator itTopDigi = oneclustertop.begin(); itTopDigi != oneclustertop.end();
         ++itTopDigi) { // I use the temp. variables from the top of this method again
      // calculate the mean charge and stripnumber

      myDigi = (PndHypDigiStrip *)fDigiArray->At(*itTopDigi);

      fstripcalcTOP->CalcFeChToStrip(myDigi->GetFE(), myDigi->GetChannel(), strip, side);
      topcharge += myDigi->GetCharge();
      meantopstrip += myDigi->GetCharge() * strip;

      if (fVerbose > 2)
        std::cout << "FE no: " << myDigi->GetFE() << " | channel no: " << myDigi->GetChannel() << " | topstrip no: " << strip << "| detID no: " << myDigi->GetDetID() << std::endl;
    }

    if (topcharge > 0) {
      meantopstrip = meantopstrip / topcharge;
      fstripcalcTOP->CalcStripPointOnLine(meantopstrip, meantopPoint);
      // loop on bottom side
      for (std::vector<Int_t>::iterator itBot = botclusters.begin(); itBot != botclusters.end(); ++itBot) {
        botIndex = *itBot;
        Double_t botcharge = 0., meanbotstrip = 0.;

        oneclusterbot = (clusters[*itBot]).GetClusterList();
        // std::cout<<" size bot "<<oneclusterbot.size()<<std::endl;

        TString detnamebot = ((PndHypDigiStrip *)fDigiArray->At(oneclusterbot[0]))->GetDetName();
        // std::cout<<" bot name "<<detnamebot<<std::endl;

        if (detnamebot != detnametop)
          continue;
        // cout<<"  name detID "<<detnametop<<" bot "<<detnamebot<<" ind "
        //  <<botIndex<<std::endl;

        for (std::vector<Int_t>::iterator itBotDigi = oneclusterbot.begin(); itBotDigi != oneclusterbot.end();
             ++itBotDigi) { // I use the temp. variables from the top of this method again

          myDigi = (PndHypDigiStrip *)fDigiArray->At(*itBotDigi);

          fstripcalcTOP->CalcFeChToStrip(myDigi->GetFE(), myDigi->GetChannel(), strip, side);
          botcharge += myDigi->GetCharge();
          meanbotstrip += myDigi->GetCharge() * strip;

          if (fVerbose > 2)
            std::cout << "FE no: " << myDigi->GetFE() << " | channel no: " << myDigi->GetChannel() << " | botstrip no: " << strip << std::endl;
        }

        if (botcharge > 0) {
          meanbotstrip = meanbotstrip / botcharge;
          // look if the charges are not too differently
          if (fVerbose > 2) {
            std::cout << "Charges: Ctop = " << topcharge << " | Cbot = " << botcharge << " | difference bot-top = " << botcharge - topcharge << std::endl;
          }
          if (fabs(botcharge - topcharge) < fChargeCut) {
            mycharge = (botcharge + topcharge) / 2.;
            fstripcalcBOT->CalcStripPointOnLine(meanbotstrip, meanbotPoint);
            // get the backmapped point
            onsensorPoint = CalcLineCross(meantopPoint, topDirection, meanbotPoint, botDirection);

            if (fVerbose > 2) {
              std::cout << "Strip no. top: " << meantopstrip << " | bot: " << meanbotstrip << " | Mean Top side: (" << meantopPoint.X() << " ; " << meantopPoint.Y() << ")"
                        << "\n"
                        << "Mean Bot side: (" << meanbotPoint.X() << " ; " << meanbotPoint.Y() << ")"
                        << "\n"
                        << "On Sensor Hit: (" << onsensorPoint.X() << " ; " << onsensorPoint.Y() << ")" << std::endl;
            }
            std::cout << " botIndex @final " << botIndex << std::endl;

            localpos.SetXYZ(onsensorPoint.X(), onsensorPoint.Y(), 0.);
            // local[0]=onsensorPoint.X();
            // local[1]=onsensorPoint.Y();//0.
            // local[2]=0.;//onsensorPoint.Y();//local coord.X-Z

            // shift sensor system into middle

            // local[0]-=sensorDim.X();
            // local[1]-=sensorDim.Y();
            // local[2]-=sensorDim.Z();

            // do the transformation from sensor to lab frame
            hitPos = fGeoH->LocalToMasterId(localpos, detnametop.Data());

            // gGeoManager->LocalToMaster(local,master);
            // hitPos.SetXYZ(master[0],master[1],master[2]);

            // calculate the errors corresponding to a skewed system!

            t = fDigiPar->GetTopPitch() * cos(fDigiPar->GetOrient());
            b = fDigiPar->GetBotPitch() * cos(fDigiPar->GetOrient() + fDigiPar->GetSkew());

            localDpos.SetX(sqrt((t * t + b * b) / 12.));

            t = fDigiPar->GetTopPitch() * sin(fDigiPar->GetOrient());
            b = fDigiPar->GetBotPitch() * sin(fDigiPar->GetOrient() + fDigiPar->GetSkew());

            localDpos.SetY(sqrt((t * t + b * b) / 12.));
            localDpos.SetZ(0.);
            // x-z c.s local[2]=sqrt((t*t+b*b)/12.);
            // local[1]=sqrt((t*t+b*b)/12.);//x-z cs.0.
            // local[2]=0.;
            // hitErr.SetXYZ(local[0],local[1],0.);
            hitErr = fGeoH->LocalToMasterErrorsId(localDpos, detnametop.Data());
            // gGeoManager->LocalToMaster(local,master);
            // hitErr.SetXYZ(master[0],master[1],master[2]);

            Int_t i = fHitArray->GetEntriesFast();

            new ((*fHitArray)[i]) PndHypHit(detID, detnametop.Data(), hitPos, hitErr, *itTop, mycharge, oneclusterbot.size() + oneclustertop.size());
            ((PndHypHit *)((*fHitArray)[i]))->SetBotIndex(*itBot);

          } else if (fVerbose > 2)
            std::cout << "Strip charge contents too differently" << std::endl;
        }
      } // loop bot clusters
    }
  } // loop top clusters

  //}// end loop itSensors

  if (fVerbose > 1)
    std::cout << "ClusterArraySize: " << fClusterArray->GetEntriesFast() << " | HitArraySize: " << fHitArray->GetEntriesFast() << " Hits calculated."
              << " out of " << fDigiArray->GetEntriesFast() << " Digis" << std::endl;

  return;
}

TVector2 PndHypStripClusterTask::CalcLineCross(TVector2 point1, TVector2 dir1, TVector2 point2, TVector2 dir2)
{
  Double_t dx, dy, s, M, x, y; // t, //[R.K. 01/2017] unused variable
  dx = point2.X() - point1.X();
  dy = point2.Y() - point1.Y();

  M = dir1.X() * dir2.Y() - dir1.Y() * dir2.X();

  if (M != 0.) {
    s = dir1.Y() * dx / M - dir1.X() * dy / M;
    x = point2.X() + dir2.X() * s;
    y = point2.Y() + dir2.Y() * s;
  } else {
    std::cout << "Warning in PndHypStripClusterTask::CalcLineCross(): M=0 setting (x,y) to 0" << std::endl;
    x = 0.;
    y = 0.;
  }

  TVector2 result(x, y);
  return result;
}

ClassImp(PndHypStripClusterTask);
