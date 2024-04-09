// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15

StoragePageForm {
    property bool initialized: false

    Timer {
        id: initTimer
        interval: 1000
        running: false
        repeat: false
    }

    // these functions call from c++
    function reset() {
        initialized = false;
    }

    function clearPratitionModelList() {
        emmcPartitionModel.clear();
        sdPartitionModel.clear();
    }

    function addToEMMCPratitionModelList(label, mountPoint, uuid, fstype, size, usedPercent) {
        let element = {
            "label": label, 
            "mountPoint": mountPoint,
            "uuid": uuid,
            "fstype": fstype,
            "size": size,
            "usedPercent": usedPercent,
        };
        emmcPartitionModel.append(element);
    }

    function addToSDPratitionModelList(label, mountPoint, uuid, fstype, size, usedPercent) {
        let element = {
            "label": label, 
            "mountPoint": mountPoint,
            "uuid": uuid,
            "fstype": fstype,
            "size": size,
            "usedPercent": usedPercent,
        };
        sdPartitionModel.append(element);
    }

    function initEMMCStoragePratitionUI() {
        // wait 200 milliseconds for UI component is ready
        // without delay might occur "Cannot read property 'children' of null"
        if (!initialized) {
            delay(200, initEMMCStoragePratitionUI);
            initialized = true;
            return;
        }
        if (storageSwipeView.currentIndex == 0) {
            // click emmc first partition
            if (emmcStorageGrid.count > 0) {
                let item = emmcStorageGrid.itemAtIndex(0);
                let column = item.children[0];
                let button = column.children[0];
                button.clicked();
                button.checked = true;
            }
        }
    }

    function initSDStoragePratitionUI() {
        // show/hide sd partition
        sdColumnLayout.visible = (sdStorageGrid.count > 0);
        sdNotExistColumnLayout.visible = (sdStorageGrid.count == 0);
        if (storageSwipeView.currentIndex == 1) {
            // click sd card first partition
            if (sdStorageGrid.count > 0) {
                let item = sdStorageGrid.itemAtIndex(0);
                let column = item.children[0];
                let button = column.children[0];
                button.clicked();
                button.checked = true;
            }
        }
    }

    // these functions call from qml
    function showPratition(label, mountPoint, uuid, fstype, size, usedPercent) {
        if (storageSwipeView.currentIndex == 0) {
            // emmc
            deviceLabel1.text = label;
            mountPointLabel1.text = mountPoint;
            uuidLabel1.text = uuid;
            fsTypeLabel1.text = fstype;
            sizeLabel1.text = size;
            updateUsedPercentBar(usedPercent, usedPercentBar1);
        } else {
            // sd card
            deviceLabel2.text = label;
            mountPointLabel2.text = mountPoint;
            uuidLabel2.text = uuid;
            fsTypeLabel2.text = fstype;
            sizeLabel2.text = size;
            updateUsedPercentBar(usedPercent, usedPercentBar2);
        }
    }

    function updateUsedPercentBar(percentage, usedPercentBar) {
        const parsed = parseInt(percentage, 10);
        usedPercentBar.percentage = parsed;
        usedPercentBar.state = "valueChanged";
    }

    // using qml Timer to delay
    function delay(delayTime, cb) {
        let timer = initTimer;
        timer.interval = delayTime;
        timer.repeat = false;
        timer.triggered.connect(cb);
        timer.start();
    }

    storageSwipeView.onCurrentIndexChanged: {
        // click first partition
        if (storageSwipeView.currentIndex == 0) {
            // emmc
            initEMMCStoragePratitionUI();
        } else {
            // sd card
            initSDStoragePratitionUI();
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
