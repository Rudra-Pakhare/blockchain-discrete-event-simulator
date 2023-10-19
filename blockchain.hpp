#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <bits/stdc++.h>
using namespace std;

extern float T_tx;
extern int num_high_cpu;
extern int num_nodes;
extern float low_hashing_power;
extern float high_hashing_power;
extern int num_transactions;
extern int num_blocks;
extern int txn_size;
extern int zeta;
extern float power_attacker;
extern vector<vector<float>> pij;
extern vector<vector<bool>> pset;

void calc_hash();
vector<string> getTransaction(string transaction);
float networkLatency(bool n1, bool n2, int size,int i,int j);
float transactionDelay();
float blockPowDelay(int id,bool n);
int getBlockSize(int ntxn);

class Block{
    public:
        int blockid;
        int prev_blockid;
        int createdby;
        int num_transactions;
        set<string> transactions;
};

class Tree{
    public:
        Tree *parent;
        Block *block;
        int length;
        float time_arrival;
        vector<int> balances;
};

class Node{

    public:
        int balance;
        int id;
        bool slow;
        bool low_CPU;
        set<int> peers;
        int my_longest_chain;
        int longest_chain_length;
        int num_blocks_generated;
        vector<Tree*> trees;
        set<int> seen_blocks;
        map<Block*,bool> block_pool;
        map<int,Tree*> blockTreeMap;
        vector<Block*> childBlock;

        map<int,bool> not_forwarded;
        map<int,string> seen_transactions;
        map<int,string> txn_in_longest_chain;

        string generateTransaction();
        void addTransaction(string transaction);
        string generateCoinbase();
        Block* generateGenesisBlock();
        void updateBalance();
        Block* generateBlock();
        void addBlock(Block *block,float time);
        bool checkValid(Block *block);

        //selfish mining
        bool isSelfish;
        bool isStubborn;
};

extern vector<Node*> nodes;
void add_neighbors(int n, Node *nodes);
void dfs(Node *nodes, int curr_node, bool *visited);

class Event{
    public:
    int createdby;
    int executedby;
    int type;
    float timecreated;
    float timeexecuted;
    string transaction;
    Block *block;

    Event(int createdby, int executedby, int type, float timecreated, float timeexecuted, string transaction, Block *block);
    Event(){}; //default constructor

    bool operator()(Event e1,Event e2);

    //type 0 - block generate
    void blockGenerate(Node &n, priority_queue<Event, vector<Event>, Event> &event_queue);
    //type 1 - transaction generate
    void transactionGenerate(Node &n, priority_queue<Event, vector<Event>, Event> &event_queue);
    //type 2 - block receive
    void blockReceive(Node &n, priority_queue<Event, vector<Event>, Event> &event_queue);
    //type 3 - transaction receive
    void transactionReceive(Node &n, priority_queue<Event, vector<Event>, Event> &event_queue);
};

void generateFirstEvent(priority_queue<Event, vector<Event>, Event> &event_queue, vector<Node*> nodes, int n);

template <typename S>
auto select_random(const S &s, size_t n){
    auto it = std::begin(s);
    std::advance(it, n);
    return it;
}

#endif