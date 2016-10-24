// hbb_code.cpp : 定义控制台应用程序的入口点。
//

#include <iostream>
#include <time.h> 
#include <sstream> 
#include <vector>
#include <map>

std::string getnowtime()
{
    time_t t = time(NULL); 
    char tmp[64]; 
    strftime( tmp, sizeof(tmp), "%Y/%m/%d %X",localtime(&t) ); 
    return tmp;
}

typedef unsigned long long uint64;


// player info
struct SPlayerInfo
{ 
	// role info
	uint64     llId;

	// battle ability
	int dwBattleAbility;
	  
	bool operator == (const SPlayerInfo &rhs)
	{
		return this->llId == rhs.llId;
	}

	SPlayerInfo() : llId(0),
		dwBattleAbility(0) {
	}
};

// arena rank info
struct SArenaRankInfo : SPlayerInfo
{
	int    nRank;              // rank
	int    nVictoryNum;        // victory 
	int    nUp;                // > 0 up, < 0 down, 0 not change, 10000 new add
	int		nGroupID;
	int		nServerID;
  
	// construct
	SArenaRankInfo() : nRank(0), nVictoryNum(0), nUp(0),nGroupID(0),nServerID(0)
	{
	}

	// < operator, just for sort function
	bool operator < (const SArenaRankInfo &rhs) const
	{
		return this->nRank < rhs.nRank;
	}
};


struct GroupPlayerInfo
{
	GroupPlayerInfo(uint64 id_ , int pos_):llId(id_),pos(pos_){}
	uint64     llId;
	int     pos;//这个llId 所在 vector 中的位置
};

class ArenaGroup
{
public:
	ArenaGroup():m_groupID(0),m_levelID(0) {};
	ArenaGroup(int id_):m_groupID(id_){};
	int m_groupID;
	int m_levelID;
	std::vector<GroupPlayerInfo> m_groupPlayer;

	int findGroupPlayer(const uint64 player_id , int pos_) const
	{
		size_t s_ = m_groupPlayer.size();
		if(s_ == 0){
			return -1;
		}
		int end_pos = (s_- 1);
		int flag =  (size_t)end_pos < (size_t)pos_ ? end_pos : pos_;
		for(int i = flag ; i >= 0 ; --i){
			if( m_groupPlayer[i].llId == player_id){
				return i;
			}
		}

		for( int i = (int)end_pos ; i > flag ; --i){
			if( m_groupPlayer[i].llId == player_id){
				return i;
			}
		}
		return -1;
	}

	bool operator < (const ArenaGroup &rhs) const
	{
		return this->m_groupID < rhs.m_groupID;
	}
};

struct Less
{
	Less(std::vector<SArenaRankInfo> & vec_):m_vec(vec_){}
	bool operator()(const GroupPlayerInfo & v1, const GroupPlayerInfo & v2)
	{
		return m_vec[v1.pos].nRank < m_vec[v2.pos].nRank; //从小到大排序
	}
	std::vector<SArenaRankInfo> &m_vec;
};

class ArenaRankData
{

	std::map<int , int> m_nextGroupID;
	std::map<uint64 , int > m_playerID2playerIndex;
	std::vector< ArenaGroup > m_group;
	std::vector<SArenaRankInfo> m_vecArenaRank;
	//to be fix  去掉去 m_vecArenaRank的排序

public:
	std::vector<SArenaRankInfo> & GetArenaRank(){return m_vecArenaRank;}

	void Upgrade(SArenaRankInfo & SArenaRankInfo_ , int pos_)
	{
		int rank_ = 100;//to be fix //晋级的条件
		int ability_ = 2000;//to be fix //晋级的条件
		if(SArenaRankInfo_.nRank <= rank_ 
			&& SArenaRankInfo_.dwBattleAbility >= ability_){
				//获取下一个组的id
				std::map<int , int>::iterator it_ = m_nextGroupID.find(SArenaRankInfo_.nGroupID);
				if(it_ != m_nextGroupID.end() ){
					//从原来组中移除
					DelGroupPlayer(SArenaRankInfo_);
					//加入新的组中
					SArenaRankInfo_.nGroupID = it_->second;
					InsertGroupAndFixRank(SArenaRankInfo_ , pos_);
				}
		}
	}


