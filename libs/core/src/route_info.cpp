#include "route_info.h"
#include <iostream>
#include <string>
#include <vector>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <windows.h>
#include <iomanip>
#include <sstream>
#include <ctime>


// Функция для форматирования времени
std::string format_time(const std::string& time_str) {
    try {
        std::tm tm = {};
        std::istringstream ss(time_str.substr(0, 19)); // Убираем временную зону
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");

        if (ss.fail()) {
            throw std::invalid_argument("Invalid time format");
        }

        std::ostringstream formatted_time;
        formatted_time << std::put_time(&tm, "%d-%m-%Y %H:%M:%S");
        return formatted_time.str();
    } catch (const std::exception& e) {
        std::cerr << "Ошибка форматирования времени: " << e.what() << std::endl;
        return time_str;  // Если не удалось, возвращаем исходную строку
    }
}

// Функция для получения информации о времени маршрута
void get_route_times(const nlohmann::json& segment) {
    std::string departure_time = segment.value("departure", "N/A");
    std::string arrival_time = segment.value("arrival", "N/A");
    departure_time = format_time(departure_time);
    arrival_time = format_time(arrival_time);

    std::cout << "Отправление: " << departure_time << " Прибытие: " << arrival_time << std::endl;
}


// Функция для получения информации о станциях
void get_stations(const nlohmann::json& segment) {
    std::string from_station = segment.contains("from") && segment["from"].contains("title") && !segment["from"]["title"].is_null()
                                ? segment["from"]["title"].get<std::string>() : "N/A";
    std::string to_station = segment.contains("to") && segment["to"].contains("title") && !segment["to"]["title"].is_null()
                              ? segment["to"]["title"].get<std::string>() : "N/A";
    // Перебираем термины и выводим их
    std::string departure_terminal = "N/A";
    std::string arrival_terminal = "N/A";

    // Проверяем, есть ли термины для этой детали
    if (segment.contains("departure_terminal") && !segment["departure_terminal"].is_null()) {
        departure_terminal = segment["departure_terminal"].get<std::string>();
    }

    if (segment.contains("arrival_terminal") && !segment["arrival_terminal"].is_null()) {
        arrival_terminal = segment["arrival_terminal"].get<std::string>();
    }

    // Выводим информацию о терминалах, если она есть
    if (departure_terminal != "N/A" && arrival_terminal != "N/A") {
        std::cout << "Отправление с терминала: " << departure_terminal << " Прибытие в терминал: " << arrival_terminal << std::endl;
    }
    if (from_station != "N/A" && to_station != "N/A") {
        std::cout << "Из: " << from_station << " В: " << to_station << std::endl;
    }
}



// Функция для получения информации о транспорте
void get_transport(const nlohmann::json& segment) {
    std::string transport_type = "N/A";
    std::string vehicle = "N/A";

    if (segment.contains("thread") && segment["thread"].is_object()) {
        transport_type = segment["thread"].contains("transport_type") && !segment["thread"]["transport_type"].is_null()
                        ? segment["thread"]["transport_type"].get<std::string>()
                        : "N/A";
        vehicle = segment["thread"].contains("vehicle") && !segment["thread"]["vehicle"].is_null()
                  ? segment["thread"]["vehicle"].get<std::string>()
                  : "N/A";
    }

    if (transport_type != "N/A" && vehicle != "N/A") {
        std::cout << "Тип транспорта: " << transport_type << " (" << vehicle << ")" << std::endl;
    }
}

// Функция для проверки пересадок
void check_transfers(const nlohmann::json& segment) {
    if (segment.value("has_transfers", false)) {
        std::cout << "Этот маршрут с пересадками.\n";
    } else {
        std::cout << "Это прямой маршрут.\n";
    }
}

// Функция для получения информации о пересадках
void get_transfers(const nlohmann::json& segment) {
    if (segment.contains("transfers") && segment["transfers"].is_array()) {
        std::cout << "Точки пересадок:\n";
        for (const auto& transfer : segment["transfers"]) {
            std::string transfer_city = transfer.contains("title") && !transfer["title"].is_null()
                                        ? transfer["title"] : "N/A";
            std::cout << "- " << transfer_city << std::endl;
        }
    }
}



