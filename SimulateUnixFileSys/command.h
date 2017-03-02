#pragma once
#include <windows.h>
#include "Init.h"
#include "format.h"
#include "color.h"

int login()
{
	char ch;
	struct dir* dir = (struct dir*)calloc(1, sizeof(struct dir));
	ReadBlock(dir, root->finode.addr[0], 0, sizeof(struct dir));
	//cout << "root file num" << dir->dirNum << endl;
	
	userinode = GetInode(dir->direct[0].inodeID);
	int usernum = userinode->finode.fileSize / sizeof(user);
	struct user* users = (struct user*)calloc(usernum, sizeof(struct user));
	ReadBlock(users, userinode->finode.addr[0], 0, sizeof(struct user), usernum);
	char user[MAXNAME] = { 0 }, pwd[MAXPASSWORD] = { 0 };
	cout << "login as user:";
	cin.getline(user, MAXNAME);
	for (int i = 0; i < usernum; i++)
	{
		//cout << users[i].userName << users[i].userPwd << endl;
		if (strcmp(users[i].userName, user) == 0)
		{
			cout << "password:";
			for (int i = 0; i < MAXPASSWORD; i++)
			{
				ch = _getch();
				if (ch == 13)
				{
					break;
				}
				pwd[i] = ch;
				cout << " ";
			}
			cout << endl;
			if (strcmp(users[i].userPwd, pwd) == 0)
			{
				nowUser = &users[i];
				userpos = i;
				time_t timer;
				time(&timer);
				cout << "last login:";
				getTime(super->lastLogin);
				cout << endl;
				super->lastLogin = timer + 8 * 60 * 60;
				SaveSuperBlock();
				return true;
			}
			else
			{
				cout << "password is not match!" << endl;
				return false;
			}
		}
	}
	cout << "User does not exist" << endl;
	return false;

//获取在str中特定字符n的位置
}int strPos(char* str, int start, const char n)
{
	for (int i = start; i < strlen(str); i++)
	{
		if (str[i] == n)
			return i;
	}
	return -1;
}
int strCpy(char *dst, char *src, int offset)
{
	int len = strlen(src);
	if (len <= offset)
		return 0;
	int i;
	for (i = 0; i < len - offset; i++)
	{
		dst[i] = src[i + offset];
	}
	dst[i] = 0;
	return 1;
}

//截取 字符串中 特定字符串
int subStr(char* src, char*dst, int start, int end = -1)
{
	int pos = 0;
	end == -1 ? end = strlen(src) : NULL;
	for (int i = start; i < end; i++)
		dst[pos++] = src[i];
	dst[pos] = 0;
	return 1;
}
//回到 父节点
void cd__()
{
	int inodeid = nowInode->inodeID;
	inode* inode = nowInode->parent;
	int count = inode->finode.fileSize / sizeof(struct direct);
	dir * dir = (struct dir*)calloc(1, sizeof(struct dir));
	int addrnum = count / 63 + (count % 63 >= 1 ? 1 : 0);
	addrnum > 4 ? addrnum = 4 : NULL;
	for (int addr = 0; addr < addrnum; addr++)
	{
		ReadBlock(dir, inode->finode.addr[addr], 0, sizeof(struct dir));
		for (int i = 0; i < dir->dirNum; i++)
		{
			if (dir->direct[i].inodeID == inodeid)
			{
				count = -1;
				nowDirectory = dir->direct[i];
				break;
			}
		}
	}
}

