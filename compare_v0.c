#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include "mymd5.h"

int checkdir (char *argv1, char *argv2, int slash1, int slash2);
int count(DIR *dp1, DIR *dp2);
int cmpfile(char *name1, char *name2);
int checkerr (int f1, int f2);

int main (int argc, char *argv[])
{
	// 命令行参数检测
    if (argc < 3)
    {
        fprintf (stderr, "./%s <dir1> <dir2>\n", argv[0]);
        exit(1);
    }

    int flag = -1;
    int i,j,slash1, slash2;
    char name1[100],name2[100];

	// 处理命令行输入的参数
    memset(name1,'0',100);
    memset(name2,'0',100);
    slash1=1;
    if(argv[1][strlen(argv[1])-1]=='/')
        slash1=2;
	// 复制不带路径名的目录名/文件名到name1
    for(i=strlen(argv[1])-slash1,j=0;(argv[1][i] !='/') && (i>=0);i--,j++)
        name1[j]=argv[1][i];
    name1[j]='\0';
    slash2=1;
    if(argv[2][strlen(argv[2])-1]=='/')
        slash2=2;
	// 复制不带路径名的目录名/文件名到name2
    for(i=strlen(argv[2])-slash2,j=0;(argv[2][i] !='/') && (i>=0);i--,j++)
        name2[j]=argv[2][i];
    name2[j]='\0';

    if (strcmp(name1,name2) == 0) // 比较目录名/文件名
    {
        DIR *dp1, *dp2;
		// 是文件
        if (((dp1 = opendir(argv[1])) == NULL && (dp2 = opendir(argv[2])) == NULL)) 
        {
            FILE *fp1, *fp2;
			// 进行文件内容比较
            if ( (fp1 = fopen(argv[1], "r")) != NULL && (fp2 = fopen(argv[2], "r")) != NULL)
            {
                flag = cmpfile (argv[1], argv[2]);
            }
            else
            {
            perror("opendir");
            exit(1); 
            }
        }
		// 是目录
		else if (((dp1 = opendir(argv[1])) != NULL && (dp2 = opendir(argv[2])) != NULL)) 
            flag = checkdir (argv[1], argv[2], slash1, slash2);
		// 一个是文件，一个是目录
		else 
			flag = 1;
    }

    if (flag == 0)
        printf ("%s和%s的内容一致\n", argv[1], argv[2]);
    else
        printf ("%s和%s的内容不一致\n", argv[1], argv[2]); 
	return 0;
}

int checkdir (char *argv1, char *argv2, int slash1, int slash2)
{
    DIR *dp1, *dp2;
    struct dirent *dirp1, *dirp2;
    struct stat buf1, buf2;

	// 打开目录dp1和(或)dp2失败
    if (((dp1 = opendir(argv1)) == NULL || (dp2 = opendir(argv2)) == NULL)) 
    {
            perror("opendir");
            exit(1); 
    }
	// 统计目录dp1和dp2各自的子目录个数
    if( count(dp1,dp2) == -1)
        return -1;
    
	// 重设读取目录的位置为开头位置
    rewinddir(dp1);
    rewinddir(dp2);
    while ((dirp1 = readdir(dp1)) != NULL)
    {
		// 忽略.和..
        if (strcmp(dirp1->d_name, ".")==0 || strcmp(dirp1->d_name, "..")==0)
            continue;
        while((dirp2 = readdir(dp2)) != NULL)
        {
			// 忽略.和..
			if (strcmp(dirp2->d_name, ".")==0 || strcmp(dirp2->d_name, "..")==0)
				continue;
            if (strcmp(dirp1->d_name, dirp2->d_name) != 0) // compare filename or dirname
                continue;
            char *name1 = (char *)malloc(sizeof(char)*1024);
            memset (name1, '\0', sizeof(char)*1024);
            strcpy (name1, argv1);
            if(slash1 == 1)
                strcat (name1, "/");
            strcat (name1, dirp1->d_name);
            int f1 = stat (name1, &buf1);
            char *name2 = (char *)malloc(sizeof(char)*1024);
            memset (name2, '\0', sizeof(char)*1024);
            strcpy (name2, argv2);
            if(slash2 == 1)
                strcat (name2, "/");
            strcat (name2, dirp2->d_name);
            int f2 = stat (name2, &buf2);

            if (checkerr(f1, f2) == -1)
                return -1;
            else if(S_ISDIR(buf1.st_mode) && S_ISDIR(buf2.st_mode))
            {
                if (checkdir(name1,name2,1,1) == -1)
                    return -1;
                free(name1);
                free(name2);
            }
            else
            {//file, compare the value of md5 of them 
                if (cmpfile(name1,name2) == -1)
                    return -1;          
            }
            break;
        }
        rewinddir(dp2);
        continue;
    }
    return 0;
}

/*
 * 调用myMD5函数进行文件内容比较
 */
int cmpfile(char *name1, char *name2)
{
    int i;
    unsigned int md5_1[16],md5_2[16];

    myMD5(name1, md5_1);
    myMD5(name2, md5_2);
    for(i=0;i<16;i++)
        if(md5_1[i] != md5_2[i])
            return -1;
    return 0;
}

/*
 * 统计目录dp1和dp2各自的子目录个数
 */
int count(DIR *dp1, DIR *dp2)
{
    struct dirent *dirp1, *dirp2;
    int i1, i2;
    i1=0;
    i2=0;
    while ((dirp1 = readdir(dp1)) != NULL)
    {
        // 忽略.和..
        if (strcmp(dirp1->d_name, ".")==0 || strcmp(dirp1->d_name, "..")==0)
            continue;
        i1++;
    }
    while((dirp2 = readdir(dp2)) != NULL)
    {
        // 忽略.和..
        if (strcmp(dirp2->d_name, ".")==0 || strcmp(dirp2->d_name, "..")==0)
            continue;
        i2++;
    }
    if (i1 != i2) // 子目录个数不等，说明不一致，返回-1
        return -1;
    return 0;
}

/*
 * 文件状态错误，错误代码存放在errno中，输出错误代码
 */
int checkerr( int f1, int f2)
{
    if(f1 == -1 || f2 == -1)
    {
        switch(errno)
        {
            case EACCES:puts("EACCES");break; // 存取文件时被拒绝
            case EBADF:puts("EBADF");break; // 
            case EFAULT:puts("EFAULT");break; // 参数f1/f2为无效指针
            case ENOENT:puts("ENOENT");break; // 参数f1/f2指向的文件不存在
            case ENOMEM:puts("ENOMEM");break; // 内存不足
            case ENOTDIR:puts("ENOTDIR");break; // 目录存在但不是真正的目录
            default:puts("UNKNOWN ERROR"); // 未知错误
        }
        return -1;
    }
    return 0; 
}
