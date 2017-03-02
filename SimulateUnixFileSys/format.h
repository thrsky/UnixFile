#pragma once
#include "Init.h"

/*
int initUnixDisk() {

	fopen_s(&unixDisk, "unix.dat", "w+");
	EnterSuperBlock("unix.dat");
	SaveSuperBlock();
	fseek(unixDisk, ZEROBLOCK + SUPERBLOCKSIZE, SEEK_SET);
	
	struct inode* tmpinode = (struct inode*)calloc(1, sizeof(struct inode));
	for (int i = 0; i < INODESNUM; i++) {
		tmpinode->inodeID = i;
		tmpinode->userCount = 0;
		tmpinode->finode.fileLink = 0;
		fseek(unixDisk, ZEROBLOCK + SUPERBLOCKSIZE + i*INODESIZE, SEEK_SET);
		fwrite(&tmpinode, sizeof(struct inode*), 1, unixDisk);
	}
	char block[1024];
	for (int j = 0; j < BLOCKSNUM; j++) {
		fseek(unixDisk, ZEROBLOCK + SUPERBLOCKSIZE + INODESIZE + j*BLOCKSIZE, SEEK_SET);
		fwrite(&block, sizeof(block), 1, unixDisk);
	}

	struct direct *root_dir = (struct direct*)calloc(1, sizeof(struct direct));
	root_dir->inodeID = 0;
	strcpy(root_dir->directName, "root");
	struct inode*root = (struct inode*)calloc(1, sizeof(struct inode));
	root->inodeID = 0;
	root->parent = 0;
	strcpy(root->finode.owner, "root");
	strcpy(root->finode.group, "root");
	time_t time_now;

	nowInode = root;
	root->finode.mode = 1744;
	mkdir("etc");
	mkdir("bin");


	return 1;
}
*/
int SyncInode(struct inode* inode)
{
	int ino;
	int ret;
	int ipos;
	ipos = ZEROBLOCK + SUPERBLOCKSIZE + inode->inodeID * sizeof(struct finode);
	fseek(unixDisk, ipos, SEEK_SET);
	ret = fwrite(&inode->finode, sizeof(struct finode), 1, unixDisk);
	fflush(unixDisk);
	if (ret != 1)
		return 0;
	return 1;
}

//写入超级块
int EnterSuperBlock(char* path)
{
	unixDisk = fopen(path, "r+");
	if (unixDisk == NULL)
	{
		return 0;
	}
	super = (struct supblock*)calloc(1, sizeof(struct supblock));
	fseek(unixDisk, ZEROBLOCK, SEEK_SET);								
	fread(super, sizeof(struct supblock), 1, unixDisk);

	fseek(unixDisk, BLOCKSTART * 1024, SEEK_SET);
	
	for (int i = 0; i<20; i++)
		super->freeBlock[i] = 912 + i;
	super->nextFreeBlock = BLOCKNUM;
	super->size = 8 * 1024 * 1024;
	super->nextFreeInode = INODENUM;
	strcpy_s(super->FsInfo, "simulate unix file system 1.1");
	for (int i = 0; i<INODENUM; i++)
	{
		super->freeInode[i] = i;
	}
	super->freeBlockNum = BLOCKSNUM;
	super->freeInodeNum = INODESNUM;

	return 1;
}

