#pragma once
#include "Init.h"
#include "format.h"

struct inode* FindFileByShell(char *shell) {
	char head = shell[0];
	struct inode *tmp = (struct inode*)calloc(1, sizeof(struct inode));
	tmp = root;
	if (head == '/') {

		//输入的是绝对路径
		struct dir*root = (struct dir*)calloc(1, sizeof(struct dir));	//申请root内存空间
		struct dir*tmp_dir = (struct dir*)calloc(1, sizeof(struct dir));
		tmp_dir = root;
		for (int i = 0; i < tmp_dir->dirNum; i++) {
			char *p = strtok(shell, "/");
			while (p) {
				//依次判断路径
				if (strcmp(p, tmp_dir->direct[i].directName) == 0) {
					NULL;
				}
				NULL;
			}

		}
	}
	return tmp;

}