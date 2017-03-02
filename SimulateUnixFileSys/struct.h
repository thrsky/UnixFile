#include "define.h"
#include <time.h>

// 一共是8M的磁盘
// 第0块留着  第一块放超级块
// 后面是iNode表
// 再后面是block


//超级块
struct supblock
{
	unsigned int size;				
	unsigned int freeBlock[BLOCKNUM];
	unsigned int nextFreeBlock;	
	unsigned int freeBlockNum;
	unsigned int freeInode[INODENUM];
	unsigned int freeInodeNum;	
	unsigned int nextFreeInode;	
	unsigned int lastLogin;
	char FsInfo[128];
};

//文件中的i节点
struct finode
{
	int mode;
	long int fileSize;
	int fileLink;
	char owner[MAXNAME];
	char group[GROUPNAME];
	long int modifyTime;
	long createTime;
	int addr[6];
	char black[45];				
};

//内存中的I节点
struct inode
{
	struct finode finode;
	struct inode *parent;
	unsigned short int inodeID;
	int userCount;			
};

struct direct
{
	char directName[DIRECTNAME];
	unsigned short int	inodeID;
};

struct dir
{
	int dirNum;
	struct direct direct[DIRNUM];
};

//文件  
struct file
{
	struct inode *fInode;         
    int f_curpos; 
};


//用户
struct user
{
	char userName[MAXNAME];
	char userPwd[MAXPASSWORD];
	char userGroup[MAXNAME];
	time_t lastLogin_time;
};