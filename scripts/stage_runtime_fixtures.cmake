if(NOT DEFINED DEST_DIR)
    message(FATAL_ERROR "DEST_DIR is required")
endif()

if(DEFINED SOURCE_SIDBASE AND EXISTS "${SOURCE_SIDBASE}")
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${SOURCE_SIDBASE}" "${DEST_DIR}/sidbase.bin"
        RESULT_VARIABLE sidbase_copy_result
    )
    message("Copying Sidbase from ${SOURCE_SIDBASE} into ${DEST_DIR}/sidbase.bin")
    if(NOT sidbase_copy_result EQUAL 0)
        message(FATAL_ERROR "Failed to copy ${SOURCE_SIDBASE} to ${DEST_DIR}/sidbase.bin")
    endif()
else()
    message(WARNING "Optional sidbase fixture not found: ${SOURCE_SIDBASE}")
endif()

if(DEFINED SOURCE_VAR_MAPS AND EXISTS "${SOURCE_VAR_MAPS}")
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${SOURCE_VAR_MAPS}" "${DEST_DIR}/var_maps"
        RESULT_VARIABLE var_maps_copy_result
    )
    if(NOT var_maps_copy_result EQUAL 0)
        message(FATAL_ERROR "Failed to copy ${SOURCE_VAR_MAPS} to ${DEST_DIR}/var_maps")
    endif()
else()
    message(WARNING "Optional var_maps fixture directory not found: ${SOURCE_VAR_MAPS}")
endif()
