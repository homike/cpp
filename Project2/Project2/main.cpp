#include <iostream>
#include <vector>
using namespace std;

class Elem{
public:
	Elem(){
		std::cout << "Construct\n";
	}
	Elem(const Elem& e){
		std::cout << "Copy\n";
	}
	~Elem(){
		std::cout << "Destruct\n";
	}   

	int m_nData; 
};


int main(int argc, char* argv[]){
	{
		std::cout << "1\n";
		std::vector<Elem> v;
		Elem oElem;
		oElem.m_nData = 0;
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