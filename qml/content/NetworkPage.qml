// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15

NetworkPageForm {
    id: networkForm
    property bool isEnterConfigured1: false
    property bool isEnterConfigured2: false

    addFirewallRuleButton.onClicked: {
        let empty = {
            "protocol": "",
            "port": "",
            "is_allowed": true,
        };
        firewallRuleModel.append(empty);
        // show empty label
        emptyFirewallLabel.visible = (firewallRuleModel.count === 0);
    }

    function removeFirewallRuleRow(index) {
        firewallRuleModel.remove(index);
        // show empty label
        emptyFirewallLabel.visible = (firewallRuleModel.count === 0);
    }

    function findProtocolIndex(name) {
        let nIndex = 0;
        for (let i = 0; i < protocolModel.count; i++)
        {
            if (protocolModel.get(i).text === name) {
                nIndex = i;
                break;
            }
        }
        return nIndex;
    }

    function setWiredLinkStatusImage(linkImage, isOnline) {
        if (isOnline) {
            linkImage.source = "images/checked.png"
        } else {
            linkImage.source = "images/unchecked.png"
        }
    }

    // these functions call from c++
    function setWired1LinkStatus(isOnline) {
        setWiredLinkStatusImage(linkImage1, isOnline);
    }

    function setWired2LinkStatus(isOnline) {
        setWiredLinkStatusImage(linkImage2, isOnline);
    }

    function switchWired1View(newIsEnterConfigured) {
        isEnterConfigured1 = newIsEnterConfigured
        ipLabel1.visible = !isEnterConfigured1
        ipAddressLabel1.visible = !isEnterConfigured1
        gatewayLabel1.visible = !isEnterConfigured1
        defaultGatewayLabel1.visible = !isEnterConfigured1
        dnsLabel1.visible = !isEnterConfigured1
        dns1Label1.visible = !isEnterConfigured1
        configButton1.visible = !isEnterConfigured1
        // configuration group
        configGroup1.visible = isEnterConfigured1
    }

    function switchWired2View(newIsEnterConfigured) {
        isEnterConfigured2 = newIsEnterConfigured
        ipLabel2.visible = !isEnterConfigured2
        ipAddressLabel2.visible = !isEnterConfigured2
        gatewayLabel2.visible = !isEnterConfigured2
        defaultGatewayLabel2.visible = !isEnterConfigured2
        dnsLabel2.visible = !isEnterConfigured2
        dns1Label2.visible = !isEnterConfigured2
        configButton2.visible = !isEnterConfigured2
        // configuration group
        configGroup2.visible = isEnterConfigured2
    }

    dhcpSwitch1.onClicked: {
        editGridLayout1.visible = !dhcpSwitch1.checked
    }

    dhcpSwitch2.onClicked: {
        editGridLayout2.visible = !dhcpSwitch2.checked
    }

    configButton1.onClicked: {
        switchWired1View(!isEnterConfigured1)
    }
    cancelButton1.onClicked: {
        switchWired1View(!isEnterConfigured1)
    }

    configButton2.onClicked: {
        switchWired2View(!isEnterConfigured2)
    }
    cancelButton2.onClicked: {
        switchWired2View(!isEnterConfigured2)
    }

    function initFirewallRulesModel(rules) {
        // clear last data
        firewallRuleModel.clear();
        // assign new data to model
        for(let i in rules)
        {
            let rule = {
                "protocol": rules[i]["protocol"],
                "port": rules[i]["port"],
                "is_allowed": true,
            };
            firewallRuleModel.append(rule);
            // assign combobox index
            firewallRuleRepeater.itemAt(i).children[1].currentIndex = findProtocolIndex(rule.protocol);
        }
        if (firewallRuleModel.count == 0)
        {
            let rule = {
                "protocol": "",
                "port": "",
                "is_allowed": true,
            };
            firewallRuleModel.append(rule);
        }
    }

    function getFirewallRuleByIndex(index) {
        let rule = {
            "protocol": firewallRuleRepeater.itemAt(index).children[1].currentText,
            "port": firewallRuleRepeater.itemAt(index).children[2].text,
            "is_allowed": true,
        };
        return rule;
    }
}
