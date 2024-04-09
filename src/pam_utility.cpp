// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#ifdef _WIN32
#else
#include <security/pam_appl.h>
#endif
#include "./include/pam_utility.h"
#include "./include/utility.h"

#include <QDebug>

using namespace std;

/**************************************************
 * @brief callback of PAM conversation
 * @param[in] num_msg the count of message
 * @param[in] msg PAM message
 * @param[out] resp our response
 * @param[in] appdata_ptr custom data passed by struct pam_conv.appdata_ptr
 * @return state
 **************************************************/
static int fake_conversation(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr)
{
#ifdef _WIN32
    return 0;
#else
    struct pam_response* aresp;
    /* check the count of message */
    if (num_msg <= 0 || num_msg >= PAM_MAX_MSG_SIZE)
    {
        return PAM_CONV_ERR;
    }
    /* alloc memory for response */
    if ((aresp = (struct pam_response *)malloc(num_msg * sizeof(struct pam_response))) == NULL)
    {
        return PAM_BUF_ERR;
    }
    /* response for message */
    aresp[0].resp_retcode = 0;
    aresp[0].resp = strdup((char*)appdata_ptr);
    *resp = aresp;
    return PAM_SUCCESS;
#endif
}

pair<string, int> sys_auth_user(const char *username, const char *password)
{
    int retval = EXIT_FAILURE;
    string result;
#ifdef _WIN32
    return make_pair(result, retval);
#else
    // check input
    if (!username || !password) {
        qDebug("missing parameter");
        return make_pair(result, retval);
    }
    if (strlen(username) == 0 || strlen(password) == 0) {
        qDebug("empty username or password");
        return make_pair(result, retval);
    }
    char input_password[BUFF_SIZE]= {0};
    memset(input_password, 0, BUFF_SIZE);
    snprintf(input_password, BUFF_SIZE, "%s", password);
    const struct pam_conv local_conversation = { fake_conversation, input_password };
    pam_handle_t *local_auth_handle = NULL;

    retval = pam_start("common-auth", username, &local_conversation, &local_auth_handle);
    /* authenticate the user */
    if (retval == PAM_SUCCESS)
        retval = pam_authenticate(local_auth_handle, 0);

    /* This is where we have been authorized or not. */
    if (retval == PAM_SUCCESS) {
        result = "Authenticated";
    } else {
        result = "Not Authenticated";
    }
    /* close Linux-PAM */
    if (retval == PAM_SUCCESS)
        retval = pam_end(local_auth_handle, retval);

    /* indicate success */
    retval = ( retval == PAM_SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE );
    return make_pair(result, retval);
#endif
}
