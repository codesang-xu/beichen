#ifndef __ACD_ALGORITHM_H__
#define __ACD_ALGORITHM_H__

#include <map>
#include <string>
#include <vector>

#include <boost/function.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

using namespace std;

class CAgentInfo;
class ACDAlgorithm
{
public:
    virtual CAgentInfo* find() = 0;
};

template <class TContainer>
class ACDMincalls : public ACDAlgorithm
{
public:
    typedef typename TContainer::iterator IteratorType;
    typedef boost::function<int(IteratorType&)> GetTaskCountFunction;
    typedef boost::function<CAgentInfo*(IteratorType&)> GetAgentInfoFunction;

public:
    ACDMincalls(TContainer& agentMap, GetTaskCountFunction getcountfunc
        , GetAgentInfoFunction getAgentInfoFunc)
        :m_AgentMap(agentMap), m_GetTaskCountFunc(getcountfunc)
        , m_GetAgentInfoFunc(getAgentInfoFunc)
    {}

    virtual CAgentInfo* find()
    {
        typedef typename TContainer::iterator AgentMapIterator;
        vector<AgentMapIterator> tmp;

        int minCounts = 10000000;//默认无穷大
        AgentMapIterator iter = m_AgentMap.begin();
        while (iter != m_AgentMap.end())
        {
            int callcount = m_GetTaskCountFunc(iter);
            if (callcount != -1) {
                if (callcount < minCounts) {
                    minCounts = callcount;
                    tmp.clear();
                    tmp.push_back(iter);
                }
                else if (callcount == minCounts) {
                    tmp.push_back(iter);
                }
            }

            iter++;
        }

        //若同时有多个满足，随机获取一个
        if (!tmp.empty())
        {
            static boost::random::mt19937 generator;
            boost::random::uniform_int_distribution<> distribution(0, tmp.size() - 1);
            return m_GetAgentInfoFunc(tmp[distribution(generator)]);
        }

        return NULL;
    }

private:
    TContainer& m_AgentMap;
    GetTaskCountFunction m_GetTaskCountFunc;
    GetAgentInfoFunction m_GetAgentInfoFunc;
};

template <class T>
class ACDPoll : public ACDAlgorithm
{
public:
    typedef std::map<string, T> AgentMap;
    typedef boost::function<bool(T&)> UnaryPredicate;
    typedef boost::function<CAgentInfo*(T&)> GetAgentInfoFunction;

public:
    ACDPoll(AgentMap& agentMap, UnaryPredicate pred,
        GetAgentInfoFunction getAgentInfoFunc)
        :m_AgentMap(agentMap), m_UnaryPredicate(pred)
        , m_GetAgentInfoFunc(getAgentInfoFunc)
    {}

    virtual CAgentInfo* find() {
	   typedef typename AgentMap::iterator AgentMapIterator;
	   AgentMapIterator iter = m_AgentMap.find(m_sCurPollAgentID);
	   AgentMapIterator tmp_iter = iter;

        if (iter != m_AgentMap.end())
        {
            iter++;
            tmp_iter = iter;
        }

        //在当前位置的下一个位置开始找
        while (tmp_iter != m_AgentMap.end())
        {
            if (m_UnaryPredicate(tmp_iter->second)) {
                m_sCurPollAgentID = tmp_iter->first;
                return m_GetAgentInfoFunc(tmp_iter->second);
            }
            else {
                tmp_iter++;
            }
        }

        //从头开始找至但前位置
        tmp_iter = m_AgentMap.begin();
        while (tmp_iter != iter)
        {
            if (m_UnaryPredicate(tmp_iter->second)) {
                m_sCurPollAgentID = tmp_iter->first;
                return m_GetAgentInfoFunc(tmp_iter->second);
            }
            else {
                tmp_iter++;
            }
        }

        return NULL;
    }

private:
    string m_sCurPollAgentID;
    AgentMap& m_AgentMap;
    UnaryPredicate m_UnaryPredicate;
    GetAgentInfoFunction m_GetAgentInfoFunc;
};

#endif