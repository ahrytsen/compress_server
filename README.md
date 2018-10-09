# compress_server

## Target platform:
  - Ubuntu 18.04.1 LTS
  - g++ (Ubuntu 7.3.0-27ubuntu1~18.04) 7.3.0
  - GNU Make 3.81
  
## Description:
  - Implemented in C++11 using boost library.
  - Server works synchronus.
  - Each client services in separate thread. 
  - Server uses regular expresions for compression algorithm.
  - Implementer defined response codes:
    - Request specific:
      - Empty Payload 33;
      - Unsupported Payload 34;
    - Server Errors(not request specific):
      - Bad Allocation  35;
      - System Error  36;

## Libraries:
  - boost (::asio , ::thread, ::regex).
  
## Assumptions:
  In case when it's not compress request and we have non-zero payload or payload too big,
  I decide just to reject that payload.
  
