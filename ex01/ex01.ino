#include <stdio.h>
#include <string.h>
int main()
{
    char str[100];
    printf("Hello World!\n");
    printf("请输入一句话：");
    fgets(str, sizeof(str), stdin);
    str[strcspn(str, "\n")] = '\0';
    printf("你输入的内容是：%s\n", str);
    return 0;
}