//格式化 磁盘
int FormatDisk(char * path)
{
	unixDisk = fopen(path, "r+");
	if (unixDisk == NULL)
	{
		return 0;
	}
	fseek(unixDisk, BLOCKSTART * 1024, SEEK_SET);
	unsigned int group[BLOCKNUM];
	for (int i = 0; i<BLOCKNUM; i++)
	{
		group[i] = i + 912;
	}
	for (int i = 0; i<363; i++)
	{
		for (int j = 0; j<BLOCKNUM; j++)
		{
			group[j] += BLOCKNUM;
		}
		fseek(unixDisk, (BLOCKSTART + i * 20) * 1024, SEEK_SET);			
		fwrite(group, sizeof(unsigned int), BLOCKNUM, unixDisk);
	}
	return 1;
}
//
int SaveSuperBlock()
{
	if (unixDisk == NULL)
		return 0;
	fseek(unixDisk, ZEROBLOCK, SEEK_SET);
	int writeSize = fwrite(super, sizeof(struct supblock), 1, unixDisk);
	fflush(unixDisk);
	if (writeSize != 1)
		return 0;
	return 1;
}
int loadSuper(char *path)
{
	unixDisk = fopen(path, "r+");
	if (unixDisk == NULL)
	{
		return 0;
	}
	super = (struct supblock*)calloc(1, sizeof(struct supblock));	
	fseek(unixDisk, ZEROBLOCK, SEEK_SET);								
	int readSize = fread(super, sizeof(struct supblock), 1, unixDisk);
	if (readSize != sizeof(struct supblock))
		return 0;
	return 1;
}

//获取对应的I节点信息
struct inode * GetInode(int ino)
{
	int ipos = 0;
	int ret = 0;
	if (usedinode[ino].userCount != 0)
	{
		usedinode[ino].userCount++;
		return &usedinode[ino];
	}
	if (unixDisk == NULL)
		return NULL;
	ipos = ZEROBLOCK + SUPERBLOCKSIZE + ino*INODE;
	fseek(unixDisk, ipos, SEEK_SET);
	ret = fread(&usedinode[ino], sizeof(struct finode), 1, unixDisk);
	if (ret != 1)
		return NULL;
	if (usedinode[ino].finode.fileLink == 0)		
	{
		usedinode[ino].finode.fileLink++;
		usedinode[ino].finode.fileSize = 0;
		usedinode[ino].inodeID = ino;
		time_t timer;
		time(&timer);
		usedinode[ino].finode.createTime = timer;
		super->freeInodeNum--;
		SyncInode(&usedinode[ino]);
	}
	usedinode[ino].userCount++;
	usedinode[ino].inodeID = ino;
	return &usedinode[ino];
}
//用于 链接文件  删除其中一个的时候
void PutInode(struct inode *inode)
{
	if (inode->userCount>0)
		inode->userCount--;
}


//分配I节点
struct inode* AllocInode()
{
	if (super->nextFreeInode == 0)
	{
		finode *tmp = (struct finode*)calloc(1, sizeof(struct finode));
		fseek(unixDisk, ZEROBLOCK + SUPERBLOCKSIZE, SEEK_SET);
		int m = 0;
		for (int i = 1; i<INODESNUM; i++)
		{
			fseek(unixDisk, ZEROBLOCK + SUPERBLOCKSIZE + i*INODE, SEEK_SET);
			fread(tmp, sizeof(struct finode), 1, unixDisk);
			if (tmp->fileLink == 0)
			{
				super->freeInode[m] = i;
				super->nextFreeInode++;
				m++;
			}
			if(super->nextFreeInode==20){
				break;
			}
		}
	}
	if (super->freeInodeNum == 0)
		return NULL;
	super->nextFreeInode--;
	SaveSuperBlock();
	return GetInode(super->freeInode[super->nextFreeInode]);
}

