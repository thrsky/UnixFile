#pragma once
#include "Init.h"
#include "format.h"

struct inode* FindFileByShell(char *shell) {
	char head = shell[0];
	struct inode *tmp = (struct inode*)calloc(1, sizeof(struct inode));
	tmp = root;
	if (head == '/') {

		//������Ǿ���·��
		struct dir*root = (struct dir*)calloc(1, sizeof(struct dir));	//����root�ڴ�ռ�
		struct dir*tmp_dir = (struct dir*)calloc(1, sizeof(struct dir));
		tmp_dir = root;
		for (int i = 0; i < tmp_dir->dirNum; i++) {
			char *p = strtok(shell, "/");
			while (p) {
				//�����ж�·��
				if (strcmp(p, tmp_dir->direct[i].directName) == 0) {
					NULL;
				}
				NULL;
			}

		}
	}
	return tmp;

}