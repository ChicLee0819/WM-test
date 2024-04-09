// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15
import SettingsGUI 1.0
import "../js/moment.js" as Moment
import "../js/moment-timezone.js" as MomentTimezone
import "../js/moment-timezone-with-data-10-year-range.js" as MomentTimezoneData

WizTimePageForm {
    // these functions call from c++
    function initTimezoneModel(sysTimezones) {
        for(let i in sysTimezones)
        {
            let tmz = {
                "text": " (GMT" + moment.tz(sysTimezones[i]).format('Z') + ") " + sysTimezones[i],
                "value": sysTimezones[i],
            };
            timezoneModel.append(tmz);
        }
    }

    function initTimezoneComboBox(timezone) {
        let tIndex;

        for(let i = 0; i < timezoneModel.count; i++)
        {
            if (timezoneModel.get(i).value === timezone) {
                tIndex = i;
                break;
            } else if (timezoneModel.get(i).value === "UTC" && timezone === "Universal") {
                tIndex = i;
                break;
            }
        }
        timezoneComboBox.currentIndex = tIndex;
    }

    function initTimeComboBox() {
        let current = new Date();
        let hour = Qt.formatDateTime(current, "hh");
        let minute = Qt.formatDateTime(current, "mm");
        let second = Qt.formatDateTime(current, "ss");
        hourComboBox.currentIndex = parseInt(hour, 10);
        minuteComboBox.currentIndex = parseInt(minute, 10);
        secondComboBox.currentIndex = parseInt(second, 10);
        dateTextField.text = Qt.formatDate(current, "yyyy-MM-dd")

        // limit the popup height of ComboBox
        timezoneComboBox.popup.contentItem.implicitHeight = Qt.binding(function () {
            return Math.min(Constants.maxComboBoxHeight, timezoneComboBox.popup.contentItem.contentHeight);
        });
        hourComboBox.popup.contentItem.implicitHeight = Qt.binding(function () {
            return Math.min(Constants.maxComboBoxHeight, hourComboBox.popup.contentItem.contentHeight);
        });
        minuteComboBox.popup.contentItem.implicitHeight = Qt.binding(function () {
            return Math.min(Constants.maxComboBoxHeight, minuteComboBox.popup.contentItem.contentHeight);
        });
        secondComboBox.popup.contentItem.implicitHeight = Qt.binding(function () {
            return Math.min(Constants.maxComboBoxHeight, secondComboBox.popup.contentItem.contentHeight);
        });
    }

    // these functions call from qml
    Timer {
        interval: 1000
        running: true
        repeat: true
        onTriggered: {
            let current = new Date();
            currentDateText.text = Qt.formatDateTime(current, "yyyy-MM-dd");
            currentTimeText.text = Qt.formatDateTime(current, "hh:mm:ss");
        }
    }

    dateButton.onClicked: {
        calendarPopup.open()
    }

    ntpRadioButton.onClicked: {
        ntpServerTextField.enabled = ntpRadioButton.checked
        dateButton.enabled = manualRadioButton.checked
        hourComboBox.enabled = manualRadioButton.checked
        minuteComboBox.enabled = manualRadioButton.checked
        secondComboBox.enabled = manualRadioButton.checked
    }

    manualRadioButton.onClicked: {
        ntpServerTextField.enabled = ntpRadioButton.checked
        dateButton.enabled = manualRadioButton.checked
        hourComboBox.enabled = manualRadioButton.checked
        minuteComboBox.enabled = manualRadioButton.checked
        secondComboBox.enabled = manualRadioButton.checked
    }

    timezoneComboBox.onCurrentIndexChanged: {
        timezoneTextField.text = timezoneModel.get(timezoneComboBox.currentIndex).value;
    }
}