	void SwapRank(uint64 winner , uint64 loser)
	{
		//id 到 组容器
		int winner_pos = UpdateRankByID(winner);
		int loser_pos =  UpdateRankByID(loser);
		SArenaRankInfo * p_winner = GetArenaRankInfo( winner_pos );
		SArenaRankInfo * p_loser = GetArenaRankInfo( loser_pos);
		if(p_winner == NULL || p_loser == NULL){
			//log
			return;
		}

		if(p_winner->nGroupID != p_loser->nGroupID){
			//log
			return;
		}

		ArenaGroup * pArenaGroup = GetGroup(p_winner->nGroupID);

		//排名的差值
		int diff_ = p_winner->nRank - p_loser->nRank;
		//交换排名
		if(diff_ > 0){
			int winner_pos = p_winner->nRank;
			int loser_pos = p_loser->nRank;
			if( (size_t)winner_pos < pArenaGroup->m_groupPlayer.size()
				&& (size_t)loser_pos < pArenaGroup->m_groupPlayer.size() ){
				const GroupPlayerInfo tGroupPlayerInfo = pArenaGroup->m_groupPlayer[winner_pos ];
				pArenaGroup->m_groupPlayer[ winner_pos] = pArenaGroup->m_groupPlayer[loser_pos];
				pArenaGroup->m_groupPlayer[ loser_pos] = tGroupPlayerInfo;
				p_winner->nRank -= diff_;
				p_loser->nRank += diff_;
			}
		}
		
		//是否突破
		Upgrade( *p_winner , winner_pos);
		Upgrade( *p_loser , loser_pos);
	}
	
	int UpdateRankByID(uint64 player_id)
	{
		std::map<uint64 , int >::iterator it_ = m_playerID2playerIndex.find(player_id);
		if(it_ != m_playerID2playerIndex.end() ){
			int pos_ = it_->second;
			if((size_t)pos_ < m_vecArenaRank.size() && player_id == m_vecArenaRank[pos_].llId ){
				SArenaRankInfo & SArenaRankInfo_ = m_vecArenaRank[pos_];
				int index_ = SArenaRankInfo_.nRank;//玩家类中 记录的排名有可能 过期
				int group_id = SArenaRankInfo_.nGroupID;

				const ArenaGroup * pArenaGroup = GetGroup(group_id);
				if(pArenaGroup)
				{
					int group_player_pos = pArenaGroup->findGroupPlayer(player_id , index_);
					if(group_player_pos != -1 ){
						SArenaRankInfo_.nRank = group_player_pos;
						return pos_ ;
					}
				}
				else
				{
					//log
				}
			}
			else{
				//log
			}
		}
		return -1;
	}

	//更新排名,并返回对象
	SArenaRankInfo *  GetArenaRankInfoAndUpdate(int pos_ , int  rank_)
	{
		SArenaRankInfo * pSArenaRankInfo = GetArenaRankInfo(pos_);
		if(pSArenaRankInfo ){
			pSArenaRankInfo->nRank = rank_;
		}
		return pSArenaRankInfo;
	}


	SArenaRankInfo * GetArenaRankInfo(int pos_)
	{
		if((size_t)pos_ < m_vecArenaRank.size() ){
			return &m_vecArenaRank[pos_];
		}
		return NULL;
	}


	ArenaGroup * GetGroup(int group_id)
	{
		std::vector< ArenaGroup >::iterator  it_ = std::lower_bound (m_group.begin() , m_group.end() ,ArenaGroup(group_id) );
		if(it_ != m_group.end() && it_->m_groupID == group_id)
		{
			return &(*it_);
		}
		return NULL;
	}


	void InitGroup()
	{
		//遍历csv 容器

		m_nextGroupID[1] = 2;//to be fix 
		for(int i = 0 ; i < 10 ; ++i)
		{
			size_t s_ = m_group.size();
			int group_id = i;//to be fix
			int group_level = i/2;//to be fix
			m_group.resize(s_ + 1);
			m_group[s_].m_groupID = group_id;
			m_group[s_].m_levelID = group_level;
		}
		sort(m_group.begin() , m_group.end() , std::less<ArenaGroup>() );
	}

	void DelGroupPlayer(const SArenaRankInfo & SArenaRankInfo_)
	{
		ArenaGroup * pArenaGroup = GetGroup(SArenaRankInfo_.nGroupID);
		if(pArenaGroup)
		{
			int index_ = SArenaRankInfo_.nRank;
			int group_player_pos = pArenaGroup->findGroupPlayer( SArenaRankInfo_.llId , index_);
			if(group_player_pos != -1 ){
				pArenaGroup->m_groupPlayer.erase(pArenaGroup->m_groupPlayer.begin() + group_player_pos);
			}
		}
		else
		{
			//log
		} 
	}

