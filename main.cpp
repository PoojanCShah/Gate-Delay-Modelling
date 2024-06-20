#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map> 
#include <queue> 
#include <unordered_map>
#include <set>
#include <algorithm> 
#include <cmath> 
#include <string>

using namespace std;



class Gate
{
    public:
    Gate();
    Gate(float dlay, float ar);
    string id ; 
    vector<float> delay;
    vector<float> area;
    float cur_delay = 0 ;
    float cur_area =0  ; 
    
     void perturb( int i );
    int type;
   
    
};


Gate::Gate()
{
    type = 0;
}

Gate::Gate(float dlay,float ar)
{
    
    delay.push_back(dlay);
    area.push_back(ar);
    if(delay.size()== 3)
    {
        sort(delay.begin(),delay.end());
    }

}

void Gate::perturb(int i)
{
    
    this->cur_area = area[i];
    this->cur_delay = delay[i];
    this->type = i;

}

class Signal
{
    public:
    Signal(string name);
    Signal* parent1;
    Signal* parent2;
    vector<Signal*> children;
    Gate ingate; 
    int mark = 0;
    float delay = 0;
    string id;
    int marked_yet = 0 ; 
     bool operator<(const Signal& other) const {
        if(this->mark == other.mark)
        {
            return this->ingate.delay[0] < other.ingate.delay[0]; 
        }
        return this->mark > other.mark;
    }
    
};

Signal::Signal(string name)
{
    parent1 = NULL;
    parent2 = NULL;
    delay = 0;
    id = name;
}








using namespace std;

class Circuit
{
public:
    Circuit();
    vector<Signal *> signals = {};
    void insert(Signal *sig);
    float total_area();
    float max_delay();
    int size = 0;
};

float Circuit::total_area()
{
    float ar = 0;
    for(auto s : signals)
    {
        if(s->parent1 || s->parent2)
        {
            ar+=s->ingate.cur_area;
        }
    }
    return ar;
}

Circuit::Circuit() {}

void Circuit::insert(Signal *sig)
{
    signals.push_back(sig);
    size++;
}

float Circuit::max_delay()
{
    map<Signal *, int> indegree;
    vector<vector<Signal *>> layers;
    for (int i = 0; i < size; i++)
    {
        int in_deg = 0;
        if (signals[i]->parent1)
            in_deg++;
        if (signals[i]->parent2)
            in_deg++;
        indegree[signals[i]] = in_deg;
    }

    queue<Signal *> Q;
    for (int i = 0; i < size; i++)
    {
        if (indegree[signals[i]] == 0)
        {
            Q.push(signals[i]);
        }
    }
    while(!Q.empty())
    {
        int curr_size = Q.size();
        vector<Signal *> L;
        for(int i = 0; i < curr_size; i++)
        {
            Signal* s = Q.front();
            Q.pop();
            L.push_back(s);
            for(auto &u : s->children)
            {
                indegree[u]--;
                if(indegree[u] == 0) Q.push(u);
            }

        }
        layers.push_back(L);
    }

    int num_layers = layers.size();
    for(int i =0; i < layers[0].size(); i++)
    {
        layers[0][i]->delay = 0;
    }
    for(int i = 1; i < num_layers; i++)
    {
        for(auto &u : layers[i])
        {
    
            if(u->parent1 && u->parent2)
            {
               
                u->delay = u->ingate.cur_delay + max(u->parent1->delay,u->parent2->delay);
            }
            else if(u->parent1&&!u->parent2)
            {
                u->delay = u->ingate.cur_delay + u->parent1->delay;
            }
        }
    }


    float mx_delay = 0;
    // for(auto &u : layers[num_layers-1])
    // {
    //     mx_delay = max(mx_delay,u->delay);
    // }

    for ( auto &u : signals)
    {
        if(u->children.size()==0)
        {
            mx_delay = max(mx_delay,u->delay);
        }
    }
   

    return mx_delay;

}


float perturb_circuit(Circuit& circ, Signal* sig, int i)
{
    sig->ingate.perturb(i);
    return circ.max_delay();
}

vector<vector<int>> permutations(int n)
{
    if(n == 1)
    {
        vector<vector<int>> v = {{0},{1},{2}};
        return v;
    }
    else
    {
        vector<vector<int>> u = permutations(n-1);
        vector<vector<int>> v ;
        for(int i = 0; i < u.size(); i++)
        {
            for(int j = 0; j < 3; j++)
            {
                vector<int> a = u[i];
                a.push_back(j);
                v.push_back(a);
            }
        }
        return v;
    }
}

