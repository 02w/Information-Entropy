#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

struct charac
{
    char data;  //字符
    int count;  //该字符的次数
    float freq; //该字符的频率
    struct charac *next;
};

struct charac *createCharLink(FILE *fp)
{
    struct charac *head, *new, *tail, *p;
    char ch;

    head = NULL;

    ch = fgetc(fp); //读取一个字符

    while (!feof(fp)) //是否到文件尾
    {
        new = (struct charac *)malloc(sizeof(struct charac));
        new->data = ch;
        new->count = 1;
        new->next = NULL;

        if (head == NULL)
        {
            head = new;
            tail = new;
        }
        else
        {
            p = head;
            while ((p->data != ch) && (p->next != NULL))
            {
                p = p->next;
            }
            if (p->data != ch) //新字符
            {
                tail->next = new;
                tail = new;
            }
            else
            {
                p->count += 1; //已有字符
                free(new);
            }
        }

        ch = fgetc(fp);
    }
    return (head);
}

void printChar(char data, int count, float freq)
{
    if (isgraph(data) != 0)
        //可打印字符
        printf("%-11c\tcount:%d\t\tfrequency:%f\n", data, count, freq);
    else
        //其它字符，打印ASCII
        printf("%-4d(ASCII)\tcount:%d\t\tfrequency:%f\n", data, count, freq);
}

void printLink(struct charac *head)
{
    struct charac *p;
    p = head;
    while (p != NULL) //遍历链表
    {
        printChar(p->data, p->count, p->freq);
        p = p->next;
    }
}

struct charac *readFile(char faddress[]) //传入文件地址
{
    FILE *fp;

    if ((fp = fopen(faddress, "rb")) == NULL)
    {
        printf("File open error, the program will exit.\n");
        system("pause");
        exit(0);
    }

    struct charac *head;
    head = createCharLink(fp); //创建链表

    return head;
}

int countChar(struct charac *head) //总字符数
{
    struct charac *p;
    p = head;
    int total = 0;
    while (p != NULL)
    {
        total += p->count;
        p = p->next;
    }
    return total;
}

float calEntropy(struct charac *head, int total) //计算信息熵
{
    float entropy = 0;
    struct charac *p;
    p = head;
    while (p != NULL)
    {
        p->freq = (float)p->count / total; //频率
        entropy -= p->freq * (log(p->freq) / log(2)); //信息熵公式
        p = p->next;
    }
    return entropy;
}

int main(void)
{
    struct charac *head;
    int total;
    char faddress[1024];

    printf("Enter file address(no more than 1024 chars, e.g. D:\\\\test.txt):\n");

    scanf("%1024s", faddress); //读入文件名

    head = readFile(faddress); //读文件创建链表

    total = countChar(head); //总字符数
    printf("Total Characters: %d\n", total);

    printf("Entropy: %f\n", calEntropy(head, total)); //打印信息熵

    printLink(head); //打印字符统计数据

    system("pause");
    return 0;
}