	void InsertGroup(const SArenaRankInfo & SArenaRankInfo_ , int player_pos)
	{
		ArenaGroup * pArenaGroup = GetGroup(SArenaRankInfo_.nGroupID);
		if(pArenaGroup)
		{
			pArenaGroup->m_groupPlayer.push_back(GroupPlayerInfo(SArenaRankInfo_.llId ,player_pos ) );
		}
		else
		{
			//log
		} 
	}

	void InsertGroupAndFixRank( SArenaRankInfo & SArenaRankInfo_ , int player_pos)
	{
		ArenaGroup * pArenaGroup = GetGroup(SArenaRankInfo_.nGroupID );
		if(pArenaGroup)
		{
			pArenaGroup->m_groupPlayer.push_back(GroupPlayerInfo(SArenaRankInfo_.llId ,player_pos ));
			SArenaRankInfo_.nRank = pArenaGroup->m_groupPlayer.size();
		}
		else
		{
			//log
		}
	}

	int AddArena( SArenaRankInfo & SArenaRankInfo_)
	{
		size_t s_ = m_vecArenaRank.size();
		if(SArenaRankInfo_.nGroupID == 0){
			//int group_id_ = 1;//to be fix //通过 serverid 获取分组信息
			//SArenaRankInfo_.nGroupID = group_id_;
		}


		m_vecArenaRank.push_back(SArenaRankInfo_);
		m_playerID2playerIndex[SArenaRankInfo_.llId] = s_;
		if(SArenaRankInfo_.nRank == -1)
		{
			InsertGroupAndFixRank(SArenaRankInfo_ , s_);
		}
		else
		{
			InsertGroup(SArenaRankInfo_ , s_);
		}
		return s_;
	}

	void OnFinishRank()
	{
		size_t s_ = m_group.size();
		size_t arenaRankSize = m_vecArenaRank.size();
		for(size_t i = 0 ; i < s_ ; ++i)
		{
			std::vector<GroupPlayerInfo> & vec_ = m_group[i].m_groupPlayer;
			for(size_t vec_i = 0 ; vec_i < vec_.size() ; )
			{
				if((size_t)vec_[vec_i].pos <  arenaRankSize)
				{
					++ vec_i;
				}
				else
				{
					vec_.erase(vec_.begin() + vec_i);
				}
			}
			std::sort(vec_.begin() , vec_.end() , Less(m_vecArenaRank) );
		}
	}

	void test_print()
	{
		std::cout<<"========================="<<std::endl;
		size_t s_ = m_group.size();
		size_t arenaRankSize = m_vecArenaRank.size();
		for(size_t i = 0 ; i < s_ ; ++i)
		{
			std::vector<GroupPlayerInfo> & vec_ = m_group[i].m_groupPlayer;
			for(size_t vec_i = 0 ; vec_i < vec_.size() ; ++vec_i)
			{
				std::cout<<"group_id:"<<i<<"\t id:"<<vec_[vec_i].llId<<"\t pos:"<<vec_[vec_i].pos <<"\t rank:"<<vec_i  <<std::endl;
			}
		}
	}
};




float fun2()
{
	std::vector<int > vec_s;
	std::map<int , int> map_;
	const int max_ = 10;
	for(int i = 0 ; i < max_ ; ++i){
		map_[i] = i;
		vec_s.push_back(i);
	}

	const int test_num = 2000000;
	auto now_t = time(NULL);
	int now_t_diff;
	for(int i = 0 ; i < test_num; ++i){
		int num = rand()%max_;
		auto it_ =  map_.find(num);
		if(it_ != map_.end() ){
			int b = it_->second;
			if(b != num){
				int c =0;
			}
		}
	}
	now_t_diff = time(NULL) -now_t ;


	now_t = time(NULL);
	for(int i = 0 ; i < test_num ; ++i){
		int num = rand()%max_;
		for(int j = 0 ; j < vec_s.size() ; ++j){
			int b = vec_s[j];
			if(b == num){
				break;
			}
		}
	}
	now_t_diff = time(NULL) -now_t ;


	now_t = time(NULL);
	for(int i = 0 ; i < test_num ; ++i){
		int num = rand()%max_;
		auto it_ =  std::lower_bound(vec_s.begin() , vec_s.end() , num);
		if(it_ !=  vec_s.end() ){
			int b = *it_;
			if(b != num){
				int c =0;
			}
		}
	}
	now_t_diff = time(NULL) -now_t ;

	 return 0;
}



int _tmain(int argc, _TCHAR* argv[])
{

	fun2();
	return 0;
}

