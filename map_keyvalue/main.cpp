#include<iostream>
#include<map>
#include<string>
#include<algorithm>

typedef std::map<std::string, std::string> mapKV;
typedef std::pair<std::string, std::string> pairKV;

mapKV::const_iterator search(const mapKV& map, const std::string& strSearch) {
    mapKV::const_iterator i = map.lower_bound(strSearch);
    if (i != map.end()) {
        const std::string& key = i->first;
        if (key.compare(0, strSearch.size(), strSearch) == 0) {
            return i;
        }
    }

    return map.end();
}

void Test(const mapKV& map, const std::string& strSearch) {
    std::cout << strSearch;

    auto i = search(map, strSearch);
    if (i != map.end()) {
        std::cout << '\t' << i->first << ", " << i->second;
    }

    std::cout << std::endl;
}

int main(int argc, char *argv[])
{
    mapKV mapTest;

    mapTest.insert(pairKV("John", "AA"));
    mapTest.insert(pairKV("Mary", "BBB"));
    mapTest.insert(pairKV("Mother", "A"));
    mapTest.insert(pairKV("Marlon", "C"));

    Test(mapTest, "Marl");
    Test(mapTest, "Mo");
    Test(mapTest, "M");
    Test(mapTest, "Mad");
    Test(mapTest, "Mom");
    Test(mapTest, "Perr");
    Test(mapTest, "Jo");

    return 0;
}