inode* cd(char* path, inode* inode)
{
	bool foundFile = false;
	if (path[0] == '/'&&strlen(path) == 1)
	{
		nowInode = root;
		return NULL;
	}
	char tmp[16] = { 0 };
	int start = path[0] == '/';
	int pos = strPos(path, 1, '/');
	subStr(path, tmp, start, pos);
	//cout << tmp << endl;
	if (strcmp(tmp, "..") == 0) {
		if (nowInode->parent != NULL)
		{
			struct inode* tmp = nowInode->parent;
			nowInode = tmp;
			if (nowInode->inodeID != 0)
				cd__();
		}
		
		return NULL;
	}
	int type = inode->finode.mode / 1000;
	//判断现在的iNode 是否是一个目录  不是的话 报错
	if (type == 1)
	{
		int count = inode->finode.fileSize / sizeof(struct direct);
		int addrnum = count / 63 + (count % 63 >= 1 ? 1 : 0);
		dir * dir = (struct dir*)calloc(1, sizeof(struct dir));
		addrnum > 4 ? addrnum = 4 : NULL;
		for (int addr = 0; addr < addrnum; addr++)
		{
			ReadBlock(dir, inode->finode.addr[addr], 0, sizeof(struct dir));
			for (int i = 0; i < dir->dirNum; i++)
			{
				if (strcmp(dir->direct[i].directName, tmp) == 0)
				{
					foundFile = true;
					struct inode*tmpInode = (struct inode*)calloc(1, sizeof(struct inode));
					tmpInode = GetInode(dir->direct[i].inodeID);
					if (tmpInode->finode.mode / 1000 == 2) {
						cout << "bash: cd: " << dir->direct[i].directName << ": Not a directory " << endl;
					}
					else {
						count = -1;
						nowDirectory = dir->direct[i];
						struct inode * tmpnode = GetInode(dir->direct[i].inodeID);
						tmpnode->parent = inode;
						inode = tmpnode;
						break;
					}
					//if(dir->direct[i].inodeID)

				}
			}
			if (count == -1)
				break;
		}
		if (count != -1)
			inode = NULL;
		if (!foundFile) {
			cout << "bash : cd: " << tmp << ": No such file or directory" << endl;
		}
	}
	else
	{
		inode = NULL;
		return inode;
	}
	if (pos != -1 && inode != NULL)
	{
		subStr(path, tmp, pos + 1);
		return cd(tmp, inode);
	}
	if (pos == -1)
		return inode;

	
}
int ls()
{
	int type = nowInode->finode.mode / 1000;
	if (type != 1)
	{
		cout << "this command only can used in directory" << endl;
		return 0;
	}
	int count = nowInode->finode.fileSize / sizeof(struct direct);
	dir * dir = (struct dir*)calloc(1, sizeof(struct dir));
	int addrnum = count / 63 + (count % 63 >= 1 ? 1 : 0);
	struct inode*tmpinode = (struct inode*)calloc(1, sizeof(struct inode));
	addrnum > 4 ? addrnum = 4 : NULL;
	for (int addr = 0; addr < addrnum; addr++)
	{
		ReadBlock(dir, nowInode->finode.addr[addr], 0, sizeof(struct dir));
		for (int i = 0; i < dir->dirNum; i++)
		{
			tmpinode = GetInode(dir->direct[i].inodeID);
			if (tmpinode->finode.mode / 1000 == 2) {
				cout<<green<< dir->direct[i].directName <<white<<""<< endl;
			}
			else {
				cout <<white<< dir->direct[i].directName <<white<<""<< endl;
			}
			
		}
	}
	return 1;

}
int ll()
{
	int type = nowInode->finode.mode / 1000;
	if (type != 1)
	{
		cout << "this is not a dir" << endl;
		return 0;
	}
	int count = nowInode->finode.fileSize / sizeof(struct direct);
	dir * dir = (struct dir*)calloc(1, sizeof(struct dir));
	int addrnum = count / 63 + (count % 63 >= 1 ? 1 : 0);
	addrnum > 4 ? addrnum = 4 : NULL;
	//是root
	getMode(nowInode->finode.mode);
	cout << " ";
	cout.write(nowInode->finode.owner, strlen(nowInode->finode.owner));
	cout << " ";
	cout.write(nowInode->finode.group, strlen(nowInode->finode.group));
	cout << " ";
	cout << nowInode->inodeID;
	cout << " ";
	getTime(nowInode->finode.createTime);
	cout << " ";
	cout << "." << endl;
	
	if (nowInode->inodeID == 0) {

	}
	else {
		inode *parent = GetInode(nowInode->parent->inodeID);
		getMode(parent->finode.mode);
		cout << " ";
		cout.write(parent->finode.owner, strlen(parent->finode.owner));
		cout << " ";
		cout.write(parent->finode.group, strlen(parent->finode.group));
		cout << " ";
		cout << parent->inodeID;
		cout << " ";
		getTime(parent->finode.createTime);
		cout << " ";
		cout << ".." << endl;
	}
	
	for (int addr = 0; addr < addrnum; addr++)
	{
		ReadBlock(dir, nowInode->finode.addr[addr], 0, sizeof(struct dir));
		for (int i = 0; i < dir->dirNum; i++)
		{
			inode * inode = GetInode(dir->direct[i].inodeID);
			getMode(inode->finode.mode);
			cout << " ";
			cout.write(inode->finode.owner, strlen(inode->finode.owner));
			cout << " ";
			cout.write(inode->finode.group, strlen(inode->finode.group));
			cout << " ";
			cout << inode->inodeID;
			cout << " ";
			getTime(inode->finode.createTime);
			cout << " ";
			if (inode->finode.mode / 1000 == 2) {
				cout << green << dir->direct[i].directName <<white<<" "<< endl;
			}
			else {
				cout << white << dir->direct[i].directName <<white<<" "<< endl;
			}
			//puts(dir->direct[i].directName);
		}
	}
	//cout << white << endl;
	return 1;

}
int mkdir(char * dirname)
{
	if (nowInode->finode.mode / 1000 != 1)
	{
		cout << "it is not a dir" << endl;
		return -1;
	}
	int count = nowInode->finode.fileSize / sizeof(struct direct);
	if (count > 252)
	{
		cout << "can not make more dir in the nowInode dir!" << endl;
		return -1;
	}
	dir * dir = (struct dir*)calloc(1, sizeof(struct dir));
	int addrnum = count / 63 + (count % 63 >= 1 ? 1 : 0);
	addrnum > 4 ? addrnum = 4 : NULL;
	for (int addr = 0; addr < addrnum; addr++)
	{
		ReadBlock(dir, nowInode->finode.addr[addr], 0, sizeof(struct dir));
		for (int i = 0; i < dir->dirNum; i++)
			if (strcmp(dir->direct[i].directName, dirname) == 0)
			{
				cout.write(dirname, strlen(dirname));
				cout << " is exist in nowInode dir!" << endl;
				return -1;
			}
	}
	nowInode->finode.fileSize += sizeof(struct direct);
	SyncInode(nowInode);
	int addr = count / 63;
	ReadBlock(dir, nowInode->finode.addr[addr], 0, sizeof(struct dir));
	strcpy(dir->direct[dir->dirNum].directName, dirname);
	struct inode * tmpinode = AllocInode();
	tmpinode->finode.addr[0] = AllocBlock();
	tmpinode->finode.mode = 1774;
	strcpy(tmpinode->finode.owner, nowUser->userName);
	strcpy(tmpinode->finode.group, nowUser->userGroup);
	SyncInode(tmpinode);
	dir->direct[dir->dirNum].inodeID = tmpinode->inodeID;
	dir->dirNum += 1;
	WriteBlock(dir, nowInode->finode.addr[addr], 0, sizeof(struct dir));
	return 1;
}

