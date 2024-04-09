// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#include <cstdio>
#include <cstring>
#include <array>
#include <sstream>
#ifdef _WIN32
#else
#include <unistd.h>
#endif
#include <QDebug>

#include "./include/utility.h"
#include "./include/network_utility.h"

const char* TYPE_IPV4 = "ipv4";
const char* TYPE_IPV6 = "ipv6";

// ex: connmanctl services ethernet_xxx
// IPv4 is read only configuration (for dhcp)
// IPv4.Configuration is read-write configuration (for manual)
// NOTE: IPv4 Address might be different from IPv4.Configuration Address
/*
  Type = ethernet
  Security = [  ]
  State = online
  Favorite = True
  Immutable = False
  AutoConnect = True
  Name = Wired
  Ethernet = [ Method=auto, Interface=eth0, Address=C4:00:AD:97:1B:D9, MTU=1500 ]
  IPv4 = [ Method=dhcp, Address=172.16.12.26, Netmask=255.255.254.0, Gateway=172.16.13.254 ]
  IPv4.Configuration = [ Method=dhcp ]
  IPv6 = [  ]
  IPv6.Configuration = [ Method=auto, Privacy=disabled ]
  Nameservers = [ 172.20.1.199, 172.20.1.99 ]
  Nameservers.Configuration = [  ]
  Timeservers = [  ]
  Timeservers.Configuration = [  ]
  Domains = [ ADVANTECH.CORP ]
  Domains.Configuration = [  ]
  Proxy = [ Method=direct ]
  Proxy.Configuration = [  ]
  mDNS = False
  mDNS.Configuration = False
  Provider = [  ]
*/
const char* LIST_AVAILABLE_NETWORK_CMD =     "connmanctl services | grep Wired | awk -F ' ' '{print $3}'";
const char* GET_NAMESERVERS_CMD =            "connmanctl services %s | grep 'Nameservers =' | tr -d ' []' | awk -F '=' '{print $2}' | tr ',' '\\n'";
const char* GET_NETWORK_INTERFACE_CMD =      "connmanctl services %s | grep 'Ethernet =' | tr -d ' []' | awk -F ',' '{print $2}' | awk -F '=' '{print $2}' | tr -d '\\n'";
const char* GET_NETWORK_MODE_CMD =           "connmanctl services %s | grep -i '%s.Configuration' |  tr -d ' []' | awk -F ',' '{print $1}' | awk -F '=' '{print $3}' | tr -d '\\n'";

const char* GET_NETWORK_MASK_CMD =           "connmanctl services %s | grep -i '%s =' | grep Netmask | tr -d ' []' | awk -F ',' '{print $3}' | awk -F '=' '{print $2}' | tr -d '\\n'";
const char* GET_DEFAULT_GATEWAY_CMD =        "connmanctl services %s | grep -i '%s =' | grep Gateway | tr -d ' []' | awk -F ',' '{print $4}' | awk -F '=' '{print $2}' | tr -d '\\n'";
const char* GET_IP_ADDRESS_CMD =             "connmanctl services %s | grep -i '%s =' | grep Address | tr -d ' []' | awk -F ',' '{print $2}' | awk -F '=' '{print $2}' | tr -d '\\n'";
const char* GET_MANUAL_NETWORK_MASK_CMD =    "connmanctl services %s | grep -i '%s.Configuration =' | grep Netmask | tr -d ' []' | awk -F ',' '{print $3}' | awk -F '=' '{print $2}' | tr -d '\\n'";
const char* GET_MANUAL_DEFAULT_GATEWAY_CMD = "connmanctl services %s | grep -i '%s.Configuration =' | grep Gateway | tr -d ' []' | awk -F ',' '{print $4}' | awk -F '=' '{print $2}' | tr -d '\\n'";
const char* GET_MANUAL_IP_ADDRESS_CMD =      "connmanctl services %s | grep -i '%s.Configuration =' | grep Address | tr -d ' []' | awk -F ',' '{print $2}' | awk -F '=' '{print $2}' | tr -d '\\n'";
// ex: connmanctl config ethernet_xxx --ipv4 manual 192.168.10.2 255.255.255.0 192.168.10.1
const char* SET_IP_ADDRESS_CMD =             "connmanctl config %s --%s manual %s %s %s";
// ex: connmanctl config ethernet_xxx --nameservers 8.8.8.8 4.4.4.4
const char* SET_DNS_SERVER_CMD =             "connmanctl config %s --nameservers %s %s";
const char* SET_DHCP_CMD =                   "connmanctl config %s --%s dhcp";
const char* OPERATE_CONNMAN_ETHERNET_CMD =   "connmanctl %s ethernet > /dev/null 2>&1";
const char *OPERATE_CONNMAN_SERVICE_CMD =    "systemctl %s connman 2>&1";

