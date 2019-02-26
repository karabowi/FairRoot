/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *              GNU Lesser General Public Licence (LGPL) version 3,             *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
// -----               FairYamlSimConfigurce file                      -----
// -----            Created 2019.02.19  by R. Karabowicz               -----
// -------------------------------------------------------------------------
#include "FairYamlSimConfig.h"

#include "FairLogger.h"
#include "FairRunSim.h"
#include "FairFastSimRunConfiguration.h"

#include "TGeant3.h"
#include "TGeant3TGeo.h"
#include "TGeant4.h"

#include "TROOT.h"
#include "TString.h"

//_____________________________________________________________________________
FairYamlSimConfig::FairYamlSimConfig()
    : FairSimulationConfig()
{
}
//_____________________________________________________________________________
FairYamlSimConfig::~FairYamlSimConfig()
{
}

//_____________________________________________________________________________
void FairYamlSimConfig::Setup(const char* mcEngine)
{
    if ( ! ( (strcmp(mcEngine,"TGeant4")==0) || (strcmp(mcEngine,"TGeant3")==0) ) ) {
        LOG(fatal) << "FairYamlSimConfig::Setup() Engine \"" << mcEngine << "\" unknown!";
    }

    TString yamlFileName = ObtainYamlFileName(mcEngine);
    fYamlConfig = YAML::LoadFile(yamlFileName.Data());

    if(strcmp(mcEngine,"TGeant4") == 0 ) {
        SetupGeant4();
    } else if(strcmp(mcEngine,"TGeant3") == 0 ) {
        SetupGeant3();
    }
    SetupStack();
    SetCuts();

    StoreYamlInfo();
}