int AllocBlock()
{
	//已经没有block可以分配了
	if (super->freeBlockNum <= 0) {
		cout << "no block can use" << endl;
		return 0;
	}
	//超级块中只剩下一块可用block
	if (super->nextFreeBlock == 1)
	{
		unsigned int blockNo = super->freeBlock[--super->nextFreeBlock];
		fseek(unixDisk, super->freeBlock[0] * BLOCKSIZE, SEEK_SET);
		int ret = fread(super->freeBlock, sizeof(unsigned int), BLOCKNUM, unixDisk);
		if (ret < 20) {
			cout<<"磁盘中的空闲空间已经不足20个，请谨慎使用"<<endl;
		}
		super->nextFreeBlock = ret;
		return blockNo;
	}
	super->freeBlockNum--;
	super->nextFreeBlock--;
	SaveSuperBlock();
	return super->freeBlock[super->nextFreeBlock];
}
void getTime(long int timeStamp)
{
	time_t timer;
	timer = timeStamp;
	struct tm *p;
	p = gmtime(&timer);
	char s[80];
	strftime(s, 80, "%Y-%m-%d %H:%M:%S", p);
	printf("%s", s);
}
//写入block块
int WriteBlock(void * _Buf, unsigned short int bno, long int offset, int size, int count = 1)
{
	int ret;
	fseek(unixDisk,bno*BLOCKSIZE+offset, SEEK_SET);
	ret = fwrite(_Buf, size, count, unixDisk);
	fflush(unixDisk);
	if (ret != count)
		return 0;//写入的block块不止一个 出现了点问题
	return 1;
}

int ReadBlock(void * _Buf, unsigned short int bno, int offset, int size, int count = 1)
{
	int ret;
	fseek(unixDisk, bno*BLOCKSIZE + offset, SEEK_SET);
	ret = fread(_Buf, size, count, unixDisk);
	if (ret != count)
		return 0;
	return 1;
}
int FreeBlock(int bno)
{
	if (super->nextFreeBlock == 20)
	{
		WriteBlock(&super->freeBlock, bno, 0, sizeof(unsigned int), 20);
		super->nextFreeBlock = 1;
		super->freeBlock[0] = bno;
	}
	else
	{
		super->freeBlock[super->nextFreeBlock] = bno;
		super->nextFreeBlock++;
	}
	super->freeBlockNum++;
	SaveSuperBlock();
	return 1;
}
void getMode(int mode)
{
	int type = mode / 1000;
	int auth = mode % 1000;
	if (type == 1)
		cout << "d";
	else
		cout << "-";
	int div = 100;
	for (int i = 0; i<3; i++)
	{
		int num = auth / div;
		auth = auth%div;
		int a[3] = { 0 };
		int time = 2;
		while (num != 0)
		{
			a[time--] = num % 2;
			num = num / 2;
		}
		for (int i = 0; i<3; i++)
		{
			if (a[i] == 1)
			{
				if (i == 2)
					cout << "x";
				else if (i == 1)
					cout << "w";
				else if (i == 0)
					cout << "r";
			}
			else
				cout << "-";
		}
		div /= 10;
	}
}
void info(inode* inode)
{
	cout << "mode:" << inode->finode.mode << endl;
	getMode(inode->finode.mode);
	cout << endl;
	cout << "owner:";
	cout.write(inode->finode.owner, MAXNAME);
	cout << endl;
	cout << "group:";
	cout.write(inode->finode.group, GROUPNAME);
	cout << endl;
	getTime(inode->finode.createTime);
	cout << endl;
	cout << "FileLink:" << inode->finode.fileLink << endl;
	cout << "fileSize:" << inode->finode.fileSize << endl;
	for (int i = 0; i<6; i++)
		cout << "addr" << i << ":" << inode->finode.addr[i] << endl;
}
void superInfo()
{
	cout << "size:" << super->size << endl;
	cout << "freeBlock:" << endl;
	for (int i = 0; i<super->nextFreeBlock; i++)
		cout << super->freeBlock[i] << " ";
	cout << endl;
	cout << "freeInode:" << endl;
	for (int i = 0; i<super->nextFreeInode; i++)
		cout << super->freeInode[i] << " ";
	cout << "nextFreeInode:" << super->nextFreeInode << endl;
	cout << "nextFreeBlock:" << super->nextFreeBlock << endl;
	cout << "freeBlockNum:" << super->freeBlockNum << endl;
	cout << "freeInodeNum:" << super->freeInodeNum << endl;
}