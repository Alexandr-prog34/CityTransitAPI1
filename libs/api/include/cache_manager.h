#ifndef CACHE_MANAGER_H
#define CACHE_MANAGER_H

#include <iostream>
#include <string>
#include <vector>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <windows.h>
#include <iomanip>
#include <sstream>
#include <ctime>

struct CachedRequest {
    std::string request;
    std::string response;
};

std::vector<CachedRequest> read_cache(const std::string& cache_filename);
void write_cache(const std::string& cache_filename, const std::vector<CachedRequest>& cache);
void update_cache(const std::string& cache_filename, const std::string& request, const std::string& response);
std::string make_request_and_cache(const std::string& url, const std::string& cache_filename);

#endif