void passwd()
{
	char ch;
	char passwd1[20] = { 0 };
	char passwd2[20] = { 0 };
	char passwd[20] = { 0 };
	cout << "please insert the old password:";
	for (int i = 0; i < MAXPASSWORD; i++)
	{
		ch = _getch();
		if (ch == 13)
		{
			break;
		}
		passwd[i] = ch;
		cout << "*";
	}
	cout << endl;
	if (strcmp(passwd, nowUser->userPwd) == 0)
	{
		cout << "new password:";
		for (int i = 0; i < MAXPASSWORD; i++)
		{
			ch = _getch();
			if (ch == 13)
			{
				break;
			}
			passwd1[i] = ch;
			cout << "*";
		}
		cout << endl;
		cout << "repeat the new password:";
		for (int i = 0; i < MAXPASSWORD; i++)
		{
			ch = _getch();
			if (ch == 13)
			{
				break;
			}
			passwd2[i] = ch;
			cout << "*";
		}
		cout << endl;
		if (strcmp(passwd1, passwd2) == 0)
		{
			strcpy(nowUser->userPwd, passwd1);
			WriteBlock(nowUser, userinode->finode.addr[0], userpos * sizeof(struct user), sizeof(struct user));
		}
		else
		{
			cout << "the password is not matched!" << endl;
		}
	}
	else
		cout << "the old password is not right!" << endl;

}
int chmod(char * para)
{
	int mode = 0;
	char dirname[100] = { 0 };
	mode += ((para[0] - '0') > 7 ? 7 : (para[0] - '0')) * 100 + ((para[1] - '0') > 7 ? 7 : (para[1] - '0')) * 10 + ((para[2] - '0') > 7 ? 7 : (para[2] - '0'));
	subStr(para, dirname, 4);
	int count = nowInode->finode.fileSize / sizeof(struct direct);
	dir * dir = (struct dir*)calloc(1, sizeof(struct dir));
	int addrnum = count / 63 + (count % 63 >= 1 ? 1 : 0);
	addrnum > 4 ? addrnum = 4 : NULL;
	for (int addr = 0; addr < addrnum; addr++)
	{
		ReadBlock(dir, nowInode->finode.addr[addr], 0, sizeof(struct dir));
		for (int i = 0; i < dir->dirNum; i++)
		{
			if (strcmp(dir->direct[i].directName, dirname) == 0)
			{
				inode* tmp = GetInode(dir->direct[i].inodeID);
				tmp->finode.mode = (tmp->finode.mode / 1000) * 1000 + mode;
				SyncInode(tmp);
				//info(tmp);
				return 1;
			}
		}
	}
	return -1;
}

