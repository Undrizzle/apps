/* forbidden cycle write if meet bad blocks when do setenv */
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <public.h>
#include <boardapi.h>

void nvm_usage(void)
{
	printf("\nParameter error:\n");
	printf("\nUsage:\n");
	printf("	printenv\n");
	printf("	-- dump env settings\n");
	printf("	setenv <parameter> <value>\n");
	printf("	-- set env peremeter\n");
	printf("	-- e.g.: setenv ethaddr 30:71:B2:00:00:00\n");
	
	printf("\n\n");
}

void nvm_set_ethaddr(const char *ethaddr)
{	
	char mac[6];
	char macaddr[18] = {0};	
	
	/* parse mac address */
	if( CMM_SUCCESS != boardapi_macs2b(ethaddr, mac) )
	{
		printf("	ERROR: mac address invalid\n");
		return;
	}
	else
	{
		sprintf(macaddr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	}
	
	if( CMM_SUCCESS == nvm_set_parameter("ethaddr", macaddr) )
	{
		printf("	Success\n\n");
	}
}

void nvm_set_devmodel(const char *devmodel)
{		
	if( CMM_SUCCESS == nvm_set_parameter("devmodel", devmodel) )
	{
		printf("	Success\n\n");
	}
}

int main(int argc, char *argv[])
{
	unsigned int phyaddr = 0;
	unsigned int regnum = 0;
	unsigned int regvalue = 0;
	char strValue[256] = {0};	

	printf("\n");

	if( argc == 1 )
	{
		if( strcmp(argv[0], "printenv") == 0 )
		{
			nvm_dump();
			return 0;
		}
		else
		{
			nvm_usage();
			return 0;
		}
	}
	else if( argc == 2 )
	{
		if( strcmp(argv[0], "getenv") == 0 )
		{
			nvm_get_parameter(argv[1], strValue);
			printf("	%s\n\n", strValue);
		}		
		else
		{
			nvm_usage();
			return 0;
		}
	}
	else if( argc == 3 )
	{
		if( strcmp(argv[0], "setenv") == 0 )
		{
			if( strcmp(argv[1], "ethaddr") == 0 )
			{
				nvm_set_ethaddr(argv[2]);
				return 0;
			}
			else if( strcmp(argv[1], "devmodel") == 0 )
			{
				nvm_set_devmodel(argv[2]);
				return 0;
			}
			else
			{
				nvm_usage();
				return 0;
			}
		}		
		else
		{
			nvm_usage();
			return 0;
		}
	}
	else
	{
		nvm_usage();
		return 0;
	}
}

