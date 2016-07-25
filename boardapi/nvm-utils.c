/* forbidden cycle write if meet bad blocks when do setenv */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <fcntl.h>
#include <assert.h>

#ifndef __user
#define __user
#endif

#include <mtd-abi.h>
#include <public.h>

#define CFG_REDUNDAND_ENVIRONMENT 1

#define CFG_ENV_SECT_SIZE	(128*1024)
#define ENV_BASE_ADDR		0x60000
#define CFG_ENV_SIZE		CFG_ENV_SECT_SIZE

#ifdef CFG_REDUNDAND_ENVIRONMENT
# define ENV_HEADER_SIZE	(sizeof(uint32_t) + 1)
#else
# define ENV_HEADER_SIZE	(sizeof(uint32_t))
#endif

#define ENV_SIZE (CFG_ENV_SIZE - ENV_HEADER_SIZE)

typedef	struct environment_s 
{
	/* CRC32 over data bytes */
	uint32_t	crc;		
#ifdef CFG_REDUNDAND_ENVIRONMENT
	/* active/obsolete flags	*/
	unsigned char	flags;		
#endif
	/* Environment data */
	unsigned char	data[ENV_SIZE]; 
}
env_t;


static const uint32_t crc_table[256] = 
{
  0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
  0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
  0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
  0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
  0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
  0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
  0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
  0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
  0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
  0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
  0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
  0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
  0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
  0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
  0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
  0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
  0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
  0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
  0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
  0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
  0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
  0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
  0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
  0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
  0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
  0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
  0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
  0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
  0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
  0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
  0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
  0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
  0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
  0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
  0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
  0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
  0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
  0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
  0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
  0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
  0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
  0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
  0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
  0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
  0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
  0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
  0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
  0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
  0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
  0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
  0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
  0x2d02ef8dL
};

/* ========================================================================= */
#define DO1(buf)  crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);
/* ========================================================================= */

uint32_t crc32 (uint32_t crc, const unsigned char *buf, unsigned int len)
{
	crc = crc ^ 0xffffffffL;
	while (len >= 8)
	{
		DO8(buf);
		len -= 8;
	}
	if (len) do {
		DO1(buf);
	} while (--len);
	return crc ^ 0xffffffffL;
}

void __debug_printf_env(env_t *env_ptr)
{
	int i, j, k, nxt;
	assert(NULL != env_ptr);

	printf("env crc:		0x%08X\n", env_ptr->crc);
	printf("env flag:		%d\n", env_ptr->flags);
	printf("env settings:\n");
	for( i=0; env_ptr->data[i] != '\0'; i=nxt+1 )
	{
		for (nxt=i; env_ptr->data[nxt] != '\0'; ++nxt)
			;
		for (k=i; k<nxt; ++k)
			putc(env_ptr->data[k], stdout);
		putc('\n', stdout);
		
	}
	printf("\nEnvironment size: %d/%ld bytes\n", i, (ulong)ENV_SIZE);
	printf("\n");
}

int envmatch (unsigned char *s1, int i2, unsigned char *data)
{

	while (*s1 == data[i2++])
		if (*s1++ == '=')
			return(i2);
	if (*s1 == '\0' && data[i2-1] == '=')
		return(i2);
	return(-1);
}

void __do_env_get_parameter(env_t *nvm, const char *key, char *value)
{
	int oldval;
	unsigned char *env, *nxt = NULL;

	unsigned char *env_data = nvm->data;

	/*
	 * search if variable with this name already exists
	 */
	oldval = -1;
	for (env=env_data; *env; env=nxt+1)
	{
		for (nxt=env; *nxt; ++nxt)
			;
		if ((oldval = envmatch((unsigned char *)key, env-env_data, env_data)) >= 0)
			break;
	}
	
	if (oldval >= 0)
	{
		strcpy(value, env_data + oldval);
	}
	else
	{
		value[0] = '\0';
	}	
}