const char* GET_ETH0_MAC_CMD =               "cat /sys/class/net/eth0/address | tr -d '\\n'";
const char* GET_ETH1_MAC_CMD =               "cat /sys/class/net/eth1/address | tr -d '\\n'";

// connmanctl service provisioning file
// ex:
/*
[service_eth0]
Type = ethernet
# network/netmask/gateway
IPv4 = 192.168.0.90/255.255.255.0/192.168.0.100
MAC  = c4:00:ad:97:1b:0b
# server1,server2
Nameservers = 192.168.0.100
*/
const char* CREATE_FILE_CMD =                      "touch %s";
const char* CONNMAN_PROVISIONING_FILE_FOLDER =     "/var/lib/connman";
const char* CONNMAN_PROVISIONING_FILE_PATTERN =    "/var/lib/connman/%s_default.config";
const char* TMP_PROVISIONING_FILE_PATTERN =        "/userdata/.tmp_%s_default.config";
const char* PROVISIONING_SECTION_PATTERN =         "service_%s";
const char* PROVISIONING_SECTION_TYPE_KEY =        "Type";
const char* PROVISIONING_SECTION_TYPE_ETHERNET =   "ethernet";
const char* PROVISIONING_SECTION_IPV4_KEY =        "IPv4";
const char* PROVISIONING_SECTION_IPV6_KEY =        "IPv6";
const char* PROVISIONING_SECTION_MAC_KEY =         "MAC";
const char* PROVISIONING_SECTION_DNS_KEY =         "Nameservers";
const char* PROVISIONING_SECTION_DEVICE_NAME_KEY = "DeviceName";

// monitor the link state of device
const char* IP_MONITOR_CMD =                 "/sbin/ip monitor link > %s &";
const char* KILL_IP_MONITOR_CMD =            "ps ax | grep 'ip monitor link' | awk -F ' ' '{print $1}' | head -n -1 | xargs -r kill";
const char* GET_UP_ETHERNET_CMD =            "cat %s | tail -n 2 | grep -a 'state UP' | awk -F ': ' '{print $2}' | tr -d '\\n'";

// firewall related
// ex: iptables -L INPUT
/*
Chain INPUT (policy DROP)
target     prot opt source               destination
ACCEPT     all  --  anywhere             anywhere
ACCEPT     all  --  anywhere             anywhere             state RELATED,ESTABLISHED
ACCEPT     tcp  --  anywhere             anywhere             state NEW tcp dpt:ssh // from settings //
ACCEPT     tcp  --  anywhere             anywhere             state NEW tcp dpt:http // from settings //
ACCEPT     tcp  --  anywhere             anywhere             state NEW tcp dpt:https // from settings //
ACCEPT     tcp  --  anywhere             anywhere             state NEW tcp dpt:http-alt // from settings //
ACCEPT     icmp --  anywhere             anywhere             icmp echo-request
*/
const char* LIST_IPTABLE_INPUT_RULES = "/usr/sbin/iptables -L INPUT | grep 'from settings'";
const char* LIST_IPTABLE_INPUT_RULES_LINE_NUMBER = "/usr/sbin/iptables -L INPUT --line-number | grep 'from settings' | awk '{print $1}'";
const char* ADD_ACCEPT_RULE_CMD = "/usr/sbin/iptables -A INPUT -p %s -m state --state NEW -m %s --dport %s -j ACCEPT -m comment --comment 'from settings'";
const char* DELETE_ACCEPT_RULE_BY_LINE_NUMBER_CMD = "/usr/sbin/iptables -D INPUT %s";
const char* SAVE_IPTABLE_RULES = "/usr/sbin/iptables-save > /etc/iptables/iptables.rules";
const char* RULE_ITEM_SPLIT_STRING = " ";
const char* RULE_ITEM_PORT_SPLIT_STRING = "dpt:";
// ex: /etc/services
/*
tcpmux          1/tcp                           # TCP port service multiplexer
echo            7/tcp
echo            7/udp
...
ftp-data        20/tcp
ftp             21/tcp
fsp             21/udp          fspd
ssh             22/tcp                          # SSH Remote Login Protocol
telnet          23/tcp
*/
const char* GET_PORT_BY_PROTOCOL_AND_SERVICE_NAME = "cat /etc/services | grep '/%s[[:blank:]]'| grep '^%s[[:blank:]]' | awk -F ' ' '{print $2}' | awk -F '/' '{print $1}' | tr -d '\\n'";

