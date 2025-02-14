#pragma once
#include "define.hpp"
#include "Settings.hpp"
#include "RuneDetector.hpp"
#include <map>
#include <list>

using namespace std;

template <class T>
class Voter
{
    public:
        Voter(const int& st)
        {
            saveTime = st;
            voteTime = st / 2;
        }
        ~Voter(){}
        void PushElement(const T& element);
        bool GetBestElement(T& element);        
        bool RemoveOldElements(int num = -1);
    
    private:
        map<T, int> count;
        list<T> data;
        int voteTime;
        int saveTime;
};

template <class T>
void Voter<T>::PushElement(const T& element)
{
    data.push_back(element);
    auto search = count.find(element);
    if (search == count.end())
    {
        count[element] = 1;
    }
    else
    {
        search->second++;
    }
    if (data.size() > saveTime)
    {
        RemoveOldElements(data.size() - saveTime);
    }
}

template <class T>
bool Voter<T>::GetBestElement(T& element)
{
    if (data.size() < saveTime)
    {
        return false;
    }
    for (auto itr = count.begin(); itr != count.end(); itr++)
    {
        if (itr->second > voteTime)
        {
            element = itr->first;
            return true;
        }
    }
    return false;
} 

template <class T>
bool Voter<T>::RemoveOldElements(int num)
{
    if (num == -1)
    {
		num = voteTime;
    }
    if (num > data.size() || num < 0)
    {
        return false;
    }
    typename map<T, int>::iterator mapItr;
    while (num--)
    {
        mapItr = count.find(data.front());
        data.pop_front();
        if (--(mapItr->second) == 0)
        {
            count.erase(mapItr);
        }
    }
	return true;
}
