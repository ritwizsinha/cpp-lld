#include <vector>
#include <list>

using namespace std;

template<typename T>
class ZigZagIterator 
{
    using IteratorList = list<vector<T>::iterator>
    IteratorList candidates;
    IteratorList::iterator curPos;

    ZigZagIterator(const vector<vector<T>> &data) {
        for(auto &l : data) {
            if (l.size() > 0) candidates.push_back(l.begin());
        }

        curPos = candidates.begin();
    }

    bool hasNext() {
        while(curPos->)
        return candidate.size() > 0;
    }

    T next() {

    }

};