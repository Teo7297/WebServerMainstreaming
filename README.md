# WebServerMainstreaming

A C++ web server with file caching, configurable settings, and logging capabilities. Developed for a job interview at *MainStreaming*

## Features

- HTTP and HTTPS support
- File caching with configurable TTL (Time To Live)
- Static file serving from a specified content directory
- Automatic request logging
- Configuration via INI file

## Requirements

- C++20 compatible compiler
- CMake for building (version 3.20+)
- Dependencies:
  - [cpp-httplib](https://github.com/yhirose/cpp-httplib) (HTTP/HTTPS server implementation)
  - [cpptime](https://github.com/clarifysky/cpptime) (Timer functionality)
  - [INIReader](https://github.com/benhoyt/inih) (INI file parser)
  - [OpenSSL](https://github.com/openssl/openssl) (SSL secure connection)

## Installation

```bash
git clone https://github.com/yourusername/WebServerMainstreaming.git
cd WebServerMainstreaming
mkdir build && cd build
cmake ..
make
```

## Configuration

Create a configuration file (e.g., `server_config.ini`) with the following settings:

```ini
Port=8080
ContentDir=/path/to/content
CacheTTL=3600
Certificate=cert.pem
CertificateKey=key.pem
LogPath=logs/server.log
```

### Configuration Options

| Option         | Description                                  | Default           |
| -------------- | -------------------------------------------- | ----------------- |
| Port           | The port on which the server will listen     | 8080              |
| ContentDir     | Directory containing files to serve          | ./content         |
| CacheTTL       | Cache time-to-live in seconds                | 3600              |
| Certificate    | Path to SSL certificate for HTTPS (optional) | -                 |
| CertificateKey | Path to SSL key for HTTPS (optional)         | -                 |
| LogPath        | Path to save log files                       | ./logs/server.log |

## Usage

```cpp
#include "Server.h"

int main(int argc, char* argv[])
{
    fs::path configPath(argv[1]);

    MC::Server server(configPath);

    if(server.IsConfigLoaded())
        server.CreateServer();

    return 0;
}
```

## Core Classes

### Server

The main server class that handles HTTP/HTTPS requests, serves files, and manages the server lifecycle.

### Cache

Implements a file caching system with TTL functionality to improve performance by reducing disk I/O.

### Logger

Provides thread-safe logging capabilities using a producer-consumer pattern with a dedicated logging thread.


## License

[MIT License](LICENSE)
