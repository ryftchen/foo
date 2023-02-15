include_guard()

if(RUNTIME_PRINTING)
    add_compile_options(-D__RUNTIME_PRINTING)
endif()
if(PRECOMPILED_HEADER)
    add_compile_options(-D__PRE_COMPILE)
endif()
