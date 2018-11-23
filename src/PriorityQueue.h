#pragma once

#include <list>
#include <set>
#include <string>
using namespace std;

#include <boost/shared_ptr.hpp>


template<class T>
struct CPriorityQueueItem {
    CPriorityQueueItem(int priority, const string &id, const T &value)
        :m_Priority(priority), m_ID(id), m_Value(value)
    {};

    int m_Priority;
    string m_ID;
    T m_Value;
};

template<class T, class PriorityCompare = std::less<int> >
class CPriorityQueue
{
public:
    typedef list<CPriorityQueueItem<T> > MyQueue;
    typedef typename MyQueue::iterator Iterator;

public:
    CPriorityQueue(){};
    ~CPriorityQueue(){};

private:
    //////Iterator FindBack(int priority)
    //////{
    //////    Iterator it = m_MyQueue.begin();
    //////    for (; it != m_MyQueue.end(); it++) {
    //////        if (m_Compare(priority, it->m_Priority)) {
    //////            break;
    //////        }
    //////    }
    //////    return it;
    //////}

    Iterator LowerBound(int priority)
    {
        Iterator it = m_MyQueue.begin();
        for (; it != m_MyQueue.end(); it++) {
            if (!m_Compare(it->m_Priority, priority)) {
                break;
            }
        }
        return it;
    }

    Iterator UpperBound(int priority)
    {
        Iterator it = m_MyQueue.begin();
        for (; it != m_MyQueue.end(); it++) {
            if (m_Compare(priority, it->m_Priority)) {
                break;
            }
        }
        return it;
    }

public:
    void PushBack(const string &id, const T &value, int priority = 3)
    {
        Iterator it = UpperBound(priority);
        if (it != m_MyQueue.end()) {
            m_MyQueue.insert(it, CPriorityQueueItem<T>(priority, id, value));
        }
        else {
            m_MyQueue.push_back(CPriorityQueueItem<T>(priority, id, value));
        }
    }

    void PushFront(string id, T &value, int priority = 3)
    {
        Iterator it = LowerBound(priority);

        if (it != m_MyQueue.end()) {
            m_MyQueue.insert(it, CPriorityQueueItem<T>(priority, id, value));
        }
        else {
            m_MyQueue.push_back(CPriorityQueueItem<T>(priority, id, value));
        }
    }

    bool Remove(string id)
    {
        Iterator it = m_MyQueue.begin();
        for (; it != m_MyQueue.end(); it++) {
            if (it->m_ID == id) {
                m_MyQueue.erase(it);
                return true;
            }
        }

        return false;
    }

    void Remove(Iterator where)
    {
        m_MyQueue.erase(where);
    }

    bool Front(string &id, T &value)
    {
        if (!m_MyQueue.empty()) {
            id = m_MyQueue.front().m_ID;
            value = m_MyQueue.front().m_Value;
            return true;
        }
        else {
            return false;
        }
    }

    bool PopFront(string &id, T &value)
    {
        if (!m_MyQueue.empty()) {
            id = m_MyQueue.front().m_ID;
            value = m_MyQueue.front().m_Value;
            m_MyQueue.pop_front();
            return true;
        }
        else {
            return false;
        }
    }

    size_t GetSize()
    {
        return m_MyQueue.size();
    }

    size_t GetSize(int priority)
    {
        Iterator it_begin = LowerBound(priority);
        Iterator it_end = UpperBound(priority);

        size_t count = 0;
        if (it_begin == m_MyQueue.end()) {
            return 0;
        }

        for (Iterator it = it_begin; it != it_end; it++) {
            count++;
        }

        return count;
    }

    Iterator Begin()
    {
        return m_MyQueue.begin();
    }

    Iterator End()
    {
        return m_MyQueue.end();
    }

    bool Empty()
    {
        return m_MyQueue.empty();
    }

    void Clear()
    {
        m_MyQueue.clear();
    }

    bool Exist(string id)
    {
        Iterator it = m_MyQueue.begin();
        for (; it != m_MyQueue.end(); it++) {
            if (id == it->m_ID) {
                return true;
            }
        }
        return false;
    }

private:
    MyQueue m_MyQueue;
    PriorityCompare m_Compare;
};

