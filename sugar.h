#ifndef tyuityuityuityuityuifghjfghjfghjvbnvbnmvnbm
#define tyuityuityuityuityuifghjfghjfghjvbnvbnmvnbm


#define decode(a) decoder decodername(a);for(int i=decodername.first();i!=-1;i=decodername.next())
#define neighborSet(a) allNeighbors decodername(a);for(int i=decodername.first();i!=-1;i=decodername.next())



class decoder{
    int domain;
    public:
    decoder(int in);
    int first();
    int next();
};




struct HSHLL{
    int mine;
    HSHLL* next;
};
class HashLL{
    HSHLL* first;
    public:
    HashLL();
    void push(int neue);
    bool exists();
    int pop();
};


class allNeighbors{
    HashLL neighbors;
    public:
    allNeighbors(int in);
    int first();
    int next();
};

#endif