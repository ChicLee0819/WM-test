// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#include <cstdio>
#include <string>
#include <array>
#include <QDebug>

#include "./include/utility.h"
#include "./include/app_utility.h"
#include "./include/config_utility.h"
#include "./include/polling_thread.h"

// ex: java -jar /usr/java/vncviewer/tightvnc-jviewer.jar
/*
Usage: java -jar (progfilename) [hostname [port_number]] [Options]
    or
 java -jar (progfilename) [Options]
    or
 java -jar (progfilename) -help
    to view this help

Where Options are:
 -help                   : Print this help.
 -host                   : Server host name.
 -port                   : Port number.
 -password               : Password to the server.
 -ShowControls           : Set to "No" if you want to get rid of that button panel at the top. Default: "Yes".
 -showConnectionDialog   : Set to "No" if you want not to show initial connection dialog. Default: "Yes".
 -ViewOnly               : When set to "Yes", then all keyboard and mouse events in the desktop window will be silently ignored and will not be passed to the remote side. Default: "No".
 -AllowClipboardTransfer : When set to "Yes", transfer of clipboard contents is allowed. Default: "Yes".
 -RemoteCharset          : Charset encoding is used on remote system. Use this option to specify character encoding will be used for encoding clipboard text content to. Default value: local system default character encoding. Set the value to 'standard' for using 'Latin-1' charset which is only specified by rfb standard for clipboard transfers.
 -ShareDesktop           : Share the connection with other clients on the same VNC server. The exact behaviour in each case depends on the server configuration. Default: "Yes".
 -AllowCopyRect          : The "CopyRect" encoding saves bandwidth and drawing time when parts of the remote screen are moving around. Most likely, you don't want to change this setting. Default: "Yes".
 -Encoding               : The preferred encoding. Possible values: "Tight", "Hextile", "ZRLE", and "Raw". Default: "Tight".
 -CompressionLevel       : Use specified compression level for "Tight" and "Zlib" encodings. Values: 1-9. Level 1 uses minimum of CPU time on the server but achieves weak compression ratios. Level 9 offers best compression but may be slow.
 -JpegImageQuality       : Use the specified image quality level in "Tight" encoding. Values: 1-9, Lossless. Default value: 6. To prevent server of using lossy JPEG compression in "Tight" encoding, use "Lossless" value here.
 -LocalPointer           : Possible values: on/yes/true (draw pointer locally), off/no/false (let server draw pointer), hide). Default: "On".
 -ConvertToASCII         : Whether to convert keyboard input to ASCII ignoring locale. Possible values: yes/true, no/false). Default: "No".
 -ColorDepth             : Bits per pixel color format. Possible values: 3 (for 8 colors), 6 (64 colors), 8 (256 colors), 16 (65 536 colors), 24 (16 777 216 colors), 32 (same as 24).
 -ScalingFactor          : Scale local representation of the remote desktop on startup. The value is interpreted as scaling factor in percents. The default value of 100% corresponds to the original framebuffer size.
 -FullScreen             : Full screen mode. Possible values: yes/true and no/false. Default: no.
 -sshHost                : SSH host name.
 -sshPort                : SSH port number. When empty, standard SSH port number (22) is used.
 -sshUser                : SSH user name.
 -Tunneling              : Tunneling. Possible values: auto - allow viewer to choose tunneling mode, none/no - no tunneling use, SSL - choose SSL tunneling when available. Default: auto
 -v                      : Verbose console output.
 -vv                     : More verbose console output.

Options format: -optionName=optionValue. Ex. -host=localhost -port=5900 -viewonly=yes
Both option name and option value are case insensitive.
*/
const char* START_TIGHTVNC_CMD = "LD_PRELOAD=/usr/local/lib/injectpassword.so java -jar /usr/java/vncviewer/tightvnc-jviewer.jar";
const char* STOP_TIGHTVNC_CMD = "ps ax | grep 'tightvnc-jviewer.jar' | awk -F ' ' '{print $1}' | head -n -1 | xargs -r kill";
const char* TMP_FILE = "/tmp/.tmpconfig";

TightVNCUtility::TightVNCUtility() {
    m_is_server_online = false;
    m_is_started = false;
    m_unavailable_count = 0;
    m_pollingThread = nullptr;
}

