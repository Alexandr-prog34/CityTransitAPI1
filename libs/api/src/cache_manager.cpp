#include "cache_manager.h"
#include <iostream>
#include <string>
#include <vector>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <windows.h>
#include <iomanip>
#include <sstream>
#include <ctime>


// Функция для чтения кеша
std::vector<CachedRequest> read_cache(const std::string& cache_filename) {
    std::vector<CachedRequest> cache;
    std::ifstream cache_file(cache_filename);
    if (cache_file.is_open()) {
        std::string line;
        while (std::getline(cache_file, line)) {
            size_t delimiter_pos = line.find("||");
            if (delimiter_pos != std::string::npos) {
                CachedRequest cached_request;
                cached_request.request = line.substr(0, delimiter_pos);
                cached_request.response = line.substr(delimiter_pos + 2);
                cache.push_back(cached_request);
            }
        }
        cache_file.close();
    }
    return cache;
}

// Функция для записи в кеш
void write_cache(const std::string& cache_filename, const std::vector<CachedRequest>& cache) {
    std::ofstream cache_file(cache_filename);
    if (cache_file.is_open()) {
        for (const auto& entry : cache) {
            cache_file << entry.request << "||" << entry.response << std::endl;
        }
        cache_file.close();
    }
}

// Функция для обновления кеша
void update_cache(const std::string& cache_filename, const std::string& request, const std::string& response) {
    std::vector<CachedRequest> cache = read_cache(cache_filename);

    // Если кеш уже содержит 10 записей, удаляем самую старую
    if (cache.size() >= 10) {
        cache.erase(cache.begin());  // Удаление самого старого запроса
    }

    // Добавляем новый запрос и его ответ в кеш
    cache.push_back({request, response});

    // Записываем обновленный кеш в файл
    write_cache(cache_filename, cache);
}

// Функция для выполнения запроса с кешированием
std::string make_request_and_cache(const std::string& url, const std::string& cache_filename) {
    // Читаем кеш
    std::vector<CachedRequest> cache = read_cache(cache_filename);

    // Проверяем, был ли уже этот запрос в кеше
    for (const auto& cached_request : cache) {
        if (cached_request.request == url) {
            std::cout << "Запрос найден в кеше: " << url << std::endl;
            return cached_request.response;  // Если запрос уже был, возвращаем его из кеша
        }
    }

    // Если запрос не найден в кеше, отправляем новый запрос
    std::cout << "Запрос не найден в кеше, отправляем запрос: " << url << std::endl;
    cpr::Response response = cpr::Get(cpr::Url{url});

    if (response.status_code == 200) {
        // Добавляем новый запрос и его ответ в кеш
        update_cache(cache_filename, url, response.text);
        return response.text;  // Возвращаем текст ответа
    } else {
        std::cerr << "Ошибка запроса, код статуса: " << response.status_code << std::endl;
        return "";
    }
}