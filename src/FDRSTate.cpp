#include "FDRSTate.h"
#include <string>
#include <map>

using namespace std;

FDRSTate::FDRSTate( vector <Action> &a, vector <Rigid> &r, vector <Fact>s, vector <Fact> problemFacts):rigids(r),actions(a)
{
//    rigids=r;
  //  actions=a;
    vars = s;
    this->problemFacts = std::move(problemFacts);

}

int FDRSTate::heuristic(State *goal) {
    /*
     * Heuristic function implemented is hmax.
     */
    FDRSTate *localGoal = (FDRSTate *) goal;

    map<string, int> hmaxTable;
    for(auto i=this->problemFacts.begin(); i<this->problemFacts.end(); i++){
        hmaxTable[i->toString()] =
                find(this->vars.begin(), this->vars.end(), *i) != this->vars.end() ? 0 : INT_MAX;
    }

    vector <Fact> U = this->vars;
    for(auto i=this->actions.begin(); i<this->actions.end(); i++){

    }

    return 0;
}

vector<GroundedAction> FDRSTate::findForks()
{
    vector<vector<Parameter>> solutions;
    vector<GroundedAction> forks;

    for (int i =0;i<actions.size();i++)
    {
        solutions = actions[i].isApplicable(rigids,vars);
        for (int k=0;k<solutions.size();k++)
            forks.push_back(GroundedAction(&actions[i],solutions[k]));
    }
    return forks;
}

vector<FDRSTate *> FDRSTate::expand()
{
    vector<FDRSTate *> children;
    vector<GroundedAction> gacts;

    gacts = findForks();

    for (int i=0;i<gacts.size();i++)
        children.push_back(getSuccessorState(gacts[i]));
    return children;
}

FDRSTate *FDRSTate::getSuccessorState(GroundedAction a)
{
    FDRSTate *r = new FDRSTate(*this);

    r->addAction(a.toString());

  //  cout<<r.toString()<<endl;

    for (int i=0;i<a.getEffectsCount();i++)
    {
        Fact f = a.getEffect(i);



      //  cout<<"Searcing for "<<f.toString()<<endl;
        for (auto it = r->vars.begin();it< r->vars.end();it++)
        {
         //   cout<<(*it).toString();
            if ( *(*it).getVariable() == *f.getVariable())
            {
              //  cout<<" YES"<<endl;
                (*it).setValue(f.getValue());
            }
//            else
  //              cout<<" NO"<<endl;

        }
    }
    return r;
}
string FDRSTate::toString()const
{
    string s="";
    if (vars.size()>0)
    {
        s="[";
        for (int i=0;i<vars.size()-1;i++)
            s+=vars[i].toString()+",";
        s+=vars[vars.size()-1].toString()+"]";
    }
    return s;
}

bool operator == (const FDRSTate a, const FDRSTate b)
{
    for (int i=0;i<b.vars.size();i++)
    {
        bool found =false;
        for (int j = 0; !found && j<a.vars.size();j++)
        {
            if (b.vars[i]==a.vars[j])
                found =true;
        }
        if (!found)
        {
           // cout<<"not found"<<endl;
            return false;
        }
    }
    return true;

//    return a==b;
}
unsigned long long FDRSTate::getKey() const
{
    unsigned long long k=0;

    for (int i=0;i<vars.size();i++)
        k+= vars[i].getNumber() * (vars[i].getDomainSize(),i);

    //cout<<k<<endl;
    return k;
}
/*
bool operator >= (pair<const FDRSTate s, bool b> s43,const FDRSTate s2)
{

}
*/