//输出绝对路径
int pwd()
{
	stack<char*> pwd;
	struct inode * inode = nowInode;
	struct inode * parent = NULL;
	while (inode != root)
	{
		int inodeid = inode->inodeID;
		parent = inode->parent;
		int count = parent->finode.fileSize / sizeof(struct direct);
		dir * dir = (struct dir*)calloc(1, sizeof(struct dir));
		int addrnum = count / 63 + (count % 63 >= 1 ? 1 : 0);
		addrnum > 4 ? addrnum = 4 : NULL;
		for (int addr = 0; addr < addrnum; addr++)
		{
			ReadBlock(dir, parent->finode.addr[addr], 0, sizeof(struct dir));
			for (int i = 0; i < dir->dirNum; i++)
			{
				if (dir->direct[i].inodeID == inodeid)
				{
					pwd.push(dir->direct[i].directName);
					count = -1;
					break;
				}
			}
			if (count == -1)
				break;
		}
		inode = inode->parent;
	}
	if (pwd.empty()) {
		cout << "/";
		
	}
		
	else
	{
		while (!pwd.empty())
		{
			cout << "/";

			cout.write(pwd.top(), strlen(pwd.top()));

			pwd.pop();
		}
	}

	return 1;
}
int mv(char* names)
{
	char oldname[16] = { 0 }, newname[16] = { 0 };
	int pos = strPos(names, 0, ' ');
	subStr(names, oldname, 0, pos);
	subStr(names, newname, pos + 1);
	int count = nowInode->finode.fileSize / sizeof(struct direct);
	dir * dir = (struct dir*)calloc(1, sizeof(struct dir));
	int addrnum = count / 63 + (count % 63 >= 1 ? 1 : 0);
	addrnum > 4 ? addrnum = 4 : NULL;
	for (int addr = 0; addr < addrnum; addr++)
	{
		ReadBlock(dir, nowInode->finode.addr[addr], 0, sizeof(struct dir));
		for (int i = 0; i < dir->dirNum; i++)
		{
			if (strcmp(dir->direct[i].directName, oldname) == 0)
			{
				strcpy(dir->direct[i].directName, newname);
				WriteBlock(dir, nowInode->finode.addr[addr], 0, sizeof(struct dir));
				return 1;
			}
		}
	}
	return 1;
}
int touch(char * filename)
{
	if (nowInode->finode.mode / 1000 != 1)
	{
		cout << "it is not a dir" << endl;
		return -1;
	}
	int count = nowInode->finode.fileSize / sizeof(struct direct);
	if (count > 252)
	{
		cout << "can not make more dir in the nowInode dir!" << endl;
		return -1;
	}
	dir * dir = (struct dir*)calloc(1, sizeof(struct dir));
	int addrnum = count / 63 + (count % 63 >= 1 ? 1 : 0);
	addrnum > 4 ? addrnum = 4 : NULL;
	for (int addr = 0; addr < addrnum; addr++)
	{
		ReadBlock(dir, nowInode->finode.addr[addr], 0, sizeof(struct dir));
		
		for (int i = 0; i < dir->dirNum; i++)
			if (strcmp(dir->direct[i].directName, filename) == 0)
			{
				cout.write(filename, strlen(filename));
				cout << " is exist in nowInode dir!" << endl;
				return -1;
			}
	}
	nowInode->finode.fileSize += sizeof(struct direct);
	SyncInode(nowInode);
	int addr = count / 63;
	ReadBlock(dir, nowInode->finode.addr[addr], 0, sizeof(struct dir));
	strcpy(dir->direct[dir->dirNum].directName, filename);
	struct inode * tmpinode = AllocInode();
	tmpinode->finode.addr[0] = AllocBlock();
	tmpinode->finode.mode = 2774;
	strcpy(tmpinode->finode.owner, nowUser->userName);
	strcpy(tmpinode->finode.group, nowUser->userGroup);
	SyncInode(tmpinode);
	dir->direct[dir->dirNum].inodeID = tmpinode->inodeID;
	dir->dirNum += 1;
	WriteBlock(dir, nowInode->finode.addr[addr], 0, sizeof(struct dir));
	return 1;
}


