#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#define COMPARE ((ch2 != '\0') ? ((p->data[0] != ch) || (p->data[1] != ch2)) : (p->data[0] != ch))

typedef struct charac
{
    char data[3];       //字符，若非中文字则data[1]置0
    unsigned int count; //该字符的次数
    float freq;         //该字符的频率
    struct charac *next;
} CHAR;

CHAR *insert(CHAR *head, CHAR **graph,
             CHAR **ascii, CHAR **zh, char ch, char ch2)
{
    CHAR *new, *p;
    CHAR *pend;
    int flag; //该字符的分类

    if (head == NULL) //首次插入
    {
        new = (CHAR *)malloc(sizeof(CHAR));

        new->data[0] = ch;
        new->data[1] = ch2;
        new->data[2] = '\0';
        new->count = 1;
        new->next = NULL;

        head = new;

        *graph = head;
        *ascii = head;
        *zh = head;
    }
    else
    {
        /* 将可打印字符、其它ASCII、汉字三类存储在链表上不同区域，
        pend 标记 p 的定位区间结束位置，减少比较次数 */
        if (ch2 != '\0')
        {
            flag = 3; //汉字
            p = *zh;
            pend = NULL;
        }
        else if (isgraph(ch) != 0)
        {
            flag = 1; //可打印字符
            p = *graph;
            pend = (*ascii)->next;
        }
        else
        {
            flag = 2; //ASCII
            p = *ascii;
            pend = (*zh)->next;
        }

        while (COMPARE && (p->next != pend)) //定位 p
        {
            p = p->next;
        }

        if (COMPARE) //新字符
        {
            new = (CHAR *)malloc(sizeof(CHAR));

            new->data[0] = ch;
            new->data[1] = ch2;
            new->data[2] = '\0';
            new->count = 1;
            new->next = NULL;

            /* 将 new 插入链表 */
            switch (flag)
            {
            case 3:
            {
                p->next = new;
                break;
            }
            case 1:
            {
                new->next = p->next;
                p->next = new;

                (*ascii) = (*ascii)->next; //移动标记指针

                if (new->next == NULL)
                {
                    (*zh) = (*zh)->next;
                }

                break;
            }
            case 2:
            {
                new->next = p->next;
                p->next = new;

                (*zh) = (*zh)->next;

                break;
            }
            }
        }
        else
        {
            p->count += 1; //已有字符
        }
    }

    return head;
}

CHAR *createCharLinkZh(FILE *fp)
{
    CHAR *head, *graph, *ascii, *zh; //标记不同区域
    int ch, ch2;

    head = NULL;
    graph = NULL;
    ascii = NULL;
    zh = NULL;

    ch = fgetc(fp); //读取一个字符

    while (!feof(fp)) //是否到文件尾
    {

        if ((ch >= 129) && (ch <= 254)) //处理中文GBK
        {
            ch2 = fgetc(fp);

            if ((ch2 >= 64) && (ch2 <= 254) && (ch != 127))
            {
                //是中文，ch2不变
                head = insert(head, &graph, &ascii, &zh, ch, ch2); //插入ch,ch2
            }
            else
            {
                fseek(fp, -1, SEEK_CUR); //不是中文，回退1字节
                ch2 = 0;                 //ch2置0
                head = insert(head, &graph, &ascii, &zh, ch, ch2);
            }
        }
        else //不是中文
        {
            ch2 = 0;
            head = insert(head, &graph, &ascii, &zh, ch, ch2);
        }

        ch = fgetc(fp);
    }

    return head;
}

CHAR *createCharLink(FILE *fp)
{
    CHAR *head, *graph, *ascii, *zh; //标记不同区域
    int ch, ch2 = 0;

    head = NULL;
    graph = NULL;
    ascii = NULL;
    zh = NULL;

    ch = fgetc(fp); //读取一个字符

    while (!feof(fp)) //是否到文件尾
    {
        head = insert(head, &graph, &ascii, &zh, ch, ch2);
        ch = fgetc(fp);
    }

    return head;
}

