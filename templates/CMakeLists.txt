 ################################################################################
 #    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    #
 #                                                                              #
 #              This software is distributed under the terms of the             # 
 #         GNU Lesser General Public Licence version 3 (LGPL) version 3,        #  
 #                  copied verbatim in the file "LICENSE"                       #
 ################################################################################

Set(MaxTestTime 100)

Add_Test(test_project_template
         ${CMAKE_CURRENT_BINARY_DIR}/test_project_template.sh)
Set_Tests_Properties(test_project_template PROPERTIES TIMEOUT ${MaxTestTime})
Set_Tests_Properties(test_project_template PROPERTIES PASS_REGULAR_EXPRESSION "TEST SUCCESS")

Add_Test(test_NewDetector
         ${CMAKE_CURRENT_BINARY_DIR}/test_NewDetector.sh)
Set_Tests_Properties(test_NewDetector PROPERTIES DEPENDS test_project_template)
Set_Tests_Properties(test_NewDetector PROPERTIES TIMEOUT ${MaxTestTime})
Set_Tests_Properties(test_NewDetector PROPERTIES PASS_REGULAR_EXPRESSION "TEST SUCCESS")

configure_file( ${CMAKE_SOURCE_DIR}/templates/project_template/test_project_template.sh.in
		${CMAKE_BINARY_DIR}/templates/test_project_template.sh @ONLY)
configure_file( ${CMAKE_SOURCE_DIR}/templates/NewDetector/test_NewDetector.sh.in
		${CMAKE_BINARY_DIR}/templates/test_NewDetector.sh @ONLY)