pair<string, bool> INetworkUtility::_get_network_info(const char* cmd, const char* network, bool isIpv4) {
    bool result = false;
    string value_buff;
    // check input
    if (!cmd || !network) {
        qDebug("missing parameter");
        return make_pair(value_buff, result);
    }
    if (strlen(cmd) == 0 || strlen(network) == 0) {
        qDebug("empty cmd:%s network:%s", cmd, network);
        return make_pair(value_buff, result);
    }

    if (isIpv4)
        return execute_cmd_get_single_info(cmd, network, TYPE_IPV4);
    else
        return execute_cmd_get_single_info(cmd, network, TYPE_IPV6);
}

pair<vector<string>, bool> TPCNetworkUtility::get_available_networks() {
    return execute_cmd_get_vector(LIST_AVAILABLE_NETWORK_CMD);
}

pair<vector<string>, bool> TPCNetworkUtility::get_network_nameservers(const char* ethernet) {
    return execute_cmd_get_vector(GET_NAMESERVERS_CMD, m_ethNetworkMap[ethernet].c_str());
}

pair<string, bool> TPCNetworkUtility::get_network_mode(const char* ethernet, bool isIpv4) {
    return _get_network_info(GET_NETWORK_MODE_CMD, m_ethNetworkMap[ethernet].c_str(), isIpv4);
}

pair<string, bool> TPCNetworkUtility::get_network_mask(const char* ethernet, bool isIpv4) {
    string network = m_ethNetworkMap[ethernet];
    const auto retMode = get_network_mode(ethernet, isIpv4);
    if (retMode.first.compare(MODE_DHCP) == 0) {
        return _get_network_info(GET_NETWORK_MASK_CMD, network.c_str(), isIpv4);
    } else {
        return _get_network_info(GET_MANUAL_NETWORK_MASK_CMD, network.c_str(), isIpv4);
    }
}

pair<string, bool> TPCNetworkUtility::get_default_gateway(const char* ethernet, bool isIpv4) {
    string network = m_ethNetworkMap[ethernet];
    const auto retMode = get_network_mode(ethernet, isIpv4);
    if (retMode.first.compare(MODE_DHCP) == 0) {
        return _get_network_info(GET_DEFAULT_GATEWAY_CMD, network.c_str(), isIpv4);
    } else {
        return _get_network_info(GET_MANUAL_DEFAULT_GATEWAY_CMD, network.c_str(), isIpv4);
    }
}

pair<string, bool> TPCNetworkUtility::get_ip_address(const char* ethernet, bool isIpv4) {
    string network = m_ethNetworkMap[ethernet];
    const auto retMode = get_network_mode(ethernet, isIpv4);
    if (retMode.first.compare(MODE_DHCP) == 0) {
        return _get_network_info(GET_IP_ADDRESS_CMD, network.c_str(), isIpv4);
    } else {
        return _get_network_info(GET_MANUAL_IP_ADDRESS_CMD, network.c_str(), isIpv4);
    }
}

pair<string, bool> TPCNetworkUtility::get_network_interface(const char* network) {
    return execute_cmd_get_single_info(GET_NETWORK_INTERFACE_CMD, network);
}

