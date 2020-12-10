
#include <stdio.h>
typedef struct Test{
    int i1;
    int i2;
} Test;

void test(){
    Test test ;
    test.i1 = 12;
    test.i2 = 13;
    Test* test2 =& test ;
    printf("%d,%d\n",test2->i1, test2->i2);
}