#pragma once
#include "struct.h"
#include <iostream>
#include <fstream>
#include <stack>
#include <stdio.h>
#include <Windows.h>
#include <conio.h>
#include <time.h>
using namespace std;

//内存i节点数组，NUM为该文件系统容纳的文件数  
struct inode usedinode[7280];
//ROOT的内存i节点  
struct inode *root;
//当前节点
struct inode *nowInode;
//已经打开文件的数组 
struct file* opened[];
//超级块  
struct supblock *super;
//模拟磁盘
FILE * unixDisk;
//当前用户
struct user* nowUser;
//当前文件或者目录名字（通过文件，获取名字和文件的inode id）
struct direct nowDirectory;
//当前目录
struct dir* curdir;
//退出
bool logout = false;
//用户文件节点
struct inode* userinode;
int userpos;

