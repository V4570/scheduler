#ifndef FDRSTATE_H
#define FDRSTATE_H
#include <utility>
#include<vector>
#include "Rigid.h"
#include "Fact.h"
#include "GroundedAction.h"
#include "State.h"

using namespace std;

class FDRSTate: public State
{
    public:
        FDRSTate( vector <Action> &,  vector <Rigid> &,vector <Fact>, vector <Fact>);
        vector<GroundedAction> findForks();
        vector<FDRSTate *> expand ();
        FDRSTate *getSuccessorState(GroundedAction);
        vector<Fact> getVars() {return vars;};
        void setVars(vector<Fact> newVars) {vars=std::move(newVars);}

        int heuristic(FDRSTate *goal);
        friend bool operator == (const FDRSTate, const FDRSTate);
/*        bool operator == ( FDRSTate o)
        {
            for (int i=0;i<o.vars.size();i++)
            {
                bool found =false;
                for (int j = 0; !found && j<vars.size();j++)
                {
                    if (o.vars[i]==vars[j])
                        found =true;
                }
                if (!found)
                {
                   // cout<<"not found"<<endl;
                    return false;
                }
            }
            return true;
        }
*/
        void addAction(string s) {path.push_back(s);}
        string toString() const;
        void printPath ()
        {
            for (int i=0;i<path.size();i++)
                cout<<i+1<<") "<<path[i]<<endl;
        }
        unsigned long long getKey() const;
    protected:

    private:
        vector <string> path;
        vector <Rigid> &rigids;
        vector <Fact> vars;
        vector <Action> &actions;
        vector <Fact> problemFacts;
};
bool operator == (const FDRSTate, const FDRSTate);
namespace std {
  template <>
  class hash<FDRSTate>
  {
      public:
        size_t operator()(const FDRSTate &a) const
        {
          //  return 1;
            return a.getKey();
        }
  };
}
#endif // FDRSTATE_H