int append(char * src)
{
	char filename[16] = { 0 }, content[8192] = { 0 };
	int pos = strPos(src, 0, ' ');
	int inodeid = -1;
	subStr(src, filename, 0, pos);
	subStr(src, content, pos + 1);

	int count = nowInode->finode.fileSize / sizeof(struct direct);
	dir * dir = (struct dir*)calloc(1, sizeof(struct dir));
	int addrnum = count / 63 + (count % 63 >= 1 ? 1 : 0);
	addrnum > 4 ? addrnum = 4 : NULL;
	for (int addr = 0; addr < addrnum; addr++)
	{
		ReadBlock(dir, nowInode->finode.addr[addr], 0, sizeof(struct dir));
		for (int i = 0; i < dir->dirNum; i++)
		{
			if (strcmp(dir->direct[i].directName, filename) == 0)
			{
				//
				inodeid = dir->direct[i].inodeID;
				count = -1;
				break;
			}
		}
		if (count == -1)
			break;
	}
	if (inodeid == -1)
	{
		cout << "can not found the file ";
		cout.write(filename, strlen(filename));
		cout << endl;
		return -1;
	}
	struct inode * inode = GetInode(inodeid);
	int fileSize = inode->finode.fileSize;
	int index = 0;
	int addr = fileSize / 1024;
	int offset = fileSize % 1024;
	int charCount = strlen(content) - index;
	int writeCount = (charCount > 1024 ? 1024 - offset : charCount);
	WriteBlock(&content[index], inode->finode.addr[addr], offset, sizeof(char), writeCount);
	index += writeCount;
	inode->finode.fileSize += writeCount;
	while (index < strlen(content) && addr < 4)
	{
		inode->finode.addr[++addr] = AllocBlock();
		offset = inode->finode.fileSize % 1024;
		charCount = strlen(content) - index;
		writeCount = (charCount > 1024 ? 1024 - offset : charCount);
		WriteBlock(&content[index], inode->finode.addr[addr], offset, sizeof(char), writeCount);
		inode->finode.fileSize += writeCount;
	}
	SyncInode(inode);
	return 1;
}

//读取文件
int cat(char * filename)
{
	int inodeid = 0;
	int count = nowInode->finode.fileSize / sizeof(struct direct);
	dir * dir = (struct dir*)calloc(1, sizeof(struct dir));
	int addrnum = count / DIRNUM + (count % DIRNUM >= 1 ? 1 : 0);
	//不考虑二级索引
	addrnum > 4 ? addrnum = 4 : NULL;
	for (int addr = 0; addr < addrnum; addr++)
	{
		ReadBlock(dir, nowInode->finode.addr[addr], 0, sizeof(struct dir));
		for (int i = 0; i < dir->dirNum; i++)
		{
			if (strcmp(dir->direct[i].directName, filename) == 0)
			{
				inodeid = dir->direct[i].inodeID;
				count = -1;
				break;
			}
		}
		if (count == -1)
			break;
	}
	if (inodeid == 0)
	{
		cout << "can not found the file ";
		cout.write(filename, strlen(filename));
		cout << endl;
		return -1;
	}
	struct inode* inode = GetInode(inodeid);
	int addr = inode->finode.fileSize / 1024;

	addr > 6 ? addr = 6 : NULL;
	int lastCount = inode->finode.fileSize % 1024;
	int i;
	for (i = 0; i < addr; i++)
	{
		char content[1024] = { 0 };
		ReadBlock(&content, inode->finode.addr[i], 0, sizeof(char), 1024);
		cout.write(content, 1024);
	}
	char lastcontent[1024] = { 0 };
	ReadBlock(&lastcontent, inode->finode.addr[i], 0, sizeof(char), lastCount);
	cout.write(lastcontent, strlen(lastcontent));
	cout << endl;
	return 1;
}


int RealRemove(struct inode* inode)
{
	struct inode * rminode = NULL;
	int count = inode->finode.fileSize / sizeof(struct direct);
	dir * dir = (struct dir*)calloc(1, sizeof(struct dir));
	int addrnum = count / 63 + (count % 63 >= 1 ? 1 : 0);
	addrnum > 4 ? addrnum = 4 : NULL;
	for (int addr = 0; addr < addrnum; addr++)
	{
		ReadBlock(dir, inode->finode.addr[addr], 0, sizeof(struct dir));
		for (int i = 0; i < dir->dirNum; i++)
		{
			rminode = GetInode(dir->direct[i].inodeID);
			//如果还是目录  递归删除
			if (rminode->finode.mode / 1000 == 1)
			{
				RealRemove(rminode);
			}
			//判断 该文件占了几个block
			int rmaddr = rminode->finode.fileSize / 1024 + (rminode->finode.fileSize % 1024 == 0 ? 0 : 1);
			if (rminode->finode.fileSize == 0)
				rmaddr = 1;
			for (int i = 0; i < rmaddr; i++)
				FreeBlock(rminode->finode.addr[i]);
			rminode->finode.fileLink = 0;
			//回收iNode
			if (super->nextFreeInode < 20) {
				super->nextFreeInode++;
				super->freeInode[super->nextFreeInode] == rminode->inodeID;
			}
			SyncInode(rminode);
		}
	}
	return 1;
}