//_____________________________________________________________________________
void FairYamlSimConfig::SetupGeant3() {
    LOG(info) << "FairYamlSimConfig::SetupGeant3() called";
    FairRunSim *fRun = FairRunSim::Instance();
    TString* gModel = fRun->GetGeoModel();
    TGeant3* geant3 = NULL;
    if ( strncmp(gModel->Data(),"TGeo",4) == 0 ) {
        geant3
            = new  TGeant3TGeo("C++ Interface to Geant3");
    }else{
        geant3
            = new  TGeant3("C++ Interface to Geant3");
    }

    if (fYamlConfig["G3_ABAN"])
        geant3->SetABAN(fYamlConfig["G3_ABAN"].as<int>());
    if (fYamlConfig["G3_ANNI"])
        geant3->SetANNI(fYamlConfig["G3_ANNI"].as<int>());
    if (fYamlConfig["G3_AUTO"])
        geant3->SetAUTO(fYamlConfig["G3_AUTO"].as<int>());
    if (fYamlConfig["G3_BOMB"])
        geant3->SetBOMB(fYamlConfig["G3_BOMB"].as<float>());
    if (fYamlConfig["G3_BREM"])
        geant3->SetBREM(fYamlConfig["G3_BREM"].as<int>());
    if (fYamlConfig["G3_CKOV"])
        geant3->SetCKOV(fYamlConfig["G3_CKOV"].as<int>());
    if (fYamlConfig["G3_COMP"])
        geant3->SetCOMP(fYamlConfig["G3_COMP"].as<int>());
    if (fYamlConfig["G3_CUTS"]) {
        if ( !fYamlConfig["G3_CUTS"].IsSequence() )
            LOG(fatal) << "FairYamlSimConfig::SetupGeant3() G3_CUTS needs 11 or 16 values.";
        std::vector<float> floatVect = fYamlConfig["G3_CUTS"].as<std::vector<float>>();
        if ( floatVect.size() != 11 && floatVect.size() != 16 )
            LOG(fatal) << "FairYamlSimConfig::SetupGeant3() G3_CUTS need 11 or 16 float values.";
        geant3->SetCUTS(floatVect[0], floatVect[1], floatVect[2],
                        floatVect[3], floatVect[4], floatVect[5],
                        floatVect[6], floatVect[7],
                        floatVect[8], floatVect[9], floatVect[10],
                        (floatVect.size()==11?NULL:&floatVect[11]));
    }
    if (fYamlConfig["G3_ECut"])
        geant3->SetECut(fYamlConfig["G3_ECut"].as<float>());
    if (fYamlConfig["G3_DCAY"])
        geant3->SetDCAY(fYamlConfig["G3_DCAY"].as<int>());
    if (fYamlConfig["G3_DEBU"]) {
        if ( !fYamlConfig["G3_DEBU"].IsSequence() ) {
            geant3->SetDEBU(fYamlConfig["G3_DEBU"].as<int>());
        }
        else {
            std::vector<int> defVect = {1,999,1};
            std::vector<int> intVect = fYamlConfig["G3_DEBU"].as<std::vector<int>>();
            for ( int ipos = intVect.size() ; ipos < defVect.size() ; ipos++ ) { // fill up default values in case needed
                intVect.push_back(defVect[ipos]);
            }
            geant3->SetDEBU(intVect[0], intVect[1], intVect[2]);
        }
    }
    if (fYamlConfig["G3_DRAY"])
        geant3->SetDRAY(fYamlConfig["G3_DRAY"].as<int>());
    if (fYamlConfig["G3_ERAN"]) {
        if ( !fYamlConfig["G3_ERAN"].IsSequence() ) {
            geant3->SetDEBU(fYamlConfig["G3_ERAN"].as<float>());
        }
        else {
            std::vector<float> defVect   = {1.e-5,1.e4,90.};
            std::vector<float> floatVect = fYamlConfig["G3_ERAN"].as<std::vector<float>>();
            for ( int ipos = floatVect.size() ; ipos < defVect.size() ; ipos++ ) { // fill up default values in case needed
                floatVect.push_back(defVect[ipos]);
            }
            geant3->SetERAN(floatVect[0], floatVect[1], (int)(floatVect[2]));
        }
    }
    if (fYamlConfig["G3_HADR"])
        geant3->SetHADR(fYamlConfig["G3_HADR"].as<int>());
    if (fYamlConfig["G3_KINE"]) {
        if ( !fYamlConfig["G3_KINE"].IsSequence() ) {
            geant3->SetKINE(fYamlConfig["G3_KINE"].as<int>());
        }
        else {
            std::vector<float> defVect(10,0.0);  // 10 default values
            std::vector<float> floatVect = fYamlConfig["G3_KINE"].as<std::vector<float>>(); // up to 11 values
            for ( int ipos = floatVect.size()-1 ; ipos < defVect.size() ; ipos++ ) { // first one has no default
                floatVect.push_back(defVect[ipos]);
            }
            geant3->SetKINE((int)(floatVect[0]), floatVect[1], floatVect[2],
                            floatVect[3], floatVect[4],
                            floatVect[5], floatVect[6], floatVect[7],
                            floatVect[8], floatVect[9], floatVect[10]);
        }
    }
    if (fYamlConfig["G3_LOSS"])
        geant3->SetLOSS(fYamlConfig["G3_LOSS"].as<int>());
    if (fYamlConfig["G3_MULS"])
        geant3->SetMULS(fYamlConfig["G3_MULS"].as<int>());
    if (fYamlConfig["G3_MUNU"])
        geant3->SetMUNU(fYamlConfig["G3_MUNU"].as<int>());
    if (fYamlConfig["G3_OPTI"])
        geant3->SetOPTI(fYamlConfig["G3_OPTI"].as<int>());
    if (fYamlConfig["G3_PAIR"])
        geant3->SetPAIR(fYamlConfig["G3_PAIR"].as<int>());
    if (fYamlConfig["G3_PFIS"])
        geant3->SetPFIS(fYamlConfig["G3_PFIS"].as<int>());
    if (fYamlConfig["G3_PHOT"])
        geant3->SetPHOT(fYamlConfig["G3_PHOT"].as<int>());
    if (fYamlConfig["G3_RAYL"])
        geant3->SetRAYL(fYamlConfig["G3_RAYL"].as<int>());
    if (fYamlConfig["G3_STRA"])
        geant3->SetSTRA(fYamlConfig["G3_STRA"].as<int>());
    if (fYamlConfig["G3_SWIT"]) {
        if ( !fYamlConfig["G3_SWIT"].IsSequence() )
            geant3->SetSWIT(fYamlConfig["G3_SWIT"].as<int>());
        else {
            std::vector<int> intVect = fYamlConfig["G3_SWIT"].as<std::vector<int>>();
            geant3->SetSWIT(intVect[0], intVect[1]);
        }
    }
    if (fYamlConfig["G3_TRIG"])
        geant3->SetTRIG(fYamlConfig["G3_TRIG"].as<int>());
}