// Функция для получения станций маршрута
void get_transfer_stations(const nlohmann::json& segment) {
    // Извлекаем станцию отправления
    std::string departure_station = "N/A";
    if (segment.contains("departure_from") && segment["departure_from"].contains("title") && !segment["departure_from"]["title"].is_null()) {
        departure_station = segment["departure_from"]["title"].get<std::string>();
    }

    // Извлекаем станцию прибытия
    std::string arrival_station = "N/A";
    if (segment.contains("arrival_to") && segment["arrival_to"].contains("title") && !segment["arrival_to"]["title"].is_null()) {
        arrival_station = segment["arrival_to"]["title"].get<std::string>();
    }

    // Станции пересадок
    std::string transfer_from_station = "N/A";
    std::string transfer_to_station = "N/A";

    // Проверяем, есть ли пересадки и извлекаем нужные данные
    if (segment.contains("details") && segment["details"].is_array()) {
        for (const auto& detail : segment["details"]) {
            // Извлекаем информацию о станциях пересадок
            if (detail.contains("transfer_from") && detail["transfer_from"].contains("title") && !detail["transfer_from"]["title"].is_null()) {
                transfer_from_station = detail["transfer_from"]["title"].get<std::string>();
            }
            if (detail.contains("transfer_to") && detail["transfer_to"].contains("title") && !detail["transfer_to"]["title"].is_null()) {
                transfer_to_station = detail["transfer_to"]["title"].get<std::string>();
            }
        }
    }
    if (segment.contains("details") && segment["details"].is_array()) {
        for (const auto& detail : segment["details"]) {

            if (detail.contains("transfer_to") && detail["transfer_to"].contains("title") && !detail["transfer_to"]["title"].is_null()) {
                transfer_to_station = detail["transfer_to"]["title"].get<std::string>();
            }
        }
    }


    // Выводим все станции
    std::cout << "Станции маршрута:\n";
    std::cout << "  Отправление: " << departure_station << std::endl;
    if (transfer_from_station==transfer_to_station){
        std::cout << "  Отправление с этой же станции: " << transfer_from_station << std::endl;
    }
    else{
        std::cout << "  Пересадка (отправление): " << transfer_from_station << std::endl;
        std::cout << "  Пересадка (прибытие): " << transfer_to_station << std::endl;
    }
    std::cout << "  Прибытие: " << arrival_station << std::endl;
}




// Функция для обработки деталей пересадок
void get_details(const nlohmann::json& segment) {
    // Проверяем, есть ли details и они не пусты
    if (segment.contains("details") && segment["details"].is_array()) {
        std::cout << "Детали пересадок:\n";

        // Перебираем все детали
        for (const auto& detail : segment["details"]) {
            std::string leg_departure = detail.value("departure", "N/A");
            std::string leg_arrival = detail.value("arrival", "N/A");

            std::string leg_from = detail.contains("from") && detail["from"].contains("title") && !detail["from"]["title"].is_null()
                                    ? detail["from"]["title"] : "N/A";
            std::string leg_to = detail.contains("to") && detail["to"].contains("title") && !detail["to"]["title"].is_null()
                                  ? detail["to"]["title"] : "N/A";

            std::string leg_transport = detail.contains("thread") && detail["thread"].contains("transport_type") && !detail["thread"]["transport_type"].is_null()
                                        ? detail["thread"]["transport_type"] : "N/A";
            std::string leg_vehicle = detail.contains("thread") && detail["thread"].contains("vehicle") && !detail["thread"]["vehicle"].is_null()
                                      ? detail["thread"]["vehicle"] : "N/A";

            if (leg_from != "N/A" && leg_to != "N/A" && leg_transport != "N/A") {
                std::cout << "- Транспорт: " << leg_transport << " (" << leg_vehicle << ")" << std::endl;
                std::cout << "  Из: " << leg_from << " В: " << leg_to << std::endl;
                std::cout << "  Отправление: " << format_time(leg_departure) << " Прибытие: " << format_time(leg_arrival) << std::endl;
            }

            // Перебираем термины и выводим их
            std::string departure_terminal = "N/A";
            std::string arrival_terminal = "N/A";

            // Проверяем, есть ли термины для этой детали
            if (detail.contains("departure_terminal") && !detail["departure_terminal"].is_null()) {
                departure_terminal = detail["departure_terminal"].get<std::string>();
            }

            if (detail.contains("arrival_terminal") && !detail["arrival_terminal"].is_null()) {
                arrival_terminal = detail["arrival_terminal"].get<std::string>();
            }

            // Выводим информацию о терминалах, если она есть
            if (departure_terminal != "N/A" && arrival_terminal != "N/A") {
                std::cout << "Отправление с терминала: " << departure_terminal << " Прибытие в терминал: " << arrival_terminal << std::endl;
            }
        }

        // Вставляем информацию о станциях пересадок
        get_transfer_stations(segment);  // Вызовем эту функцию один раз после обработки всех деталей
    }
}




// Основная функция обработки сегментов
void process_segment(const nlohmann::json& segment) {
    std::cout << "\nМаршрут:\n";
    
    get_route_times(segment);         // Время отправления и прибытия
    get_stations(segment);            // Станции отправления и прибытия
    get_transport(segment);           // Тип транспорта и транспортное средство
    check_transfers(segment);         // Проверка наличия пересадок
    get_transfers(segment);           // Пересадки
    get_details(segment);             // Детали пересадок
    //get_transfer_stations(segment);
}