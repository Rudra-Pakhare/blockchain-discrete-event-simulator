#include "blockchain.hpp"

float T_tx;
int num_high_cpu;
int num_nodes;
float low_hashing_power;
float high_hashing_power;
int num_transactions;
int num_blocks;
int txn_size;
float power_attacker ;
int zeta;
vector<vector<float>> pij;
vector<vector<bool>> pset;
vector<Node*> nodes;
vector<int> nblocks;
vector<int> ntxn;

vector<string> getTransaction(string transaction){
    vector<string> txn(6);
    string temp = "";
    int i=0;
    for(auto a: transaction){
        if(a == ' ' || a == ':'){
            if(temp == "")
                continue;
            txn[i] = temp;
            i++;
            temp = "";
        }
        else{
            temp += a;
        }
    }
    txn[i] = temp;
    return txn;
}

void calc_hash(){
   
    low_hashing_power = (1-power_attacker)/((1.0*num_nodes - num_high_cpu) + 10*(num_high_cpu) );
    high_hashing_power = 10 * low_hashing_power;
}

void generateFirstEvent(priority_queue<Event, vector<Event>, Event> &event_queue, vector<Node*> nodes, int n){
    int j = rand() % num_nodes +1;
    Event e = Event(j, j, 0, 0, blockPowDelay(j,nodes[j-1]->low_CPU), "", nodes[j-1]->generateGenesisBlock());
    event_queue.push(e);
    for (int i = 0; i < n; i++){
        Event e2 = Event(i+1, i+1, 1, 0, transactionDelay(), nodes[i]->generateTransaction(), NULL);
        event_queue.push(e2);
    }
}

float networkLatency(bool n1, bool n2, int size,int i,int j){
    float larency = 0;
    std::random_device rd;
    std::mt19937 gen(rd());

    if(pset[i][j] || pset[j][i]){}
    else {
        std::uniform_real_distribution<> dis(10, 500);
        pij[i][j] = dis(gen) * 10e-3;
        pset[i][j] = true;
        pij[j][i] = pij[i][j];
        pset[j][i] = true;
    }
    if (n1 || n2){
        float cij = 5 * 10e6;
        std::exponential_distribution<> d(1/((96 / cij) * 10e3));
        float dij = d(gen);
        return pij[i][j] + size / cij + dij;
    }
    else{
        float cij = 100 * 10e6;
        std::exponential_distribution<> d(1/((96 / cij) * 10e3));
        float dij = d(gen);
        return pij[i][j] + size / cij + dij;
    }
}

float transactionDelay(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::exponential_distribution<> d(1/T_tx);
    float delay = d(gen);
    return delay;
}

float blockPowDelay(int id,bool n){
    std::random_device rd;
    std::mt19937 gen(rd());
    if(id == 1){
        std::exponential_distribution<> d(power_attacker/5);
        float delay = d(gen);
        return delay;
    }
    if (n){
        std::exponential_distribution<> d(low_hashing_power/5);
        float delay = d(gen);
        return delay;
    }
    std::exponential_distribution<> d(high_hashing_power/5);
    float delay = d(gen);
    return delay;
}

int getBlockSize(int ntxn){
    int size = ntxn * 1000*8;
    return size;
}

bool Event::operator()(Event e1,Event e2){
    return e1.timeexecuted > e2.timeexecuted;
}

void Node::addTransaction(string transaction){
    vector <string> v = getTransaction(transaction);
    this->seen_transactions[stoi(v[0])] = transaction;
}

string Node::generateTransaction(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, num_nodes), dis2(0, max(this->balance,0));
    int to = dis(gen);
    int amount = dis2(gen);
    int a;
    if(ntxn.size() == 0){
        a = num_transactions++;
    }
    else{
        a = ntxn[0];
        ntxn.erase(ntxn.begin());
    }
    string transaction = to_string(a) + ": " + to_string(id) + " pays " + to_string(to) + " " + to_string(amount) + " coins";
    if(amount != 0)
        this->seen_transactions[a] = transaction;
    this->balance -= amount;
    return transaction;
}

string Node::generateCoinbase(){
    int a;
    if(ntxn.size() == 0){
        a = num_transactions++;
    }
    else{
        a = ntxn[0];
        ntxn.erase(ntxn.begin());
    }
    string s = to_string(a) + ": " + to_string(id) + " mines " + to_string(50) + " coins";
    return s;
}