int rm(struct inode*inode, char *filename) {
	bool foundFile = false;
	struct inode* rminode = NULL;
	int count = inode->finode.fileSize / sizeof(struct direct);
	dir * dir = (struct dir*)calloc(1, sizeof(struct dir));
	int addrnum = count / 63 + (count % 63 >= 1 ? 1 : 0);
	addrnum > 4 ? addrnum = 4 : NULL;
	for (int addr = 0; addr < addrnum; addr++)
	{
		ReadBlock(dir, inode->finode.addr[addr], 0, sizeof(struct dir));
		for (int i = 0; i < dir->dirNum; i++)
		{
			if (strcmp(dir->direct[i].directName, filename) == 0)
			{
				rminode = GetInode(dir->direct[i].inodeID);
				if (rminode->finode.mode / 1000 == 1) {
					cout << "rmdir: failed to remove " << filename << ": Not a directory   " << endl;
					return 0;
				}
				for (int j = i; j < dir->dirNum; j++)
					dir->direct[j] = dir->direct[j + 1];
				dir->dirNum--;
				WriteBlock(dir, inode->finode.addr[addr], 0, sizeof(struct dir));
				foundFile = true;
				count = -1;
				break;
			}
		}
		if (count == -1)
			break;
	}

	if (!foundFile) {
		cout << "rm: failed to remove " << filename << ": No such file or directory  " << endl;
		return 0;
	}
	RealRemove(rminode);
	int rmaddr = rminode->finode.fileSize / 1024 + (rminode->finode.fileSize % 1024 == 0 ? 0 : 1);
	if (rminode->finode.fileSize == 0)
		rmaddr = 1;
	for (int i = 0; i < rmaddr; i++)
		FreeBlock(rminode->finode.addr[i]);
	rminode->finode.fileLink = 0;
	super->freeInodeNum++;
	//回收iNode
	if (super->nextFreeInode < 20) {
		super->nextFreeInode++;
		super->freeInode[super->nextFreeInode] == rminode->inodeID;
	}
	SaveSuperBlock();
	loadSuper("unixDisk.dat");
	SyncInode(rminode);
	return 1;
}

int rmdir(struct inode* inode, char* filename)
{
	bool foundFile = false;
	struct inode* rminode = NULL;
	int count = inode->finode.fileSize / sizeof(struct direct);
	dir * dir = (struct dir*)calloc(1, sizeof(struct dir));
	int addrnum = count / 63 + (count % 63 >= 1 ? 1 : 0);
	addrnum > 4 ? addrnum = 4 : NULL;
	for (int addr = 0; addr < addrnum; addr++)
	{
		ReadBlock(dir, inode->finode.addr[addr], 0, sizeof(struct dir));
		for (int i = 0; i < dir->dirNum; i++)
		{
			if (strcmp(dir->direct[i].directName, filename) == 0)
			{
				rminode = GetInode(dir->direct[i].inodeID);
				if (rminode->finode.mode / 1000 == 2) {
					cout << "rmdir: failed to remove "<<filename<<": Not a directory   " << endl;
					return 0;
				}
				for (int j = i; j < dir->dirNum; j++)
					dir->direct[j] = dir->direct[j + 1];
				dir->dirNum--;
				WriteBlock(dir, inode->finode.addr[addr], 0, sizeof(struct dir));
				foundFile = true;
				count = -1;
				break;
			}
		}
		if (count == -1)
			break;
	}
	
	if (!foundFile) {
		cout << "rmdir: failed to remove " << filename << ": No such file or directory  " << endl;
		return 0;
	}
	RealRemove(rminode);
	int rmaddr = rminode->finode.fileSize / 1024 + (rminode->finode.fileSize % 1024 == 0 ? 0 : 1);
	if (rminode->finode.fileSize == 0)
		rmaddr = 1;
	for (int i = 0; i < rmaddr; i++)
		FreeBlock(rminode->finode.addr[i]);
	rminode->finode.fileLink = 0;
	//回收 该目录的i节点
	super->freeInodeNum++;
	if (super->nextFreeInode < 20) {
		super->nextFreeInode++;
		super->freeInode[super->nextFreeInode] == rminode->inodeID;
	}
	SaveSuperBlock();
	loadSuper("unixDisk.dat");
	SyncInode(rminode);
	return 1;
}

