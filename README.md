CompareDir
==========

比较两个文件/目录(可能含子目录)的文件内容是否一致

to do:把这三个版本综合成一个版本。。。

题目：
比较两个文件/目录（含子目录）的文件内容是否一致
一致是指：文件名相同，文件内容相同，不用管文件属性
hint:先用md5或sha1计算文件的md5值，再比较

编程语言：C

思路：
1、命令行给出两个文件/目录：argv[1],argv[2]
2、比较给出的目录名是否相同（去掉最后一个slash后再比较，如：/home/llh/compare和/home/llh/test/compare，最后需要比较的是compare和compare）
3、判断是否目录，若不是目录则判断是否文件，若是则比较md5值，若md5值等，返回“一致”，若md5值不等，返回“不一致”；若是目录则转3
3、成功打开目录后，首先统计该目录下的文件和子目录的总个数（不递归，只统计当前层次），如果数目不等，则可直接返回“不一致”
4、指向argv[1]的指针作外层循环，指向argv[2]的指针作内层循环，注意忽略掉.和..目录
5、无论是文件还是目录，都首先判断名字是否相同，不同则返回“不一致”，相同的话：对于目录进行递归，对于文件比较二者的md5值是否相等，不等则返回“不一致”
6、若循环递归完毕，则返回“一致”
关于统计目录下的文件和子目录的总个数需要注意：要记得把指针位置重置到目录流的开始处，因为后面要继续使用它们。
使用到的函数：
关于目录操作的一系列函数，opendir,readdir,rewinddir


测试用例设计：
1、两个完全相同的目录
2、A根目录下有多的文件
3、A的子目录下有多的文件
4、B根目录下有多的文件
5、B的子目录下有多的文件
6、A根目录中有文件不同于B根目录下的相应文件
7、A子目录中有文件不同于B子目录下的相应文件
8、B根目录中有文件不同于A根目录下的相应文件
9、B子目录中有文件不同于A子目录下的相应文件

测试结果：
对WindowXP.vdi（5.3G的大文件，虚拟机的操作系统镜像）测试失败：出现UNKNOWN ERROR
这个问题和stat内部有关，64位。。。去查资料。。。。

2010-10-28开会
readdir()读取出的文件名顺序是没有任何规则的。
append()函数，可以代替strcat()函数。
先比较所有的文件名/目录名（需要保存一份文件名，用动态数据结构如链表来保存），再比较文件内容，这样效率高一些。
读了目录A下的文件/目录c后，用系统调用（如open）来判断c在目录B下是否存在。
调用md5函数时，分块读取数据并计算每个小块的md5值可以节约时间（因为一旦发现有不同的md5值就不用再计算下去了）

2010-11-4开会
strlen算长度时到\0就停止，若是电影编码等在中间可能含有\0（即ascii0），这样用strlen会出错，所以应该把read（读到文件尾才结束）的返回值传给md5计算函数。
用md5sum：
system("md5sum xxx >xxxxx");
md5sum -c xxx///这个的意思及用法？
A：man md5sum
1）hash:用数组不要用链表（求模来定范围）
2）hash：不求模，但要对数组排序
