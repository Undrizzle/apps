#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <pwd.h>
#include <crypt.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <dirent.h>
#include <ctype.h>
#include <net/if.h>
#include <net/route.h>
#include <string.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <typedefs.h>
#include <bcmadsl.h>
#include <ifcdefs.h>
#include <public.h>
#include "syscall.h"
#include "http2cmm.h"

//#define IFC_LARGE_LEN        264

#if 0
int __macs2b(const char *str, unsigned char *bin)
{
	int result, i;
	unsigned int mac[6];
	
	if((str == NULL) || (bin == NULL))
	{
		return -1;
	}
	
	if( strlen(str) == 0 )
	{
		return -1;
	}
	
	result = sscanf(str,"%2X:%2X:%2X:%2X:%2X:%2X",
						mac + 0, mac + 1, mac + 2,
						mac + 3, mac + 4, mac + 5);
	if( 6 == result )
	{
		for(i = 0; i < 6; i++)
		{
			bin[i] = mac[i];
		}
		return 0;
	}
	else
	{
		return -1;
	}
}


char * __getSModByMid(uint16_t mid)
{
	switch(mid)
	{
		case MID_SNMP:
		{
			return "MID_SNMP";
		}
		case MID_CLI:
		{
			return "MID_CLI";
		}
		case MID_HTTP:
		{
			return "MID_HTTP";
		}
		case MID_CMM:
		{
			return "MID_CMM";
		}
		case MID_ALARM:
		{
			return "MID_ALARM";
		}
		case MID_LLOG:
		{
			return "MID_LLOG";
		}
		case MID_DBA:
		{
			return "MID_DBA";
		}
		case MID_REGISTER:
		{
			return "MID_REG";
		}
		case MID_MMEAD:
		{
			return "MID_MMEAD";
		}
		case MID_SQL:
		{
			return "MID_SQL";
		}
		case MID_TEMPLATE:
		{
			return "MID_TM";
		}case MID_DBS:
		{
			return "MID_DBS";
		}
		case MID_SYSMONITOR:
		{
			return "MID_MON";
		}
		case MID_SYSEVENT:
		{
			return "MID_EVENT";
		}
		default:
		{
			return "MID_OTHER";
		}
	}
}

char * __getSDevTypeByModel(uint32_t model)
{
	switch(model)
	{
		case WEC_3501I_X7:
		{
			return "WEC-3501I X7";
		}
		case WEC_3501I_E31:
		{
			return "WEC-3501I E31";
		}
		case WEC_3501I_C22:
		{
			return "WEC-3501I C22";
		}
		case WEC_3501I_S220:
		{
			return "WEC-3501I S220";
		}
		case WEC_3501I_S60:
		{
			return "WEC-3501I S60";
		}
		case WEC_3501I_Q31:
		{
			return "WEC-3501I Q31";
		}
		case WEC_3601I:
		{
			return "WEC-3601I";
		}
		case WEC_3602I:
		{
			return "WEC-3602I";
		}
		case WEC_3604I:
		{
			return "WEC-3604I";
		}
		case WEC_3702I:
		{
			return "WEC-3702I L2";
		}
		case WEC_3703I:
		{
			return "WEC-3703I L3";
		}
		case WEC_3704I:
		{
			return "WEC-3704I L4";
		}
		case WEC_602:
		{
			return "WEC-3702I C2";
		}
		case WEC_604:
		{
			return "WEC-3702I C4";
		}
		case WEC9720EK_C22:
		{
			return "WEC9720EK C22";
		}
		case WEC9720EK_E31:
		{
			return "WEC9720EK E31";
		}
		case WEC9720EK_Q31:
		{
			return "WEC9720EK Q31";
		}
		case WEC9720EK_S220:
		{
			return "WEC9720EK S220";
		}
		case WEC9720EK_SD220:
		{
			return "WEC9720EK SD220";
		}
		case WEC701_C2:
		{
			return "WEC701 C2";
		}
		case WEC701_C4:
		{
			return "WEC701 C4";
		}
		default:
		{
			return "UNKNOWN";
		}
	}
}


int bcmGetIntfNameSocket(int socketfd, char *intfname)
{
   int i = 0, fd = 0;
   int numifs = 0, bufsize = 0;
   struct ifreq *all_ifr = NULL;
   struct ifconf ifc;
   struct sockaddr local_addr;
   socklen_t local_len = sizeof(struct sockaddr_in);

   memset(&ifc, 0, sizeof(struct ifconf));
   memset(&local_addr, 0, sizeof(struct sockaddr));

   if (getsockname(socketfd, &local_addr,&local_len) < 0) {
     printf("bcmGetIntfNameSocket: Error in getsockname!\n");
     return -1;
   }

   //printf("bcmGetIntfNameSocket: Session comes from: %s\n",inet_ntoa(((struct sockaddr_in *)&local_addr)->sin_addr));
   
   if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
     printf("bcmGetIntfNameSocket: Error openning socket when getting socket intface info\n");
     return -1;
   }

   numifs = 16;

   bufsize = numifs*sizeof(struct ifreq);
   all_ifr = (struct ifreq *)malloc(bufsize);
   if (all_ifr == NULL) {
      printf("bcmGetIntfNameSocket: out of memory!\n");
      close(fd);
      return -1;
   }

   ifc.ifc_len = bufsize;
   ifc.ifc_buf = (char *)all_ifr;
   if (ioctl(fd, SIOCGIFCONF, &ifc) < 0) {
      printf("bcmGetIntfNameSocket: Error getting interfaces\n");
      close(fd);
      free(all_ifr);
      return -1;
   }

   numifs = ifc.ifc_len/sizeof(struct ifreq);
   //printf("bcmGetIntfNameSocket: numifs=%d\n",numifs);
   for (i = 0; i < numifs; i ++) {
	   //printf("bcmGetIntfNameSocket: intface name=%s\n",all_ifr[i].ifr_name); 
	   struct in_addr addr1,addr2;
	   addr1 = ((struct sockaddr_in *)&(local_addr))->sin_addr;
	   addr2 = ((struct sockaddr_in *)&(all_ifr[i].ifr_addr))->sin_addr;
	   if (addr1.s_addr == addr2.s_addr) {
		strcpy(intfname, all_ifr[i].ifr_name);
	   	break;
	   }
   }

   close(fd);
   free(all_ifr);
   return 0;
}
#endif

int sysFlashSizeGet(void)
{
	return 1024*1024*20;
}

void bcmKillAllApps(void)
{
	system("killall snmpd");
	system("killall register");
	system("killall mmead");
	system("killall tmcore");
	printf("killall snmpd\n");
	printf("killall register\n");
	printf("killall mmead\n");
	printf("killall tmcore\n");
}

void bcmSystemReboot(void)
{
	printf("bcmSystemReboot\n");
	http2cmm_sysReboot();
} 

