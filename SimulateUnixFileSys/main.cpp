#include "format.h"
#include "command.h"
#include "shell.h"
#include "function.h"
void main()
{
	/*
	struct inode* tmpInode = (struct inode*)calloc(1, sizeof(struct inode));
	tmpInode = GetInode(20);
	time_t timer;
	time(&timer);
	tmpInode->finode.createTime = timer;
	SyncInode(tmpInode);
	*/
	//initUnixDisk();
	loadSuper("unixDisk.dat");
	//cout << "welcome" << endl;
	//cout << super->FsInfo << endl;
	root=GetInode(0);
	//cout << root->finode.fileLink << endl;
	while(!login())
	{
		NULL;
	}
	nowInode=root;
	while(!logout)
	{
		
		dispatcher();
	}

	
}