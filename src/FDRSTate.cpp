#include "FDRSTate.h"
#include <string>
#include <unordered_map>

using namespace std;

FDRSTate::FDRSTate( vector <Action> &a, vector <Rigid> &r, vector <Fact>s, vector <Fact> problemFacts):rigids(r),actions(a)
{
//    rigids=r;
  //  actions=a;
    vars = s;
    this->problemFacts = std::move(problemFacts);
}

int FDRSTate::heuristic(FDRSTate *goal) {
    /*
     * Heuristic function implemented is hmax.
     */

    unordered_map<string, int> hmaxTable;
    // initialize hmaxTable with 0 if fact is in current state and MAX_INT if not
    for(auto i=this->problemFacts.begin(); i<this->problemFacts.end(); i++){
        hmaxTable[i->toString()] =
                find(this->vars.begin(), this->vars.end(), *i) != this->vars.end() ? 0 : INT_MAX;
    }

    auto *U = new FDRSTate(*this);

    while (true){
        // main hmax loop that stops when there is no update in both the U and the hmaxTable
        vector<GroundedAction> forks;

        // find applicable actions for the U state
        forks = U->findForks();

        // boolean variables to check when we get an update
        bool uUpdated = false;
        bool hMaxUpdated = false;

        for (auto f=forks.begin(); f<forks.end(); f++){
            // for each applicable action
            int d_max_local = 0;
            Action *a;
            a = f->getAction();

            // find the hmax of the action's preconditions
            // calculated once here since it is the same for all effects
            for(auto pr=0; pr<a->getPrecsCount(); pr++){
                Precondition *p = a->getPrec(pr);
                Fact pf;
                if(p->isValueObject()){
                    auto *val = new Object(p->getValue().getDescription());
                    pf.setValue(val);
                }
                else{
                    int pp = a->getParameterPosition(p->getValueP()->getName());
                    auto *val = new Object(f->getParam()[pp].getValue().getDescription());
                    pf.setValue(val);
                }

                auto *var = new Variable(p->getName());

                vector<Parameter *> &pv = p->getParameterVector();

                for(auto & j : pv){
                    int paramPos = a->getParameterPosition(j->getName());
                    var->add(Object(f->getParam()[paramPos].getValue().getDescription()));
                }
                pf.setVariable(var);

                int h_max_old = hmaxTable[pf.toString()];
                if (d_max_local < h_max_old) d_max_local = h_max_old;
            }

            // for each effect of the action store it in U if it is not already there
            // and then calculate the new hmax and update the hmaxTable if it is smaller
            // than what we already have
            vector <Fact> oldVars = U->getVars();
            vector <Fact> newVars = oldVars;
            for(auto e=0; e<f->getEffectsCount(); e++){
                Fact effFact = f->getEffect(e);

                if( !(find(oldVars.begin(), oldVars.end(), effFact) != oldVars.end()) ){
                    newVars.push_back(effFact);
                }

                int oldHMax = hmaxTable[effFact.toString()];
                int newHMax = min(oldHMax, 1 + d_max_local);
                if (newHMax != oldHMax){
                    hMaxUpdated = true;
                }
                hmaxTable[effFact.toString()] = newHMax;

            }
            if(newVars.size() > oldVars.size()){
                U->setVars(newVars);
                uUpdated = true;
            }
        }

        // end if there were no updates
        if(!uUpdated && !hMaxUpdated) break;
    }

    vector <Fact> goalVars = goal->getVars();
    int heuristic = 0;
    for(Fact f : goalVars){
        int f_hmax = hmaxTable[f.toString()];
        if (heuristic < f_hmax) heuristic = f_hmax;
    }

    return heuristic;
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

    for (int i=0;i<gacts.size();i++){
        FDRSTate *child = getSuccessorState(gacts[i]);
        child->setDepth(getDepth()+1);
        children.push_back(child);
    }
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
