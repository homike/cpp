#include "struct.h"
#include <algorithm>
#include <map>

class ArenaGroup {
public:
    ArenaGroup()
        : m_nGroupID(0)
        , m_nLevelID(0) {};

    ArenaGroup(int id)
        : m_nGroupID(id) {};

    int findGroupPlayer(const uint64 playerID, int pos) const {
        size_t nSize = m_vevGroupPlayer.size();
        if (nSize == 0) {
            return -1;
        }

        int endPos = (nSize - 1);
        int realPos = (size_t)endPos < (size_t)pos ? endPos : pos;
        for (int i = realPos; i >= 0; --i) {
            if (m_vevGroupPlayer[i].llId == playerID) {
                return i;
            }
        }

        for (int i = (int)endPos; i > realPos; --i) {
            if (m_vevGroupPlayer[i].llId == playerID) {
                return i;
            }
        }
        return -1;
    }

    bool operator<(const ArenaGroup& rhs) const {
        return this->m_nGroupID < rhs.m_nGroupID;
    }

    int m_nGroupID;
    int m_nLevelID;
    std::vector<GroupPlayerInfo> m_vevGroupPlayer;
};

class ArenaRankData {
    std::map<int, int>          m_mapNextGroupID;
    std::map<uint64, int>       m_mapPlayerID2Index;
    std::vector<ArenaGroup>     m_vevGroup;
    std::vector<SArenaRankInfo> m_vecArenaRank;

public:
    std::vector<SArenaRankInfo>& GetArenaRank() {
        return m_vecArenaRank;
    }

    void Upgrade(SArenaRankInfo& rankInfo, int pos) {
        const int maxRank = 100;
        const int minAbility = 2000;
        if (rankInfo.nRank <= maxRank
                && rankInfo.dwBattleAbility >= minAbility) {

            std::map<int, int>::iterator it = m_mapNextGroupID.find(rankInfo.nGroupID);
            if (it != m_mapNextGroupID.end()) {
                DelGroupPlayer(rankInfo);             // delete old group
                rankInfo.nGroupID = it->second;
                InsertGroupAndFixRank(rankInfo, pos); // insert new group
            }
        }
    }

    void SwapRank(uint64 winner, uint64 loser) {
        int winner_pos = UpdateRankByID(winner);
        int loser_pos = UpdateRankByID(loser);
        SArenaRankInfo* p_winner = GetArenaRankInfo(winner_pos);
        SArenaRankInfo* p_loser = GetArenaRankInfo(loser_pos);
        if (p_winner == NULL || p_loser == NULL) {
            return;
        }

        if (p_winner->nGroupID != p_loser->nGroupID) {
            return;
        }

        ArenaGroup* pArenaGroup = GetGroup(p_winner->nGroupID);

        int diff_ = p_winner->nRank - p_loser->nRank;
        //½»»»ÅÅÃû
        if (diff_ > 0) {
            int winner_pos = p_winner->nRank;
            int loser_pos = p_loser->nRank;
            if ((size_t)winner_pos < pArenaGroup->m_vevGroupPlayer.size()
                    && (size_t)loser_pos < pArenaGroup->m_vevGroupPlayer.size()) {
                const GroupPlayerInfo tGroupPlayerInfo = pArenaGroup->m_vevGroupPlayer[winner_pos];
                pArenaGroup->m_vevGroupPlayer[winner_pos] = pArenaGroup->m_vevGroupPlayer[loser_pos];
                pArenaGroup->m_vevGroupPlayer[loser_pos] = tGroupPlayerInfo;
                p_winner->nRank -= diff_;
                p_loser->nRank += diff_;
            }
        }

        Upgrade(*p_winner, winner_pos);
        Upgrade(*p_loser, loser_pos);
    }

    int UpdateRankByID(uint64 playerID) {
        std::map<uint64, int>::iterator it = m_mapPlayerID2Index.find(playerID);
        if (it != m_mapPlayerID2Index.end()) {
            int pos = it->second;
            if ((size_t)pos < m_vecArenaRank.size()
                    && playerID == m_vecArenaRank[pos].llId) {

                SArenaRankInfo& rankInfo = m_vecArenaRank[pos];
                int index   = rankInfo.nRank;
                int groupID = rankInfo.nGroupID;

                const ArenaGroup* pArenaGroup = GetGroup(groupID);
                if (pArenaGroup) {
                    int groupPos = pArenaGroup->findGroupPlayer(playerID, index);
                    if (groupPos != -1) {
                        rankInfo.nRank = groupPos;
                        return pos;
                    }
                }
            }
        }
        return -1;
    }

