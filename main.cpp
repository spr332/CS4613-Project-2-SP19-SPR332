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
    unsigned int self; 
    // A bitwise representation of a number. 
    //bits [31-19] Unused
    //bits [9-17] Domain Set 1-9
    //bits [0-8] Number 1-9
    public:
    num(){
        self = 0b111111111000000000; //init with domain 123456789
    }
    num(int neue){
        self = ((neue==0)?0b111111111000000000:(1 << neue-1));
    }
    num(int old, bool spec){
        self = old;
    }
    operator int()const{ //Implicit casting to int
        return this->getNum();
    }
    
    int getNum() const {  
    //Returns a numerical representation of num
    //If domain only or failure returns 0;
        if (self == 0)return 0;
        if (self > 256)return 0;
        if (self<=256){
            for(int i =0;i<9;i++){
                if (  (self & (1<<i) ) != 0 )
                    return i+1;
            }
        }
        return -90210;
    }
    void setNum(int neue){
        self = ((neue==0)?0b111111111000000000:(1 << neue-1));
    }
    unsigned int getInt() const {return self;} //Returns the unmodified "self"
    num& becomeDomain(){ // shift Domain to Final
        self = self >> 9;
        return *this;
    }
    bool isComplete()const{
        return ( (self<512) && (self > 0) );
    }
    num& operator-= (const num& rhs){//Removes a number to the Domain set  bits [9-17]
        int neue = rhs.getInt();
        neue = neue << 9; //Shift left logical by 9
        self = self ^ neue; //bitwise xor
        return *this;
    }
    int countRemainingDomain()const{
        if (this->isComplete())
            return 0;
        int count=0;
        decode(self>>9){
            count++;
        }
        return count;
    }
    void removeFromDomain(int the){
        self-=(1<<(the+9-1));
    }
    int nextPossibleDomain(){
        decode(self>>9){
            this->removeFromDomain(i);//remove the first and 
            return i;//dont continue loop.
        }
        return -1;
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

struct state{
    num* layout;
    state(){
        layout = new num[81] ;
    }
    state(state* old){
        layout = new num[81];
        for(int i=0;i<81;i++){
            layout[i] = num(old->layout[i].getInt(), true);
        }
    }
    
    void doForwardChecking(int changedValueLocation){
        neighborSet(changedValueLocation){
            if (!(layout[i].isComplete() )){
                layout[i].removeFromDomain( layout[changedValueLocation].getNum() );
            }
        }
    }
    void globalForwardChecking(){
        for(int l=0;l<81;l++){
            if (layout[l].isComplete()){
                this->doForwardChecking(l);
            }
        }
    }
    bool checkValidityOfDomainItem(int loc, int domItem)const{
        neighborSet(loc){
            if(layout[i].getNum() == domItem && i!=loc){
                return false;
            }
        }
        return true;
    }
    num* selectUnassignedVariable()const{
        return this->minimumRemainingValueWithDegreeHeuristic();
    }
    
    int getDegreeAtLoc(int loc)const{
        int count=0;
        neighborSet(loc){ //a loop where i is loc of each neighbor
            if (!(layout[i].isComplete())){
                count++;
            }
        }
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
                        //
            }
        }
        return a;
    }
    bool goalTest()const{
        for(int l=0;l<81;l++){
            if (!(layout[l].isComplete()))//if l'th item not complete
                return false;
            neighborSet(l)
                if ( layout[l].getNum() == layout[i].getNum() )
                    return false;
        }
        return true;
    }
    void removeFromMemory(){
        delete layout;
        return;
    }


    state* backTrack();
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
state* state::backTrack(){
    if (this->goalTest())
        return this;
    
    this->globalForwardChecking();
    state* result;
    state* duplicate;
    int location;
    doItAgain:
    num* var = this->selectUnassignedVariable();
    location = this->getLocationOf(var);
    //cout<<endl<<*this<<location<<"A "<<var->countRemainingDomain()<<endl;
    if( var->countRemainingDomain() == 1){
        var->becomeDomain();
        goto doItAgain;
    }
    if( var->countRemainingDomain() == 0 ){
        cout<<*this;
        cout<<var->getNum()<<" "<<location<<" "<<var->getInt()<<" "<< var->countRemainingDomain()<<endl;
        return NULL;
    }
    cout<<location;
    for(int domainItem = var->nextPossibleDomain(); domainItem != -1; domainItem = var->nextPossibleDomain() ){
        cout<<" A"<<domainItem;
        if ( this->checkValidityOfDomainItem(location, domainItem) ){ //If domainItem consistent with assignment
            duplicate = new state(this);
            duplicate->layout[location].setNum(domainItem);
            cout<<"LOC:"<<location<<" DOM:"<<domainItem<<" Val@Loc@dupe:"<<duplicate->layout[location].getNum()<<endl<<*duplicate<<endl;
            duplicate->doForwardChecking(location);
            cout<<"calling backTrack()\n";
            result = duplicate->backTrack();
            if (result != NULL){
                return result;
            }
            duplicate->removeFromMemory();//remove from assignment
        }
    }//nextPossibleDomain will remove the domain item from var
    //cout<<"FAILURE\n";
    return NULL; //NULL is failure.
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


void unitTests(){
    int failures = 0;
    num* J;
    num* L = new num();
    cout<<"Starting tests\n";
    for(int i=1;i<10;++i){
        cout<<"Testing with final: "<<i<<"\n";
        J = new num(i);
        cout<<"\tgetNum: "<<J->getNum()<<" should be: "<<i<<endl;
        if(J->getNum() != i) failures++;
        cout<<"\tgetInt: "<<J->getInt()<<" should be: "<<(1<<i-1)<<endl;
        if(J->getInt() != (1<<i-1)) failures++;
        cout<<"\tisComplete: "<<J->isComplete()<<" should be: "<< true <<endl;
        if(J->isComplete() != true) failures++;
        cout<<"\tcountRemainingDomain: "<<J->countRemainingDomain()<<" should be: "<<0<<endl;
        if(J->countRemainingDomain() != 0 ) failures++;
        cout<<"\tnextPossibleDomain: "<<J->nextPossibleDomain()<<" should be: "<<-1<<endl;
        if(J->nextPossibleDomain() != -1) failures++;
        cout<<"END\n\n";
        delete J;
    }
    cout<<"Testing with Domainset: FULL\n";
    cout<<"\tgetNum: "<<L->getNum()<<" should be: "<<0<<endl;
    if(L->getNum() != 0) failures++;
    cout<<"\tgetInt: "<<L->getInt()<<" should be: "<<(0b111111111<<9)<<endl;
    if(L->getInt() != (0b111111111<<9)) failures++;
    cout<<"\tisComplete: "<<L->isComplete()<<" should be: "<< false <<endl;
    if(L->isComplete() != false)failures++;
    cout<<endl;
    cout<<"\tcountRemainingDomain: "<<L->countRemainingDomain()<<" should be: "<<9<<endl;
    if(L->countRemainingDomain() != 9)failures++;
    cout<<"\tnextPossibleDomain: "<<L->nextPossibleDomain()<<" should be: "<<1<<endl;
    
    cout<<"\tnextPossibleDomain: "<<L->nextPossibleDomain()<<" should be: "<<2<<endl;
    *L -= num(3);
    cout<<"\tINFO removed 3 from domain with -=\n";
    cout<<endl;
    cout<<"\tnextPossibleDomain: "<<L->nextPossibleDomain()<<" should be: "<<4<<endl;
    cout<<"\tcountRemainingDomain: "<<L->countRemainingDomain()<<" should be: "<<(9-4)<<endl;
    cout<<endl;
    cout<<"\tnextPossibleDomain: "<<L->nextPossibleDomain()<<" should be: "<<5<<endl;
    cout<<"\tnextPossibleDomain: "<<L->nextPossibleDomain()<<" should be: "<<6<<endl;
    cout<<endl;
    cout<<"\tnextPossibleDomain: "<<L->nextPossibleDomain()<<" should be: "<<7<<endl;
    cout<<"\tnextPossibleDomain: "<<L->nextPossibleDomain()<<" should be: "<<8<<endl;
    cout<<"\tcountRemainingDomain: "<<L->countRemainingDomain()<<" should be: "<<1<<endl;
    failures+=(L->countRemainingDomain()-1);
    L->becomeDomain();
    cout<<"\tINFO did L->becomeDomain()\n";
    cout<<endl;
    cout<<"\tgetNum: "<<L->getNum()<<" should be: "<<9<<endl;
    if(L->getNum() != 9)failures++;
    cout<<"\tgetInt: "<<L->getInt()<<" should be: "<<(1<<8)<<endl;
    if(L->getInt() != (1<<8))failures++;
    cout<<endl;
    cout<<"\tisComplete: "<<L->isComplete()<<" should be: "<< true <<endl;
    if(!L->isComplete())failures++;
    cout<<"\tcountRemainingDomain: "<<L->countRemainingDomain()<<" should be: "<<0<<endl;
    if(L->countRemainingDomain()!=0)failures++;
    cout<<"\tnextPossibleDomain: "<<L->nextPossibleDomain()<<" should be: "<<-1<<endl;
    if(L->nextPossibleDomain()!=-1)failures++;
    cout<<"END of domainSet FULL tests\n\n";
    
    state theState;
    FileHandler fh("SUDUKO_Input1.txt", &theState);
    theState.globalForwardChecking();
    unsigned int test;
    cout<<"testing domain traversal and forward checking. (pt1)\n";
    for(int l=0;l<81;l++){
        test = theState.layout[l].getInt();
        if(theState.layout[l].isComplete()){
            decode(test>>9){
                cout<<"Failure: \n\tdomain found: "<<i<<"\n";
                failures++;
            }
        }
        else{
            test = theState.layout[l].countRemainingDomain();
            neighborSet(l){
                if(theState.layout[i].isComplete()){
                    theState.layout[l].removeFromDomain( theState.layout[i].getNum() );
                }
            }
            if(theState.layout[l].countRemainingDomain() != test){
                cout<<"Domain not updated properly at:"<<l<<"\n";
                failures++;
            }
        }
    }
    
    cout<<theState.layout[79].getInt()<<"\n\n";
    cout<<theState<<endl;
    
    cout<<"END\nTOTAL NUMBER OF FAILURES: "<<failures<<"\n";
}

int main(){
    unitTests();
    // state theState;
    // state* ans;
    // FileHandler fh("SUDUKO_Input1.txt", &theState);
    // theState.globalForwardChecking();
    // ans = theState.backTrack();
    // cout<<"\nEND\n";
    // cout<< *ans << endl;
}
