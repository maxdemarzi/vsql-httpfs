# VillageSQL HTTPFS Extension

HTTP file system extension for VillageSQL - read/write remote files over HTTP/HTTPS and S3.

This extension allows VillageSQL to interact with remote files and object storage services through SQL functions.

## Features

- 🌐 **HTTP/HTTPS Support**: Read files from any HTTP(S) URL
- ☁️ **S3 Compatible**: Support for Amazon S3 and S3-compatible storage
- 🔒 **SSL/TLS**: Secure connections with certificate verification
- ⚙️ **Configurable**: Timeouts, retries, and connection pooling
- 🧪 **Well-Tested**: Comprehensive test suite included

## Prerequisites

- VillageSQL build directory (with completed build)
- CMake 3.16 or higher
- C++ compiler with C++17 support
- OpenSSL development libraries
- libcurl development libraries

## Building the Extension

### 1. Install Dependencies

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get install libcurl4-openssl-dev libssl-dev cmake build-essential
```

**macOS:**
```bash
brew install curl openssl cmake
```

### 2. Build

```bash
mkdir build
cd build
cmake .. -DVillageSQL_BUILD_DIR=$HOME/build/villagesql
make -j $(($(getconf _NPROCESSORS_ONLN) - 2))
```

**macOS with Homebrew OpenSSL:**
```bash
cmake .. -DVillageSQL_BUILD_DIR=~/build/villagesql -DWITH_SSL=/opt/homebrew/opt/openssl@3
make -j $(($(sysctl -n hw.ncpu) - 2))
```

### 3. Install (Optional)

```bash
make install
```

This creates the `vsql_httpfs.veb` package in the build directory.

## Using the Extension

After building, load the extension in VillageSQL:

```sql
INSTALL EXTENSION vsql_httpfs;
```

### Available Functions

#### `read_file(url STRING) -> STRING`
Read the content of a remote file via HTTP(S) or S3.

```sql
SELECT read_file('https://example.com/data.txt');
SELECT read_file('s3://mybucket/myfile.txt');
```

#### `get_file_size(url STRING) -> STRING`
Get the size of a remote file (returns size in bytes as string).

```sql
SELECT get_file_size('https://example.com/largefile.bin');
```

#### `http_get(url STRING) -> STRING`
Perform an HTTP GET request and return the response body.

```sql
SELECT http_get('https://api.example.com/endpoint');
```

#### `list_files(path STRING) -> STRING`
List objects in an S3 bucket (path format: `s3://bucket/prefix/`).

```sql
SELECT list_files('s3://mybucket/data/');
```

## Configuration

HTTP and S3 connection parameters can be configured via environment variables or VillageSQL settings:

```bash
# HTTP configuration
export HTTP_TIMEOUT=30          # seconds
export HTTP_RETRIES=3
export HTTP_VERIFY_SSL=true

# S3 configuration
export AWS_ACCESS_KEY_ID=xxx
export AWS_SECRET_ACCESS_KEY=yyy
export AWS_SESSION_TOKEN=zzz    # optional
export AWS_DEFAULT_REGION=us-east-1
```

## Testing

```bash
cd $HOME/build/villagesql/mysql-test
perl mysql-test-run.pl --suite=/path/to/vsql-httpfs/mysql-test
```

### Running Specific Tests

```bash
# Test HTTP functionality
perl mysql-test-run.pl --suite=/path/to/vsql-httpfs/mysql-test --testcase=http_read

# Test S3 functionality
perl mysql-test-run.pl --suite=/path/to/vsql-httpfs/mysql-test --testcase=s3_read
```

## Troubleshooting

### Build Failures

**CURL not found:**
```bash
# Linux
sudo apt-get install libcurl4-openssl-dev

# macOS
brew install curl
```

**OpenSSL not found:**
```bash
# macOS with Homebrew
brew install openssl@3
cmake .. -DVillageSQL_BUILD_DIR=~/build/villagesql -DWITH_SSL=/opt/homebrew/opt/openssl@3
```

### Connection Issues

**SSL Certificate Errors:**
Disable SSL verification (development only):
```sql
SET httpfs_verify_ssl = false;
```

**Connection Timeouts:**
Increase timeout settings:
```sql
SET httpfs_timeout = 60;  -- seconds
```

## Architecture

### Main Components

- **httpfs.cc**: Extension entry point and SQL function implementations
- **http_client.cc**: libcurl-based HTTP/HTTPS client
- **s3_client.cc**: AWS S3 API implementation
- **httpfs.h**: Type definitions and class declarations

### Connection Pooling

The extension maintains connection pools for both HTTP and S3 clients to improve performance for repeated operations.

### Error Handling

All functions return proper error messages on failure:

```sql
SELECT read_file('https://invalid.example.com/notfound');
-- Error: HTTP Error: 404 Not Found
```

## Development

### Adding New Functions

1. Add function declaration to `src/include/httpfs.h`
2. Implement the function in `src/httpfs.cc`
3. Register it in `VEF_GENERATE_ENTRY_POINTS()` macro
4. Add tests to `mysql-test/t/`

Example:

```cpp
void my_new_function_impl(vef_context_t* ctx, vef_invalue_t* arg, vef_vdf_result_t* result) {
    // Implementation
}

// Register in VEF_GENERATE_ENTRY_POINTS:
.func(make_func<&my_new_function_impl>("my_function")
  .returns(STRING)
  .param(STRING)
  .buffer_size(1024)
  .build())
```

## Performance Tips

1. **Connection Pooling**: Reuse connections within a session
2. **Batch Requests**: Use connection caching for multiple file reads
3. **Local Caching**: Cache frequently accessed remote files locally
4. **Compression**: Enable gzip compression for large responses

## License

GPL-2.0 - See LICENSE file for details

## Contributing

Contributions are welcome! Please ensure:

- Code follows C++17 standards
- Tests are added for new functionality
- Commit messages are clear and descriptive
- GPL-2.0 license header is included in source files

## Resources

- [VillageSQL Documentation](https://villagesql.com/docs)
- [CURL Documentation](https://curl.se/libcurl/)
- [AWS S3 API Reference](https://docs.aws.amazon.com/s3/)
- [OpenSSL Documentation](https://www.openssl.org/docs/)

## Support

For issues and questions:
- GitHub Issues: https://github.com/maxdemarzi/vsql-httpfs/issues
- Discussions: https://github.com/maxdemarzi/vsql-httpfs/discussions
