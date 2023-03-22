#include "Caller.h"

#include <chrono>
#include <sstream>
#include <stdexcept>
#include <thread>

#include "../Station.h"
#include "../logging/loginit.h"
#include "../Configuration.h"

const std::string VLAN_ID_PREFIX = "vlan_id=";

ipc::Caller::Caller() {}

uint32_t ipc::Caller::vlan_for_station(const Station &station) {
    Socket &socket = get_socket(station.ifname);
    std::istringstream stream(socket.send_and_receive({"STA", station.mac}));
    std::string line;
    while (std::getline(stream, line)) {
        if (line.rfind(VLAN_ID_PREFIX) == 0) {
            return std::stol(line.substr(VLAN_ID_PREFIX.size(), line.size()));
        }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    throw std::runtime_error("no vlan_id attribute found for station");
}

std::vector<Station> ipc::Caller::connected_stations() {
    std::vector<std::string> interface_names = Configuration::get_instance().ifnames;
    std::vector<Station> stations;
    for (auto & ifname : interface_names)
    {
        Socket &socket = get_socket(ifname);
        std::string ipc_result = socket.send_and_receive({"STA-FIRST"});
        while (!ipc_result.empty()) {
            if (ipc_result != "FAIL\n" && ipc_result.length() >= MAC_ADDRESS_LENGTH) {
                stations.emplace_back(ifname, ipc_result.substr(0, MAC_ADDRESS_LENGTH));
            }
            ipc_result = socket.send_and_receive({"STA-NEXT", stations[stations.size() - 1].mac});
        }
    }
    return stations;
}

void ipc::Caller::deauth_station(const Station &station) {
    Socket &socket = get_socket(station.ifname);
    std::string result = socket.send_and_receive({"DEAUTHENTICATE", station.mac});
    if (result != Socket::HOSTAPD_OK) {
        WMLOG(WARNING) << "Did not receive OK on DEAUTH request: " << result;
    }
}
ipc::Socket &ipc::Caller::get_socket(std::string name) {
    if(sockets.find(name) == sockets.end()) {
        sockets.emplace(name, Socket{std::chrono::seconds(1), name});
    }
    return sockets.at(name);
}
