// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#ifndef NETWORK_UTILITY_H
#define NETWORK_UTILITY_H

#include <string>
#include <vector>

#define MONITOR_FILE "/tmp/.ip_link_event"

#define MODE_DHCP   "dhcp"
#define MODE_MANUAL "manual"

#define ETHERNET_0  "eth0"
#define ETHERNET_1  "eth1"
#define ETHERNET_0_TITLE "Wired 1"
#define ETHERNET_1_TITLE "Wired 2"

#define PROTOCOL_STRING "protocol"
#define PORT_STRING "port"
#define IS_ALLOWED_STRING "is_allowed"

#define PROTOCOL_TCP "tcp"
#define PROTOCOL_UDP "udp"

#define DHCP_TIMEOUT 30
#define CONNMAN_SERVICE_TIMEOUT 5

using namespace std;

class INetworkUtility {
public:
    virtual ~INetworkUtility() {}
    virtual pair<vector<string>, bool> get_available_networks() = 0;
    virtual pair<vector<string>, bool> get_network_nameservers(const char* ethernet) = 0;
    virtual pair<string, bool> get_network_mode(const char* ethernet, bool isIpv4) = 0;
    virtual pair<string, bool> get_network_mask(const char* ethernet, bool isIpv4) = 0;
    virtual pair<string, bool> get_default_gateway(const char* ethernet, bool isIpv4) = 0;
    virtual pair<string, bool> get_ip_address(const char* ethernet, bool isIpv4) = 0;
    virtual pair<string, bool> get_eth0_mac_address() = 0;
    virtual pair<string, bool> get_eth1_mac_address() = 0;
    virtual pair<string, bool> get_ethernet_mac_address(const char* ethernet) = 0;
    virtual pair<string, bool> get_ethernet_status(const char* ethernet) = 0;
    virtual pair<string, bool> get_ethernet_status_until_timeout(const char* ethernet) = 0;
    virtual pair<string, bool> get_network_interface(const char* network) = 0;
    virtual pair<vector<map<string, string>>, bool> get_firewall_accept_ports() = 0;
    virtual pair<string, bool> get_port_by_protocol_and_service_name(const char* protocol, const char* name) = 0;
    virtual bool is_network_available(string ethernet) = 0;
    virtual bool set_static_ip_address(const char* ethernet, const char* ipv4, const char* ipv6, const char* subnetMask, const char* gateway) = 0;
    virtual bool set_dhcp(const char* ethernet, bool isIpv4) = 0;
    virtual bool set_dns_server(const char* ethernet, const char* dns1, const char* dns2) = 0;
    virtual bool create_offline_provisioning_file(const char* ethernet) = 0;
    virtual bool delete_offline_provisioning_file(const char* ethernet) = 0;
    virtual bool set_static_ip_address_offline(const char* ethernet, const char* ipv4, const char* ipv6, const char* subnetMask, const char* gateway) = 0;
    virtual bool set_dhcp_offline(const char* ethernet, bool isIpv4) = 0;
    virtual bool set_dns_server_offline(const char* ethernet, const char* dns1, const char* dns2) = 0;
    virtual bool set_firewall_accept_ports(vector<map<string, string>> portRules) = 0;
    virtual bool remove_firewall_accept_ports() = 0;
    virtual bool enable_connman_technology_ethernet() = 0;
    virtual bool disable_connman_technology_ethernet() = 0;
    virtual bool restart_connman_service() = 0;
    virtual bool start_ip_monitor() = 0;
    virtual bool stop_ip_monitor() = 0;
    virtual pair<string, bool> get_up_ethernet_from_monitor_file(const char* monitorFile) = 0;

    pair<string, bool> _get_network_info(const char* cmd, const char* network, bool isIpv4);
};

class TPCNetworkUtility: public INetworkUtility {
public:
    pair<vector<string>, bool> get_available_networks() override;
    pair<vector<string>, bool> get_network_nameservers(const char* ethernet) override;
    pair<string, bool> get_network_mode(const char* ethernet, bool isIpv4) override;
    pair<string, bool> get_network_mask(const char* ethernet, bool isIpv4) override;
    pair<string, bool> get_default_gateway(const char* ethernet, bool isIpv4) override;
    pair<string, bool> get_ip_address(const char* ethernet, bool isIpv4) override;
    pair<string, bool> get_eth0_mac_address() override;
    pair<string, bool> get_eth1_mac_address() override;
    pair<string, bool> get_ethernet_mac_address(const char* ethernet) override;
    pair<string, bool> get_ethernet_status(const char* ethernet) override;
    pair<string, bool> get_ethernet_status_until_timeout(const char* ethernet) override;
    pair<string, bool> get_network_interface(const char* network) override;
    pair<vector<map<string, string>>, bool> get_firewall_accept_ports() override;
    pair<string, bool> get_port_by_protocol_and_service_name(const char* protocol, const char* name) override;
    bool is_network_available(string ethernet) override;
    bool set_static_ip_address(const char* ethernet, const char* ipv4, const char* ipv6, const char* subnetMask, const char* gateway) override;
    bool set_dhcp(const char* ethernet, bool isIpv4) override;
    bool set_dns_server(const char* ethernet, const char* dns1, const char* dns2) override;
    bool create_offline_provisioning_file(const char* ethernet) override;
    bool delete_offline_provisioning_file(const char* ethernet) override;
    bool set_static_ip_address_offline(const char* ethernet, const char* ipv4, const char* ipv6, const char* subnetMask, const char* gateway) override;
    bool set_dhcp_offline(const char* ethernet, bool isIpv4) override;
    bool set_dns_server_offline(const char* ethernet, const char* dns1, const char* dns2) override;
    bool set_firewall_accept_ports(vector<map<string, string>> portRules) override;
    bool remove_firewall_accept_ports() override;
    bool enable_connman_technology_ethernet() override;
    bool disable_connman_technology_ethernet() override;
    bool restart_connman_service() override;
    bool start_ip_monitor() override;
    bool stop_ip_monitor() override;
    pair<string, bool> get_up_ethernet_from_monitor_file(const char* monitorFile) override;

private:
    bool _create_tmp_offline_provisioning_file(const char* ethernet);
    bool _set_offline_provisioning_file_value(const char* ethernet, const char* key, const char* value);
    pair<string, bool> _get_eth_status(string eth);
    map<string, string> m_ethNetworkMap;
    map<string, bool> m_ethOnlineMap;
};

#endif // NETWORK_UTILITY_H
