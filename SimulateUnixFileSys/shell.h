#pragma once
#include "Init.h"
#include "command.h"
#include <iomanip>
int dispatcher()
{
	char shell[8192] = { 0 };
	char command[8192] = { 0 };
	char ch;
	
	cout << nowUser->userName << "@LIKANG_MINT: ";
	pwd();
	cout << "#";
	ch = getchar();
	int i = 0;
	if (ch == 10)
		return 0;
	while (ch != 10)
	{
		shell[i] = ch;
		ch = getchar();
		i++;
	}
	_strlwr(shell);
	strcpy(command, shell);
	strtok(command, " ");
	if (strcmp(command, "ls") == 0)
	{
		ls();
	}
	else if (strcmp(command, "ll") == 0)
	{
		ll();
	}
	else if (strcmp(command, "mkdir") == 0)
	{
		strCpy(command, shell, strlen(command) + 1);
		mkdir(command);
	}
	else if (strcmp(command, "cd") == 0)
	{
		inode* inode = NULL, *ret = NULL;
		strCpy(command, shell, strlen(command) + 1);
		if (command[0] == '/')
			inode = root;
		else
			inode = nowInode;
		ret = cd(command, inode);
		if (ret != NULL)
			nowInode = ret;
	}
	else if (strcmp(command, "passwd") == 0)
	{
		strCpy(command, shell, strlen(command) + 1);
		passwd();
	}
	else if (strcmp(command, "chmod") == 0)
	{
		strCpy(command, shell, strlen(command) + 1);
		chmod(command);
	}
	else if (strcmp(command, "mv") == 0)
	{
		strCpy(command, shell, strlen(command) + 1);
		mv(command);
	}
	else if (strcmp(command, "touch") == 0)
	{
		strCpy(command, shell, strlen(command) + 1);
		touch(command);
	}
	else if (strcmp(command, ">>") == 0)
	{
		strCpy(command, shell, strlen(command) + 1);
		append(command);
	}
	else if (strcmp(command, "cat") == 0)
	{
		strCpy(command, shell, strlen(command) + 1);
		cat(command);
	}
	else if (strcmp(command, "rm") == 0) {
		strCpy(command,shell, strlen(command) + 1);
		rm(nowInode, command);
	}
	else if (strcmp(command, "rmdir") == 0)
	{
		strCpy(command, shell, strlen(command) + 1);
		rmdir(nowInode, command);
	}
	else if (strcmp(command, "chgrp") == 0)
	{
		strCpy(command, shell, strlen(command) + 1);
		chgrp(command);
	}
	else if (strcmp(command, "chown") == 0)
	{
		strCpy(command, shell, strlen(command) + 1);
		chown(command);
	}
	else if (strcmp(command, "cp") == 0)
	{
		strCpy(command, shell, strlen(command) + 1);
		cp(command);
	}
	else if (strcmp(command, "pwd") == 0) {
		pwd();	cout << endl;
	}

	/*
	else if (strcmp(command, "ln") == 0) {
		strCpy(command, shell, strlen(command) + 1);
		ln(command);
	}
	*/

	else if (strcmp(command, "info") == 0)
		superInfo();
	else if (strcmp(command, "exit") == 0)
		logout = true;
	else if (strcmp(command, "kernal") == 0) {
		cout << "unix file system 1.1" << endl;
	}
	else if (strcmp(command, "test") == 0) {
		test();
	}
	else if (strcmp(command, "help") == 0) {
		cout <<white<< endl;
		cout << green << setw(10) << setiosflags(ios::left) << "ls:" << white << "�����ǰĿ¼�����пɼ��ļ�" << endl;
		cout << green << setw(10) << setiosflags(ios::left) << "ll:" << white << "�����ǰĿ¼�������ļ���������Ϣ" << endl;
		cout << green << setw(10) << setiosflags(ios::left) << "touch:" << white << "�½�һ���ļ�" << endl;
		cout << green << setw(10) << setiosflags(ios::left) << "cat:" << white << "���һ���ļ��������" << endl;
		cout << green << setw(10) << setiosflags(ios::left) << "rm:" << white << "ɾ��һ���ļ�" << endl;
		cout << green << setw(10) << setiosflags(ios::left) << "mkdir:" << white << "�½�һ���ļ���" << endl;
		cout << green << setw(10) << setiosflags(ios::left) << "rmdir:" << white << "ɾ��һ���ļ���" << endl;
		cout << green << setw(10) << setiosflags(ios::left) << "pwd:" << white << "�����ǰĿ¼�ľ���·��" << endl;
		cout << green << setw(10) << setiosflags(ios::left) << "chmod:" << white << "�ı��ļ�Ȩ��" << endl;
		cout << green << setw(10) << setiosflags(ios::left) << "chown:" << white << "�޸��ļ�������" << endl;
		cout << green << setw(10) << setiosflags(ios::left) << "chgrp:" << white << "�޸��ļ�����Ⱥ" << endl;
		cout << green << setw(10) << setiosflags(ios::left) << "cd:" << white << "��תĿ¼" << endl;
		cout << green << setw(10) << setiosflags(ios::left) << "mv:" << white << "�޸��ļ���" << endl;
		cout << green << setw(10) << setiosflags(ios::left) << "cp:" << white << "�����ļ�" << endl;
		cout << green << setw(10) << setiosflags(ios::left) << "help:" << white << "��ʾ������Ϣ" << endl << endl;
	}
	else {
		cout << "" << command << " : command not found" << endl;
	}
	//strCpy(command,shell,strlen(command)+1);
	return 1;
}