// 改变文件 所属组
int chgrp(char *src)
{
	char filename[16] = { 0 }, groupname[GROUPNAME] = { 0 };
	int pos = strPos(src, 0, ' ');
	int inodeid = -1;
	subStr(src, filename, 0, pos);
	subStr(src, groupname, pos + 1);
	
	if (strcmp(nowUser->userName, "root") != 0)
	{
		cout << "chmod: changing permissions of '"<<filename<<"': Operation not permitted" << endl;
		//cout << "Access deny!" << endl;
		return -1;
	}
	struct inode *group = (struct inode*)calloc(1, sizeof(struct inode));
	struct dir*group_dir = (struct dir*)calloc(1, sizeof(struct dir));
	try {
		group = GetInode(2);//2是GROUP  系统固定
	}
	catch (exception e) {
		cout << "系统文件损坏" << endl;
	}
	bool foundGroup = false;
	int count_group = group->finode.fileSize / sizeof(struct direct);
	int addrnum_gourp = count_group / 63 + (count_group % 63 >= 1 ? 1 : 0);
	addrnum_gourp > 4 ? addrnum_gourp = 4 : NULL;
	for (int addr_gourp = 0; addr_gourp < addrnum_gourp; addr_gourp++) {
		ReadBlock(group_dir, group->finode.addr[addr_gourp], 0, sizeof(struct dir));
		for (int i = 0; i < group_dir->dirNum; i++) {
			if (strcmp(group_dir->direct[i].directName, groupname) == 0) {
				//group文件里有 该用户组 可以赋值
				foundGroup = true;
				continue;
			}
		}
		if (count_group == -1)
			break;
	}
	if (!foundGroup) {
		cout << "no such group in system" << endl;
		return 0;
	}
	int count = nowInode->finode.fileSize / sizeof(struct direct);
	dir * dir = (struct dir*)calloc(1, sizeof(struct dir));
	int addrnum = count / 63 + (count % 63 >= 1 ? 1 : 0);
	addrnum > 4 ? addrnum = 4 : NULL;
	for (int addr = 0; addr < addrnum; addr++)
	{
		ReadBlock(dir, nowInode->finode.addr[addr], 0, sizeof(struct dir));
		for (int i = 0; i < dir->dirNum; i++)
		{
			if (strcmp(dir->direct[i].directName, filename) == 0)
			{
				inode * inode = GetInode(dir->direct[i].inodeID);
				strcpy(inode->finode.group, groupname);
				SyncInode(inode);
				
				count = -1;
				return 1;
			}
		}
		if (count == -1)
			break;
	}
	cout << "no such file ";
	puts(filename);
	return 1;
}


