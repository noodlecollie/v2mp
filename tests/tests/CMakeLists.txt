add_executable(V2MP_Tests
	src/Components/CircularBuffer.cpp

	src/Helpers/BaseMockDevice.cpp
	src/Helpers/EmitterMockDevice.cpp
	src/Helpers/SinkMockDevice.cpp
	src/Helpers/TestHarnessVM.cpp

	src/Instructions/AddInstruction.cpp
	src/Instructions/AssignmentInstruction.cpp
	src/Instructions/BitShiftInstruction.cpp
	src/Instructions/BitwiseANDInstruction.cpp
	src/Instructions/BitwiseNOTInstruction.cpp
	src/Instructions/BitwiseORInstruction.cpp
	src/Instructions/BitwiseXORInstruction.cpp
	src/Instructions/ConditionalBranchInstruction.cpp
	src/Instructions/DevicePortOperationInstruction.cpp
	src/Instructions/DevicePortQueryInstruction.cpp
	src/Instructions/DevicePortReadInstruction.cpp
	src/Instructions/DevicePortWriteInstruction.cpp
	src/Instructions/DivideInstruction.cpp
	src/Instructions/HaltInstruction.cpp
	src/Instructions/LoadStoreInstruction.cpp
	src/Instructions/MultiplyInstruction.cpp
	src/Instructions/StackOperationInstruction.cpp
	src/Instructions/SubtractInstruction.cpp

	src/Modules/DevicePorts.cpp

	src/Main.cpp
)

target_link_libraries(V2MP_Tests PRIVATE Catch2::Catch2 ${TARGETNAME_V2MP} ${TARGETNAME_V2MP_INTERNAL})
target_include_directories(V2MP_Tests PRIVATE src)

include(CTest)
include(Catch)
catch_discover_tests(V2MP_Tests)
