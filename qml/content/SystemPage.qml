// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15

SystemPageForm {

    startupComboBox.onCurrentIndexChanged: {
        let autoRestartShow = false;
        let vncViewerShow = false;
        let staticPageShow = false;
        let staticPageCustomShow = false;
        let webPageShow = false;
        if (startupComboBox.currentIndex == 1) {
            // select VNC Viewer
            vncViewerShow = true;
        } else if (startupComboBox.currentIndex == 2) {
            // select Chromium
            webPageShow = true;
        } else if (startupComboBox.currentIndex == 3) {
            // select Chromium Kiosk
            autoRestartShow = true;
            webPageShow = true;
        } else if (startupComboBox.currentIndex == 4) {
            // select Static Page
            staticPageShow = true;
            autoRestartShow = true;
        } else if (startupComboBox.currentIndex == 5) {
            // select custom Static Page
            staticPageCustomShow = true;
            autoRestartShow = true;
        } else {
            // select others
        }
        autoRestartGrid.visible = autoRestartShow;
        vncViewerGroup.visible = vncViewerShow;
        staticPageGroup.visible = staticPageShow;
        staticPageCustomGroup.visible = staticPageCustomShow;
        webPageGroup.visible = webPageShow;
    }

    vncShowPasswordSwitch.onClicked: {
        vncPasswordTextField.echoMode = vncShowPasswordSwitch.checked ? TextInput.Normal : TextInput.Password
    }

    selectButton.onClicked: {
        fileDialog.nameFilters = ["HTML files (*.html)"];
        fileDialog.open();
    }

    addWebPageButton.onClicked: {
        let empty = {
            "page": "",
            "is_startup": false,
        };
        webPageModel.append(empty);
    }

    function removeWebPageRow(index) {
        webPageModel.remove(index);
    }

    rebootSystemCronJobSwitch.onClicked: {
        initCrontabGroup(rebootSystemCronJobSwitch.checked);
    }

    cronModeComboBox.onCurrentIndexChanged: {
        let dayofweekShow = false;
        if (cronModeComboBox.currentIndex == 0) {
            // select daily
            dayofweekShow = false;
        } else {
            // select weekly
            dayofweekShow = true;
        }
        cronDayofweekLabel.visible = dayofweekShow;
        cronDayofweekComboBox.visible = dayofweekShow;
    }

    // these functions call from c++
    function initStartupComboBox(name) {
        let nIndex = 0;

        for(let i = 0; i < startupModel.count; i++)
        {
            if (startupModel.get(i).value === name) {
                nIndex = i;
                break;
            }
        }
        startupComboBox.currentIndex = nIndex;
    }

    function initCom1ModeComboBox(name) {
        let nIndex = 0;

        for (let i = 0; i < com1ModeModel.count; i++)
        {
            if (com1ModeModel.get(i).value === name) {
                nIndex = i;
                break;
            }
        }
        com1ModeComboBox.currentIndex = nIndex;
    }

    function initCom1BaudRateComboBox(name) {
        let nIndex = 0;

        for (let i = 0; i < comBaudRateModel.count; i++)
        {
            if (comBaudRateModel.get(i).value === name) {
                nIndex = i;
                break;
            }
        }
        com1BaudRateComboBox.currentIndex = nIndex;
    }

    function initCom2BaudRateComboBox(name) {
        let nIndex = 0;

        for (let i = 0; i < comBaudRateModel.count; i++)
        {
            if (comBaudRateModel.get(i).value === name) {
                nIndex = i;
                break;
            }
        }
        com2BaudRateComboBox.currentIndex = nIndex;
    }

    function initCrontabGroup(enabled) {
        cronModeComboBox.enabled = enabled
        cronMinuteComboBox.enabled = enabled
        cronHourComboBox.enabled = enabled
        cronDayofweekComboBox.enabled = enabled
        crontabGroup.visible = enabled
    }

    function initCronModeComboBox(name) {
        let nIndex = 0;

        for (let i = 0; i < cronModeModel.count; i++)
        {
            if (cronModeModel.get(i).value === name) {
                nIndex = i;
                break;
            }
        }
        cronModeComboBox.currentIndex = nIndex;
    }

    function initCronMinuteComboBox(value) {
        cronMinuteComboBox.currentIndex = value;
    }

    function initCronHourComboBox(value) {
        cronHourComboBox.currentIndex = value;
    }

    function initCronDayofweekComboBox(value) {
        cronDayofweekComboBox.currentIndex = value;
    }

    function initWebPageModel(webpages) {
        // clear last data
        webPageModel.clear();
        // assign new data to model
        for(let i in webpages)
        {
            let page = {
                "page": webpages[i]["page"],
                "is_startup": webpages[i]["is_startup"] === "true",
            };
            webPageModel.append(page);
        }
        if (webPageModel.count == 0)
        {
            let page = {
                "page": "",
                "is_startup": true,
            };
            webPageModel.append(page);
        }
    }

    function getWebPageByIndex(index) {
        let page = {
            "page": webPageRepeater.itemAt(index).children[1].text,
            "is_startup": webPageRepeater.itemAt(index).children[0].checked,
        };
        return page;
    }

    function getCurrentStartup() {
        return startupModel.get(startupComboBox.currentIndex).value;
    }

    function getCurrentCom1Mode() {
        return com1ModeModel.get(com1ModeComboBox.currentIndex).value;
    }

    function getCurrentCronMode() {
        return cronModeModel.get(cronModeComboBox.currentIndex).value;
    }

    function removeCOMPage() {
        const comPageIndex = 2;
        if (systemTabBar.count > comPageIndex) {
            let item = systemTabBar.itemAt(comPageIndex);
            systemTabBar.removeItem(item);
        }
    }
}