void __do_env_set_parameter(env_t *nvm, const char *key, const char *value)
{
	int   i, len, oldval;
	unsigned char *env, *nxt = NULL;
	//char *name = "ethaddr";

	unsigned char *env_data = nvm->data;

	/*
	 * search if variable with this name already exists
	 */
	oldval = -1;
	for (env=env_data; *env; env=nxt+1)
	{
		for (nxt=env; *nxt; ++nxt)
			;
		if ((oldval = envmatch((unsigned char *)key, env-env_data, env_data)) >= 0)
			break;
	}
	/*
	 * Delete any existing definition
	 */
	if (oldval >= 0)
	{
		if (*++nxt == '\0')
		{
			if (env > env_data) {
				env--;
			}
			else
			{
				*env = '\0';
			}
		}
		else
		{
			for (;;)
			{
				*env = *nxt++;
				if ((*env == '\0') && (*nxt == '\0'))
					break;
				++env;
			}
		}
		*++env = '\0';
	}

	/*
	 * Append new definition at the end
	 */
	for (env=env_data; *env || *(env+1); ++env)
		;
	if (env > env_data)
		++env;
	/*
	 * Overflow when:
	 * "name" + "=" + "val" +"\0\0"  > ENV_SIZE - (env-env_data)
	 */
	len = strlen(key) + 2;
	len += strlen(value) + 1;
	
	if (len > (&env_data[ENV_SIZE]-env)) {
		printf ("## Error: environment overflow, \"%s\" deleted\n", key);
		return;
	}
	while ((*env = *key++) != '\0')
		env++;
	*env = '=';
	while ((*++env = *value++) != '\0')
			;

	/* end is marked with double '\0' */
	*++env = '\0';
}

void __env_crc_update(env_t *nvm)
{
	nvm->crc = crc32(0, nvm->data, ENV_SIZE);
}

int __save_env(env_t *nvm, int index)
{
	int fd = 0;
	uint32_t envoffs = 0;
	struct erase_info_user erase;

	//printf("	Saving Environment to NAND...\n");

	fd = open("/dev/mtd2", O_RDWR);
	if( fd <= 0 )
	{
		printf("	ERROR: cannot open /dev/mtd2\n");
		return CMM_FAILED;
	}

	/* erase */
#ifdef CFG_REDUNDAND_ENVIRONMENT
	if( (0 == index ) || (index == 2 ) )
	{
		envoffs = 0;
	}
	else if( 1 == index )
	{
		envoffs = CFG_ENV_SIZE;
	}
	else
	{
		return CMM_FAILED;
	}
#endif
#if 0
	if( 0 != envoffs )
	{
		printf("	Erasing redundant Nand...\n");
	}
	else
	{
		printf("	Erasing Nand...\n");
	}
#endif
	erase.start = envoffs;	
	erase.length = CFG_ENV_SECT_SIZE;
	if( ioctl(fd, MEMERASE, &erase) != 0 )
	{
		printf("	ERROR: ioctl MEMERASE failed\n");
		close(fd);
		return CMM_FAILED;
	}
#if 0
	else
	{
		printf("	Erasing at 0x%X -- 100%% complete\n", ENV_BASE_ADDR + envoffs);
	}
#endif
	/* write */
	lseek(fd, envoffs, SEEK_SET);
	nvm->flags += 1;
	if( write(fd, nvm, sizeof(env_t)) != sizeof(env_t) )
	{
		if( 0 != envoffs )
		{
			printf("	ERROR: Writing to redundant Nand failed\n");
		}
		else
		{
			printf("	ERROR: Writing to Nand failed\n");
		}
		close(fd);
		return CMM_FAILED;
	}
#if 0
	if( 0 != envoffs )
	{
		printf("	Writing to redundant Nand... done\n");
	}
	else
	{
		printf("	Writing to Nand... done\n");
	}
#endif
	close(fd);
	return CMM_SUCCESS;
}


void env_destroy(env_t *p)
{
	if( NULL != p )
	{
		free(p);
		p = NULL;
	}
}