pair<string, bool> TPCNetworkUtility::get_ethernet_mac_address(const char* ethernet) {
    if (strcmp(ethernet, ETHERNET_0) == 0)
        return execute_cmd_get_single_info(GET_ETH0_MAC_CMD);
    else
        return execute_cmd_get_single_info(GET_ETH1_MAC_CMD);
}
pair<string, bool> TPCNetworkUtility::get_eth0_mac_address() {
    return execute_cmd_get_single_info(GET_ETH0_MAC_CMD);
}

pair<string, bool> TPCNetworkUtility::get_eth1_mac_address() {
    return execute_cmd_get_single_info(GET_ETH1_MAC_CMD);
}

pair<string, bool> TPCNetworkUtility::_get_eth_status(string eth) {
    const auto retAvailableNet = get_available_networks();
    string wiredName;
    bool isOnline = false;
    // get wired status
    for (int i = 0; i < (int)retAvailableNet.first.size(); i++) {
        const auto reteth = get_network_interface(retAvailableNet.first.at(i).c_str());
        if (reteth.first.compare(eth) == 0) {
            wiredName = retAvailableNet.first.at(i);
            isOnline = true;
            break;
        }
    }
    // save eth map to wired
    if (m_ethNetworkMap[eth].length() == 0 && wiredName.length() > 0) {
        m_ethNetworkMap[eth] = wiredName;
        qDebug("initialized wired:%s", wiredName.c_str());
    }
    m_ethOnlineMap[eth] = isOnline;
    return make_pair(wiredName, isOnline);
}

pair<string, bool> TPCNetworkUtility::get_ethernet_status(const char* ethernet) {
    return _get_eth_status(ethernet);
}

pair<string, bool> TPCNetworkUtility::get_ethernet_status_until_timeout(const char* ethernet) {
#ifdef _WIN32
    return make_pair("", EXIT_FAILURE);
#else
    int seconds = 0;
    auto ret = _get_eth_status(ethernet);
    // connmanctl service need some time to be ready
    while (!ret.second && seconds < CONNMAN_SERVICE_TIMEOUT) {
        qDebug("connmanctl %s is not online", ethernet);
        sleep(1);
        ret = _get_eth_status(ethernet);
        seconds++;
    }
    return ret;
#endif
}

bool TPCNetworkUtility::is_network_available(string ethernet) {
    const auto retAvailableNet = get_available_networks();
    string network = m_ethNetworkMap[ethernet];
    bool isOnline = false;
    // get wired status
    for (int i = 0; i < (int)retAvailableNet.first.size(); i++) {
        if (retAvailableNet.first.at(i).compare(network) == 0) {
            isOnline = true;
            break;
        }
    }
    return isOnline;
}

bool TPCNetworkUtility::set_static_ip_address(const char* ethernet, const char* ipv4, const char* ipv6, const char* subnetMask, const char* gateway) {
    bool result = false;
    // check input
    if (!ethernet || (!ipv4 && !ipv6) || !subnetMask || !gateway) {
        qDebug("missing parameter");
        return result;
    }
    if (strlen(ethernet) == 0) {
        qDebug("empty ethernet:%s", ethernet);
        return result;
    }

    string network = m_ethNetworkMap[ethernet];
    if (ipv4 && strlen(ipv4) > 0)
        return execute_cmd_set_info(SET_IP_ADDRESS_CMD, network.c_str(), TYPE_IPV4, ipv4, subnetMask, gateway);
    else if (ipv6 && strlen(ipv6) > 0)
        return execute_cmd_set_info(SET_IP_ADDRESS_CMD, network.c_str(), TYPE_IPV6, ipv6, subnetMask, gateway);
    else {
        qDebug("empty ip");
        return result;
    }
}

bool TPCNetworkUtility::set_dhcp(const char* ethernet, bool isIpv4) {
    // check input
    if (!ethernet || strlen(ethernet) == 0) {
        qDebug("missing parameter");
        return false;
    }

    string network = m_ethNetworkMap[ethernet];
    if (isIpv4)
        return execute_cmd_set_info(SET_DHCP_CMD, network.c_str(), TYPE_IPV4);
    else
        return execute_cmd_set_info(SET_DHCP_CMD, network.c_str(), TYPE_IPV6);
}

