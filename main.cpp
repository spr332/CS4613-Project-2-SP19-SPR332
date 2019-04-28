#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;
int casttonum[25] = {0,1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,131072,262144,524288,1048576,2097152,4194304};
int *sections[18][9]{{ 0, 1, 2, 3, 4, 5, 6, 7, 8},{ 9,10,11,12,13,14,15,16,17},
                   {18,19,20,21,22,23,24,25,26},{27,28,29,30,31,32,33,34,35},
                   {36,37,38,39,40,41,42,43,44},{45,46,47,48,49,50,51,52,53},
                   {54,55,56,57,58,59,60,61,62},{63,64,65,66,67,68,69,70,71},
                   {72,73,74,75,76,77,78,79,80},{ 0, 1, 2, 9,10,11,18,19,20},
                   { 3, 4, 5,12,13,14,21,22,23},{ 6, 7, 8,15,16,17,24,25,26},
                   {27,28,29,36,37,38,45,46,47},{30,31,32,39,40,41,48,49,50},
                   {33,34,35,42,43,44,51,52,53},{54,55,56,63,64,65,72,73,74},
                   {57,58,59,66,67,68,75,78,79},{60,61,62,69,70,71,78,79,80}};

class num{
    int self; 
    // A bitwise representation of a number. 
    //bits [31-19] Unused
    //bits [9-17] Domain Set 1-9
    //bits [0-8] Number 1-9
    public:
    num(){
        self = 0;
    }
    num(int neue){
        neue %=25;
        self = casttonum[neue];
    }
    operator int(){ //Implicit casting to int
        return this->getNum();
    }
    operator bool(){ //Implicit casting to bool
        return (self<=512 || self > 0); //returns if value is final (not a domain value)
    }
    int getNum() const {  
    //Returns a numerical representation of num
    //If there are only domain values returns 0
        if (self == 0)return 0;
        int k=0; //unused? - carroll
        for(int i =1;i<9;i++){
            if (self&(1<<i))
                return i;
        }
        return 0;
    }
    int getInt() const {return self;} //Returns the unmodified "self"
    num& operator+= (const num& rhs){//Adds a number to the Domain set  bits [9-17]
        int neue = rhs.getInt();
        neue = neue << 9; //Shift left logical by 9
        self = self | neue; //bitwise or
        return *this;
    }
    num& operator+= (const int& rhs){//Adds a number to the Domain set  bits [9-17]
        int neue = casttonum[rhs];
        neue = neue << 9; //Shift left logical by 9
        self = self | neue; //bitwise or
        return *this;
    }
    int const getDomain(){
        int neue = self;
        neue = neue << 14;//clear high bits
        neue = neue >> 23;//clear low bits
        return neue;
    }
    
};
inline bool operator==(const num& lhs, const num& rhs){return lhs.getInt() == rhs.getInt();}
inline bool operator!=(const num& lhs, const num& rhs){return !operator==(lhs,rhs);}
inline bool operator< (const num& lhs, const num& rhs){return lhs.getInt() < rhs.getInt();}
inline bool operator> (const num& lhs, const num& rhs){return  operator< (rhs,lhs);}
inline bool operator<=(const num& lhs, const num& rhs){return !operator> (lhs,rhs);}
inline bool operator>=(const num& lhs, const num& rhs){return !operator< (lhs,rhs);}
ostream& operator<<(ostream& os, const num& n){
    os<<n.getNum();
    return os;
}
class decoder{
    int domain;
    public:
    decoder(int in){domain=in;}
    int next(){
        if (domain==0)
            return 0;
        for (int i = 1; i<9; i++){
            if (*domain & (1<<i)){ //if i bit exists
                *domain = *domain ^ (1<<i);//bitwise remove a bit using xor
                return i; //return that i existed
            }
        }
        return 0;
    }
};  //Syntactical sugar: for(int i = nameofdecoder(9); i!=0; i = nameofdecoder.next())


//Above this line is the code setting up the num type.

struct state{
    num* layout;
    state(){
        layout = new (num[81]) ;
    }
    
    
};


int main(){
    
    
}