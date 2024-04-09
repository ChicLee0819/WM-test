// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: control
    implicitWidth: 320
    implicitHeight: 350

    border.color: "black"

    property alias font: month_grid.font
    property alias locale: month_grid.locale
    property date selectedDate: new Date()
    // custom button style
    component CalendarButton : AbstractButton {
        id: c_btn
        implicitWidth: 30
        implicitHeight: 30
        contentItem: Text {
            font: control.font
            text: c_btn.text
            color: "white"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        background: Item{}
    }

    /*!
        \qmlsignal Calendar::clicked(date date)

        Emitted when the mouse is clicked on a valid date in the calendar.

        \e date is the date that the mouse was clicked on.

        The corresponding handler is \c onClicked.
    */
    signal clicked(date date)

    GridLayout {
        anchors.fill: parent
        anchors.margins: 2
        columns: 1
        rows: 3
        columnSpacing: 1
        rowSpacing: 1

        Rectangle {
            Layout.fillWidth: true
            implicitHeight: 40
            color: "gray"
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                CalendarButton {
                    text: "<"
                    onClicked: {
                        month_grid.year-=1;
                    }
                }
                Text {
                    font: control.font
                    color: "white"
                    text: month_grid.year
                }
                CalendarButton {
                    text: ">"
                    onClicked: {
                        month_grid.year+=1;
                    }
                }
                Item {
                    implicitWidth: 20
                }
                CalendarButton {
                    text: "<"
                    onClicked: {
                        if(month_grid.month===0){
                            month_grid.year-=1;
                            month_grid.month=11;
                        }else{
                            month_grid.month-=1;
                        }
                    }
                }
                Text {
                    font: control.font
                    color: "white"
                    text: month_grid.month+1
                }
                CalendarButton {
                    text: ">"
                    onClicked: {
                        if(month_grid.month===11){
                            month_grid.year+=1;
                            month_grid.month=0;
                        }else{
                            month_grid.month+=1;
                        }
                    }
                }
            }
        }

        // week
        DayOfWeekRow {
            id: week_row
            Layout.fillWidth: true
            implicitHeight: 40
            spacing: 1
            topPadding: 0
            bottomPadding: 0
            font: control.font
            locale: control.locale
            delegate: Text {
                Layout.fillWidth: true
                Layout.fillHeight: true
                text: shortName
                font: week_row.font
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                required property string shortName
            }
            contentItem: Rectangle {
                color: "gray"
                border.color: "black"
                RowLayout {
                    anchors.fill: parent
                    spacing: week_row.spacing
                    Repeater {
                        model: week_row.source
                        delegate: week_row.delegate
                    }
                }
            }
        }

        // month grid
        MonthGrid {
            id: month_grid
            Layout.fillWidth: true
            Layout.fillHeight: true
            locale: Qt.locale("en_US")
            spacing: 1
            delegate: Rectangle {
                color: model.today
                       ?"orange"
                       :control.selectedDate.valueOf()===model.date.valueOf()
                         ?"darkCyan"
                         :"gray"
                border.color: "black"
                border.width: 1
                Rectangle {
                    anchors.fill: parent
                    anchors.margins: 2
                    color: "transparent"
                    border.color: "white"
                    visible: item_mouse.containsMouse
                }
                Text {
                    anchors.centerIn: parent
                    text: model.day
                    color: model.month===month_grid.month?"white":"black"
                }
                MouseArea {
                    id: item_mouse
                    anchors.fill: parent
                    hoverEnabled: true
                    acceptedButtons: Qt.NoButton
                }
            }
            onClicked: (date)=> {
               control.selectedDate = date;
               control.clicked(date);
           }
        }
    }
}