Block* Node::generateGenesisBlock(){
    // function to generate genesis block
    Block *block = new Block(); // create a new block
    if(nblocks.size() == 0){
        block->blockid = num_blocks++;
    }
    else{
        block->blockid = nblocks[0];
        nblocks.erase(nblocks.begin());
    }
    block->prev_blockid = -1;
    block->createdby = this->id;

    // verify transactions
    int verifyval[num_nodes] = {0};
    int i=0;
    for(auto a:this->seen_transactions){
        if(i==999)break;
        vector<string> txn = getTransaction(this->seen_transactions[a.first]);
        verifyval[stoi(txn[1])-1] += stoi(txn[4]);

        // if my total balance becomes negative, then don't add this transaction
        if(verifyval[stoi(txn[1])-1] > 0)verifyval[stoi(txn[1])-1] -= stoi(txn[4]);
        else block->transactions.insert(this->seen_transactions[a.first]);
        i++;
    }

    block->num_transactions = block->transactions.size()+1;

    // add coinbase transaction
    block->transactions.insert(generateCoinbase());
    return block;
}

Block* Node::generateBlock(){
    Block* block = new Block(); // create a new block
    if(nblocks.size() == 0){
        block->blockid = num_blocks++;
    }
    else{
        block->blockid = nblocks[0];
        nblocks.erase(nblocks.begin());
    }
    block->prev_blockid = this->trees[this->my_longest_chain]->block->blockid; // set the previous block id to the block id of the longest chain
    block->createdby = this->id;

    // create a map of all the transactions that have been added to the longest chain
    map<int,string> added_transactions({});
    Tree *t = this->trees[this->my_longest_chain];
    while (t != NULL){
        for (auto a : t->block->transactions){
            added_transactions[stoi(getTransaction(a)[0])] = a;
        }
        t = t->parent;
    }
    // find the difference between the transactions that have been seen by this node and the transactions that have been added to the longest chain
    map<int,string> diff({});
    set_difference(this->seen_transactions.begin(), this->seen_transactions.end(), added_transactions.begin(), added_transactions.end(), inserter(diff, diff.begin()));

    // verify transactions
    int verifyval[num_nodes] = {0};
    int i=0;
    for(auto a:diff){
        if(i==999)break;
        vector<string> txn = getTransaction(this->seen_transactions[a.first]);
        verifyval[stoi(txn[1])-1] += stoi(txn[4]);
        // if my total balance becomes negative, then don't add this transaction
        if(verifyval[stoi(txn[1])-1] > t->balances[stoi(txn[1])-1])verifyval[stoi(txn[1])-1] -= stoi(txn[4]);
        else block->transactions.insert(this->seen_transactions[a.first]);
        i++;
    }
    // add coinbase transaction
    block->transactions.insert(generateCoinbase());
    block->num_transactions = block->transactions.size();
    return block;
}

Event::Event(int createdby, int executedby, int type, float timecreated, float timeexecuted, string transaction, Block *block){
    this->type = type;
    this->createdby = createdby;
    this->executedby = executedby;
    this->timecreated = timecreated;
    this->timeexecuted = timeexecuted;
    this->transaction = transaction;
    this->block = block;
}

void Node::addBlock(Block *block,float time){
    // add the block to the block tree
    Tree *t = new Tree();
    t->parent = this->blockTreeMap[block->prev_blockid]; // set the parent of the new block to the block that was added before this block
    t->block = block;
    t->time_arrival = time;
    t->length = this->blockTreeMap[block->prev_blockid]==NULL? 1 : this->blockTreeMap[block->prev_blockid]->length + 1;
    this->blockTreeMap[block->blockid] = t;
    if(t->parent != NULL)
        t->balances = t->parent->balances;
    else t->balances = vector<int>(num_nodes, 0);
    vector<Tree*>::iterator it = find(this->trees.begin(), this->trees.end(), t->parent);
    // if the parent of the new block is in the list of trees, then replace the parent with the new block
    for (auto a : block->transactions){
        vector<string> txn = getTransaction(a);
        this->txn_in_longest_chain[stoi(txn[0])] = a;
        if (txn[2] == "mines"){
            t->balances[stoi(txn[1])-1] += 50;
        }
        else{
            t->balances[stoi(txn[1])-1] -= stoi(txn[4]);
            t->balances[stoi(txn[3])-1] += stoi(txn[4]);
        }
    }
    
    if(it != this->trees.end()){
        this->trees[it-this->trees.begin()] = t;
        // update balance
        if (t->length > this->longest_chain_length){
            this->longest_chain_length = t->length;
            this->my_longest_chain = it - this->trees.begin();
        }
    }
    else {
        // side chain is created
        this->trees.push_back(t);

        // if this becomes the longest chain, then update balance
        if (t->length > this->longest_chain_length){
            this->longest_chain_length = t->length;
            this->my_longest_chain = trees.size() - 1;
            // updateBalance();
        }
    }
    // find the child of the block that was added in the block pool
    Block *child;
    bool found = false;
    for(auto a : this->block_pool){
        if(block->blockid == a.first->prev_blockid && a.second == true){
            child = a.first;
            found = true;
            a.second = false;
            this->childBlock.push_back(child);
            break;
        }
    }
    if(found){
        // found the child, now add it to the block tree and recursively remove blocks from the block pool
        if(checkValid(child))
            addBlock(child,time);
    }
    return ;
}

