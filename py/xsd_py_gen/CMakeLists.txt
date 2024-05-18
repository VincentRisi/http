project (gends)

function (pathed result ext_dir)
  file(MAKE_DIRECTORY ${ext_dir})
  set ("${result}" ${ext_dir} PARENT_SCOPE)
endfunction ()

pathed (xsdDir ${CMAKE_CURRENT_BINARY_DIR}/py)

set (xsd_list
  example|GPSPayment|GPS_PAYMENT
  example|pacs.008.001.08|PACS_008
  example|pain.001.001.09|PAIN_001
  messageorch/Common|file_message|FILE_MESSAGE
  aeoi|SARSForeignTaxpayerInfoDeclarationV2.3.1|SARS_FOREIGNTAXPAYERINFO_DECLARATION_V231
  aeoi|SARSForeignTaxpayerInfoDeclarationV2.3|SARS_FOREIGNTAXPAYERINFO_DECLARATION_V23
  aeoi|SARSForeignTaxpayerInfoSupportingDataSubmissionV1.1|SARS_FOREIGNTAXPAYERINFO_SUPPORTINGDATASUBMISSION_V11
  aeoi|SARSForeignTaxpayerInfoSupportingDataV1.1|SARS_FOREIGNTAXPAYERINFO_SUPPORTINGDATA
  aeoi|SARSRequestForeignTaxpayerInfoDeclarationV2.3|SARS_REQUEST_FOREIGNTAXPAYERINFO_DECLARATION_V23
  aeoi|SARSRequestForeignTaxpayerInfoDeclarationV2.3.1|SARS_REQUEST_FOREIGNTAXPAYERINFO_DECLARATION_V231
  aeoi|SARSSubmitForeignTaxpayerInfoDeclarationV2.3|SARS_SUBMIT_FOREIGNTAXPAYERINFO_DECLARATION_V23
  aeoi|SARSSubmitForeignTaxpayerInfoDeclarationV2.3.1|SARS_SUBMIT_FOREIGNTAXPAYERINFO_DECLARATION_V231
  aeoi|SARSThirdPartyCorrespondenceV1.10|SARS_THIRDPARTYCORRESPONDENCE_V110
  aeoi|SARSThirdPartySubmissionHeaderV1.11|SARS_THIRDPARTYSUBMISSION_HEADER_V111
  soapy|Accept_And_Reject|Accept_And_Reject
  soapy|CDV|CDV
  soapy|Documentum|Documentum
  soapy|SecurityAdminWebService|SecurityAdminWebService
  soapy|SecurityPrincipleWebService|SecurityPrincipleWebService
  soapy|SubmitWSDL|SubmitWSDL
  soapy|TrackingService|TrackingService
  )

if (WIN32)
  set (PYTHON_EXECUTABLE C:/Python37/python.exe)
  set (PYTHON_SCRIPTS ${CMAKE_CURRENT_SOURCE_DIR})
  set (inFiles)
  set (outFiles)
  set (logFiles)
  list (APPEND inFiles 
    C:/sarsUtil/pytools/xsd_py_gen/xsds/aeoi/SARSGMD_BaseTypesV54.19.xsd
    C:/sarsUtil/pytools/xsd_py_gen/xsds/aeoi/SARSGMD_FormTypesV1.5.xsd
    )
  foreach (xsd ${xsd_list})
    string (REPLACE "|" ";" xsd ${xsd})
    list (LENGTH xsd count)
    if (3 EQUAL count)
      list (GET xsd o dir_name)
      list (GET xsd 1 base_name)
      list (GET xsd 2 make_name)
      pathed (outPath ${CMAKE_CURRENT_BINARY_DIR}/${dir_name}/py)
      pathed (logPath ${CMAKE_CURRENT_BINARY_DIR}/${dir_name}/log)
      set (inFile  ${CMAKE_CURRENT_SOURCE_DIR}/xsds/${dir_name}/${base_name}.xsd)
      set (inPath  ${CMAKE_CURRENT_SOURCE_DIR}/xsds/${dir_name})
      set (outFile ${outPath}/XSD_${make_name}.py)
      set (logFile ${logPath}/${base_name}.log)
      list (APPEND inFiles  ${inFile})
      list (APPEND outFiles ${outFile})
      list (APPEND logFiles ${logFile})
      message (STATUS "--(info)-- ${PYTHON_EXECUTABLE} ${PYTHON_SCRIPTS}/xsd_py_gen.py -i ${inFile} -I ${inPath} -o ${outFile} -l ${logPath}")
      add_custom_command (
        OUTPUT  ${outFile}
        COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_SCRIPTS}/xsd_py_gen.py -i ${inFile} -I ${inPath} -o ${outFile} -l ${logPath}
        DEPENDS ${inFile}
        BYPRODUCTS ${logFile}
      )
    endif ()    
  endforeach ()
  add_custom_target (putty3_xsd_build ALL
    DEPENDS ${outFiles}
    SOURCES ${inFiles} ${outFiles} ${logFiles}
  )
  set_source_files_properties (${logFiles} PROPERTIES GENERATED TRUE)
  #folder(putty3_xsd_build)
endif()