//_____________________________________________________________________________
void FairYamlSimConfig::SetupGeant4() {
    if (!fYamlConfig["Geant4_UserGeometry"]) {
        LOG(fatal) << "User geometry not provided";
    }
    if (!fYamlConfig["Geant4_PhysicsList"]) {
        LOG(fatal) << "Physics list not provided";
    }
    if (!fYamlConfig["Geant4_SpecialProcess"]) {
        LOG(fatal) << "Special processy not provided";
    }
    bool specialStacking = false;
    if (fYamlConfig["Geant4_SpecialStacking"]) {
        //        LOG(info) << "Special stacking used";
        specialStacking = fYamlConfig["Geant4_SpecialStacking"].as<bool>();
    }
    bool mtMode = FairRunSim::Instance()->IsMT();
    
    FairFastSimRunConfiguration* runConfiguration
        = new FairFastSimRunConfiguration(fYamlConfig["Geant4_UserGeometry"]  .as<std::string>(),
                                          fYamlConfig["Geant4_PhysicsList"]   .as<std::string>(),
                                          fYamlConfig["Geant4_SpecialProcess"].as<std::string>(),
                                          specialStacking,
                                          mtMode);

    TGeant4* geant4 = new TGeant4("TGeant4", "The Geant4 Monte Carlo", runConfiguration);
}

//_____________________________________________________________________________
void FairYamlSimConfig::SetCuts() {
    LOG(info) << "FairYamlSimConfig::SetCuts() called";
    if ( fYamlConfig["MonteCarlo_Process"] ) {
        YAML::Node mcProcess = fYamlConfig["MonteCarlo_Process"];
        TVirtualMC *MC =TVirtualMC::GetMC();
        for (auto it = mcProcess.begin(); it != mcProcess.end(); ++it) {
            //            LOG(info) << "proc: " << it->first << " --> " << it->second;
            MC->SetProcess(it->first.as<std::string>().c_str(),it->second.as<int>());
        }
    }

    if ( fYamlConfig["MonteCarlo_Cut"] ) {
        YAML::Node mcProcess = fYamlConfig["MonteCarlo_Cut"];
        TVirtualMC *MC =TVirtualMC::GetMC();
        for (auto it = mcProcess.begin(); it != mcProcess.end(); ++it) {
            //            LOG(info) << "cut: " << it->first << " --> " << it->second;
            MC->SetCut(it->first.as<std::string>().c_str(),it->second.as<double>());
        }
    }
}

//_____________________________________________________________________________
TString FairYamlSimConfig::ObtainYamlFileName(const char* mcEngine) {

    TString lUserConfig = FairRunSim::Instance()->GetUserConfig();

    TString work = getenv("VMCWORKDIR");
    TString work_config=work+"/gconfig/";
    work_config.ReplaceAll("//","/");

    TString config_dir= getenv("CONFIG_DIR");
    config_dir.ReplaceAll("//","/");

    Bool_t AbsPath=kFALSE;

    TString configFileWithPath;

    TString configFile;
    if(lUserConfig.IsNull()) {
        if(strcmp(mcEngine,"TGeant4") == 0 ) {
            configFile = "g4Config.yaml";
        }
        if(strcmp(mcEngine,"TGeant3") == 0 ) {
            configFile = "g3Config.yaml";
        }
        lUserConfig = configFile;
    } else {
        if (lUserConfig.Contains("/")) { AbsPath=kTRUE; }
        configFile = lUserConfig;
        LOG(info) << "---------------User config is used: "
                  << configFile.Data();
    }
    if (!AbsPath && TString(gSystem->FindFile(config_dir.Data(),configFile)) != TString("")) {
        LOG(info) << "---------------CONFIG_DIR is used: "
                  << config_dir.Data() << " to get \"" << configFile.Data() << "\"";
        configFileWithPath=configFile;
    } else {
        if(AbsPath) { configFileWithPath = lUserConfig; }
        else { configFileWithPath =work_config+lUserConfig; }
    }
    return configFileWithPath;
}

//_____________________________________________________________________________
void FairYamlSimConfig::StoreYamlInfo() {
    std::ostringstream nodestring;
    nodestring << "TGeant3\n";
    nodestring << fYamlConfig;
    nodestring << "\n";
    TObjString* configObject = new TObjString(nodestring.str().c_str());
    FairRunSim::Instance()->GetSink()->WriteObject(configObject,"SimulationSetup", TObject::kSingleKey);

    LOG(info) << "FairYamlSimConfig::StoreYamlInfo() done.";
}

ClassImp(FairYamlSimConfig)
