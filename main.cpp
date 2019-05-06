#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "sugar.h"
using namespace std;


// Everything above this line is global constants and includes
/* 
Sugar.h info:

Adds the decode(int) loop
essentially decodes a num
into ints accessable by the
name i, an int.
*/

class num{
    int self; 
    // A bitwise representation of a number. 
    //bits [31-19] Unused
    //bits [9-17] Domain Set 1-9
    //bits [0-8] Number 1-9
    public:
    num(){
        self = 0b1111111100000000;
    }
    num(int neue){
        self = ((neue==0)?self:(1 << neue-1));
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
        for(int i =0;i<9;i++){
            if (self&(1<<i))
                return i;
        }
        return 0;
    }
    int getInt() const {return self;} //Returns the unmodified "self"
    num& DtoF(){ // shift Domain to Final
        self = self >> 9;
        return *this;
    }
    num& FtoD(){ // shift Final to Domain
        self = self << 9;
        return *this;
    }
    num& selectOnlyFinal(){
        self = self & 0b11111111;//
        return *this;
    }
    num& onesComplement(){
        self = ~self;
        return *this;
    }
    

    num& addToDomain(const num& rhs){//Adds the domain of a number to the Domain set  bits [9-17]
        int neue = rhs.getInt();
        neue = neue << 9;
        self = self | neue; //bitwise or
        return *this;
        }
    bool checkComplete(){
        int count=0;
        decode(self>>9)
            count++;
        if (count==1){
            self = self>>9;
            return true;
        }
        return false;
    }
    num& operator+= (const num& rhs){//Adds the domain of a number to the Domain set  bits [9-17]
        int neue = rhs.getInt();
        neue = neue << 9;
        self = self | neue; //bitwise or
        return *this;
    }
    num& operator+= (const int& rhs){//Adds a number to the Domain set  bits [9-17]
        int neue =  ((rhs==0)?0:(1 << rhs-1));
        neue = neue << 9; //Shift left logical by 9
        self = self | neue; //bitwise or
        return *this;
    }
    num& operator-= (const num& rhs){//Removes a number to the Domain set  bits [9-17]
        int neue = rhs.getInt();
        neue = neue << 9; //Shift left logical by 9
        self = self ^ neue; //bitwise xor
        return *this;
    }
    num& operator-= (const int& rhs){//Removes a number to the Domain set  bits [9-17]
        int neue =  ((rhs==0)?0:(1 << rhs-1));
        neue = neue << 9; //Shift left logical by 9
        self = self ^ neue; //bitwise xor
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
/*
class num info:

a num is a representation of a
single spot in the 9x9 grid
the low 18 bits represent the Domain and Final 
Unused:[31-18]  Domain:[17-9]  Final Setting:[8-0]
bits in the domain and final work like this:
987654321 and 9876543231 respectively
so if a domain looks like 010000100 
then the domain is 8 and 3.

 
*/


struct state{
    num* layout;
    state(){
        layout = new (num[81]) ;
    }
    void doForwardChecking(int changedValueLocation){
        //After a variable is assigned a value, update the remaining legal values of its neighbors.
        num* theNum = &layout[changedValueLocation];
        neighborSet(changedValueLocation) //This is a loop. i will iteratively be the neighbors of changedValueLocation.
            layout[i]-= *theNum;
    }
    void globalForwardChecking(){
        num a;
        for(int l=0;l<81;l++){
            a = layout[l];
            neighborSet(l)//This is a loop. i will iteratively be the neighbors of l.
                a+=layout[i];//add layout[i] to domain
            if (layout[l].addToDomain(   a.DtoF().onesComplement().selectOnlyFinal()   ).checkComplete()  )// update the domains.
                doForwardChecking(layout[l]);
        }
         
    }
};
/*
state struct info:
state is a struct for easier access to the layout.

*/



ostream& operator<<(ostream&os , const state& st){
    for(int row=72;row>=0;row-=9){
        for(int col=0;col<9;col++){
            os<<st.layout[row+col]<<" ";
        }
        os<<"\n";
    }
    return os;
}
int convertChar(char a){ 
    if (a=='0'){
        return 0;
    }
    else if (a=='1'){
        return 1;
    }
    else if (a=='2'){
        return 2;
    }
    else if (a=='3'){
        return 3;
    }
    else if (a=='4'){
        return 4;
    }
    else if (a=='5'){
        return 5;
    }
    else if (a=='6'){
        return 6;
    }
    else if (a=='7'){
        return 7;
    }
    else if (a=='8'){
        return 8;
    }
    else if (a=='9'){
        return 9;
    }
    return -1;
}
class FileHandler{ //A file handler for problems
    string filename;
    state * st;
    public:
    FileHandler(string fileName, state* a){
        st = a;
        filename = fileName;
        ifstream file;
        string temp="";
        file.open(fileName);
        if (file.is_open()){
            for(int row=72;row>=0;row-=9){
                getline(file,temp);
                for(int col=0;col<9;col++){
                    a->layout[row+col] = num( //The number at place in file
                                  convertChar(
                                  temp[col*2]  ));
                }
            }
            file.close();
        }
        else{
            cerr << "FILE "<< fileName <<" NOT FOUND"<<endl<<"Exiting..."<<endl;
        }
    }
    void out(){
        return;
    }

};

int main(){
    state theState;
    FileHandler fh("inputa.txt", &theState);
    cout<<endl<<theState<<endl;
    
}