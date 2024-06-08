/*
 * Cli.c
 *
 *  Created on: 2023. 9. 12.
 *      Author: ShinSung Industrial Electric
 */
#include <stdlib.h>

#include "cli.h"
#include "uart.h"
#include "ringbuf.h"

#define GET_MIN(a,b)            (((a) > (b))? (b):(a))

#define CLI_KEY_BACK              0x08
#define CLI_KEY_DELETE            0x7F
#define CLI_KEY_ENTER             0x0D
#define CLI_LF                    0x0A
#define CLI_KEY_ESC               0x1B
#define CLI_KEY_LEFT              0x44
#define CLI_KEY_RIGHT             0x43
#define CLI_KEY_UP                0x41
#define CLI_KEY_DOWN              0x42
#define CLI_KEY_HOME              0x31
#define CLI_KEY_END               0x34

#define CLI_PROMPT_STR            "\r\nCLI# "

#define CLI_ARGS_MAX              32
#define CLI_PRINT_BUF_MAX         256

#define TXBUF_SIZE  256
#define RXBUF_SIZE  256

enum
{
  CLI_RX_IDLE,
  CLI_RX_SP1,
  CLI_RX_SP2,
  CLI_RX_SP3,
  CLI_RX_SP4,
};

cli_t cli_node;

static Ringbuf_t RxRingBufHd;
static Ringbuf_t TxRingBufHd;

uint16 TxBuffer[TXBUF_SIZE];
uint16 RxBuffer[RXBUF_SIZE];

/*
 * Forward declaration
 */
static void cliLineClear(cli_t *p_cli);
static void cliLineAdd(cli_t *p_cli);
static bool cliRunCmd(cli_t *p_cli);
static void cliShowList(cli_args_t *args);


static void cliLineAdd(cli_t *p_cli)
{
    p_cli->line_buf[p_cli->hist_line_last] = p_cli->line;

    if(p_cli->hist_line_count < 4)
    {
        p_cli->hist_line_count++;
    }

    p_cli->hist_line_i = p_cli->hist_line_last;
    p_cli->hist_line_last = (p_cli->hist_line_last + 1)% 4;
    p_cli->hist_line_new = true;

}

static bool cliIsStr(const char *p_arg, const char *p_str)
{
    if(strcmp(p_arg, p_str) == 0)
        return true;

    else
        return false;
}

static bool cliParseArgs(cli_t *p_cli)
{
    bool Rtn = false;
    char *tok;
    uint16 argc = 0;

    char *cmdline;
    char **argv;

    p_cli->argc = 0;

    cmdline = (char *)p_cli->line.buf;
    argv = (char **)p_cli->argv;

    argv[argc] = NULL;

    tok = strtok(cmdline, " ");

    while(tok != NULL)
    {
        argv[argc++] = tok;
        tok = strtok(NULL, " ");
    }

    p_cli->argc = argc;

    if(argc)
        Rtn = true;

    return Rtn;

}


static uint16 cliArgsGetData(uint16 index)
{
    uint16 Rtn = 0;
    cli_t *p_cli = &cli_node;

    if(index >= p_cli->cmd_args.argc)           return false;

    Rtn = (uint16)strtoul((const char *)p_cli->cmd_args.argv[index], (char **)NULL, (int)0);

    return Rtn;
}

static float32 cliArgsGetFloat(uint16 index)
{
    float32 Rtn = 0.0;
    cli_t *p_cli = &cli_node;

    if(index >= p_cli->cmd_args.argc)           return false;

    Rtn = (float32)strtof((const char *)p_cli->cmd_args.argv[index], (char **)NULL);

    return Rtn;
}

static char* cliArgsGetStr(uint16 index)
{
    char *Rtn = NULL;
    cli_t *p_cli = &cli_node;

    if(index >= p_cli->cmd_args.argc)           return false;

    Rtn = (char *)p_cli->cmd_args.argv[index];

    return Rtn;
}

static bool cliArgsIsStr(uint16 index, char *p_str)
{
    bool Rtn = false;
    cli_t *p_cli = &cli_node;

    if(index >= p_cli->cmd_args.argc)           return false;

    if(strcmp(p_str, (const char *)p_cli->cmd_args.argv[index]) == 0)
    {
        Rtn = true;
    }

    return Rtn;
}
static uint16 cliGetData(const char *p_arg)
{
    uint16 Rtn;

    Rtn = (uint16)strtoul(p_arg, (char **)NULL, 0);

    return Rtn;

}



