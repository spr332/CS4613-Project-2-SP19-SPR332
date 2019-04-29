//This is a syntactical sugar thing
//to make code more ledgible
#include "sugar.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

decoder::decoder(int in){domain=in;}
int decoder::first(){
    return this->next();
}
int decoder::next(){
    if (domain==0)
        return 0;
    for (int i = 0; i<9; i++){
        if (domain & (1<<i)){ //if i'th bit exists (starts at 1 goes to 256 aka 9)
            domain = domain ^ (1<<i);//bitwise remove a bit using xor
            return i; //return that i existed
        }
    }
    return 0;
}


//Code for an int linked list stolen from my project 1 code.
//We're generating the neighbors every time we access. Might make more optimal later.

    HashLL::HashLL(){
        first = new HSHLL();
        first->mine = NULL;
        first->next = NULL;
    }
    void HashLL::push(int neue){
        HSHLL* guy = new HSHLL;
        guy->mine = neue;
        guy->next = first;
        first = guy;
        return;
    }
    bool HashLL::exists(){
        return first!= NULL;
    }
    
    int HashLL::pop(){
        HSHLL* guy = first;
        int ret = first->mine;
        first = first->next;
        delete guy;
        return ret;
    }



allNeighbors::allNeighbors(int in){
    //from in generate all neighbors push into HashLL
    neighbors = HashLL();
    //Verticals
    for(int i=0;i<9;i++)
        neighbors.push(  (in%9)+(i*9)  );
    
    //Horizontals
    int ndexs[]={80,71,62,53,44,35,26,17, 8};
    int k;
    for(k=0;k<9;k++)
        if(in<=ndexs[8-k])
            goto there;
    std::cerr<<"ALLNEIGHBORS GOT *cough* *cough* messed up!!!"<<in<<"  "<<k<<std::endl;
    there:;;
    for(int i=0;i<9;i++)
        neighbors.push(  ndexs[8-k] - i   );
    
    // 3x3 excludes duplicates.
    int b,n,m,f,j;
    k = neighbors.pop();
    neighbors.push(k);
    for(b=0;b<9;b++)
        if ((9*b)==k)
            break;
    
    if(b%3==0){
        f=9;j=18;
    }
    else if(b%3==1){
        f=9;j=-9;
    }
    else if(b%3==2){
        f=-9;j=-18;
    }
    
    if(in%3==0){
        n=in+1;m=in+2;
    }
    else if(in%3==1){
        n=in+1;m=in-1;
    }
    else if(in%3==2){
        n=in-1;m=in-2;
    }
    
    neighbors.push(n+f);
    neighbors.push(m+f);
    neighbors.push(n+j);
    neighbors.push(m+j);
    
}

int allNeighbors::first(){
    return this->next();
}
int allNeighbors::next(){
    if (neighbors.exists())
        return neighbors.pop();
    return -1;
}