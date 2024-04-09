// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15

WizScreenPageForm {
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
}
