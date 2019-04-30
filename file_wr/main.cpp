#include<iostream>
#include<sstream>
#include "stdio.h"

void write(std::string strPath) {
    FILE *handle = fopen(strPath.c_str(), "a");
    if(handle == NULL) {
        printf("#### open error \n");
        return;
    } 

    std::stringstream ssOutStream;
    ssOutStream<<"礼包id,类型,有效日期,渠道id\n";
    ssOutStream<<"id,type,date,channel\n";
    ssOutStream<<"string,int,string,int\n";
    ssOutStream<<"1,1,2018-01-01 00:00:00,1\n";

    if(!ssOutStream.str().empty() ){
        fputs(ssOutStream.str().c_str() , handle);
    }

    std::cout << "gift code create complete" << std::endl;
    fclose(handle);handle = NULL;
}

int main() {
    write("test.txt");
}