bool TPCNetworkUtility::set_dns_server(const char* ethernet, const char* dns1, const char* dns2) {
    // check input
    if (!ethernet || (!dns1 && !dns2)) {
        qDebug("missing parameter");
        return false;
    }
    if (strlen(ethernet) == 0) {
        qDebug("empty ethernet:%s", ethernet);
        return false;
    }

    string network = m_ethNetworkMap[ethernet];
    return execute_cmd_set_info(SET_DNS_SERVER_CMD, network.c_str(), dns1, dns2);
}

bool TPCNetworkUtility::_create_tmp_offline_provisioning_file(const char* ethernet) {
    bool result = false;
    char localFile[BUFF_SIZE]= {0};
    snprintf(localFile, BUFF_SIZE, TMP_PROVISIONING_FILE_PATTERN, ethernet);
    result = execute_cmd_set_info(CREATE_FILE_CMD, localFile);
    if (!result) {
        qDebug("create file failed");
        return result;
    }
    const auto retMac = get_ethernet_mac_address(ethernet);
    result &= _set_offline_provisioning_file_value(ethernet, PROVISIONING_SECTION_TYPE_KEY, PROVISIONING_SECTION_TYPE_ETHERNET);
    result &= _set_offline_provisioning_file_value(ethernet, PROVISIONING_SECTION_MAC_KEY, retMac.first.c_str());
    result &= _set_offline_provisioning_file_value(ethernet, PROVISIONING_SECTION_DEVICE_NAME_KEY, ethernet);
    return result;
}

bool TPCNetworkUtility::_set_offline_provisioning_file_value(const char* ethernet, const char* key, const char* value) {
    bool result = false;
    char localFile[BUFF_SIZE]= {0};
    char section[BUFF_SIZE]= {0};
    // check input
    if (!ethernet || !key || !value) {
        qDebug("missing parameter");
        return result;
    }
    if (strlen(ethernet) == 0 || strlen(key) == 0 || strlen(value) == 0) {
        qDebug("empty ethernet:%s key:%s value:%s", ethernet, key, value);
        return result;
    }
    snprintf(localFile, BUFF_SIZE, TMP_PROVISIONING_FILE_PATTERN, ethernet);
    if (!is_file_exist(localFile)) {
        qDebug("create tmp file:%s", localFile);
        result = _create_tmp_offline_provisioning_file(ethernet);
    }
    snprintf(section, BUFF_SIZE, PROVISIONING_SECTION_PATTERN, ethernet);
    return execute_cmd_set_info(GENERAL_SET_INI_VALUE_CMD, localFile, section, key, value);
}

bool TPCNetworkUtility::create_offline_provisioning_file(const char* ethernet) {
    bool result = false;
    char tmpFile[BUFF_SIZE]= {0};
    char localFile[BUFF_SIZE]= {0};
    // check input
    if (!ethernet) {
        qDebug("missing parameter");
        return result;
    }
    if (strlen(ethernet) == 0) {
        qDebug("empty ethernet");
        return result;
    }
    snprintf(tmpFile, BUFF_SIZE, TMP_PROVISIONING_FILE_PATTERN, ethernet);
    snprintf(localFile, BUFF_SIZE, CONNMAN_PROVISIONING_FILE_PATTERN, ethernet);
    return mv_file(tmpFile, localFile);
}

bool TPCNetworkUtility::delete_offline_provisioning_file(const char* ethernet) {
    bool result = false;
    char localFile[BUFF_SIZE]= {0};
    int ret = 0;
    // check input
    if (!ethernet) {
        qDebug("missing parameter");
        return result;
    }
    if (strlen(ethernet) == 0) {
        qDebug("empty ethernet");
        return result;
    }
    snprintf(localFile, BUFF_SIZE, CONNMAN_PROVISIONING_FILE_PATTERN, ethernet);
    if (!is_file_exist(localFile)) {
        qDebug("provisioning file:%s not exist", localFile);
        return result;
    }
    // get ethernet status
    auto retethStatus = get_ethernet_status(ethernet);
    bool isWiredOnline = retethStatus.second;
    // disable ethernet to prevent ethernet enter idle state
    result = disable_connman_technology_ethernet();
    if (!result) {
        qDebug("disable ethernet failed");
        return result;
    }
    // delete file
    ret = remove(localFile);
    result = (ret == 0);
    if (!result) {
        qDebug("delete file failed");
        return result;
    }
    qDebug("delete provisioning file:%s", localFile);
    // enable ethernet back to normal
    result = enable_connman_technology_ethernet();
    if (!result) {
        qDebug("enable ethernet failed");
        return result;
    }
    if (isWiredOnline) {
        // wait ethernet back to online
        retethStatus = get_ethernet_status_until_timeout(ethernet);
        if (!retethStatus.second) {
            qDebug("ethernet is not online");
            return result;
        }
    }
    return result;
}

