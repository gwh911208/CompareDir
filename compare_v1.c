#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <openssl/md5.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

int checkdir (char *argv1, char *argv2, int blank);
int checkfile (char *file1);
void printHex (char* buffer, int size, FILE *fp);

int main (int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf (stderr, "%s <dir1> <dir2>\n", argv[0]);
        return 1;
    }
    int flag = 0;//flag=1文件（夹）名字不一致;flag=2文件内容不一致
    int i,j,k;
    char name1[100],name2[100];

    memset(name1,'0',100);
    memset(name2,'0',100);
    k=1;
    if(argv[1][strlen(argv[1])-1]=='/')
        k=2;
    for(i=strlen(argv[1])-k,j=0;argv[1][i] !='/' || i=0;i--,j++)
        name1[j]=argv[1][i];
    name1[j]='\0';
    k=1;
    if(argv[2][strlen(argv[2])-1]=='/')
        k=2;
    for(i=strlen(argv[2])-k,j=0;argv[2][i] !='/' || i=0;i--,j++)
        name2[j]=argv[2][i];
    name2[j]='\0';

    if (strcmp(name1,name2)==0)
        flag = checkdir(argv[1], argv[2], 2);

    if (flag == 0)
        printf ("%s和%s的内容不一致\n", argv[1], argv[2]);
    else
        printf ("%s和%s的内容一致\n", argv[1], argv[2]); 
   return 0;
}
int checkdir (char *argv1, char *argv2, int blank)
{
    DIR *dp1, *dp2;
    struct dirent *dirp1, *dirp2;
    char buffer1[16] = {0};
    char buffer2[16] = {0};
    struct stat buf1, buf2;
    int i1, i2;
    if ((dp1 = opendir(argv1)) == NULL || (dp2 = opendir(argv2)) == NULL)
    {
        perror("opendir"); 
    }
    while ((dirp1 = readdir(dp1)) != NULL && (dirp2 = readdir(dp2)) != NULL)
    {
        //忽略.和..
        if (strcmp(dirp1->d_name, ".")==0 || strcmp(dirp1->d_name, "..")==0 || strcmp(dirp1->d_name, ".")==0 || strcmp(dirp1->d_name, "..")==0)
            continue;
        if (!strcmp(dirp1->d_name, dirp2->d_name))
            return -1;
        char *name1 = (char *)malloc(sizeof(char)*1024);
        memset (name1, '\0', sizeof(char)*1024);
        strcpy (name1, argv1);
        strcat (name1, "/");
        strcat (name1, dirp1->d_name);
        int f1 = stat (name1, &buf1);
        char *name2 = (char *)malloc(sizeof(char)*1024);
        memset (name2, '\0', sizeof(char)*1024);
        strcpy (name2, argv2);
        strcat (name2, "/");
        strcat (name2, dirp2->d_name);
        int f2 = stat (name2, &buf2);
        if(f1 == -1 || f2 == -1)
        {
            switch(errno)
            {
                case EACCES:puts("EACCES");break;
                case EBADF:puts("EBADF");break;
                case EFAULT:puts("EFAULT");break;
                case ENOENT:puts("ENOENT");break;
                case ENOMEM:puts("ENOMEM");break;
                case ENOTDIR:puts("ENOTDIR");break;
            } 
        }
        if(S_ISDIR(buf1.st_mode))
        {
            for(i1=0;i1<blank;i1++)
                printf(" ");
            char *name1 = (char *)malloc(sizeof(char)*1024);
            memset(name1,'\0',sizeof(char)*1024);
            strcpy(name1,argv1);
            strcat(name1,"/");
            strcat(name1,dirp1->d_name);
            printf("%s\n",dirp1->d_name);
            free(name1);
            for(i2=0;i2<blank;i2++)
                printf(" ");
            char *name2 = (char *)malloc(sizeof(char)*1024);
            memset(name2,'\0',sizeof(char)*1024);
            strcpy(name2,argv2);
            strcat(name2,"/");
            strcat(name2,dirp2->d_name);
            printf("%s\n",dirp2->d_name);
            checkdir(name1,name2,blank+2);
            free(name2);

        }
        else
        {
            for(i1=0;i1<blank;i1++)
                printf(" ");
            printf("%s\n",dirp1->d_name);
            for(i1=0;i1<blank;i1++)
                printf(" ");
            printf("%s\n",dirp1->d_name);

        }
    }
/*
        if (stat(strcat(argv1,dirp->d_name), &buf) == -1)
        {
            exit(1);
        }
        if (S_ISDIR(buf.st_mode))
        {
                        fprintf (stdout, "%20s=", dirp->d_name);
            (char *)MD5((unsigned char *)(dirp->d_name), strlen(dirp->d_name), (unsigned char *)buffer);
            printHex (buffer, sizeof(buffer), stdout);
            checkdir(argv1);
        }
        else if (S_ISREG(buf.st_mode))
            checkfile(argv1);
    }*/
    return 0;
}
int checkfile (char *file1)
{
    char tmp[100] = "md5sum ";
    strcat (tmp, file1);
    //strcat (tmp, " >1");不可行。。。
    system (tmp);//重定向到文件中再读出来
    return 0;
}
void printHex (char* buffer, int size, FILE *fp)
{
    assert (NULL != buffer);
    char *p = NULL;
    int i;
    for (i=0,p=buffer; i<size; i++,p++)
    {
        fprintf (fp, "%02x", (unsigned char)*p);
        if (i%16 == 15)
            fprintf (fp, "");
    }
    fprintf (fp, "\n");
}
