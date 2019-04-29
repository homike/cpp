#include"Elem.h"

Elem::Elem(){
		std::cout << "Construct\n";
}

Elem::Elem(const Elem& e){
        std::cout << "Copy\n";
}

void Elem::Init() {
    std::cout<<"init"<<std::endl;
}

Elem::~Elem(){
    std::cout << "Destruct\n";
}   
