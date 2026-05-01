/* Copyright (c) 2026 VillageSQL Contributors */

#include "httpfs.h"
#include <curl/curl.h>
#include <stdexcept>

namespace httpfs {

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t realsize = size * nmemb;
    userp->append((char*)contents, realsize);
    return realsize;
}

HTTPClient::HTTPClient(const HTTPConfig& config) : config_(config) {
    curl_handle_ = curl_easy_init();
    if (!curl_handle_) {
        throw std::runtime_error("Failed to initialize CURL");
    }
}

HTTPClient::~HTTPClient() {
    if (curl_handle_) {
        curl_easy_cleanup((CURL*)curl_handle_);
    }
}

std::string HTTPClient::get(const std::string& url) {
    CURL* curl = (CURL*)curl_handle_;
    std::string readBuffer;
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, (long)config_.timeout_seconds);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&readBuffer);
    
    if (!config_.verify_ssl) {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    }
    
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(res));
    }
    
    return readBuffer;
}

std::string HTTPClient::head(const std::string& url) {
    CURL* curl = (CURL*)curl_handle_;
    std::string readBuffer;
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, (long)config_.timeout_seconds);
    
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(res));
    }
    
    curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
    return readBuffer;
}

std::string HTTPClient::post(const std::string& url, const std::string& data) {
    CURL* curl = (CURL*)curl_handle_;
    std::string readBuffer;
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, (long)config_.timeout_seconds);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&readBuffer);
    
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(res));
    }
    
    return readBuffer;
}

int64_t HTTPClient::get_file_size(const std::string& url) {
    CURL* curl = (CURL*)curl_handle_;
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, (long)config_.timeout_seconds);
    
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(res));
    }
    
    double filesize = 0.0;
    curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &filesize);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
    
    return (int64_t)filesize;
}

bool HTTPClient::file_exists(const std::string& url) {
    try {
        get_file_size(url);
        return true;
    } catch (...) {
        return false;
    }
}

} // namespace httpfs
