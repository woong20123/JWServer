
if not exist external\protobuf (
    powershell expand-archive -Force external\protobuf.zip external\protobuf
)

pause