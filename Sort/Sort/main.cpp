//
//  main.cpp
//  Sort
//
//  Created by PengHao on 16/7/20.
//  Copyright © 2016年 WolfPeng. All rights reserved.
//

#include <iostream>

void stackSort(int *sort, int num) {
    
}

void fastSort(int *sort, int num) {
    int temp  = sort[0];
    int i = 0;
    for (int j = num - 1; j>=0 ; --j) {
        if (sort[j] < temp) {
            sort[i] = sort[j];
            for (; i < j; ++i) {
                if (sort[i] > temp) {
                    sort[j] = sort[i];
                    break;
                }
            }
        }
        
        if (i >= j) {
            sort[i] = temp;
            fastSort(sort, i++);
            fastSort(sort + i, num - i);
            break;
        }
    }
}

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    int num = 1000000;
    int *sort = (int *)calloc(sizeof(int), num);
    for (int i = 0; i < num; ++i) {
        sort[i] = rand()%100;
    }
    fastSort(sort, num);
    
    printf("sort finished!\n");
    for (int k = 0; k < num; ++k) {
        printf("%d, ", sort[k]);
    }

    return 0;
}