bool TPCNetworkUtility::enable_connman_technology_ethernet() {
    return execute_cmd_set_info(OPERATE_CONNMAN_ETHERNET_CMD, STRING_ENABLE);
}

bool TPCNetworkUtility::disable_connman_technology_ethernet() {
    return execute_cmd_set_info(OPERATE_CONNMAN_ETHERNET_CMD, STRING_DISABLE);
}

bool TPCNetworkUtility::restart_connman_service() {
    return execute_cmd_set_info(OPERATE_CONNMAN_SERVICE_CMD, STRING_RESTART);
}

bool TPCNetworkUtility::set_static_ip_address_offline(const char* ethernet, const char* ipv4, const char* ipv6, const char* subnetMask, const char* gateway) {
    bool result = false;
    char value_buff[BUFF_SIZE]= {0};
    // check input
    if (!ethernet || (!ipv4 && !ipv6) || !subnetMask || !gateway) {
        qDebug("missing parameter");
        return result;
    }
    if (strlen(ethernet) == 0) {
        qDebug("empty ethernet:%s", ethernet);
        return result;
    }

    if (ipv4 && strlen(ipv4) > 0) {
        snprintf(value_buff, BUFF_SIZE, "%s/%s/%s", ipv4, subnetMask, gateway);
        return _set_offline_provisioning_file_value(ethernet, PROVISIONING_SECTION_IPV4_KEY, value_buff);
    } else if (ipv6 && strlen(ipv6) > 0) {
        snprintf(value_buff, BUFF_SIZE, "%s/%s/%s", ipv6, subnetMask, gateway);
        return _set_offline_provisioning_file_value(ethernet, PROVISIONING_SECTION_IPV6_KEY, value_buff);
    } else {
        qDebug("empty ip");
        return result;
    }
}

bool TPCNetworkUtility::set_dhcp_offline(const char* ethernet, bool isIpv4) {
    // check input
    if (!ethernet || strlen(ethernet) == 0) {
        qDebug("missing parameter");
        return false;
    }

    if (isIpv4)
        return _set_offline_provisioning_file_value(ethernet, PROVISIONING_SECTION_IPV4_KEY, MODE_DHCP);
    else
        return _set_offline_provisioning_file_value(ethernet, PROVISIONING_SECTION_IPV6_KEY, MODE_DHCP);
}

bool TPCNetworkUtility::set_dns_server_offline(const char* ethernet, const char* dns1, const char* dns2) {
    char value_buff[BUFF_SIZE]= {0};
    // check input
    if (!ethernet || (!dns1 && !dns2)) {
        qDebug("missing parameter");
        return false;
    }
    if (strlen(ethernet) == 0 || ((dns1 && strlen(dns1) == 0) && (dns2 && strlen(dns2) == 0))) {
        qDebug("empty ethernet:%s dns", ethernet);
        return false;
    }

    snprintf(value_buff, BUFF_SIZE, "%s,%s", dns1, dns2);
    return _set_offline_provisioning_file_value(ethernet, PROVISIONING_SECTION_DNS_KEY, value_buff);
}

bool TPCNetworkUtility::start_ip_monitor() {
    return execute_cmd_set_info(IP_MONITOR_CMD, MONITOR_FILE);
}

bool TPCNetworkUtility::stop_ip_monitor() {
    return execute_cmd_set_info(KILL_IP_MONITOR_CMD);
}

pair<string, bool> TPCNetworkUtility::get_up_ethernet_from_monitor_file(const char* monitorFile) {
    // check input
    if (!monitorFile || strlen(monitorFile) == 0) {
        qDebug("missing parameter");
        return make_pair(string(), false);
    }
    return execute_cmd_get_single_info(GET_UP_ETHERNET_CMD, monitorFile);
}

