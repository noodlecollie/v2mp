add_executable(V2MP_Tests
	src/Components/CircularBuffer.cpp

	src/Helpers/TestHarnessVM.cpp

	src/Instructions/AddInstruction.cpp
	src/Instructions/AssignmentInstruction.cpp
	src/Instructions/BitShiftInstruction.cpp
	src/Instructions/BitwiseANDInstruction.cpp
	src/Instructions/BitwiseNOTInstruction.cpp
	src/Instructions/BitwiseORInstruction.cpp
	src/Instructions/BitwiseXORInstruction.cpp
	src/Instructions/ConditionalBranchInstruction.cpp
	src/Instructions/DevicePortReadInstruction.cpp
	src/Instructions/HaltInstruction.cpp
	src/Instructions/LoadStoreInstruction.cpp
	src/Instructions/SubtractInstruction.cpp

	src/Modules/DevicePorts.cpp

	src/Main.cpp
)

target_link_libraries(V2MP_Tests PUBLIC Catch2 ${TARGETNAME_V2MP} ${TARGETNAME_V2MP_INTERNAL})
target_include_directories(V2MP_Tests PRIVATE src)

install(TARGETS V2MP_Tests
	ARCHIVE DESTINATION V2MP_Tests/lib
	LIBRARY DESTINATION V2MP_Tests/lib
	RUNTIME DESTINATION V2MP_Tests/bin
)