float brute_force(Circuit& circ, float delay_constraint)
{
    int sz = 0;
    for(auto& s : circ.signals)
    {
        if(s->parent1 || s->parent2)
        {
            sz++;
        }
    }
    // std::cout<<sz<<endl;
    vector<vector<int>> combinations = permutations(sz);
    float curr_area = circ.total_area();
    for(auto comb : combinations)
    {
        int count = 0;
        int i = 0;
        while(count < comb.size() && i < circ.size)
        {
            if(circ.signals[i]->parent1 || circ.signals[i]->parent2)
            {
                circ.signals[i]->ingate.perturb(comb[count]);
                i++; count++;
            }
            else i++;
        }
        // for(auto i : comb)
        // {
        //     sig_with_gate[i]->ingate.perturb(i);
        // }

        float curr_delay = circ.max_delay();
        // std::cout<<curr_delay<<" "<<circ.total_area()<<endl;
      
        if(curr_delay <= delay_constraint)
        {
            
            curr_area = min(curr_area,circ.total_area());

            
        }
       
    }

    
    return curr_area;
}



//  bool compared(const pair<int,Gate> &p1,const  pair<int,Gate> &p2) 
// {
//     std::cout<<"hello";
//     if(p1.first==p2.first)
//         return true;
//     return p1.first > p2.first;
// }




void compute_count(Signal* sig,vector<Signal *> &marked)
{

    if(!sig)
    {
        return;
    }
    if(!sig->parent1 && !sig->parent2)
    {

        return;
    }
    else if(sig->parent1 && !sig->parent2)
    {
        if(sig->marked_yet ==0 ){
        sig->mark++;
        marked.push_back(sig);}
        return compute_count(sig->parent1, marked);
    }
    else{
         if(sig->marked_yet ==0 ){
        sig->mark++;
        marked.push_back(sig);}
        if(sig->parent1->delay > sig->parent2->delay)
        {
            return compute_count(sig->parent1,marked);
        }
        return compute_count(sig->parent2,marked);
    }

}

void mark_rst(Circuit &circ)

{
    for(int i = 0 ; i< circ.signals.size(); i ++ )
    {
        circ.signals[i]->mark = 0 ;
      
    }

}
void mark_reset(Circuit &circ)

{
    for(int i = 0 ; i< circ.signals.size(); i ++ )
    {
        circ.signals[i]->marked_yet = 0 ;
      
    }

}

float method3(Circuit& circ, float delay_constraint,vector<Signal *> &primary_outputs,int pert)
{
    
    
    mark_rst(circ);

    float cur_delay = circ.max_delay();
  
    vector<Signal *> flagged_sig; 
    for(int i= 0 ; i < primary_outputs.size(); i++)

    {
        
        if(primary_outputs[i]->delay > delay_constraint)
        {
          
            flagged_sig.push_back(primary_outputs[i]);
        }
    }

    if(flagged_sig.size()==0){
        
      
        return circ.total_area();}
    vector<Signal*> marked; 
    for(int i =0 ;i< flagged_sig.size(); i++)
    {
        
        compute_count(flagged_sig[i],marked);
    }
    if(marked.size() == 0)
    {
           
        mark_reset(circ);
        return method3(circ,delay_constraint,primary_outputs,0);
    }
    sort(marked.begin(),marked.end());
    
    marked[0]->ingate.perturb(pert);
    marked[0]->marked_yet = 1;

    return method3(circ,delay_constraint,primary_outputs,pert);

    
    

}

void merge(vector<Signal *> &arr, int left, int mid, int right)
{
    int n1 = mid - left + 1;
    int n2 = right - mid;

    vector<Signal *> leftArr(n1);
    vector<Signal *> rightArr(n2);

    for (int i = 0; i < n1; i++)
    {
        leftArr[i] = arr[left + i];
    }
    for (int i = 0; i < n2; i++)
    {
        rightArr[i] = arr[mid + 1 + i];
    }

    int i = 0, j = 0, k = left;

    while (i < n1 && j < n2)
    {
        if (leftArr[i]->children.size() <= rightArr[j]->children.size())
        {
            arr[k] = leftArr[i];
            i++;
        }
        else
        {
            arr[k] = rightArr[j];
            j++;
        }
        k++;
    }

    while (i < n1)
    {
        arr[k] = leftArr[i];
        i++;
        k++;
    }

    while (j < n2)
    {
        arr[k] = rightArr[j];
        j++;
        k++;
    }
}

void mergeSort(vector<Signal *> &arr, int left, int right)
{
    if (left < right)
    {
        int mid = left + (right - left) / 2;

        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);

        merge(arr, left, mid, right);
    }
}

