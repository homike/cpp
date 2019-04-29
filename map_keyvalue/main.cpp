#include<iostream>
#include<map>
#include<string>
#include<algorithm>
using namespace std;

typedef map<string, string> mapKV;
typedef pair<string, string> pairKV;

mapKV::const_iterator search(const mapKV& map, const string& strSearch) {
    mapKV::const_iterator i = map.lower_bound(strSearch);
    if (i != map.end()) {
        const string& key = i->first;
        if (key.compare(0, strSearch.size(), strSearch) == 0) {
            return i;
        }
    }

    return map.end();
}

void Test(const mapKV& map, const string& strSearch) {
    cout << strSearch;

    auto i = search(map, strSearch);
    if (i != map.end()) {
        cout << '\t' << i->first << ", " << i->second;
    }

    cout << endl;
}

int main(int argc, char *argv[])
{
    mapKV tMap;

    tMap.insert(pairKV("John", "AA"));
    tMap.insert(pairKV("Mary", "BBB"));
    tMap.insert(pairKV("Mother", "A"));
    tMap.insert(pairKV("Marlon", "C"));

    Test(tMap, "Marl");
    Test(tMap, "Mo");
    Test(tMap, "M");
    Test(tMap, "Mad");
    Test(tMap, "Mom");
    Test(tMap, "Perr");
    Test(tMap, "Jo");

    return 0;
}