TightVNCUtility::~TightVNCUtility() {
    if (m_pollingThread) 
        delete m_pollingThread;
}

void TightVNCUtility::start_monitoring() {
    if (!m_pollingThread) 
        m_pollingThread = new PollingThread(this);
    // start polling thread
    m_pollingThread->start();
    m_pollingThread->wait();
}

bool TightVNCUtility::is_configuration_ready() {
    ConfigUtility configUtil;
    string address = configUtil.get_vnc_server_address();
    string port = configUtil.get_vnc_server_port();
    return (address.length() > 0 && port.length() > 0);
}

pair<string, int> TightVNCUtility::start() {
    ConfigUtility configUtil;
    char value_buff[BUFF_SIZE]= {0};
    string cmd;

    string address = configUtil.get_vnc_server_address();
    string port = configUtil.get_vnc_server_port();
    string password = configUtil.get_vnc_server_password();
    bool viewonly = configUtil.get_vnc_server_viewonly();
    int imageQuality = configUtil.get_vnc_server_image_quality();
    bool fullscreen = configUtil.get_vnc_server_fullscreen();
    bool fitWindow = configUtil.get_vnc_server_fit_window();
    cmd = START_TIGHTVNC_CMD;

    memset(value_buff, 0, BUFF_SIZE);
    snprintf(value_buff, BUFF_SIZE, " -showConnectionDialog=no");
    cmd.append(value_buff);

    if (address.length() > 0) {
        memset(value_buff, 0, BUFF_SIZE);
        snprintf(value_buff, BUFF_SIZE, " -host=%s", address.c_str());
        cmd.append(value_buff);
    }
    if (port.length() > 0) {
        memset(value_buff, 0, BUFF_SIZE);
        snprintf(value_buff, BUFF_SIZE, " -port=%s", port.c_str());
        cmd.append(value_buff);
    }
    if (viewonly) {
        memset(value_buff, 0, BUFF_SIZE);
        snprintf(value_buff, BUFF_SIZE, " -ViewOnly=yes");
        cmd.append(value_buff);
    }
    if (imageQuality > 0) {
        memset(value_buff, 0, BUFF_SIZE);
        snprintf(value_buff, BUFF_SIZE, " -JpegImageQuality=%d", imageQuality);
        cmd.append(value_buff);
    }
    if (fullscreen) {
        memset(value_buff, 0, BUFF_SIZE);
        snprintf(value_buff, BUFF_SIZE, " -FullScreen=yes");
        cmd.append(value_buff);
    }
    if (fitWindow) {
        memset(value_buff, 0, BUFF_SIZE);
        snprintf(value_buff, BUFF_SIZE, " -FitWindow=yes");
        cmd.append(value_buff);
    }
    if (password.length() > 0) {
        memset(value_buff, 0, BUFF_SIZE);
        snprintf(value_buff, BUFF_SIZE, " -password=*");
        cmd.append(value_buff);

        // save to file
        FILE *fptr = fopen(TMP_FILE, "w");
        if (fptr == NULL) { 
            exit(1);
        }
        fprintf(fptr, "%s", password.c_str());
        fclose(fptr);
    }
    // run in background
    cmd.append(" &");

    qDebug("Start tightVNC viewer!");
    const auto ret = execute_cmd_without_read(cmd.c_str());
    qDebug("ret:%d stderr:%s", ret.second, ret.first.c_str());
    return ret;
}

pair<string, int> TightVNCUtility::stop() {
    qDebug("Stop tightVNC viewer!");
    return execute_cmd(STOP_TIGHTVNC_CMD);
}

void TightVNCUtility::pollingVNCServerIsReady(bool isSuccess, bool isServerOnline) {
    if (!m_is_server_online && isServerOnline) {
        // from offline to online
        qDebug("VNC server become online!");
        // start once
        if (!m_is_started) {
            this->stop();
            this->start();
            m_is_started = true;
        }
        m_is_server_online = isServerOnline;
        m_unavailable_count = 0;
    }
    else if (m_is_server_online && !isServerOnline) {
        m_unavailable_count++;
        // from online to offline
        qDebug("VNC server become offline!");
        m_is_server_online = isServerOnline;
    }
}
