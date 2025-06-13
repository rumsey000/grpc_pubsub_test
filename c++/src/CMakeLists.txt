find_library(GRPC_PROTO_LIB grpc_proto_lib)
if (NOT GRPC_PROTO_LIB)
    message(FATAL_ERROR "Not found GRPC_PROTO Library.")
endif()

add_executable(PowerNodeTestPub "PowerNodeTestPub.cpp")
target_link_libraries(PowerNodeTestPub
   ${_CORE_LIBRARIES} ${GRPC_PROTO_LIB})
add_executable(PowerNodeTestSub "PowerNodeTestSub.cpp")
target_link_libraries(PowerNodeTestSub
   ${_CORE_LIBRARIES} ${GRPC_PROTO_LIB})

add_executable(MotorNodeTestPub "MotorNodeTestPub.cpp")
target_link_libraries(MotorNodeTestPub
   ${_CORE_LIBRARIES} ${GRPC_PROTO_LIB})
add_executable(MotorNodeTestSub "MotorNodeTestSub.cpp")
target_link_libraries(MotorNodeTestSub
   ${_CORE_LIBRARIES} ${GRPC_PROTO_LIB})

add_executable(NodeTestServiceServer "NodeTestServiceServer.cpp")
target_link_libraries(NodeTestServiceServer
  ${_CORE_LIBRARIES}
  hello_proto)
add_executable(NodeTestServiceClient "NodeTestServiceClient.cpp")
target_link_libraries(NodeTestServiceClient
  ${_CORE_LIBRARIES}
  hello_proto)