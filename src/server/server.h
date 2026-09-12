#pragma once

#include <string>
#include <functional>
#include <vector>
#include "../utils/json_writer.h"

namespace carbongrid {

/**
 * @brief Dashboard server stub.
 * Writes metrics to a JSON file that the dashboard polls.
 * No actual HTTP/WebSocket server (MinGW 6.3 lacks thread support).
 */
class DashboardServer {
public:
    using CommandCallback = std::function<void(const std::string&, const std::string&)>;

    DashboardServer(int http_port = 8080, int ws_port = 9002);
    ~DashboardServer();

    void start();
    void stop();
    bool is_running() const;

    void broadcast(const std::string& json_data);
    void push_metrics(const std::string& metrics_json);
    void push_node_status(const std::string& node_json);
    void push_scheduling_decision(const std::string& decision_json);
    void push_carbon_update(const std::string& carbon_json);

    void write_metrics_file(const std::string& json_data,
                            const std::string& filepath = "dashboard/data/metrics.json");

    void set_command_callback(CommandCallback cb);

private:
    int http_port_;
    int ws_port_;
    bool running_ = false;
    CommandCallback command_callback_;
};

} // namespace carbongrid
