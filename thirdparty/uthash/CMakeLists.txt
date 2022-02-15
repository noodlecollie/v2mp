add_library(${TARGETNAME_UTHASH} INTERFACE
	include/uthash/uthash.h
)

target_include_directories(${TARGETNAME_UTHASH} INTERFACE include)
