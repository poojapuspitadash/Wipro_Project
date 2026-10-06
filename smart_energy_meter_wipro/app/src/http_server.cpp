#include "http_server.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <thread>

static std::string urlDecode(std::string s) {
    std::string out;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '%' && i + 2 < s.size()) {
            char hex[3] = {s[i+1], s[i+2], 0};
            char* end = nullptr;
            long v = std::strtol(hex, &end, 16);
            if (end && *end == 0) { out.push_back(static_cast<char>(v)); i += 2; continue; }
        }
        if (s[i] == '+') out.push_back(' ');
        else out.push_back(s[i]);
    }
    return out;
}

static std::string httpResponse(const std::string& body,
                                const std::string& type = "text/html",
                                const std::string& disposition = "") {
    std::ostringstream out;
    out << "HTTP/1.1 200 OK\r\n"
        << "Content-Type: " << type << "; charset=utf-8\r\n"
        << "Cache-Control: no-store\r\n"
        << (disposition.empty() ? "" : "Content-Disposition: " + disposition + "\r\n")
        << "Content-Length: " << body.size() << "\r\n"
        << "Connection: close\r\n\r\n" << body;
    return out.str();
}

static std::string errorResponse(const std::string& msg) {
    return "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n"
           "Content-Length: " + std::to_string(msg.size()) +
           "\r\nConnection: close\r\n\r\n" + msg;
}

HttpServer::HttpServer(int port, StatusProvider status, ResetHandler reset,
                       PulseHandler pulse, ExportHandler export_csv)
    : port_(port), status_(std::move(status)), reset_(std::move(reset)),
      pulse_(std::move(pulse)), export_csv_(std::move(export_csv)) {}

void HttpServer::run(std::atomic<bool>& running) {
    int server = ::socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) return;

    int yes = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(static_cast<uint16_t>(port_));

    if (::bind(server, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        ::close(server);
        return;
    }
    if (::listen(server, 16) < 0) {
        ::close(server);
        return;
    }

    timeval tv{1, 0};
    setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    while (running.load()) {
        int client = ::accept(server, nullptr, nullptr);
        if (client < 0) continue;

        char buf[8192]{};
        ssize_t n = ::recv(client, buf, sizeof(buf)-1, 0);
        if (n <= 0) { ::close(client); continue; }

        std::istringstream req(std::string(buf, n));
        std::string method, target, version;
        req >> method >> target >> version;

        std::string response;
        if (method != "GET") {
            response = errorResponse("Only GET is supported");
        } else if (target == "/" || target == "/index.html") {
            std::ifstream f("web/index.html");
            std::stringstream ss;
            ss << f.rdbuf();
            response = httpResponse(ss.str());
        } else if (target == "/style.css") {
            std::ifstream f("web/style.css");
            std::stringstream ss; ss << f.rdbuf();
            response = httpResponse(ss.str(), "text/css");
        } else if (target == "/app.js") {
            std::ifstream f("web/app.js");
            std::stringstream ss; ss << f.rdbuf();
            response = httpResponse(ss.str(), "application/javascript");
        } else if (target == "/api/status") {
            response = httpResponse(status_(), "application/json");
        } else if (target == "/api/export") {
            response = httpResponse(export_csv_(), "text/csv", "attachment; filename=smart_energy_meter.csv");
        } else if (target == "/api/reset") {
            response = httpResponse(reset_() ? "{\"ok\":true}" : "{\"ok\":false}");
        } else if (target.rfind("/api/pulse", 0) == 0) {
            auto q = target.find("?n=");
            unsigned long count = 0;
            if (q != std::string::npos) {
                try { count = std::stoul(urlDecode(target.substr(q + 3))); } catch (...) {}
            }
            if (count == 0 || count > 1000000) response = errorResponse("n must be 1..1000000");
            else response = httpResponse(pulse_(count) ? "{\"ok\":true}" : "{\"ok\":false}");
        } else {
            response = "HTTP/1.1 404 Not Found\r\nContent-Length: 9\r\nConnection: close\r\n\r\nNot Found";
        }

        ::send(client, response.data(), response.size(), 0);
        ::close(client);
    }

    ::close(server);
}
