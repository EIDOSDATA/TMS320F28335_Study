
/*
 * ap.c
 *
 *  Created on: 2024. 5. 30.
 *      Author: ShinSung Industrial Electric
 */
#include "ap.h"

#include "cli.h"
#include "tagdb.h"

extern Task_Handle   Task_Cli;

void CliTask(UArg arg0, UArg arg1)
{
    CliOpen(SCI_C, 115200);

    while(1)
    {
        TASK_SLEEP(1);

        if(CliMain() == false)
        {
            /*Task Block forever*/
            TASK_SET_PRI(Task_Cli, -1);
        }
    }
}


void apInit(void)
{
    TagDbInit();
    CliInit();

}

void apMain(void)
{
    DEBUG_Msg("Hello tmw dnp stack \n");

    BIOS_start();    /* does not return */

}
