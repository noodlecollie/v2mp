function(generate_export_header_template exportTargetName outputFile)
	string(TOUPPER "${exportTargetName}" uppercaseTargetName)
	configure_file("${CMAKE_SOURCE_DIR}/cmake/export_header_template.in" "${outputFile}")
endfunction()
