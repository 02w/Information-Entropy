#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#define COMPARE ((ch2 != '\0') ? ((p->data[0] != ch) || (p->data[1] != ch2)) : (p->data[0] != ch))

struct charac
{
    char data[3];       //字符，若非中文字则data[1]置0
    unsigned int count; //该字符的次数
    float freq;         //该字符的频率
    struct charac *next;
};

struct charac *insert(struct charac *head, char ch, char ch2)
{
    struct charac *new, *tail, *p;
    if (head == NULL)
    {
        new = (struct charac *)malloc(sizeof(struct charac));

        new->data[0] = ch;
        new->data[1] = ch2;
        new->data[2] = '\0';
        new->count = 1;
        new->next = NULL;

        head = new;
        tail = new;
    }
    else
    {
        p = head;
        //定位 p
        while (COMPARE && (p->next != NULL))
        {
            p = p->next;
        }

        if (COMPARE) //新字符
        {
            new = (struct charac *)malloc(sizeof(struct charac));

            new->data[0] = ch;
            new->data[1] = ch2;
            new->data[2] = '\0';
            new->count = 1;
            new->next = NULL;

            tail->next = new;
            tail = new;
        }
        else
        {
            p->count += 1; //已有字符
        }
    }
    return head;
}

struct charac *createCharLink(FILE *fp)
{
    struct charac *head, *new, *tail, *p;
    int ch, ch2;

    head = NULL;

    ch = fgetc(fp); //读取一个字符

    while (!feof(fp)) //是否到文件尾
    {

        if ((ch >= 161) && (ch <= 247)) //处理中文
        {
            ch2 = fgetc(fp);
            if ((ch2 >= 161) && (ch2 <= 254))
            {
                //是中文，ch2不变
                head = insert(head, ch, ch2); //插入ch,ch2
            }
            else
            {
                fseek(fp, -1, SEEK_CUR); //不是中文，回退1字节
                ch2 = 0;                 //ch2置0，因为若之前有中文会残留ch2
                head = insert(head, ch, ch2);
            }
        }
        else // 不是中文
        {
            ch2 = 0;
            head = insert(head, ch, ch2);
        }
        ch = fgetc(fp);
    }
    return head;
}

void printChar(char x[3], int count, float freq)
{
    if (isgraph(x[0]) != 0)
        //可打印字符
        printf("%-11c\tcount:%-10d\tfrequency:%f\n", x[0], count, freq);
    else if (x[1] == '\0')
        //其它字符，打印ASCII
        printf("%-4d(ASCII)\tcount:%-10d\tfrequency:%f\n", x[0], count, freq);
    else
        //打印中文
        printf("%-11s\tcount:%-10d\tfrequency:%f\n", x, count, freq);
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
        p->freq = (float)p->count / total;            //频率
        entropy -= p->freq * (log(p->freq) / log(2)); //信息熵公式
        p = p->next;
    }
    return entropy;
}

void listSort(struct charac **head)
{
    if (head == NULL || *head == NULL)
        return;

    struct charac *current, *post, *previous, *sentinel = NULL; // sentinel 标记有序区的首个节点
    int sorted;

    do
    {
        previous = NULL;
        current = *head;
        sorted = 1;

        //冒泡排序
        while ((post = current->next) != sentinel)
        {
            if (current->count < post->count)
            {
                sorted = 0; //发生交换，则仍未有序
                current->next = post->next;
                post->next = current;

                if (previous != NULL)
                    previous->next = post;
                previous = post;

                if (current == *head)
                    *head = post; //保证头指针指向最小的节点
            }
            else
            {
                previous = current;
                current = post;
            }
        }
        sentinel = current; // current 为该趟排好序的节点
    } while (!sorted);      //若未发生交换，则已经有序
}

int main(void)
{
    struct charac *head;
    int total;
    char faddress[1024];

    printf("Enter file address(no more than 1024 chars, e.g. D:\\\\test.txt):\n");

    scanf("%1024s", faddress); //读入文件名

    head = readFile(faddress); //读文件创建链表

    listSort(&head); //按count排序

    total = countChar(head); //总字符数
    printf("Total Characters: %d\n", total);

    printf("Entropy: %f\n", calEntropy(head, total)); //打印信息熵

    printLink(head); //打印字符统计数据

    system("pause");
    return 0;
}