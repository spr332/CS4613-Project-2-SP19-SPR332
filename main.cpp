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
        self = 0b111111111000000000; //init with domain 123456789
    }
    num(int neue){
        self = ((neue==0)?0b111111111000000000:(1 << neue));
        cout<<self<<" ";
    }
    operator int()const{ //Implicit casting to int
        return this->getNum();
    }
    num& setTo(int neue){
        self = ((neue==0)?self:(1 << neue-1));
        return *this;
    }
    int getNum() const {  
    //Returns a numerical representation of num
    //If there are only domain values returns 0
        if (self == 0)return 0;
        if (self<=512 || self > 0){
            for(int i =0;i<9;i++){
                if (self&(1<<i))
                    return i;
            }
        }
        return 0;
    }
    int getInt() const {return self;} //Returns the unmodified "self"
    num& becomeDomain(){ // shift Domain to Final
        self = self >> 9;
        return *this;
    }
    bool isComplete()const{
        return (self<=512 && self > 0);
    }
    // -------------------------------------------------------------------------------------------ADD THE ++
    num& operator-= (const num& rhs){//Removes a number to the Domain set  bits [9-17]
        int neue = rhs.getInt();
        neue = neue << 9; //Shift left logical by 9
        self = self ^ neue; //bitwise xor
        return *this;
    }
    int countRemainingDomain()const{
        int count=0;
        decode(self>>9){
            count++;
        }
        return count;
    }
    int nextPossibleDomain(){
        decode(self<<9){
            *this-=num(i);//get the first and 
            return i;//dont continue loop.
        }
        return -1;
    }
    bool checkValidityOfDomainItem(int t)const{ //ffffffic this as it probably should check neighborset;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        return true;
    }
    
};

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

int* memcpy (int *dest, const int *src, size_t len){
  int *d = (int*)dest;
  const int *s = (int*)src;
  while (len--)
    *d++ = *s++;
  return dest;
}
struct state{
    num* layout;
    state(){
        layout = new (num[81]) ;
    }
    state(state* old){
        layout = new (num[81]);
        memcpy(layout, old->layout, 81);
    }
    
    void doForwardChecking(int changedValueLocation){
        //After a variable is assigned a value, update the remaining legal values of its neighbors.
        num* theNum = &layout[changedValueLocation];
        neighborSet(changedValueLocation){ //This is a loop. i will iteratively be the neighbors of changedValueLocation.
            if (! (layout[i].isComplete() )) //if not complete, update domain
                layout[i]-= *theNum;
        }
    }
    void globalForwardChecking(){ //Update the domain values of everything possible
        num a;
        for(int l=0;l<81;l++){
            if (layout[l].isComplete())
                this->doForwardChecking(l);
        }
         
    }
    num* selectUnassignedVariable()const{
        return this->minimumRemainingValueWithDegreeHeuristic();
    }
    
    int getDegreeAtLoc(int loc)const{
        int count=0;
        neighborSet(loc) //a loop where i is loc of each neighbor
            if (!(layout[i].isComplete()))
                count++;
        return count;
    }
    int getLocationOf(num* teh)const{
        for(int row=72;row>=0;row-=9){
            for(int col=0;col<9;col++){
                if( &layout[row+col] == teh)
                    return (row+col);
            }
        }
        return -1;
    }
    
    num* minimumRemainingValueWithDegreeHeuristic()const{
        num* a = layout;
        for(int l=0;l<81;l++){
            if (!(layout[l].isComplete())){
                if (layout[l].countRemainingDomain() < a->countRemainingDomain()) //If mrv[l] < mrv(a) ; a = l
                    a = &layout[l];
                else if (layout[l].countRemainingDomain() == a->countRemainingDomain()) //if equal
                    if (this->getDegreeAtLoc(l)  > this->getDegreeAtLoc(  this->getLocationOf(a)  ) )     // and degree l > degree a
                        a = &layout[l];                                    //  then a = l
            }
        }
        return a;
    }
    bool goalTest()const{
        for(int i=0;i<81;i++)
            if (!(layout[i].isComplete()))//if i'th item not complete
                return false;
        return true;
    }
    void removeFromMemory(){
        delete layout;
        return;
    }


    num* backTrack();
};
/*
state struct info:
This is the problem definition

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
num* state::backTrack(){
    this->globalForwardChecking(); //Update all domains with forward checking.
    if (this->goalTest()){
        cout<<"HERE215"<<endl;
        return layout;
    }
    num* var;
    state* neue = NULL;
    num* neueReturn;
    doItAgain:
    var = this->selectUnassignedVariable();
    if (var->countRemainingDomain() == 1){
        var->becomeDomain();
        cout<<"$ "<<var<<endl;
        goto doItAgain;
    }
    cout<<"VAR: "<< var->getInt() <<endl;
    //
    if (var->countRemainingDomain() == 0){
        cout<<"HERE229"<<endl;
        return NULL;
    }
    for (int i = var->nextPossibleDomain();i != -1; i = var->nextPossibleDomain() ){
        if (var->checkValidityOfDomainItem(i)){
            //duplicate layout
            *neue = state(this);
            //set duplicated var as i
            cout<<"DUPLEIC::\n"<<*neue<<"\n";
            neue->layout[this->getLocationOf(var)].setTo(i);
            neueReturn = neue->backTrack(); //Try with that value.
            if (neueReturn != NULL){ //if we found soln
                cout<<"HERE240"<<endl;
                return neueReturn;
            }
            else{ //and if not
                neue->removeFromMemory();
                neue=NULL;
            }
        }
        //nextPossibleDomain() will remove i from the domain so else statement not necessary.
    }
    cout<<"HERE250"<<endl;
    return NULL;
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
                    a->layout[row+col] = num(convertChar(temp[col*2]));
                                        //The number at place in file
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
    cout<<endl<<theState<<endl<<"Starting solver\n";
    cout<<  *theState.backTrack()  <<endl;
    cout<<"END\n";
}