#include "rank.h"
#include <iostream>
#include <sstream>
#include <time.h>
#include <vector>
#include <sys/timeb.h>

long long nowTime()
{
    timeb t;
    ftime(&t);
    return t.time * 1000 + t.millitm;
}

void compareVectorMap()
{
    std::vector<int> vecTest;
    std::map<int, int> mapTest;

    const int nLen = 20;
    for (int i = 0; i < nLen; ++i) {
        mapTest[i] = i;
        vecTest.push_back(i);
    }

    const int nTestCnt = 20000000;
    // select map
    auto sTime = nowTime();
    int costTime;
    for (int i = 0; i < nTestCnt; ++i) {
        int num = rand() % nLen;

        auto it = mapTest.find(num);
        if (it != mapTest.end()) {
            int b = it->second;
            if (b != num) {
            }
        }
    }
    costTime = nowTime() - sTime;
    std::cout << "map select: " << costTime << std::endl;

    // select vector
    sTime = nowTime(); 
    for (int i = 0; i < nTestCnt; ++i) {
        int num = rand() % nLen;

        for (int j = 0; j < int(vecTest.size()); ++j) {
            int b = vecTest[j];
            if (b == num) {
                break;
            }
        }
    }
    costTime = nowTime() - sTime;
    std::cout << "vector select: " << costTime << std::endl;

    // select binary vector
    sTime = nowTime();
    for (int i = 0; i < nTestCnt; ++i) {
        int num = rand() % nLen;

        auto it = std::lower_bound(vecTest.begin(), vecTest.end(), num);
        if (it != vecTest.end()) {
            int b = *it;
            if (b != num) {
            }
        }
    }
    costTime = nowTime() - sTime;
    std::cout << "vector binary select: " << costTime << std::endl;
}

int main()
{
    compareVectorMap();
}
