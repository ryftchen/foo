include_guard()

if(PRECOMPILED_HEADER)
    add_compile_options(-D__PRECOMPILED_HEADER)
endif()
if(RUNTIME_PRINTING)
    add_compile_options(-D__RUNTIME_PRINTING)
endif()