bool Node::checkValid(Block *block){
    // if the block is genesis block
    if(block->prev_blockid == -1){
        // verify transactions
        for (auto a : block->transactions){
            vector<string> txn = getTransaction(a);
            if (txn[2] == "mines"){
                if (stoi(txn[3]) > 50)
                    return false;
            }
            else{
                if (0 < stoi(txn[4]))
                    return false;
            }
        }
        return true;
    }
    // if the parent of the block is in the tree
    if(this->blockTreeMap.find(block->prev_blockid) != this->blockTreeMap.end()){
        Tree *t1;
        t1 = this->blockTreeMap[block->prev_blockid];
        int verifybal[num_nodes] = {0};
        for (auto a : block->transactions){
            vector<string> txn = getTransaction(a);
            if (txn[2] == "mines"){
                if (stoi(txn[3]) > 50)
                    return false;
            }
            else{
                verifybal[stoi(txn[1])-1] += stoi(txn[4]);
            }
        }
        for(int i=0; i<num_nodes; i++){
            if(t1->balances[i] < verifybal[i])return false;
        }
    }
    else {
        this->block_pool[block] = true;
        return false;
    }
    return true;
}

void Event::transactionGenerate(Node &n, priority_queue<Event, vector<Event>, Event> &event_queue){
    if(n.my_longest_chain == -1){
        n.balance=0;
    }
    string txn = n.generateTransaction();
    vector<string> tx = getTransaction(txn);
    // if the amount is 0, then schedule next transaction without generating recieve events
    if(tx[4] == "0"){
        ntxn.push_back(stoi(tx[0]));
        float delay = transactionDelay();
        Event e = Event(n.id, n.id, 1, this->timeexecuted, this->timeexecuted + delay, txn, NULL);
        event_queue.push(e);
        return;
    }
    n.seen_transactions[stoi(tx[0])] = txn;
    float delay = transactionDelay();
    Event e = Event(n.id, n.id, 1, this->timeexecuted, this->timeexecuted + delay, txn, NULL);
    event_queue.push(e);

    // generate recieve events
    for(auto a : n.peers){
        float latency = networkLatency(nodes[n.id-1]->slow, nodes[a-1]->slow, txn_size,n.id-1,a-1);
        Event e = Event(n.id, a, 3, this->timeexecuted, this->timeexecuted + latency, txn, NULL);
        event_queue.push(e);
    }
}

void Event::transactionReceive(Node &n, priority_queue<Event, vector<Event>, Event> &event_queue){
    vector<string> tx = getTransaction(this->transaction);
    // if the amount is 0, then return
    if(tx[4] == "0"){
        ntxn.push_back(stoi(tx[0]));
        return;
    }
    // if the transaction is not seen before
    if(n.seen_transactions.find(stoi(tx[0])) == n.seen_transactions.end()){
        n.seen_transactions[stoi(tx[0])] = this->transaction;
        
        for(auto a : n.peers){
            if(a == this->createdby)continue;
            float latency = networkLatency(nodes[n.id-1]->slow, nodes[a-1]->slow, txn_size,n.id-1,a-1);
            Event e = Event(n.id, a, 3, this->timeexecuted, this->timeexecuted + latency, this->transaction, NULL);
            event_queue.push(e);
        }
    }
    // if the transaction is seen before but not forwarded as i have not seen it before in block
    else if(n.not_forwarded[stoi(tx[0])] == false){
        n.not_forwarded[stoi(tx[0])] = true;
        for(auto a : n.peers){
            if(a == this->createdby)continue;
            float latency = networkLatency(nodes[n.id-1]->slow, nodes[a-1]->slow, txn_size,n.id-1,a-1);
            Event e = Event(n.id, a, 3, this->timeexecuted, this->timeexecuted + latency, this->transaction, NULL);
            event_queue.push(e);
        }
    }
}