bool CliOpen(uint16 ch, uint32 baudrate)
{
    cli_node.ch = ch;
    cli_node.baudrate = baudrate;

    UART_InitTypeDef Instance;

    /*Default Setting*/
    Instance.DataBits       = DATA_BITS_8;
    Instance.FlowControl    = MODE_NONE;
    Instance.Parity         = PARITY_NONE;
    Instance.StopBit        = STOP_BITS_1;

    /*Variable Value*/
    Instance.Baudrate = baudrate;

    cli_node.is_open = UART_Init(ch, &Instance);

    return cli_node.is_open;

}
static void cliLineClear(cli_t *p_cli)
{
    p_cli->line.count = 0;
    p_cli->line.cursor = 0;
    p_cli->line.buf_len = CLI_LINE_BUF_MAX - 1;
    p_cli->line.buf[0] = 0;
}


bool CliAdd(const char *cmd_str, void (*p_func)(cli_args_t *))
{
    bool Rtn = true;
    cli_t *p_cli = &cli_node;
    uint16 index;

    if(p_cli->cmd_count >= CLI_CMD_LIST_MAX)        return false;

    index = p_cli->cmd_count;

    strcpy(p_cli->cmd_list[index].cmd_str, cmd_str);
    p_cli->cmd_list[index].cmd_func = p_func;

    p_cli->cmd_count++;

    return Rtn;

}

void CliPrintf(char *fmt, ...)
{
    char buf[128];

    va_list args;
    uint16 len = 0;
    va_start(args, fmt);

    len = vsnprintf(buf, 128, fmt, args);
    va_end(args);

    Ringbuf_Write(&TxRingBufHd, buf, len);

}

static void cliShowList(cli_args_t *args)
{
    uint16 i;
    cli_t *p_cli = &cli_node;

    CliPrintf("\r\n------------- cmd list -------------\r\n");

    for(i=0; i<p_cli->cmd_count; i++)
    {
        CliPrintf("%s\r\n",p_cli->cmd_list[i].cmd_str);
    }

    CliPrintf("------------------------------------\r\n");
}

static void cliShowPrompt(cli_t *p_cli)
{
    CliPrintf(CLI_PROMPT_STR);
}

void Clifloat(cli_args_t *args)
{
    if(args->argc == 2 && args->isStr(0, "float"))
    {
        float32 tempf = 0.0;
        tempf = args->getFloat(1);
        CliPrintf("float = %f\r\n", tempf);
    }
    else
    {
        CliPrintf("Clifloat test\r\n");
    }
}
void CliInt(cli_args_t *args)
{
    if(args->argc == 2 && args->isStr(0, "int"))
    {
        uint16 temp = 0;
        temp = args->getData(1);
        CliPrintf("int = %d\r\n", temp);
    }
    else
    {
        CliPrintf("Cli int test\r\n");
    }
}
static bool cliRunCmd(cli_t *p_cli)
{
    bool Rtn = false;

    if(cliParseArgs(p_cli) == true)
    {
        uint16 i;
        for(i=0; i<p_cli->cmd_count; i++)
        {
            if(strcmp(p_cli->argv[0], p_cli->cmd_list[i].cmd_str) == 0)
            {
                p_cli->cmd_args.argc = p_cli->argc -1;
                p_cli->cmd_args.argv = &p_cli->argv[1];
                p_cli->cmd_list[i].cmd_func(&p_cli->cmd_args);
                break;
            }

        }
    }
    return Rtn;
}


