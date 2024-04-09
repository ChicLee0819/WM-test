// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15

ScreenPageForm {
    id: screenForm

    // the signal's handler is in c++
    signal brightChangedSignal(int value)

    brightSlider.onMoved: {
        brightSpinbox.value = brightSlider.value
        screenForm.brightChangedSignal(brightSlider.value)
    }

    brightSpinbox.onValueModified: {
        brightSlider.value = brightSpinbox.value
        screenForm.brightChangedSignal(brightSlider.value)
    }

    screensaverSwitch.onClicked: {
        screensaverSpinbox.enabled = screensaverSwitch.checked
    }

    gestureSwitch.onClicked: {
        // enabled all sub-gesture
        gestureSwipeDownSwitch.enabled = gestureSwitch.checked
        gestureSwipeUpSwitch.enabled = gestureSwitch.checked
        gestureSwipeRightSwitch.enabled = gestureSwitch.checked
        // checked all sub-gesture
        gestureSwipeDownSwitch.checked = gestureSwitch.checked
        gestureSwipeUpSwitch.checked = gestureSwitch.checked
        gestureSwipeRightSwitch.checked = gestureSwitch.checked
    }

    // these functions call from c++
    function initTopBarPositionComboBox(name) {
        let nIndex = 0;

        for(let i = 0; i < topbarPositionModel.count; i++)
        {
            if (topbarPositionModel.get(i).value === name) {
                nIndex = i;
                break;
            }
        }
        topbarPositionComboBox.currentIndex = nIndex;
    }

    function getCurrentTopBarPosition() {
        return topbarPositionModel.get(topbarPositionComboBox.currentIndex).value;
    }

    function initRotateScreenComboBox(name) {
        let nIndex = 0;

        for(let i = 0; i < rotateScreenModel.count; i++)
        {
            if (rotateScreenModel.get(i).value === name) {
                nIndex = i;
                break;
            }
        }
        rotateScreenComboBox.currentIndex = nIndex;
    }

    function getCurrentRotateScreen() {
        return rotateScreenModel.get(rotateScreenComboBox.currentIndex).value;
    }
}
