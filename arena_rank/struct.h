#include <iostream>
#include <vector>

typedef unsigned long long uint64;

struct SPlayerInfo {
    uint64 llId;
    int dwBattleAbility;

    bool operator==(const SPlayerInfo& rhs)
    {
        return this->llId == rhs.llId;
    }

    SPlayerInfo()
        : llId(0)
        , dwBattleAbility(0)
    {
    }
};

struct SArenaRankInfo : SPlayerInfo {
    int nRank;       
    int nVictoryNum;    
    int nUp;
    int nGroupID;
    int nServerID;

    SArenaRankInfo()
        : nRank(0)
        , nVictoryNum(0)
        , nUp(0)
        , nGroupID(0)
        , nServerID(0)
    {
    }

    bool operator<(const SArenaRankInfo& rhs) const
    {
        return this->nRank < rhs.nRank;
    }
};

struct GroupPlayerInfo {
    GroupPlayerInfo(uint64 id_, int pos_)
        : llId(id_)
        , pos(pos_)
    {
    }

    uint64 llId;
    int    pos;
};

struct Less {
    Less(std::vector<SArenaRankInfo>& vec)
        : m_vec(vec)
    {
    }

    bool operator()(const GroupPlayerInfo& v1, const GroupPlayerInfo& v2)
    {
        return m_vec[v1.pos].nRank < m_vec[v2.pos].nRank;
    }

    std::vector<SArenaRankInfo>& m_vec;
};
