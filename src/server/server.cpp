#include "server.h"
#include <iostream>
#include <fstream>
#include "../utils/logger.h"

// Windows mkdir
#ifdef _WIN32
#include <direct.h>
#define MKDIR(dir) _mkdir(dir)
#else
#include <sys/stat.h>
#define MKDIR(dir) mkdir(dir, 0755)
#endif

namespace carbongrid {

DashboardServer::DashboardServer(int http_port, int ws_port)
    : http_port_(http_port), ws_port_(ws_port), running_(false) {}

DashboardServer::~DashboardServer() {
    stop();
}

void DashboardServer::start() {
    if (running_) return;
    running_ = true;
    // Create data directory for metrics file
    MKDIR("dashboard");
    MKDIR("dashboard/data");
    Logger::get_instance().log(LogLevel::INFO,
        "DashboardServer started (file-based). Metrics → dashboard/data/metrics.json");
}

void DashboardServer::stop() {
    running_ = false;
}

bool DashboardServer::is_running() const {
    return running_;
}

void DashboardServer::broadcast(const std::string& json_data) {
    // Stub — no WebSocket in this build
}

void DashboardServer::push_metrics(const std::string& metrics_json) {
    write_metrics_file(metrics_json);
}

void DashboardServer::push_node_status(const std::string& node_json) {
    broadcast(node_json);
}

void DashboardServer::push_scheduling_decision(const std::string& decision_json) {
    broadcast(decision_json);
}

void DashboardServer::push_carbon_update(const std::string& carbon_json) {
    broadcast(carbon_json);
}

void DashboardServer::write_metrics_file(const std::string& json_data, const std::string& filepath) {
    std::ofstream out(filepath);
    if (out.is_open()) {
        out << json_data;
        out.close();
    } else {
        Logger::get_instance().log(LogLevel::ERROR, "Failed to write metrics file: " + filepath);
    }
}

void DashboardServer::set_command_callback(CommandCallback cb) {
    command_callback_ = std::move(cb);
}

} // namespace carbongrid
