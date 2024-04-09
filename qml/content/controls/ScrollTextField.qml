// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15
import SettingsGUI 1.0

Rectangle {
    id: root
    implicitWidth: Constants.textFieldWidth
    implicitHeight: Constants.textFieldHeight + Constants.scrollBarHeight
    color: "transparent"
    property string text

    MouseArea {
        anchors.fill: root
        onClicked: {
            textfield.forceActiveFocus()
        }
    }

    Flickable {
        id: flickable
        anchors.fill: parent
        contentWidth: textfield.width
        contentHeight: root.height
        clip: true
        flickableDirection: Flickable.HorizontalFlick
        ScrollBar.horizontal: ScrollBar {
            id: hbar
            active: true
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    // Because ScrollBar stack on the TextField
                    // forceActiveFocus when click on the ScrollBar
                    textfield.forceActiveFocus();
                }
            }
        }

        function ensureVisible(r)
        {
            // use current content x position or
            // use cursor x position + add cursor retangle width
            if (contentX >= r.x)
                contentX = r.x;
            else if ((contentX + width) <= (r.x + r.width))
                contentX = r.x + r.width - width;
        }

        TextField {
            id: textfield
            text: root.text
            width: Math.max(root.width, contentWidth + padding)
            height: Constants.textFieldHeight
            font.pointSize: Constants.baseLabelFontSize
            property int padding: 20

            onTextChanged: {
                root.text = textfield.text;
            }
            onCursorRectangleChanged: {
                // let the visibleArea follow the cursor movement
                flickable.ensureVisible(cursorRectangle);
            }
        }
    }
}
