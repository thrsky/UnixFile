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

//�ڴ�i�ڵ����飬NUMΪ���ļ�ϵͳ���ɵ��ļ���  
struct inode usedinode[7280];
//ROOT���ڴ�i�ڵ�  
struct inode *root;
//��ǰ�ڵ�
struct inode *nowInode;
//�Ѿ����ļ������� 
struct file* opened[];
//������  
struct supblock *super;
//ģ�����
FILE * unixDisk;
//��ǰ�û�
struct user* nowUser;
//��ǰ�ļ�����Ŀ¼���֣�ͨ���ļ�����ȡ���ֺ��ļ���inode id��
struct direct nowDirectory;
//��ǰĿ¼
struct dir* curdir;
//�˳�
bool logout = false;
//�û��ļ��ڵ�
struct inode* userinode;
int userpos;