void Event::blockGenerate(Node &n, priority_queue<Event, vector<Event>, Event> &event_queue){

    n.num_blocks_generated++;
    // if i have successfully generated genesis block before other nodes then event success
    if(this->block->prev_blockid == -1 && n.my_longest_chain == -1){
        n.addBlock(this->block,this->timeexecuted);
    }
    // if i have other blocks in my block tree and i have not generated genesis block before other nodes then event failure
    else if(this->block->prev_blockid == -1 && n.my_longest_chain != -1){nblocks.push_back(this->block->blockid);return;}
    // if i have successfully generated a block before other nodes on the longest chain then event success
    else if(n.my_longest_chain != -1 && this->block->prev_blockid == n.trees[n.my_longest_chain]->block->blockid){
        n.addBlock(this->block,this->timeexecuted);
    }
    else {nblocks.push_back(this->block->blockid);return;}

    n.seen_blocks.insert(this->block->blockid);

    //schedule next block generation

    Block *blk = n.generateBlock();
    float delay = blockPowDelay(n.id,n.low_CPU);
    Event e = Event(n.id, n.id, 0, this->timeexecuted, this->timeexecuted + delay, "", blk);
    event_queue.push(e);

    if(n.isSelfish || n.isStubborn){return;}

    // schedule block recieve events
    for(auto a : n.peers){
        float latency = networkLatency(nodes[n.id-1]->slow, nodes[a-1]->slow, getBlockSize(this->block->num_transactions),n.id-1,a-1);
        Event e = Event(n.id, a, 2, this->timeexecuted, this->timeexecuted + latency, "", this->block);
        event_queue.push(e);
    }
}

void Event::blockReceive(Node &n, priority_queue<Event, vector<Event>, Event> &event_queue){
    if(this->block->blockid < 0){
        return;
    }
    // if i have already seen this block then return
    if(n.seen_blocks.find(this->block->blockid) != n.seen_blocks.end()){return;}
    n.seen_blocks.insert(this->block->blockid);

    // add transactions to seen transactions with not forwarded flag
    for(auto a : this->block->transactions){
        vector<string> tx = getTransaction(a);
        if(n.seen_transactions.find(stoi(tx[0])) == n.seen_transactions.end()){
            if(tx[2] == "mines")continue;
            n.not_forwarded[stoi(tx[0])] = true;
        }
        n.seen_transactions[stoi(tx[0])] = a;
    }

    if(this->block->createdby == 1){
            int a,b;
            b=a;
        }

    // add block to block tree after checking validity
    if(n.checkValid(this->block)){
        n.addBlock(this->block,this->timeexecuted);
        if(n.my_longest_chain==-1 || this->block->blockid == n.trees[n.my_longest_chain]->block->blockid){
            Block *blk = n.generateBlock();
            float delay = blockPowDelay(n.id,n.low_CPU);
            Event e = Event(n.id, n.id, 0, this->timeexecuted, this->timeexecuted + delay, "", blk);
            event_queue.push(e);
        }
        else if (n.isSelfish || n.isStubborn) {
            if(n.longest_chain_length == (n.blockTreeMap[this->block->blockid]->length + 1)){
                Block *blk = n.trees[n.my_longest_chain]->block;
                if(!n.isStubborn){
                    for(auto a : n.peers){
                        if(a == this->createdby)continue;
                        float latency = networkLatency(nodes[n.id-1]->slow, nodes[a-1]->slow, getBlockSize(blk->num_transactions),n.id-1,a-1);
                        Event e = Event(n.id, a, 2, this->timeexecuted, this->timeexecuted + latency, "", blk);
                        event_queue.push(e);
                    }
                }
                if(blk->prev_blockid != -1){
                    blk = n.blockTreeMap[blk->prev_blockid]->block;
                    for(auto a : n.peers){
                        if(a == this->createdby)continue;
                        float latency = networkLatency(nodes[n.id-1]->slow, nodes[a-1]->slow, getBlockSize(blk->num_transactions),n.id-1,a-1);
                        Event e = Event(n.id, a, 2, this->timeexecuted, this->timeexecuted + latency, "", blk);
                        event_queue.push(e);
                    }
                }
            }
            else if(n.longest_chain_length == n.blockTreeMap[this->block->blockid]->length){
                Block *blk = n.trees[n.my_longest_chain]->block;
                for(auto a : n.peers){
                    if(a == this->createdby)continue;
                    float latency = networkLatency(nodes[n.id-1]->slow, nodes[a-1]->slow, getBlockSize(blk->num_transactions),n.id-1,a-1);
                    Event e = Event(n.id, a, 2, this->timeexecuted, this->timeexecuted + latency, "", blk);
                    event_queue.push(e);
                }
            }
            else if(n.longest_chain_length > n.blockTreeMap[this->block->blockid]->length + 1) {
                Tree *t = n.trees[n.my_longest_chain];
                Block *blk = n.trees[n.my_longest_chain]->block;
                while( t != NULL){
                    Block *blk = t->block;
                    if(t->length == n.blockTreeMap[this->block->blockid]->length){
                        for(auto a : n.peers){
                            if(a == this->createdby)continue;
                            float latency = networkLatency(nodes[n.id-1]->slow, nodes[a-1]->slow, getBlockSize(blk->num_transactions),n.id-1,a-1);
                            Event e = Event(n.id, a, 2, this->timeexecuted, this->timeexecuted + latency, "", blk);
                            event_queue.push(e);
                        }
                        break;
                    }
                    t = t->parent;
                }
            }

            while(!n.childBlock.empty()){
                Block *blk = n.childBlock.back();
                n.childBlock.pop_back();
            }
            return;
        } // new code assign2 ----------

        // schedule block recieve events
        for(auto a : n.peers){
            if(a == this->createdby)continue;
            float latency = networkLatency(nodes[n.id-1]->slow, nodes[a-1]->slow, getBlockSize(this->block->num_transactions),n.id-1,a-1);
            Event e = Event(n.id, a, 2, this->timeexecuted, this->timeexecuted + latency, "", this->block);
            event_queue.push(e);
        }
        // schedule block recieve events for all the blocks in child block list which were in the block pool
        while(!n.childBlock.empty()){
            Block *blk = n.childBlock.back();
            n.childBlock.pop_back();
            for(auto a : n.peers){
                float latency = networkLatency(nodes[n.id-1]->slow, nodes[a-1]->slow, getBlockSize(blk->num_transactions),n.id-1,a-1);
                Event e = Event(n.id, a, 2, this->timeexecuted, this->timeexecuted + latency, "", blk);
                event_queue.push(e);
            }
        }
    }
}

