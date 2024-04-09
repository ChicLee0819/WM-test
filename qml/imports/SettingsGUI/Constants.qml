// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

pragma Singleton
import QtQuick 2.15
import QtQuick.Window 2.15

QtObject {
    readonly property int screenTopStatusBarHeight: 34

    readonly property int refScreenWidth: 1920
    readonly property int refScreenHeight: 1080
    // screen size list
    // 800x600
    // 1024x600
    // 1280x800
    // 1366x768
    // 1920x1080

    readonly property int screenWidth: Screen.width
    readonly property int screenHeight: Screen.height
    readonly property bool inPortrait: Screen.width < Screen.height
    readonly property bool inMiniScreen: (Screen.width < 1000 && Screen.height < 1000)

    function hscale(size) {
        if (inPortrait)
            return Math.round(size * (screenWidth / refScreenHeight))
        else
            return Math.round(size * (screenWidth / refScreenWidth))
    }

    function vscale(size) {
        if (inPortrait)
            return Math.round(size * (screenHeight / refScreenWidth))
        else
            return Math.round(size * (screenHeight / refScreenHeight))
    }

    function tscale(size) {
        return Math.round((hscale(size) + vscale(size)) / 2)
    }

    property alias fontDirectory: directoryFontLoader.fontDirectory
    property alias relativeFontDirectory: directoryFontLoader.relativeFontDirectory

    /* Edit this comment to add your custom font */
    readonly property font font: Qt.font({
                                             family: Qt.application.font.family,
                                             pixelSize: Qt.application.font.pixelSize
                                         })
    readonly property font largeFont: Qt.font({
                                                  family: Qt.application.font.family,
                                                  pixelSize: Qt.application.font.pixelSize * 1.6
                                              })


    readonly property int baseFontSize: tscale(36)
    readonly property int baseLabelFontSize: tscale(26)
    readonly property int baseBtnFontSize: tscale(20)
    readonly property int baseListBtnFontSize: tscale(14)
    readonly property int baseScale: tscale(1)
    readonly property int timezoneComboBoxFontSize: tscale(30)

    readonly property int inputPanelMargin: inPortrait ? 0 : hscale(380)
    readonly property int leftSideBarWidth: hscale(350)
    readonly property int sideBarButtonHeight: vscale(100)
    readonly property int portraitMaxLabelWidth: inMiniScreen ? 600 : 720
    readonly property int landscapeMaxLabelWidth: inMiniScreen ? 960 : 1200
    readonly property int maximumLabelWidth: inPortrait ? hscale(portraitMaxLabelWidth) : hscale(landscapeMaxLabelWidth)
    readonly property int portraitAboutLayoutWidth: inMiniScreen ? 720 : 960
    readonly property int landscapeAboutLayoutWidth: inMiniScreen ? 960 : 1280
    readonly property int aboutLayoutWidth: inPortrait ? hscale(portraitAboutLayoutWidth) : hscale(landscapeAboutLayoutWidth)
    readonly property int portraitAboutColumnWidth: inMiniScreen ? 340 : 480
    readonly property int landscapeAboutColumnWidth: inMiniScreen ? 480 : 720
    readonly property int aboutColumnWidth: inPortrait ? hscale(portraitAboutColumnWidth) : hscale(landscapeAboutColumnWidth)
    readonly property int portraitSliderWidth: inMiniScreen ? 200 : 300
    readonly property int landscapeSliderWidth: inMiniScreen ? 600 : 700
    readonly property int sliderWidth: inPortrait ? hscale(portraitSliderWidth) : hscale(landscapeSliderWidth)
    readonly property int smallTextFieldWidth: inPortrait ? hscale(400) : hscale(500)
    readonly property int textFieldWidth: inPortrait ? hscale(500) : hscale(600)
    readonly property int textFieldHeight: inMiniScreen ? vscale(86) : vscale(56)
    readonly property int percentageBarWidth: hscale(900)
    readonly property int percentageBarFrontWidth: hscale(795)
    readonly property int tabBarWidth: hscale(260)
    readonly property int tabBarHeight: vscale(60)
    readonly property int buttonWidth: inPortrait ? hscale(180) : hscale(230)
    readonly property int largeButtonWidth: hscale(400)
    readonly property int buttonHeight: vscale(60)
    readonly property int switchWidth: tscale(58)
    readonly property int switchHeight: tscale(26)
    readonly property int maxComboBoxHeight: vscale(350)
    readonly property int gridCellWidth: hscale(150)
    readonly property int gridCellHeight: vscale(140)
    readonly property int generalComboBoxWidth: hscale(300)
    readonly property int timezoneComboBoxWidth: inPortrait ? hscale(700) : hscale(1200)
    readonly property int timezoneComboBoxHeight: inMiniScreen ? vscale(80) : vscale(60)
    readonly property int portraitLargeComboBoxWidth: inMiniScreen ? 500 : 560
    readonly property int landscapeLargeComboBoxWidth: inMiniScreen ? 600 : 750
    readonly property int largeComboBoxWidth: inPortrait ? hscale(portraitLargeComboBoxWidth) : hscale(landscapeLargeComboBoxWidth)
    readonly property int middleComboBoxWidth: hscale(500)
    readonly property int smallComboBoxWidth: hscale(200)
    readonly property int radioCircle: tscale(30)
    readonly property int imageSquare: tscale(32)
    readonly property int baseMargin: hscale(30)
    readonly property int itemMargin: tscale(20)
    readonly property int splitMargin: tscale(10)
    readonly property int indentMargin: hscale(50)
    readonly property int windowMargin: 2
    readonly property int borderWidth: 2
    readonly property int scrollBarHeight: 10
    readonly property int pageWidth: inPortrait ? screenWidth : screenWidth - leftSideBarWidth
    readonly property int pageHeight: screenHeight
    readonly property int popupWidth: pageWidth * 0.8
    readonly property int sideBarButtonCount: 14

    property DirectoryFontLoader directoryFontLoader: DirectoryFontLoader {
        id: directoryFontLoader
    }
}
