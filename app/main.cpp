#include <iostream>
#include "route_info.h"
#include "cache_manager.h"
#include <string>
#include <vector>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <windows.h>
#include <iomanip>
#include <sstream>
#include <ctime>


int main() {
    SetConsoleOutputCP(65001);  // Устанавливаем кодировку UTF-8

    std::cout << "Введите название файла для кеширования: ";
    std::string cache_filename;
    std::getline(std::cin, cache_filename);
    std::cout<<"Перечань городов: Санкт-Петербург(c2), Волгоград(c38)"<<std::endl;
    std::cout<<"Введите код города отправления:"<<std::endl;
    std::string from;
    std::getline(std::cin, from);
    std::cout<<"Введите код города прибытия:"<<std::endl;
    std::string to;
    std::getline(std::cin, to);

    // Ввод даты отправления
    std::cout << "Введите дату отправления (YYYY-MM-DD): ";
    std::string departure_date;
    std::getline(std::cin, departure_date);

    // API-ключ Яндекса
    std::string api_key = "708d9c46-3aa2-47c4-8019-6bd24f7a2f2e";  

    // Формируем URL запроса с параметром transfers=true
    std::string url = "https://api.rasp.yandex.net/v3.0/search/?apikey=" + api_key +
                      "&from=" + from + "&to=" + to +
                      "&date=" + departure_date +  
                      "&format=json&transport_types=plane,bus,train&transfers=true"; 

    // Отправляем GET-запрос
    cpr::Response response = cpr::Get(cpr::Url{url});

    // Проверка статуса ответа
    if (response.status_code == 200) {
        try {
            auto json_response = nlohmann::json::parse(response.text);

            bool found_routes = false;
            if (json_response.contains("segments") && !json_response["segments"].empty()) {
                found_routes = true;

                for (const auto& segment : json_response["segments"]) {
                    process_segment(segment);
                }
            }

            if (!found_routes) {
                std::cout << "\nМаршруты для указанной даты не найдены." << std::endl;
            }

        } catch (const std::exception& e) {
            std::cerr << "Ошибка при разборе JSON: " << e.what() << std::endl;
        }
    } else {
        std::cerr << "Ошибка запроса. Код состояния: " << response.status_code << std::endl;
        std::cerr << "Ответ сервера: " << response.text << std::endl;
    }


    // Выполняем запрос с кешированием
    std::string response_text = make_request_and_cache(url, cache_filename);

    if (!response_text.empty()) {
        std::cout << "Ответ: " << response_text << std::endl;
    }


    return 0;
}