void printChar(char x[3], int count, float freq)
{
    if (isgraph(x[0]) != 0)
    {
        //可打印字符
        printf("%-11c\tcount:%-10d\tfrequency:%f\n", x[0], count, freq);
    }
    else if (x[1] == '\0')
    {
        //其它字符，打印ASCII
        printf("%-4d(ASCII)\tcount:%-10d\tfrequency:%f\n", x[0], count, freq);
    }
    else
    {
        //打印中文
        printf("%-11s\tcount:%-10d\tfrequency:%f\n", x, count, freq);
    }
}

void printLink(CHAR *head)
{
    CHAR *p;
    p = head;

    while (p != NULL) //遍历链表
    {
        printChar(p->data, p->count, p->freq);
        p = p->next;
    }
}

CHAR *readFile(char faddress[], int isZh) //传入文件地址
{
    FILE *fp;

    if ((fp = fopen(faddress, "rb")) == NULL)
    {
        printf("File open error, the program will exit.\n");
        system("pause");
        exit(1);
    }

    CHAR *head;
    head = (isZh == 1) ? createCharLinkZh(fp) : createCharLink(fp); //创建链表

    fclose(fp);

    return head;
}

/* 总字符数 */
unsigned int countChar(CHAR *head)
{
    CHAR *p;
    p = head;

    unsigned int total = 0;

    while (p != NULL)
    {
        total += p->count;
        p = p->next;
    }

    return total;
}

/* 计算信息熵 */
float calEntropy(CHAR *head, int total)
{
    float entropy = 0;
    CHAR *p;
    p = head;

    while (p != NULL)
    {
        p->freq = (float)p->count / total;            //频率
        entropy -= p->freq * (log(p->freq) / log(2)); //信息熵公式
        p = p->next;
    }

    return entropy;
}

void listSort(CHAR **head)
{
    if (head == NULL || *head == NULL)
    {
        return;
    }

    CHAR *current, *post, *previous, *sentinel = NULL; //sentinel 标记有序区的首个节点
    int sorted;

    do
    {
        previous = NULL;
        current = *head;
        sorted = 1;

        /* 冒泡排序 */
        while ((post = current->next) != sentinel)
        {
            if (current->count < post->count)
            {
                sorted = 0; //发生交换，则仍未有序
                current->next = post->next;
                post->next = current;

                if (previous != NULL)
                {
                    previous->next = post;
                }
                previous = post;

                if (current == *head)
                {
                    *head = post; //保证头指针指向最小的节点
                }
            }
            else
            {
                previous = current;
                current = post;
            }
        }

        sentinel = current; //current 为该趟排好序的节点

    } while (!sorted); //若未发生交换，则已经有序
}

int main(void)
{
    CHAR *head;
    int total;
    char faddress[1024];
    char zh; //输入 y/n
    int isZh = 0; //标记是否采用中文处理

    printf("Enter file address(no more than 1024 chars, e.g. D:\\\\test.txt):\n");

    scanf("%1024s", faddress); //读入文件名

    while (1)
    {
        fflush(stdin);

        printf("Enable Chinese processing?\nNote:Only support GBK.\nEnter Y or N:");
        scanf("%c", &zh);

        if (zh == 'y' || zh == 'Y')
        {
            isZh = 1;
            break;
        }
        else if (zh == 'n' || zh == 'N')
        {
            isZh = 0;
            break;
        }
        else
        {
            printf("Error!\n");
        }
    }

    head = readFile(faddress, isZh); //读文件创建链表

    listSort(&head); //按count排序

    total = countChar(head); //总字符数
    printf("Total Characters: %d\n", total);

    printf("Entropy: %f\n", calEntropy(head, total)); //打印信息熵

    printLink(head); //打印字符统计数据

    system("pause");

    return 0;
}