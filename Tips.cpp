//==================================== Hacker rank input line ========================================//
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* readline();



int main()
{
    char* n_endptr;
    char* n_str = readline();
    int n = strtol(n_str, &n_endptr, 10);

    if (n_endptr == n_str || *n_endptr != '\0') { exit(EXIT_FAILURE); }

    // Write Your Code Here
    if (n == 1) {
        printf ("one\n");
    }
    else if (n == 2) {
        printf ("two\n");
    }
    else if (n == 3) {
        printf ("three\n");
    }
    else if (n == 4) {
        printf ("four\n");
    }
    else if (n == 5) {
        printf ("five\n");
    }
    else if (n == 6) {
        printf ("six\n");
    }
    else if (n == 7) {
        printf ("seven\n");
    }
    else if (n == 8) {
        printf ("eight\n");
    }
    else if (n == 9) {
        printf ("nine\n");
    }
    else {
        printf ("Greater than 9\n");
    }
    return 0;
}

char* readline() {
    size_t alloc_length = 1024;
    size_t data_length = 0;
    char* data = malloc(alloc_length);

    while (true) {
        char* cursor = data + data_length;
        char* line = fgets(cursor, alloc_length - data_length, stdin);

        if (!line) { break; }

        data_length += strlen(cursor);

        if (data_length < alloc_length - 1 || data[data_length - 1] == '\n') { break; }

        size_t new_length = alloc_length << 1;
        data = realloc(data, new_length);

        if (!data) { break; }

        alloc_length = new_length;
    }

    if (data[data_length - 1] == '\n') {
        data[data_length - 1] = '\0';
    }

    data = realloc(data, data_length);

    return data;
}
//========================= END TIP1

//============================ TIP 2: ===========================

#include "bsakr_commu.h"

Event_t arr_event[MAX_EVENT];
int32_t evt_num = 0;

int8_t* search_evt_obj(int32_t flgid) {
    return arr_event[flgid].name;
}

bool check_timeout(int32_t timeout) {
    if (timeout < BSA_WAIT_FOREVER) {
        return false;
    }
    else {
        return true;
    }
}

bool check_mode(int32_t mode) {
    if ((mode != WAIT_EVTFLG_AND) && (mode != WAIT_EVTFLG_OR)) {
        return false;
    }
    else {
        return true;
    }
}

bool get_mode(int32_t mode) {
    return (mode == WAIT_EVTFLG_AND) ? true : false;
}

int32_t get_timeout(int32_t timeout) {
    int32_t ret_time;
    if (timeout == BSA_WAIT_FOREVER) {
        ret_time = INFINITE;
    }
    else {
        ret_time = timeout;
    }
    return ret_time;
}

int32_t bsa_create_msg(int8_t *name, int32_t msgbfsz, int32_t maxbytes)
{
    return 0;
}
int32_t bsakr_create_evtflg(int8_t *name, uint32_t flgptn)
{
#ifdef _WIN32
    HANDLE evt_obj[BIT_WIDTH];
    bool dup_flgptn;
    int32_t ret_cre;

    //dup_flgptn = flgptn;
    for (int i = 0; i < BIT_WIDTH; i++) {
        dup_flgptn = ((flgptn >> i) & (uint8_t)0x1);
        evt_obj[i] = CreateEventA(NULL, true, dup_flgptn, (LPCSTR)name);
        if (!evt_obj) {
            dump_message_dummy("There is no spcace for creating event\n");
            return E_LIMIT;
        }
        arr_event[evt_num].hdl_obj[i] = evt_obj;
    }
    memcpy(arr_event[evt_num].name, name, strlen((char*)name));
    arr_event[evt_num].id = evt_num;
    ret_cre = evt_num;
    //arr_event[evt_num]->hdl_obj = evt_obj;
    //memcpy(arr_event[evt_num]->name, name, strlen((char*)name));
    evt_num++;
    return ret_cre;
#endif
}

int32_t bsakr_wait_evtflg(int32_t flgid, uint32_t flgptn, uint32_t *p_flgptn, int32_t mode, int32_t timeout)
{
    HANDLE ptn_obj[BIT_WIDTH];
    bool bit_ptn, bwaitall;
    int32_t idx_ptn_obj, inter_time;

    idx_ptn_obj = 0;
    if (!search_evt_obj(flgid)) {
        dump_message_dummy("Flag id is not exist\n");
        return E_NOEXS;
    }

    if ((!check_timeout(timeout)) || (!check_mode(mode))) {
        dump_message_dummy("Argument is invalid\n");
        return E_PAR;
    }

    for (int i = 0; i < BIT_WIDTH; i++) {
        bit_ptn = (flgptn >> i) & 0x1;
        if (bit_ptn) {
            ptn_obj[idx_ptn_obj++] = arr_event[flgid].hdl_obj[i];
        }
    }
    bwaitall = get_mode(mode);
    inter_time = get_timeout(timeout);
    *p_flgptn = WaitForMultipleObjects(idx_ptn_obj, ptn_obj, bwaitall, inter_time);
    
    return E_OK;
}

int32_t bsakr_set_evtflg(int32_t flgid, uint32_t flgptn)
{
    return 0;
}

int32_t bsakr_clear_evtflg(int32_t flgid, uint32_t flgptn)
{
    return 0;
}

