// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.VirtualKeyboard.Settings 2.15

SettingPageForm {
    arrowButton.onClicked: {
        drawerSideBar.open()
    }

    fileDialog.onAccepted: {
        let result = "";
        if (fileDialog.file)
            result = String(fileDialog.file).replace("file://", "");
        else
            result = String(fileDialog.selectedFile).replace("file://", "");
        if (systemForm.visible)
            systemForm.fileTextField.text = result;
        if (updateForm.visible)
            updateForm.fileTextField.text = result;
        if (logoForm.visible)
            logoForm.logoPathTextField.text = result;
        if (wizstartupForm.visible)
            wizstartupForm.fileTextField.text = result;
    }

    folderDialog.onAccepted: {
        let result = "";
        if (folderDialog.folder)
            result = String(folderDialog.folder).replace("file://", "");
        else
            result = String(folderDialog.selectedFolder).replace("file://", "");
        if (ftpForm.visible)
            ftpForm.localPathTextField.text = result;
    }

    dateCalendar.onClicked: {
        if (timeForm.visible)
            timeForm.dateTextField.text = Qt.formatDate(dateCalendar.selectedDate, "yyyy-MM-dd")
        if (wiztimeForm.visible)
            wiztimeForm.dateTextField.text = Qt.formatDate(dateCalendar.selectedDate, "yyyy-MM-dd")
        calendarPopup.close()
    }

    loadingIndicatorTimer.onTriggered: {
        loadingIndicator.close();
    }

    function loadingIndicatorDelayClose(delayTime) {
        loadingIndicatorTimer.interval = delayTime;
        loadingIndicatorTimer.repeat = false;
        loadingIndicatorTimer.start();
    }

    // these functions call from c++
    function initAppKeyboardLocale(locale) {
        VirtualKeyboardSettings.locale = locale
    }
}