//改变文件 所属用户
int chown(char *src)
{
	bool foundUser = false;
	char filename[16] = { 0 }, owner[MAXNAME] = { 0 };
	int pos = strPos(src, 0, ' ');
	int inodeid = -1;
	subStr(src, filename, 0, pos);
	subStr(src, owner, pos + 1);
	if (strcmp(nowUser->userName, "root") != 0)
	{
		cout << "chmod: changing permissions of '" << filename << "': Operation not permitted" << endl;
		//cout << "Access deny!" << endl;
		return -1;
	}
	struct inode *group = (struct inode*)calloc(1, sizeof(struct inode));
	struct dir*group_dir = (struct dir*)calloc(1, sizeof(struct dir));
	try {
		group = GetInode(19);//2是GROUP  系统固定
	}
	catch (exception e) {
		cout << "系统文件损坏" << endl;
	}
	bool foundGroup = false;
	int count_group = group->finode.fileSize / sizeof(struct direct);
	int addrnum_gourp = count_group / 63 + (count_group % 63 >= 1 ? 1 : 0);
	addrnum_gourp > 4 ? addrnum_gourp = 4 : NULL;
	for (int addr_gourp = 0; addr_gourp < addrnum_gourp; addr_gourp++) {
		ReadBlock(group_dir, group->finode.addr[addr_gourp], 0, sizeof(struct dir));
		for (int i = 0; i < group_dir->dirNum; i++) {
			if (strcmp(group_dir->direct[i].directName, owner) == 0) {
				//user文件里有 该用户组 可以赋值
				foundUser = true;
				continue;
			}
		}
		if (count_group == -1)
			break;
	}
	if (!foundUser) {
		cout << "no such User in system" << endl;
		return 0;
	}
	int count = nowInode->finode.fileSize / sizeof(struct direct);
	dir * dir = (struct dir*)calloc(1, sizeof(struct dir));
	int addrnum = count / 63 + (count % 63 >= 1 ? 1 : 0);
	addrnum > 4 ? addrnum = 4 : NULL;
	for (int addr = 0; addr < addrnum; addr++)
	{
		ReadBlock(dir, nowInode->finode.addr[addr], 0, sizeof(struct dir));
		for (int i = 0; i < dir->dirNum; i++)
		{
			if (strcmp(dir->direct[i].directName, filename) == 0)
			{
				inode * inode = GetInode(dir->direct[i].inodeID);
				strcpy(inode->finode.owner, owner);
				SyncInode(inode);
				count = -1;
				return 1;
			}
		}
		if (count == -1)
			break;
	}
	cout << "can not find file ";
	puts(filename);
	return 1;
}
int cp(char * src)
{
	char srcfile[16] = { 0 }, newfile[GROUPNAME] = { 0 };
	int pos = strPos(src, 0, ' ');
	subStr(src, srcfile, 0, pos);
	subStr(src, newfile, pos + 1);
	int inodeid = 0;
	int count = nowInode->finode.fileSize / sizeof(struct direct);
	dir * dir = (struct dir*)calloc(1, sizeof(struct dir));
	int addrnum = count / 63 + (count % 63 >= 1 ? 1 : 0);
	addrnum > 4 ? addrnum = 4 : NULL;
	for (int addr = 0; addr < addrnum; addr++)
	{
		ReadBlock(dir, nowInode->finode.addr[addr], 0, sizeof(struct dir));
		for (int i = 0; i < dir->dirNum; i++)
		{
			if (strcmp(dir->direct[i].directName, srcfile) == 0)
			{
				inodeid = dir->direct[i].inodeID;
				count = -1;
				break;
			}
		}
		if (count == -1)
			break;
	}
	if (inodeid == 0)
	{
		cout << "can not found the file ";
		cout.write(srcfile, strlen(srcfile));
		cout << endl;
		return -1;
	}
	inode* srcinode = GetInode(inodeid);
	count = nowInode->finode.fileSize / sizeof(struct direct);
	if (count > 252)
	{
		cout << "can not make more dir in the nowInode dir!" << endl;
		return -1;
	}
	addrnum = count / 63 + (count % 63 >= 1 ? 1 : 0);
	addrnum > 4 ? addrnum = 4 : NULL;
	for (int addr = 0; addr < addrnum; addr++)
	{
		ReadBlock(dir, nowInode->finode.addr[addr], 0, sizeof(struct dir));
		for (int i = 0; i < dir->dirNum; i++)
			if (strcmp(dir->direct[i].directName, newfile) == 0)
			{
				cout.write(newfile, strlen(newfile));
				cout << " is exist in nowInode dir!" << endl;
				return -1;
			}
	}
	nowInode->finode.fileSize += sizeof(struct direct);
	SyncInode(nowInode);
	int addr = count / 63;
	ReadBlock(dir, nowInode->finode.addr[addr], 0, sizeof(struct dir));
	strcpy(dir->direct[dir->dirNum].directName, newfile);
	struct inode * tmpinode = AllocInode();
	//addr distribut
	count = srcinode->finode.fileSize / 1024;
	int srcaddr = srcinode->finode.fileSize / 1024 + (srcinode->finode.fileSize % 1024 == 0 ? 0 : 1);
	if (srcinode->finode.fileSize == 0)
		srcaddr = 1;
	for (int i = 0; i < srcaddr; i++)
	{
		tmpinode->finode.addr[i] = AllocBlock();
		char content[1024] = { 0 };
		ReadBlock(&content, srcinode->finode.addr[i], 0, sizeof(char), 1024);
		WriteBlock(&content, tmpinode->finode.addr[i], 0, sizeof(char), 1024);
	}
	tmpinode->finode.fileSize = srcinode->finode.fileSize;
	tmpinode->finode.mode = srcinode->finode.mode;
	strcpy(tmpinode->finode.owner, srcinode->finode.owner);
	strcpy(tmpinode->finode.group, srcinode->finode.group);
	SyncInode(tmpinode);
	dir->direct[dir->dirNum].inodeID = tmpinode->inodeID;
	dir->dirNum += 1;
	WriteBlock(dir, nowInode->finode.addr[addr], 0, sizeof(struct dir));
	return 1;
}

/*
int ln(char *src) {
	return 1;
}

*/

//测试代码
int test() {

	/*
	struct inode*temp = (struct inode*)calloc(1, sizeof(struct inode));
	struct dir*dir = (struct dir*)calloc(1, sizeof(struct dir));
	temp = GetInode(19);
	for (int i = 0; i < 4; i++) {
		ReadBlock (dir, temp->finode.addr[i], 0, sizeof(struct dir));
		for (int j = 0; j < 63; j++) {
			if (dir->direct[j].directName, "linux") {
				cout << "find" << endl;
				dir->direct[i].inodeID = 123;
				WriteBlock(dir, temp->finode.addr[i], 0, sizeof(struct dir));
			}
		}
	}
	*/
	/*
	struct inode* tempInode = (struct inode*)calloc(1, sizeof(struct inode));
	tempInode = GetInode(16);
	time_t t;
	time(&t);
	cout << t << endl;
	cout<<tempInode->finode.createTime;

	tempInode->finode.createTime = 1483879859;
	SyncInode(tempInode);
	return 1;
	*/
	return 1;
}


