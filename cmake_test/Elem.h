#include<iostream>

class Elem{
public:
	Elem();
	Elem(const Elem& e);
    void Init();
	~Elem();

	int m_nData; 
};