env_t *env_init(int *env_valid)
{
	int index = 0;
	int crc1_ok = 0;
	int crc2_ok = 0;
	size_t cnt = 0;
	int fd = 0;
	env_t *tmp_env1 = NULL;
	env_t *tmp_env2 = NULL;
	env_t *env_ptr = NULL;
	
	tmp_env1 = (env_t *)malloc(sizeof(env_t));
	if( NULL == tmp_env1 ) return NULL;

	tmp_env2 = (env_t *)malloc(sizeof(env_t));
	if( NULL == tmp_env2 ) return NULL;

	fd = open("/dev/mtd2", O_RDONLY);
	if( fd <= 0 )
	{
		free(tmp_env1);
		free(tmp_env2);
		return NULL;
	}
	
	if( (cnt = read(fd, tmp_env1, sizeof(env_t))) != sizeof(env_t) )
	{
		free(tmp_env1);
		free(tmp_env2);
		close(fd);
		return NULL;
	}

	if( (cnt = read(fd, tmp_env2, sizeof(env_t))) != sizeof(env_t) )
	{
		free(tmp_env1);
		free(tmp_env2);
		close(fd);
		return NULL;
	}

	crc1_ok = (crc32(0, tmp_env1->data, ENV_SIZE) == tmp_env1->crc);
	crc2_ok = (crc32(0, tmp_env2->data, ENV_SIZE) == tmp_env2->crc);

	if (!crc1_ok && !crc2_ok)
		index = 0;
	else if(crc1_ok && !crc2_ok)
		index = 1;
	else if(!crc1_ok && crc2_ok)
		index = 2;
	else {
		/* both ok - check serial */
		if(tmp_env1->flags == 255 && tmp_env2->flags == 0)
			index = 2;
		else if(tmp_env2->flags == 255 && tmp_env1->flags == 0)
			index = 1;
		else if(tmp_env1->flags > tmp_env2->flags)
			index = 1;
		else if(tmp_env2->flags > tmp_env1->flags)
			index = 2;
		else /* flags are equal - almost impossible */
			index = 1;
	}

	if (index == 1)
	{
		env_ptr = tmp_env1;
		free(tmp_env2);
	}		
	else if (index == 2)
	{
		env_ptr = tmp_env2;
		free(tmp_env1);
	}
	else
	{
		env_ptr = NULL;
		free(tmp_env1);
		free(tmp_env2);
	}

	*env_valid = index;

	close(fd);
	return env_ptr;
}

/* retuen  -1: failed; 0:good block; 1:bad block */
int check_block(char *devname, loff_t offs)
{
	int fd = 0;
	int ret = 0;

	fd = open(devname, O_RDONLY);
	if( fd <= 0 )
	{
		return -1;
	}

	if( (ret = ioctl(fd, MEMGETBADBLOCK, &offs)) < 0 )
	{
		close(fd);
		return -1;
	}
	else
	{
		close(fd);
		return ret?1:0;
	}
}

int get_mtd_info(char *devname, struct mtd_info_user *mtdinfo)
{
	int fd = 0;
	
	fd = open(devname, O_RDONLY);
	
	if( fd <= 0 )
	{
		return CMM_FAILED;
	}

	if( ioctl(fd, MEMGETINFO, mtdinfo) != 0 )
	{
		close(fd);
		return CMM_FAILED;
	}
	else
	{
		close(fd);
		return CMM_SUCCESS;
	}
}

int nvm_set_mt_parameters(stMTmsgInfo *para)
{
	int env_valid = 0;
	int isEnv1Bad = -1;
	int isEnv2Bad = -1;
	int opt_sts = CMM_SUCCESS;
	char strDevModel[32] = {0};
	
	env_t *env_ptr = NULL;	

	isEnv1Bad = check_block("/dev/mtd2", 0);
	isEnv2Bad = check_block("/dev/mtd2", CFG_ENV_SIZE);

	if( -1 == isEnv1Bad )
	{
		printf("	ERROR: check env block 1 failed\n");
		return CMM_FAILED;
	}
	if( -1 == isEnv2Bad )
	{
		printf("	ERROR: check env block 2 failed\n");
		return CMM_FAILED;
	}

	/* either env1 or env2 is bad block, we do not permit set operation */
	if( ( isEnv1Bad != 0 ) || ( isEnv2Bad != 0 ) )
	{
		printf("	ERROR: env is broken\n");
		return CMM_FAILED;
	}

	/* Get NVM */
	env_ptr = env_init(&env_valid);
	if( NULL == env_ptr )
	{
		printf("	ERROR: read env failed\n");
		return CMM_FAILED;
	}

	/* modify ethaddr */
	__do_env_set_parameter(env_ptr, "ethaddr", para->mac);
	/* modify devmodel */
	sprintf(strDevModel, "%d", para->model);
	__do_env_set_parameter(env_ptr, "devmodel", strDevModel);

	/* update env crc */
	__env_crc_update(env_ptr);
	
	/* save env to flash */
	if( CMM_SUCCESS != __save_env(env_ptr, env_valid) )
	{
		printf("	ERROR: save env failed\n\n");
		opt_sts = CMM_FAILED;
	}
	else
	{
		printf("	Success\n\n");
		opt_sts = CMM_SUCCESS;
	}

	/* free env_ptr malloced by function env_init() */
	env_destroy(env_ptr);	
	return opt_sts;
}

