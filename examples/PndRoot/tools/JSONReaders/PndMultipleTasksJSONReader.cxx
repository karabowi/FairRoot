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

/*
 * PndMultipleTasksJSONReader.cxx
 *
 *  Created on: 20.09.2022
 *      Author: tstockmanns
 */

#include "PndMultipleTasksJSONReader.h"
#include "PndTaskJSONReader.h"

#include "FairLogger.h"

void PndMultipleTasksJSONReader::AssignTasks(FairRun *run, pt::ptree &tree)
{
  for (pt::ptree::value_type &property : tree.get_child("tasks")) {
    LOG(info) << property.first.data() << " : " << property.second.data();
    pt::ptree &tasks = property.second;
    for (pt::ptree::value_type &task : tasks) {
      LOG(info) << task.first.data() << " : " << task.second.data();
      std::unique_ptr<PndTaskJSONReader> taskReader(PndTaskJSONReader::createTaskReader(task.first.data()));
      if (taskReader == nullptr) {
        LOG(warning) << "Task not recognized: " << task.first.data();
      } else {
        LOG(info) << "Task recognized: " << task.first.data();
        taskReader->AddTask(run, task.second);
      }
    }
  }
}

ClassImp(PndMultipleTasksJSONReader);
