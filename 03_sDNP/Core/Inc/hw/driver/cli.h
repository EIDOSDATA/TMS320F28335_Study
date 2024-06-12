/*
 * Cli.h
 *
 *  Created on: 2023. 9. 12.
 *      Author: ShinSung Industrial Electric
 */

#ifndef COMPONENTS_CLI_INTERFACE_CLI_H_
#define COMPONENTS_CLI_INTERFACE_CLI_H_

#include "hw_def.h"

#define CLI_PRINT_BUF_MAX       256
#define CLI_CMD_NAME_MAX 32
#define CLI_LINE_BUF_MAX 32
#define CLI_CMD_LIST_MAX 16

#define CLI_PROMPT_STR            "\r\nCLI# "


typedef struct
{
    uint16      argc;
    uint16      **argv;

    uint16      (*getData)(uint16 index);
    float32     (*getFloat)(uint16 index);
    char        *(*getStr)(uint16 index);
    bool        (*isStr)(uint16 index, char *p_str);

} cli_args_t;

typedef struct
{
    char cmd_str[CLI_CMD_NAME_MAX];

    void (*cmd_func)(cli_args_t *);
} cli_cmd_t;

typedef struct
{
    uint16 buf[CLI_LINE_BUF_MAX];
    uint16 buf_len;
    uint16 cursor;
    uint16 count;

} cli_line_t;

typedef struct
{
    uint16 ch;
    uint16 baudrate;

    uint16 is_open;
    uint16 is_log;

    uint16 log_ch;
    uint16 log_baudrate;

    uint16 state;
    uint16 print_buffer[CLI_PRINT_BUF_MAX];

    uint16 argc;
    uint16 *argv[4];

    uint16 hist_line_new;
    uint16 hist_line_i;
    uint16 hist_line_last;
    uint16 hist_line_count;

    cli_line_t  line_buf[4];
    cli_line_t  line;

    uint16      cmd_count;
    cli_cmd_t   cmd_list[CLI_CMD_LIST_MAX];
    cli_args_t  cmd_args;

} cli_t;
/*
 * Public Function
 */

void CliPrintf(char *fmt, ...);
bool CliAdd(const char *cmd_str, void (*p_func)(cli_args_t *));
void CliInit(void);
bool CliOpen(uint16 ch, uint32 baudrate);

void CliUpdate(uint16 rx_data);
void CliSendCheck(void);
bool CliMain(void);

#endif /* COMPONENTS_CLI_INTERFACE_CLI_H_ */
