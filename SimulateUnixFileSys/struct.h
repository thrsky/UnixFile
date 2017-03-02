#include "define.h"
#include <time.h>

// һ����8M�Ĵ���
// ��0������  ��һ��ų�����
// ������iNode��
// �ٺ�����block


//������
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

//�ļ��е�i�ڵ�
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

//�ڴ��е�I�ڵ�
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

//�ļ�  
struct file
{
	struct inode *fInode;         
    int f_curpos; 
};


//�û�
struct user
{
	char userName[MAXNAME];
	char userPwd[MAXPASSWORD];
	char userGroup[MAXNAME];
	time_t lastLogin_time;
};