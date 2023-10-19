#include <bits/stdc++.h>
#include "blockchain.hpp"
using namespace std;

extern float T_tx;
extern int num_high_cpu;
extern int num_nodes;
extern float low_hashing_power;
extern float high_hashing_power;
extern int num_transactions;
extern int num_blocks;
extern int txn_size;
extern vector<vector<float>> pij;
extern vector<vector<bool>> pset;
extern vector<Node*> nodes;

void print_tree(int id){
    
    ofstream node_file,data_file;
    string filename = "tree" + to_string(id) + ".dot";
    string datafilename = "./output/node" + to_string(id) + "/data" + to_string(id) + ".txt";
    node_file.open (filename);
    data_file.open (datafilename);
    node_file << "digraph D { node [ordering=in]" << endl;
    data_file << "tree data for " << id << endl;
    set<string> st;
    map<int,bool> mp;
    for(auto b : nodes[id-1]->trees){
        vector<string> pt;
        Tree *t = b;
        while(t != NULL){
            string s = "";
            // node_file << t->block->blockid << " [label = \"" << t->block->blockid << " " << t->block->createdby << " " << t->time_arrival << "\"]" << endl;
            node_file << t->block->blockid << " [label = \"" << t->block->blockid << " " << t->block->createdby<< "\"]" << endl;
            s = to_string(t->block->blockid);
            pt.push_back(s);
            if(!mp[t->block->blockid])
                data_file <<"block id "<< t->block->blockid << " created by "<< t->block->createdby<< " arrived at " << t->time_arrival << endl;
            mp[t->block->blockid] = true;
            t = t->parent;
        }
        for(int i = 0; i < pt.size() - 1; i++){
            string s = "";
            s = pt[i] + " -> " + pt[i+1];
            st.insert(s);
        }
    }
    for(auto a : st){
        node_file << a << endl;
    }
    node_file << "}" << endl;
    node_file.close();
    return;
}

int main(int argc, char *argv[]){
    int n;
    float z0, z1,pa;

    n = atoi(argv[1]);
    z0 = atoi(argv[2]);
    z1 = atoi(argv[3]);
    power_attacker = 1.0*atoi(argv[4])/100;
    zeta = (n-1)*(1.0*atoi(argv[5])/100);
    
    int num_slow = (z0 / 100) * n;
    int num_low_cpu = (z1 / 100) * n;

    num_high_cpu = n - num_low_cpu;
    num_nodes = n;

    txn_size = 8000;
    calc_hash();

    T_tx = 0.1;
    num_transactions = 1;
    num_blocks = 1;

    Node *node = new Node[n];
    for (int i = 0; i < n; i++){
        node[i].id = i + 1;
        node[i].balance = 0;
        node[i].my_longest_chain = -1;
        node[i].longest_chain_length = 0;
        node[i].num_blocks_generated =0;
        node[i].isSelfish = false;
        node[i].isStubborn = false;
    }

    vector<int> slow_nodes;
    vector<int> low_cpu_nodes;

    pij = vector<vector<float>>(num_nodes, vector<float>(num_nodes, 0));
    pset = vector<vector<bool>>(num_nodes, vector<bool>(num_nodes, false));

    for (int i = 0; i < n; i++){
        if(i!=0)slow_nodes.push_back(i + 1);
        low_cpu_nodes.push_back(i + 1);
    }



    shuffle(slow_nodes.begin(), slow_nodes.end(), default_random_engine(0));
    shuffle(low_cpu_nodes.begin(), low_cpu_nodes.end(), default_random_engine(12));
  
    for (int i = 0; i < num_slow; i++){
        node[slow_nodes[i] - 1].slow = true;
    }
    for (int i = 0; i < num_low_cpu; i++){
        node[low_cpu_nodes[i] - 1].low_CPU = true;
    }

    add_neighbors(n, node);
    
    // for(int i=0;i<n;i++){
    //     cout << "peers of " << i+1 << " : ";
    //     for(auto a : node[i].peers){
    //         cout << a << " ";
    //     }
    //     cout << endl;
    // }

    for(int i=0;i<n;i++){
        nodes.push_back(&node[i]);
    }
    nodes[0]->isSelfish = true;
    nodes[0]->slow = false;
    int j=0;
    vector<Node*> &nds = nodes;
    priority_queue<Event, vector<Event>, Event> pq;
    calc_hash();
    // ------------------------------------------------------------------------------------------

    generateFirstEvent(pq, nodes, num_nodes);
    
    while (!pq.empty()){

        Event e = pq.top();
        pq.pop();
        int id = e.executedby;
        if(j==25)break;
        switch (e.type){
        case 0:
            e.blockGenerate(*nodes[id-1], pq);
            break;
        case 1:
            e.transactionGenerate(*nodes[id-1], pq);
            break;
        case 2:
            e.blockReceive(*nodes[id-1], pq);
            break;
        case 3:
            e.transactionReceive(*nodes[id-1], pq);
            break;

        default:
            break;
        }
        for(auto a : nodes){
            if(a->id == 1)continue;
            j = a->longest_chain_length > j ? a->longest_chain_length : j;
        }
    }

    for(int i=1;i<=n;i++){
        print_tree(i);
    }

    Node* a;
    int num_mind_by_attacker = 0;
    a = nodes[0];
    int num_blocks_mainchain=a->longest_chain_length ;
    Tree *t= a->trees[a->my_longest_chain];
    while(t->parent!=NULL)
    {
        if(t->block->createdby == 1)
        {
            num_mind_by_attacker++;
        }
        t = t->parent;
    }

    ofstream myfile;
    myfile.open ("output.csv", ios::app);
    float mpu1 = (float)num_mind_by_attacker / (float)nodes[0]->num_blocks_generated;
    float mpu2 = (float)num_blocks_mainchain / (float)num_blocks;
    myfile << power_attacker << "," << mpu1 << "," << mpu2 << endl;
    cout  << power_attacker << "," << mpu1 << "," << mpu2 << endl;
    
}