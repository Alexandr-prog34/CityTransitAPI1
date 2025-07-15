#ifndef ROUTE_INFO_H
#define ROUTE_INFO_H

#include <nlohmann/json.hpp>

void get_route_times(const nlohmann::json& segment);
void get_stations(const nlohmann::json& segment);
void get_transport(const nlohmann::json& segment);
void check_transfers(const nlohmann::json& segment);
void get_transfers(const nlohmann::json& segment);
void get_transfer_stations(const nlohmann::json& segment);
void get_details(const nlohmann::json& segment);
void process_segment(const nlohmann::json& segment);

#endif // ROUTE_INFO_H