int nvm_set_parameter(const char *key, const char *value)
{
	int env_valid = 0;
	int isEnv1Bad = -1;
	int isEnv2Bad = -1;
	int opt_sts = CMM_SUCCESS;
	
	env_t *env_ptr = NULL;	

	isEnv1Bad = check_block("/dev/mtd2", 0);
	isEnv2Bad = check_block("/dev/mtd2", CFG_ENV_SIZE);

	if( -1 == isEnv1Bad )
	{
		printf("	ERROR: check env block 1 failed\n");
		return CMM_FAILED;
	}
	if( -1 == isEnv2Bad )
	{
		printf("	ERROR: check env block 2 failed\n");
		return CMM_FAILED;
	}

	/* either env1 or env2 is bad block, we do not permit set operation */
	if( ( isEnv1Bad != 0 ) || ( isEnv2Bad != 0 ) )
	{
		printf("	ERROR: env is broken\n");
		return CMM_FAILED;
	}

	/* Get NVM */
	env_ptr = env_init(&env_valid);
	if( NULL == env_ptr )
	{
		printf("	ERROR: read env failed\n");
		return CMM_FAILED;
	}

	/* modify ethaddr */
	__do_env_set_parameter(env_ptr, key, value);

	/* update env crc */
	__env_crc_update(env_ptr);
	
	/* save env to flash */
	if( CMM_SUCCESS != __save_env(env_ptr, env_valid) )
	{
		printf("	ERROR: save env failed\n\n");
		opt_sts = CMM_FAILED;
	}
	else
	{		
		opt_sts = CMM_SUCCESS;
	}

	/* free env_ptr malloced by function env_init() */
	env_destroy(env_ptr);	
	return opt_sts;
}

int nvm_get_parameter(const char *key, char *value)
{
	int env_valid = 0;
	int isEnv1Bad = -1;
	int isEnv2Bad = -1;
	
	env_t *env_ptr = NULL;	

	isEnv1Bad = check_block("/dev/mtd2", 0);
	isEnv2Bad = check_block("/dev/mtd2", CFG_ENV_SIZE);

	if( -1 == isEnv1Bad )
	{
		printf("	ERROR: check env block 1 failed\n");
		return CMM_FAILED;
	}
	if( -1 == isEnv2Bad )
	{
		printf("	ERROR: check env block 2 failed\n");
		return CMM_FAILED;
	}

	/* either env1 or env2 is bad block, we do not permit set operation */
	if( ( isEnv1Bad != 0 ) || ( isEnv2Bad != 0 ) )
	{
		printf("	ERROR: env is broken\n");
		return CMM_FAILED;
	}

	/* Get NVM */
	env_ptr = env_init(&env_valid);
	if( NULL == env_ptr )
	{
		printf("	ERROR: read env failed\n");
		return CMM_FAILED;
	}

	/* get parameter */
	__do_env_get_parameter(env_ptr, key, value);	

	/* free env_ptr malloced by function env_init() */
	env_destroy(env_ptr);	
	return CMM_SUCCESS;
}

void nvm_dump(void)
{
	int env_valid = 0;
	int isEnv1Bad = -1;
	int isEnv2Bad = -1;
	
	struct mtd_info_user info;

	env_t *env_ptr = NULL;

	if( CMM_SUCCESS != get_mtd_info("/dev/mtd2", &info) )
	{
		printf("	ERROR: can not get mtd info\n");
		return;
	}
	
	isEnv1Bad = check_block("/dev/mtd2", 0);
	isEnv2Bad = check_block("/dev/mtd2", CFG_ENV_SIZE);

	if( -1 == isEnv1Bad )
	{
		printf("	ERROR: check env block 1 failed\n");
		return;
	}
	if( -1 == isEnv2Bad )
	{
		printf("	ERROR: check env block 2 failed\n");
		return;
	}

	if( ( isEnv1Bad != 0 ) || ( isEnv2Bad != 0 ) )
	{
		printf("	ERROR: env is broken\n");
		return;
	}

	env_ptr = env_init(&env_valid);
	if( NULL == env_ptr )
	{
		printf("	ERROR: read env failed\n");
		return;
	}

	printf("nvm device name:	mtd2\n");
	printf("device type:		%d\n", info.type);
	printf("nvm total size:		%d KiB\n", info.size/1024);
	printf("block size:		%d KiB\n", info.erasesize/1024);
	printf("page size:		%d KiB\n", info.writesize/1024);
	printf("oob size:		%d bytes\n", info.oobsize);
	printf("bad blocks:		0\n");
	printf("env config size:	%d KiB\n", CFG_ENV_SIZE/1024);
	printf("env valid:		%d\n", env_valid);

	__debug_printf_env(env_ptr);
	
	env_destroy(env_ptr);
}


