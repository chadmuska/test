/*--------------------------------------------------------------------------*
*  File name	 : App_jet_cmd.c
*  Function	     : "Parse jet-opto Command"
*  First editor: 
*  First date  : 
*  History:
*  2018.04.12  Weikai Hsu     "Added File Header"
*  
*  
*  
*--------------------------------------------------------------------------*/
#include "common.h"
#include <stdio.h>
#include <string.h>

#include "api/sp5k_global_api.h"
#include "api/sp5k_fs_api.h"
#include "api/sp5k_os_api.h"
#include "api/sp5k_disk_api.h"

char appPowerOffCommand[32] = {'\0'};


/*--------------------------------------------------------------------------*
* Function name : parseCommand 
* Function		: Parse Jet commands.
* Return value	:  static void 
* Parameter1	: char *buf
* Parameter2	: UINT32 size
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void parseCommand(char *buf, UINT32 size)
{
    char *t, *s;

    printf("START jetcmd\n");
    s = buf;
    do {
		char *cmd;
        cmd = s;
        t = strchr(s, '\n');
        if (t) {
            s = t + 1;
            if (*(t - 1) == '\r')
                t--;
            *t = '\0';
        }

        if (strlen(cmd)) {
            if (cmd[0] == ':') {
                cmd++;
                printf("jetcmd set power off command : %s\n", cmd);
                strncpy(appPowerOffCommand, cmd, sizeof(appPowerOffCommand)-1);
            } else {
                printf("jetcmd run : %s\n", cmd);
                sp5kCmdExec(cmd);
            }
        }
    } while (t != NULL);

}


/*--------------------------------------------------------------------------*
* Function name : cmdJetCmd 
* Function		: Run and parse Jet command in files
* Return value	:  void 
* Parameter1	: UINT32 argc
* Parameter2	: UINT8 *argv[]
* Parameter3	: UINT32 v[]
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void cmdJetCmd(UINT32 argc, UINT8 *argv[], UINT32 v[])
{
    if (argc != 2) {
        printf("jetcmd\n"
                "\tjetcmd filename: run command on \"filename\"\n");
    } else {
		UINT32 fd, srclen;
    	char *srcbuf;
	
        fd = sp5kFsFileOpen((const UINT8 *)argv[1], SP5K_FS_OPEN_RDONLY);
        if (fd == 0) {
            printf("%s: open file \"%s\" fail\n", argv[0], argv[1]);
            return;
        }
        srclen = sp5kFsFileSizeGet(fd);
        srcbuf = sp5kMalloc(srclen+1);
        if (srcbuf==NULL)
        {
            sp5kFsFileClose(fd);
            printf("%s: no memory for read file\n", argv[0]);
            return;
        }
        sp5kFsFileRead(fd, (UINT8 *)srcbuf, srclen);
        sp5kFsFileClose(fd);
        srcbuf[srclen] = 0;

        parseCommand(srcbuf, srclen);
    }
    return;
}
