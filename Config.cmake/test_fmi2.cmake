#    Copyright (C) 2012 Modelon AB

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the BSD style license.

# #    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    FMILIB_License.txt file for more details.

#    You should have received a copy of the FMILIB_License.txt file
#    along with this program. If not, contact Modelon AB <http://www.modelon.com>.

include_directories(${RTTESTDIR}/FMI2)


#Defines for the test FMUs
set(FMU2_DUMMY_ME_MODEL_IDENTIFIER BouncingBall2) #This must be the same as in the xml-file
set(FMU2_DUMMY_CS_MODEL_IDENTIFIER BouncingBall2) #This must be the same as in the xml-file

set(FMU2_DUMMY_FOLDER ${RTTESTDIR}/FMI2/fmu_dummy)
to_native_c_path(${TEST_OUTPUT_FOLDER}/tempfolder/FMI2 FMU2_TEMPFOLDER)

set(FMU2_DUMMY_ME_SOURCE
  ${FMU2_DUMMY_FOLDER}/fmu2_model_me.c
)
set(FMU2_DUMMY_CS_SOURCE
  ${FMU2_DUMMY_FOLDER}/fmu2_model_cs.c
)
set(FMU2_DUMMY_HEADERS
  ${FMU2_DUMMY_FOLDER}/fmu2_model.h
  ${FMU2_DUMMY_FOLDER}/fmu2_model_defines.h
)

add_library(fmu2_dll_me SHARED ${FMU2_DUMMY_ME_SOURCE} ${FMU2_DUMMY_HEADERS})
add_library(fmu2_dll_cs SHARED ${FMU2_DUMMY_CS_SOURCE} ${FMU2_DUMMY_HEADERS})

set(XML_ME_PATH ${FMU2_DUMMY_FOLDER}/modelDescription_me.xml)
set(XML_CS_PATH ${FMU2_DUMMY_FOLDER}/modelDescription_cs.xml)
set(XML_CS_TC_PATH ${FMU2_DUMMY_FOLDER}/modelDescription_cs_tc.xml)

set(SHARED_LIBRARY_ME_PATH ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}fmu2_dll_me${CMAKE_SHARED_LIBRARY_SUFFIX})
set(SHARED_LIBRARY_CS_PATH ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}fmu2_dll_cs${CMAKE_SHARED_LIBRARY_SUFFIX})

#Create FMU 2.0 ME/CS Model and generate library path

to_native_c_path("\"${SHARED_LIBRARY_ME_PATH}\"" DLL_OUTPUT_PATH_ME_DEFINE)
to_native_c_path("\"${SHARED_LIBRARY_CS_PATH}\"" DLL_OUTPUT_PATH_CS_DEFINE)

to_native_c_path("\"${CMAKE_CURRENT_BINARY_DIR}/\" CMAKE_INTDIR \"/${CMAKE_SHARED_LIBRARY_PREFIX}fmu2_dll_me${CMAKE_SHARED_LIBRARY_SUFFIX}\""
				fmu2_DLL_ME_PATH)
to_native_c_path("\"${CMAKE_CURRENT_BINARY_DIR}/\" CMAKE_INTDIR \"/${CMAKE_SHARED_LIBRARY_PREFIX}fmu2_dll_cs${CMAKE_SHARED_LIBRARY_SUFFIX}\""
				fmu2_DLL_CS_PATH)

#function(compress_fmu OUTPUT_FOLDER MODEL_IDENTIFIER FILE_NAME_CS_ME_EXT TARGET_NAME XML_PATH SHARED_LIBRARY_PATH)
compress_fmu("${TEST_OUTPUT_FOLDER}" "${FMU2_DUMMY_ME_MODEL_IDENTIFIER}" "me" "fmu2_dll_me" "${XML_ME_PATH}" "${SHARED_LIBRARY_ME_PATH}")
compress_fmu("${TEST_OUTPUT_FOLDER}" "${FMU2_DUMMY_CS_MODEL_IDENTIFIER}" "cs" "fmu2_dll_cs" "${XML_CS_PATH}" "${SHARED_LIBRARY_CS_PATH}")

add_executable (fmi2_import_xml_test ${RTTESTDIR}/FMI2/fmi2_import_xml_test.cc )
target_link_libraries (fmi2_import_xml_test  ${FMILIBFORTEST}  )

add_test(ctest_fmi2_import_xml_test_me fmi2_import_xml_test ${TEST_OUTPUT_FOLDER}/${FMU2_DUMMY_ME_MODEL_IDENTIFIER}_me)
add_test(ctest_fmi2_import_xml_test_cs fmi2_import_xml_test ${TEST_OUTPUT_FOLDER}/${FMU2_DUMMY_ME_MODEL_IDENTIFIER}_cs)

if(FMILIB_BUILD_BEFORE_TESTS)
	SET_TESTS_PROPERTIES ( 
		ctest_fmi2_import_xml_test_me
		ctest_fmi2_import_xml_test_cs
		PROPERTIES DEPENDS ctest_build_all)
endif()