// firewall related
pair<vector<map<string, string>>, bool> TPCNetworkUtility::get_firewall_accept_ports() {
    vector<map<string, string>> result;
    auto retRuleList = execute_cmd_get_vector(LIST_IPTABLE_INPUT_RULES);
    for (auto &retRule : retRuleList.first)
    {
        map<string, string> rule;
        // ex: ACCEPT     tcp  --  anywhere             anywhere             state NEW tcp dpt:ssh /* from settings */
        // index: 0        1    2     3                     4                  5    6   7     8    9   10     11    12
        //qDebug("rule:%s", rule.c_str());
        auto valueList = split_string_to_vector(retRule.c_str(), RULE_ITEM_SPLIT_STRING);
        for (auto &value : valueList) 
        {
            if (value.compare(PROTOCOL_TCP) == 0 || value.compare(PROTOCOL_UDP) == 0 ) {
                rule[PROTOCOL_STRING] = value;
            } 
            else if (value.find(RULE_ITEM_PORT_SPLIT_STRING) != std::string::npos) {
                // ex: dpt:ssh or dpt:999
                auto portList = split_string_to_vector(value.c_str(), RULE_ITEM_PORT_SPLIT_STRING);
                if (is_numbers(portList.at(0).c_str())) {
                    rule[PORT_STRING] = portList.at(0);
                } else {
                    // ex: ssh to 22
                    string protocol = rule.find(PROTOCOL_STRING)->second;
                    auto retPort = get_port_by_protocol_and_service_name(protocol.c_str(), portList.at(0).c_str());
                    if (!retPort.second) {
                        qDebug("get port:%s by protocol and service name:%s failed", retPort.first.c_str(), portList.at(0).c_str());
                    }
                    rule[PORT_STRING] = retPort.first;
                }
                result.push_back(rule);
            }
        }
    }
    return make_pair(result, true);
}

pair<string, bool> TPCNetworkUtility::get_port_by_protocol_and_service_name(const char* protocol, const char* name) {
    // check input
    if (!protocol || !name) {
        qDebug("missing parameter");
        return make_pair(string(), false);
    }
    if (strlen(protocol) == 0 || strlen(name) == 0) {
        qDebug("empty protocol:%s name:%s", protocol, name);
        return make_pair(string(), false);
    }
    return execute_cmd_get_single_info(GET_PORT_BY_PROTOCOL_AND_SERVICE_NAME, protocol, name);
}

bool TPCNetworkUtility::set_firewall_accept_ports(vector<map<string, string>> portRules) {
    bool result = true;
    // check input
    if (portRules.size() == 0) {
        qDebug("empty port rules");
        return false;
    }
    // remove all rules from settings tool
    remove_firewall_accept_ports();
    // add all rules
    for (auto &portRule : portRules) {
        result &= execute_cmd_set_info(
            ADD_ACCEPT_RULE_CMD, 
            portRule[PROTOCOL_STRING].c_str(), 
            portRule[PROTOCOL_STRING].c_str(), 
            portRule[PORT_STRING].c_str());
        if (!result) {
            qDebug("add rule protocol:%s port:%s failed", portRule[PROTOCOL_STRING].c_str(), portRule[PORT_STRING].c_str());
            break;
        }
    }
    result &= execute_cmd_set_info(SAVE_IPTABLE_RULES);
    return result;
}

bool TPCNetworkUtility::remove_firewall_accept_ports() {
    bool result = true;
    auto retRuleLineNumberList = execute_cmd_get_vector(LIST_IPTABLE_INPUT_RULES_LINE_NUMBER);
    if (!retRuleLineNumberList.second) {
        qDebug("list iptable input rules with line number failed");
        return retRuleLineNumberList.second;
    }
    if (retRuleLineNumberList.first.size() == 0) {
        qDebug("iptable input rules is empty");
        return retRuleLineNumberList.second;
    }
    // delete rule from last
    for (int i = retRuleLineNumberList.first.size() - 1; i >= 0; i--) {
        result &= execute_cmd_set_info(DELETE_ACCEPT_RULE_BY_LINE_NUMBER_CMD, retRuleLineNumberList.first.at(i).c_str());
    }
    return result;
}