    SArenaRankInfo* GetArenaRankInfoAndUpdate(int pos, int rank_) {
        SArenaRankInfo* pSArenaRankInfo = GetArenaRankInfo(pos);
        if (pSArenaRankInfo) {
            pSArenaRankInfo->nRank = rank_;
        }
        return pSArenaRankInfo;
    }

    SArenaRankInfo* GetArenaRankInfo(int pos) {
        if ((size_t)pos < m_vecArenaRank.size()) {
            return &m_vecArenaRank[pos];
        }
        return NULL;
    }

    ArenaGroup* GetGroup(int group_id) {
        std::vector<ArenaGroup>::iterator it_ = std::lower_bound(m_vevGroup.begin(), m_vevGroup.end(), ArenaGroup(group_id));
        if (it_ != m_vevGroup.end() && it_->m_nGroupID == group_id) {
            return &(*it_);
        }
        return NULL;
    }

    void InitGroup() {
        m_mapNextGroupID[1] = 2; //to be fix
        for (int i = 0; i < 10; ++i) {
            size_t s_ = m_vevGroup.size();
            int group_id = i;        //to be fix
            int group_level = i / 2; //to be fix
            m_vevGroup.resize(s_ + 1);
            m_vevGroup[s_].m_nGroupID = group_id;
            m_vevGroup[s_].m_nLevelID = group_level;
        }
        sort(m_vevGroup.begin(), m_vevGroup.end(), std::less<ArenaGroup>());
    }

    void DelGroupPlayer(const SArenaRankInfo& rankInfo) {
        ArenaGroup* pArenaGroup = GetGroup(rankInfo.nGroupID);
        if (pArenaGroup) {
            int index_ = rankInfo.nRank;
            int group_player_pos = pArenaGroup->findGroupPlayer(rankInfo.llId, index_);
            if (group_player_pos != -1) {
                pArenaGroup->m_vevGroupPlayer.erase(pArenaGroup->m_vevGroupPlayer.begin() + group_player_pos);
            }
        } else {
            //log
        }
    }

    void InsertGroup(const SArenaRankInfo& rankInfo, int player_pos) {
        ArenaGroup* pArenaGroup = GetGroup(rankInfo.nGroupID);
        if (pArenaGroup) {
            pArenaGroup->m_vevGroupPlayer.push_back(GroupPlayerInfo(rankInfo.llId, player_pos));
        } else {
            //log
        }
    }

    void InsertGroupAndFixRank(SArenaRankInfo& rankInfo, int player_pos) {
        ArenaGroup* pArenaGroup = GetGroup(rankInfo.nGroupID);
        if (pArenaGroup) {
            pArenaGroup->m_vevGroupPlayer.push_back(GroupPlayerInfo(rankInfo.llId, player_pos));
            rankInfo.nRank = pArenaGroup->m_vevGroupPlayer.size();
        } else {
            //log
        }
    }

    int AddArena(SArenaRankInfo& rankInfo) {
        size_t s_ = m_vecArenaRank.size();
        if (rankInfo.nGroupID == 0) {
            //int group_id_ = 1;//to be fix //Í¨¹ý serverid »ñÈ¡·Ö×éÐÅÏ¢
            //rankInfo.nGroupID = group_id_;
        }

        m_vecArenaRank.push_back(rankInfo);
        m_mapPlayerID2Index[rankInfo.llId] = s_;
        if (rankInfo.nRank == -1) {
            InsertGroupAndFixRank(rankInfo, s_);
        } else {
            InsertGroup(rankInfo, s_);
        }
        return s_;
    }

    void OnFinishRank() {
        size_t s_ = m_vevGroup.size();
        size_t arenaRankSize = m_vecArenaRank.size();
        for (size_t i = 0; i < s_; ++i) {
            std::vector<GroupPlayerInfo>& vec_ = m_vevGroup[i].m_vevGroupPlayer;
            for (size_t vec_i = 0; vec_i < vec_.size();) {
                if ((size_t)vec_[vec_i].pos < arenaRankSize) {
                    ++vec_i;
                } else {
                    vec_.erase(vec_.begin() + vec_i);
                }
            }
            std::sort(vec_.begin(), vec_.end(), Less(m_vecArenaRank));
        }
    }

    void test_print() {
        std::cout << "=========================" << std::endl;
        size_t s_ = m_vevGroup.size();
        size_t arenaRankSize = m_vecArenaRank.size();
        for (size_t i = 0; i < s_; ++i) {
            std::vector<GroupPlayerInfo>& vec_ = m_vevGroup[i].m_vevGroupPlayer;
            for (size_t vec_i = 0; vec_i < vec_.size(); ++vec_i) {
                std::cout << "group_id:" << i << "\t id:" << vec_[vec_i].llId << "\t pos:" << vec_[vec_i].pos << "\t rank:" << vec_i << std::endl;
            }
        }
    }
};