float method2(Circuit &circ, float delay_constraint)
{
    vector<Signal *> sorted_signals;
    for (auto sig : circ.signals)
    {
        if (sig->parent1 || sig->parent2)
        {
            sig->ingate.perturb(0);
            sorted_signals.push_back(sig);
        }
    }
    int sz = sorted_signals.size();
    float curr_delay = circ.max_delay();
    float curr_area = circ.total_area();
    mergeSort(sorted_signals, 0, sz - 1);
    for (int i = sz - 1; i >= 0; i--)
    {

        float temp_delay = circ.max_delay();
        float temp_area = circ.total_area();
        // cout << "delay " << temp_delay << " area " << temp_area << endl;
        if (temp_delay > delay_constraint)
            break;
        curr_delay = temp_delay;
        curr_area = temp_area;
        sorted_signals[i]->ingate.perturb(1);
    }
    for (int i = sz - 1; i >= 0; i--)
    {
        float temp_delay = circ.max_delay();
        float temp_area = circ.total_area();
 
        if (temp_delay > delay_constraint)
            break;
        curr_delay = temp_delay;
        curr_area = temp_area;
        sorted_signals[i]->ingate.perturb(2);
    }

    return curr_area;
}



int main()
{   
    int number_of_primary_inp = 0;

    float delay_contraint;
    ifstream maxdel("delay_constraint.txt");
    string line2; 
    while(getline(maxdel,line2))
    {
        if(line2.empty() || all_of(line2.begin(), line2.end(), [](char c) {return isspace(c);})) continue;
        vector<string> tokenized_line;
            std::istringstream iss1(line2);
            string token;
            bool comment = false;
        
            while(iss1 >> token)
            {

               
                
                if(token == "//")
                {
                    comment = true;
                
                    break;

                }
                
                tokenized_line.push_back(token);
                
            }
            if(!comment)
            {
                delay_contraint = stof(tokenized_line[0]);
            }

        }


    map <string, Gate> gates;
    ifstream delayconst("gate_delays.txt");
    string line1;
    while(getline(delayconst,line1))
    {
            if(line1.empty() || all_of(line1.begin(), line1.end(), [](char c) {return isspace(c);})) continue;
            vector<string> tokenized_line;
            std::istringstream iss1(line1);
            string token;
            bool comment = false;
        
            while(iss1 >> token)
            {

               
                
                if(token == "//")
                {
                    comment = true;
                
                    break;

                }
                
                tokenized_line.push_back(token);
                
            }
          
            if(!comment)
            {
                
                
                if(gates.find(tokenized_line[1]) == gates.end())
                {
                    Gate temp;
                    
                    temp.id = tokenized_line[1];
                    temp.area.push_back(stof(tokenized_line[3]));
                    temp.delay.push_back(stof(tokenized_line[2]));
                    gates[tokenized_line[1]] = temp;


                }
                else
                {
                    gates[tokenized_line[1]].area.push_back(stof(tokenized_line[3]));
                    gates[tokenized_line[1]].delay.push_back(stof(tokenized_line[2]));
                }
            }
           

    }

     for(auto &element:gates)
            {
                
                sort(element.second.area.rbegin(),element.second.area.rend());
                sort(element.second.delay.begin(),element.second.delay.end());
                element.second.perturb(0);
            }
  

   
    map <string,Signal *> m;
    ifstream circuit("circuit.txt");
    string line;
    vector<string> Primary_Inputs;
    vector<string> Primary_Outputs;
    vector<string> Internal_Signals;
    while(getline(circuit,line))
    {
            if(line.empty() || all_of(line.begin(), line.end(), [](char c) {return isspace(c);})) continue;
            vector<string> tokenized_line;
            std::istringstream iss(line);
            string token;
            bool comment = false;
            while(iss >> token)
            {
                if(token == "//")
                {
                    comment = true;
                
                    break;

                }
                tokenized_line.push_back(token);
                
            }
            if(!comment)

            {
                
                if(tokenized_line[0] == "PRIMARY_INPUTS")
                {
                    Primary_Inputs = tokenized_line;
                    // Primary_Inputs.erase(Primary_Inputs.begin());

                }
                else if(tokenized_line[0] == "PRIMARY_OUTPUTS")
                {
                    Primary_Outputs = tokenized_line;
                    // Primary_Outputs.erase(Primary_Outputs.begin());
                   
                }
                else if(tokenized_line[0] == "INTERNAL_SIGNALS")
                {
                    Internal_Signals = tokenized_line;
                    // Internal_Signals.erase(Primary_Inputs.begin());
                }

                else if(tokenized_line[0] == "INV")

                {
                    
                    string s1 = tokenized_line[1]; string s2 = tokenized_line[2];
                    if(m.find(s1) == m.end())
                    {
                        Signal* temp1 = new Signal(s1);
                        m[s1] = temp1;
                    }
                    if(m.find(s2) == m.end())
                    {
                        Signal* temp2 = new Signal(s2);
                        m[s2] = temp2;
                    }
                    m[s2]->parent1 = m[s1];
                    m[s1]->children.push_back(m[s2]);
                    m[s2]->ingate = gates["INV"];
                    

                    
                }

                else if(tokenized_line[0] == "AND2" || tokenized_line[0] == "NAND2" || tokenized_line[0] == "NOR2" || tokenized_line[0] == "OR2")

                {
                    string s1 = tokenized_line[1]; string s2 = tokenized_line[2]; string s3 = tokenized_line[3];
                     if(m.find(s1) == m.end())
                    {
                        Signal* temp1 = new Signal(s1);
                        m[s1] = temp1;
                    }
                    if(m.find(s2) == m.end())
                    {
                        Signal* temp2 = new Signal(s2);
                        m[s2] = temp2;
                    }
                    if(m.find(s3) == m.end())
                    {
                        Signal* temp3 = new Signal(s3);
                        m[s3] = temp3;
                    }
                    m[s3]->parent1 = m[s1];
                    m[s3]->parent2 = m[s2];
                    
                    m[s3]->ingate = gates[tokenized_line[0]];

                    m[s1]->children.push_back(m[s3]);
                    m[s2]->children.push_back(m[s3]);



                }

                else if( tokenized_line[0] == "DFF")
                {
                    string s1 = tokenized_line[1]; string s2 = tokenized_line[2];

                    Primary_Outputs.push_back(s1);
                    Primary_Inputs.push_back(s2);
                
                    if(m.find(s1) == m.end())
                    {
                        
                        
                        Signal* temp1 = new Signal(s1);
                        m[s1] = temp1;
                    }
                    if(m.find(s2) == m.end())
                    {
                      
                        Signal* temp2 = new Signal(s2);
                        m[s2] = temp2;
                    }
                }

            }
    }
    
   
    vector<Signal *> vector_of_sig;
    for(auto g:m)
    {
        
        vector_of_sig.push_back(g.second);

    }
    
    Circuit temp_circuit ;
    temp_circuit.signals = vector_of_sig;
    temp_circuit.size = vector_of_sig.size();
    vector<Signal * > pouts; 
    for( int jj = 0 ; jj < vector_of_sig.size(); jj ++ )
    {
        if(vector_of_sig[jj]->children.size()==0)
        {
            pouts.push_back(vector_of_sig[jj]);
        }
       
    }

    for(int jj = 0 ; jj < vector_of_sig.size() ; jj ++ )
    {
        if(vector_of_sig[jj]->parent1 == NULL && vector_of_sig[jj]->parent2 == NULL )
        {
            number_of_primary_inp ++;
        }

    }
    int ifwebrute = vector_of_sig.size() - number_of_primary_inp;
    float maxdelayed = temp_circuit.max_delay();
    float longdel = temp_circuit.max_delay();
    float brutans;
   
    if(ifwebrute <= 10)

     brutans = brute_force(temp_circuit,delay_contraint);
    else
        brutans = 99999;
    



    float ans = method2(temp_circuit,delay_contraint);

    std::cout<<" max delay is "<<maxdelayed<<endl;
    std::cout<<"brute force ans is "<<brutans<<endl;
    std::cout<<ans<<" by method1"<<endl;
    
    
    std::ofstream longestdel("longest_delay.txt");
    if(longestdel.is_open())
    {
        longestdel<<longdel;
        longestdel.close();
    }
    else
    {
        cout<<"longest delay file could not be made"<<endl;
    }

    std::ofstream minarea("minimum_area.txt");
    if(minarea.is_open())
    {
        minarea<<ans;
        minarea.close();
    }
    else
    {
        cout<<" area  file could not be made"<<endl;
    }
   
   
   
    
    for(int pp =0 ;pp < temp_circuit.signals.size(); pp++)
    {
         
        if(temp_circuit.signals[pp]->parent1 || temp_circuit.signals[pp]->parent2)
        {
            temp_circuit.signals[pp]->ingate.perturb(2);
        }
    }
 
    float metho2 = method3(temp_circuit,delay_contraint,pouts,1);
std::cout<<metho2<<" by method2"<<endl;
     ans = std::min(ans,metho2);
     ans = std::min(ans,brutans);

        std::cout<<ans<<"  best of all in limited running time"<<endl;

    
    
    
    std::ofstream longestde("longest_delay.txt");
    if(longestde.is_open())
    {
        longestde<<longdel;
        longestde.close();
    }
    else
    {
        cout<<"longest delay file could not be made"<<endl;
    }

    std::ofstream minare("minimum_area.txt");
    if(minare.is_open())
    {
        minare<<ans;
        minare.close();
    }
    else
    {
        cout<<" area  file could not be made"<<endl;
    }
   
   


    return 0;
}