void dfs(Node *nodes, int curr_node, bool *visited){
    visited[curr_node] = true;
    for (auto it = nodes[curr_node].peers.begin(); it != nodes[curr_node].peers.end(); it++){
        if (!visited[*it - 1]){
            dfs(nodes, *it - 1, visited);
        }
    }
}

void add_neighbors(int n, Node *nodes){
    bool c = 0;
    bool visited[n];
    for (int i = 0; i < n; i++){
        visited[i] = false;
    }
    while (!c){
        set<int> s;
        for (int i = 0; i < n; i++){
            s.insert(i + 1);
            nodes[i].peers.clear();
        }
        for (int i = 0; i < n; i++){
            if (nodes[i].peers.size() < 8){
                set<int> temp = s;
                temp.erase(i + 1);
                if(nodes[0].peers.size()==zeta)
                    temp.erase(1);
                for (auto it = nodes[i].peers.begin(); it != nodes[i].peers.end(); it++){
                    temp.erase(*it);
                }
                for (int i = 0; i < n; i++){
                    if (nodes[i].peers.size() == 8){
                        temp.erase(i + 1);
                        s.erase(i + 1);
                    }
                }
                int nn = rand() % 5 + 4;
                if(i==0)
                    nn = zeta;
                int req_nn = nn - nodes[i].peers.size();
                for (int k = 0; k < req_nn; k++){

                    if(temp.size() == 0)break;
                    auto r = rand() % temp.size();
                    auto x = *select_random(temp, r);
                    nodes[i].peers.insert(x);
                    nodes[x - 1].peers.insert(i + 1);
                    temp.erase(x);
                }
            }
            if (i == n - 1){
                c = 1;
            }
        }
        dfs(nodes, 1, visited);

        for (int i = 0; i < n; i++){
            if (!visited[i]){
                c = 0;
                break;
            }
            if (nodes[i].peers.size() < 4){
                if(i!=0)
                {
                    c = 0;
                    break;
                }
            }
        }
        if (c == 1){
            break;
        }
        else{
            cout << "restarting" << endl;
            s.clear();
        }
    }
    return;
}
