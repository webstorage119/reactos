
spec2def(ksuser.dll ksuser.spec ADD_IMPORTLIB)

add_library(ksuser SHARED ksuser.c ksuser.rc ${CMAKE_CURRENT_BINARY_DIR}/ksuser.def)

set_module_type(ksuser win32dll)

add_importlibs(ksuser advapi32 msvcrt kernel32 ntdll)
add_dependencies(ksuser psdk bugcodes)
add_cd_file(TARGET ksuser DESTINATION reactos/system32 FOR all)

