proto_enum_make_number.exe JWServer/Packet/EditGamaPacket.proto JWServer/Packet/GamaPacket.proto

external\protobuf\protoc\protoc.exe --cpp_out=JWServer\Packet\GamePacket\Cpp --proto_path=JWServer\Packet\ GamaPacket.proto
external\protobuf\protoc\protoc.exe --csharp_out=JWServer\Packet\GamePacket\CSharp --proto_path=JWServer\Packet\ GamaPacket.proto
external\protobuf\protoc\protoc.exe --csharp_out=JWServer\SampleClient\Packet --proto_path=JWServer\Packet\ GamaPacket.proto


pause