void CliUpdate(uint16 rx_data)
{

    cli_t*      p_cli = &cli_node;
    cli_line_t* line  = &p_cli->line;

   if(p_cli->state == CLI_RX_IDLE)
   {
       switch(rx_data)
       {
           case CLI_KEY_ENTER:
               if(line->count)
               {
                   cliLineAdd(p_cli);
                   cliRunCmd(p_cli);
               }
               line->count = 0;
               line->cursor = 0;
               line->buf[0] = 0;
               cliShowPrompt(p_cli);
          break;
           case CLI_KEY_ESC:
               p_cli->state = CLI_RX_SP1;
               break;
           case CLI_KEY_DELETE:
               break;
           case CLI_KEY_BACK:
               if(line->count && line->cursor)
               {
                   if(line->cursor == line->count)
                   {
                       line->count--;
                       line->buf[line->count] = 0;
                   }
                   if(line->cursor < line->count)
                   {
                       uint16 i;
                       uint16 mov_len;
                       mov_len = line->count - line->cursor;

                       for(i=0; i<mov_len; i++)
                       {
                           line->buf[line->cursor + i -1] = line->buf[line->cursor +i];
                       }
                       line->count--;
                       line->buf[line->count] = 0;
                   }
               }

               if(line->cursor)
               {
                   line->cursor--;

                   CliPrintf("\b \b\x1b[1P");

               }
               break;

           default:
               if((line->count + 1) < line->buf_len)
               {
                   if(line->cursor == line->count)
                   {
                       Ringbuf_Write(&TxRingBufHd, &rx_data, 1);

                       line->buf[line->cursor] = rx_data;
                       line->count++;
                       line->cursor++;
                       line->buf[line->count] = 0;
                   }
                   if(line->cursor < line->count)
                   {
                       uint16 i;
                       uint16 mov_len;

                       mov_len = line->count - line->cursor;
                       for(i=0; i<mov_len; i++)
                       {
                           line->buf[line->count-i] = line->buf[line->count - i - 1];
                       }
                       line->buf[line->cursor] = rx_data;
                       line->count++;
                       line->cursor++;
                       line->buf[line->count] = 0;

                       CliPrintf("\x1B[4h%c\x1B[4l", rx_data);

                   }
               }

               break;

       }
   }

   switch(p_cli->state)
   {
       case CLI_RX_SP1:
           p_cli->state = CLI_RX_SP2;
           break;
       case CLI_RX_SP2:
           p_cli->state = CLI_RX_SP3;
           break;
       case CLI_RX_SP3:
           p_cli->state = CLI_RX_IDLE;

           if(rx_data == CLI_KEY_LEFT)
           {

           }
           break;
   }
}
void CliSendCheck(void)
{
    uint16 RingBufLen = Ringbuf_Length(&TxRingBufHd);

    if(RingBufLen > 0)
    {
        uint16 Available = UARTTx_Check(cli_node.ch);

        RingBufLen = RingBufLen > Available ? Available : RingBufLen;

        uint16 Buf[SCI_FIFO_MAX];

        Ringbuf_Read(&TxRingBufHd, Buf, RingBufLen);

        UART_Write(cli_node.ch, Buf, RingBufLen);

    }
}


void CliInit(void)
{
    cli_node.is_open    = false;
    cli_node.is_log     = false;
    cli_node.state      = CLI_RX_IDLE;

    cli_node.hist_line_i        = 0;
    cli_node.hist_line_last     = 0;
    cli_node.hist_line_count    = 0;
    cli_node.hist_line_new      = false;

    cli_node.cmd_count = 0;
    cli_node.cmd_args.getData   = cliArgsGetData;
    cli_node.cmd_args.getFloat  = cliArgsGetFloat;
    cli_node.cmd_args.getStr    = cliArgsGetStr;
    cli_node.cmd_args.isStr     = cliArgsIsStr;

    cliLineClear(&cli_node);

    Ringbuf_Create(&RxRingBufHd, &RxBuffer[0], RXBUF_SIZE);
    Ringbuf_Create(&TxRingBufHd, &TxBuffer[0], TXBUF_SIZE);

    CliAdd("help", cliShowList);

}
void CliClose(void)
{
    cli_node.is_open = false;
}

bool CliMain(void)
{
    uint16 receiveBytes = 0;
    uint16 Buf[SCI_FIFO_SIZE];
    if(cli_node.is_open != true)
    {
        return false;
    }

    receiveBytes = UART_Read(SCI_C, Buf);

    if(receiveBytes > 0)
    {
        Ringbuf_Write(&RxRingBufHd, Buf, receiveBytes);
    }

    if(Ringbuf_Length(&RxRingBufHd) > 0)
    {
        uint16 RxData;

        Ringbuf_Read(&RxRingBufHd, &RxData, 1);

        CliUpdate(RxData);
    }


    CliSendCheck();

    return true;
}

