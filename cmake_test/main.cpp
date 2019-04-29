#include <iostream>
#include <vector>
#include "Elem.h"

int main(int argc, char* argv[]){
	{
		std::cout << "1\n";
		std::vector<Elem> v;
		Elem oElem;
		oElem.m_nData = 0;
        oElem.Init();
		v.push_back(oElem);
	}

	{
		std::cout << "\n2\n";
		std::vector<Elem> v;
		int nSize = v.size();
		v.resize(nSize+1);
		v[nSize].m_nData = 0;
	}

	system("pause");
}
