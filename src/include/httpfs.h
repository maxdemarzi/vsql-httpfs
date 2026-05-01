/* Copyright (c) 2026 VillageSQL Contributors
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#pragma once

#include <string>
#include <memory>
#include <map>

namespace httpfs {

struct HTTPConfig {
    int timeout_seconds = 30;
    int max_retries = 3;
    int retry_wait_ms = 100;
    float retry_backoff = 4.0f;
    bool verify_ssl = true;
    std::string ca_cert_file;
};

struct S3Config {
    std::string region = "us-east-1";
    std::string access_key_id;
    std::string secret_access_key;
    std::string session_token;
    std::string endpoint;
    bool use_ssl = true;
    bool url_compatibility_mode = false;
};

class HTTPClient {
public:
    HTTPClient(const HTTPConfig& config = HTTPConfig());
    ~HTTPClient();
    
    std::string get(const std::string& url);
    std::string head(const std::string& url);
    std::string post(const std::string& url, const std::string& data);
    
    int64_t get_file_size(const std::string& url);
    bool file_exists(const std::string& url);

private:
    HTTPConfig config_;
    void* curl_handle_;
};

class S3Client {
public:
    S3Client(const S3Config& config = S3Config());
    ~S3Client();
    
    std::string get_object(const std::string& bucket, const std::string& key);
    std::string list_objects(const std::string& bucket, const std::string& prefix);
    bool put_object(const std::string& bucket, const std::string& key, const std::string& data);
    
private:
    S3Config config_;
    std::string sign_request(const std::string& method, const std::string& bucket, 
                            const std::string& key, const std::string& amz_date);
};

} // namespace httpfs
