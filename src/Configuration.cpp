//
// Created by aarons on 08.03.23.
//

#include "Configuration.h"

#include <stdexcept>

void Configuration::populate(const ConfigParser& parser) {
    set_string_if_valid(parser, this->hapd_sock, "hapd_sock");
    set_string_if_valid(parser, this->log_path, "log_path");
    set_uint32_if_valid(parser, this->cleanup_interval, "cleanup_interval");
    set_uint32_if_valid(parser, this->max_vni, "max_vni");
}

void Configuration::set_string_if_valid(const ConfigParser& parser, std::string& config_target,
                                        const std::string& key) {
    try {
        config_target = parser.get_config_string(key);
    } catch (const std::out_of_range& err) {
    }
}

void Configuration::set_uint32_if_valid(const ConfigParser& parser, uint32_t& config_target, const std::string& key) {
    try {
        config_target = parser.get_config_uint32(key);
    } catch (const std::out_of_range& err) {
    }
}