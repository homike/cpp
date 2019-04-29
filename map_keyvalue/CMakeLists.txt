#1.cmake verson，指定cmake版本
cmake_minimum_required(VERSION 2.4)

#project name
SET(PROJECTNAME test)

#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
#SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

#output path
#SET(OUTPUT ../../bin/${PROJECTNAME}/debug)
#SET(EXECUTABLE_OUTPUT_PATH ${OUTPUT})

#2.project name，指定项目的名称，一般和项目的文件夹名称对应
project(${PROJECTNAME})

SET(CMAKE_C_COMPILER gcc)
SET(CMAKE_CXX_COMPILER g++)

#definition,添加预处理定义
ADD_DEFINITIONS(
	-Wno-unused-variable 
	-D_REENTRANT
	-DTIXML_USE_STL
	-D_LINUX
	-DDEBUG
    -Wno-deprecated
    -Wno-write-strings
	)

#3.head file path，头文件目录
INCLUDE_DIRECTORIES( 
	./
)

#4.source directory，源文件目录
SET(SRC_FILES
    ./main.cpp
)

SET(LIBRARIES
    #-lgnpipe
        )   
# link directories
LINK_DIRECTORIES(
    /usr/local/lib
    )   

#6.add executable file，添加要编译的可执行文件
ADD_EXECUTABLE(
	${PROJECTNAME}
	${SRC_FILES} 
	)

 #7.add link library，添加可执行文件所需要的库，比如我们用到了libm.so（命名规则：lib+name+.so），就添加该库的名称
TARGET_LINK_LIBRARIES(${PROJECTNAME} ${LIBRARIES})
