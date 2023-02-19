macro(set_version prefix major minor patch)
	set(${prefix}_VERSION_MAJOR ${major})
	set(${prefix}_VERSION_MINOR ${minor})
	set(${prefix}_VERSION_PATCH ${patch})
endmacro()

set_version(LIBV2MP 1 0 0)
set_version(LIBV2MPASM 1 0 0)
set_version(LIBV2MPLINK 1 0 0)
set_version(V2MPASM 1 0 0)
set_version(V2MPLINK 1 0 0)
