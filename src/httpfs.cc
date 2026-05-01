/* Copyright (c) 2026 VillageSQL Contributors
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <villagesql/extension.h>
#include "httpfs.h"

#include <cstring>
#include <sstream>

using namespace villagesql::extension_builder;
using namespace villagesql::func_builder;
using namespace httpfs;

static HTTPClient* g_http_client = nullptr;
static S3Client* g_s3_client = nullptr;

// read_file(url) - Read remote file content
void read_file_impl(vef_context_t* ctx, vef_invalue_t* url_arg, vef_vdf_result_t* result) {
    if (!url_arg || url_arg->is_null) {
        result->type = VEF_RESULT_NULL;
        return;
    }
    
    try {
        if (!g_http_client) {
            g_http_client = new HTTPClient();
        }
        
        std::string url(url_arg->str_value, url_arg->str_len);
        std::string content = g_http_client->get(url);
        
        if (content.size() > 4096) {
            // Buffer too small for large files
            snprintf(result->error_msg, 256, "File too large (>4096 bytes)");
            result->type = VEF_RESULT_ERROR;
            return;
        }
        
        strncpy(result->str_buf, content.c_str(), content.size());
        result->type = VEF_RESULT_VALUE;
        result->actual_len = content.size();
    } catch (const std::exception& e) {
        snprintf(result->error_msg, 256, "HTTP Error: %s", e.what());
        result->type = VEF_RESULT_ERROR;
    }
}

// list_files(path) - List remote directory contents
void list_files_impl(vef_context_t* ctx, vef_invalue_t* path_arg, vef_vdf_result_t* result) {
    if (!path_arg || path_arg->is_null) {
        result->type = VEF_RESULT_NULL;
        return;
    }
    
    try {
        if (!g_s3_client) {
            g_s3_client = new S3Client();
        }
        
        std::string path(path_arg->str_value, path_arg->str_len);
        // Simplified: parse s3://bucket/prefix format
        std::string listing = g_s3_client->list_objects("bucket", path);
        
        if (listing.size() > 4096) {
            snprintf(result->error_msg, 256, "Listing too large");
            result->type = VEF_RESULT_ERROR;
            return;
        }
        
        strncpy(result->str_buf, listing.c_str(), listing.size());
        result->type = VEF_RESULT_VALUE;
        result->actual_len = listing.size();
    } catch (const std::exception& e) {
        snprintf(result->error_msg, 256, "S3 Error: %s", e.what());
        result->type = VEF_RESULT_ERROR;
    }
}

// get_file_size(url) - Get file metadata
void get_file_size_impl(vef_context_t* ctx, vef_invalue_t* url_arg, vef_vdf_result_t* result) {
    if (!url_arg || url_arg->is_null) {
        result->type = VEF_RESULT_NULL;
        return;
    }
    
    try {
        if (!g_http_client) {
            g_http_client = new HTTPClient();
        }
        
        std::string url(url_arg->str_value, url_arg->str_len);
        int64_t size = g_http_client->get_file_size(url);
        
        // Return size as string
        std::ostringstream oss;
        oss << size;
        std::string size_str = oss.str();
        
        strncpy(result->str_buf, size_str.c_str(), size_str.size());
        result->type = VEF_RESULT_VALUE;
        result->actual_len = size_str.size();
    } catch (const std::exception& e) {
        snprintf(result->error_msg, 256, "HTTP Error: %s", e.what());
        result->type = VEF_RESULT_ERROR;
    }
}

// http_get(url) - Perform HTTP GET request
void http_get_impl(vef_context_t* ctx, vef_invalue_t* url_arg, vef_vdf_result_t* result) {
    if (!url_arg || url_arg->is_null) {
        result->type = VEF_RESULT_NULL;
        return;
    }
    
    try {
        if (!g_http_client) {
            g_http_client = new HTTPClient();
        }
        
        std::string url(url_arg->str_value, url_arg->str_len);
        std::string content = g_http_client->get(url);
        
        if (content.size() > 4096) {
            snprintf(result->error_msg, 256, "Response too large");
            result->type = VEF_RESULT_ERROR;
            return;
        }
        
        strncpy(result->str_buf, content.c_str(), content.size());
        result->type = VEF_RESULT_VALUE;
        result->actual_len = content.size();
    } catch (const std::exception& e) {
        snprintf(result->error_msg, 256, "HTTP Error: %s", e.what());
        result->type = VEF_RESULT_ERROR;
    }
}

// Extension registration
VEF_GENERATE_ENTRY_POINTS(
  make_extension("vsql_httpfs", "1.0.0")
    .func(make_func<&read_file_impl>("read_file")
      .returns(STRING)
      .param(STRING)
      .buffer_size(4096)
      .build())
    .func(make_func<&list_files_impl>("list_files")
      .returns(STRING)
      .param(STRING)
      .buffer_size(4096)
      .build())
    .func(make_func<&get_file_size_impl>("get_file_size")
      .returns(STRING)
      .param(STRING)
      .buffer_size(64)
      .build())
    .func(make_func<&http_get_impl>("http_get")
      .returns(STRING)
      .param(STRING)
      .buffer_size(4096)
      .build())
)
