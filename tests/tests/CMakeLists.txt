add_executable(V2MP_Tests
	src/Helpers/MinimalVirtualMachine.cpp
	src/AddInstruction.cpp
	src/AssignmentInstruction.cpp
	src/BitShiftInstruction.cpp
	src/BitwiseANDInstruction.cpp
	src/BitwiseNOTInstruction.cpp
	src/BitwiseORInstruction.cpp
	src/BitwiseXORInstruction.cpp
	src/ConditionalBranchInstruction.cpp
	src/HaltInstruction.cpp
	src/LoadStoreInstruction.cpp
	src/Main.cpp
	src/SubtractInstruction.cpp
)

target_link_libraries(V2MP_Tests PUBLIC Catch2 V2MP)
target_include_directories(V2MP_Tests PRIVATE src)

install(TARGETS V2MP_Tests
	ARCHIVE DESTINATION V2MP_Tests/lib
	LIBRARY DESTINATION V2MP_Tests/lib
	RUNTIME DESTINATION V2MP_Tests/bin
)
