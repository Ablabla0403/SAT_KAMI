/****************************************************************************
  FileName     [ sat.h ]
  PackageName  [ sat ]
  Synopsis     [ Define miniSat solver interface functions ]
  Author       [ Chung-Yang (Ric) Huang, Cheng-Yin Wu ]
  Copyright    [ Copyleft(c) 2010-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef SAT_H
#define SAT_H

#include <cassert>
#include <vector>
#include <iostream>
#include "Solver.h"

using namespace std;

/********** MiniSAT_Solver **********/
class SatSolver
{
   public : 
      SatSolver():_solver(0) { }
      ~SatSolver() { if (_solver) delete _solver; }

      // Solver initialization and reset
      void initialize() {
         reset();
         if (_curVar == 0) { _solver->newVar(); ++_curVar; }
      }
      void reset() {
         if (_solver) delete _solver;
         _solver = new Solver();
         _assump.clear(); _curVar = 0;
      }

      // Constructing proof model
      // Return the Var ID of the new Var
      inline Var newVar() { _solver->newVar(); return _curVar++; }
      // fa/fb = true if it is inverted
      void addAigCNF(Var vf, Var va, bool fa, Var vb, bool fb) {
         vec<Lit> lits;
         Lit lf = Lit(vf);
         Lit la = fa? ~Lit(va): Lit(va);
         Lit lb = fb? ~Lit(vb): Lit(vb);
         lits.push(la); lits.push(~lf);
         _solver->addClause(lits); lits.clear();
         lits.push(lb); lits.push(~lf);
         _solver->addClause(lits); lits.clear();
         lits.push(~la); lits.push(~lb); lits.push(lf);
         _solver->addClause(lits); lits.clear();
      }
      // fa/fb = true if it is inverted
      void addXorCNF(Var vf, Var va, bool fa, Var vb, bool fb) {
         vec<Lit> lits;
         Lit lf = Lit(vf);
         Lit la = fa? ~Lit(va): Lit(va);
         Lit lb = fb? ~Lit(vb): Lit(vb);
         lits.push(~la); lits.push( lb); lits.push( lf);
         _solver->addClause(lits); lits.clear();
         lits.push( la); lits.push(~lb); lits.push( lf);
         _solver->addClause(lits); lits.clear();
         lits.push( la); lits.push( lb); lits.push(~lf);
         _solver->addClause(lits); lits.clear();
         lits.push(~la); lits.push(~lb); lits.push(~lf);
         _solver->addClause(lits); lits.clear();
      }

      void addCNF(vector<Var> vv, vector<bool> vb) {
         vec<Lit> lits;
         Lit tmp;
         for(size_t i=0; i<vv.size(); ++i) {
            tmp = vb[i]? ~Lit(vv[i]): Lit(vv[i]);
            lits.push(tmp);
         }
         _solver->addClause(lits);
         lits.clear();
      }

      void addOnelitCNF(Var vv, bool vb) {
         vec<Lit> lits;
         Lit tmp;
         tmp = vb? ~Lit(vv): Lit(vv);
         lits.push(tmp);
         _solver->addClause(lits);
         lits.clear();
      } 

      void step2_1CNF(vector<Var>& vars_2, vector<Var>& vars_1, Var k) {
         vec<Lit> lits, lit_2, lit_1;
         Lit lk = Lit(k);
         for (size_t i=0; i<vars_2.size(); ++i) {
            lit_2.push(Lit(vars_2[i]));
            lit_1.push(Lit(vars_1[i]));
         }
         for (size_t i=0; i<vars_2.size(); ++i) {
            lits.push(~lit_2[i]); lits.push(lit_1[i]); lits.push(lk);
            _solver->addClause(lits); lits.clear();
            lits.push(lit_2[i]); lits.push(~lit_1[i]); lits.push(lk);
            _solver->addClause(lits); lits.clear();
         }
      }
      void step2_2CNF(vector<Var>& vars_2, vector<Var>& vars_1, Var k) {
         vec<Lit> lits, lit_2, lit_1;
         Lit lk = Lit(k);
         for (size_t i=0; i<vars_2.size(); ++i) {
            lit_2.push(Lit(vars_2[i]));
            lit_1.push(Lit(vars_1[i]));
         }
         for (size_t i=0; i<vars_2.size(); ++i) {
            lits.push(~lit_2[i]); lits.push(lit_1[i]); lits.push(~lk);
            _solver->addClause(lits); lits.clear();
            lits.push(lit_2[i]); lits.push(~lit_1[i]); lits.push(~lk);
            _solver->addClause(lits); lits.clear();
         }
      }

      void step3_1CNF(vector<Var>& vars_k) {
         vec<Lit> lits, lit_k;
         for (size_t i=0; i<vars_k.size(); ++i) {
            lit_k.push(Lit(vars_k[i]));
         }
         for (size_t i=0; i<vars_k.size(); ++i) {
            for (size_t j=0; j<vars_k.size(); ++j) {
               if(i < j) {
                  lits.push(~lit_k[i]); lits.push(~lit_k[j]);
                  _solver->addClause(lits); lits.clear();
               }
            }
         }
         for (size_t i=0; i<vars_k.size(); ++i) {
            lits.push(lit_k[i]);
         }
         _solver->addClause(lits); lits.clear();
      } 

      void step4_1CNF(vector<Var>& vars_k, vector<Var>& _vars_k) {
         vec<Lit> lits, lit_k, _lit_k;
         for (size_t i=0; i<vars_k.size(); ++i) {
            lit_k.push(Lit(vars_k[i]));
            _lit_k.push(Lit(_vars_k[i]));
         }
         for (size_t i=0; i<vars_k.size(); ++i) {
            lits.push(~lit_k[i]); lits.push(_lit_k[i]);
            _solver->addClause(lits); lits.clear();
         }
      }

      void addORCNF(Lit l1, Lit l2, Lit l3) {
         vec<Lit> lits;
         lits.push(~l1); lits.push(l3); 
         _solver->addClause(lits); lits.clear();
         lits.push(~l2); lits.push(l3);
         _solver->addClause(lits); lits.clear();
         lits.push(~l3); lits.push(l1); lits.push(l2);
         _solver->addClause(lits); lits.clear();

      }

      void addANDCNF(Lit l1, vec<Lit>& ls) {
         vec<Lit> lits;
         for (size_t i=0; i<ls.size(); ++i) {
            lits.push(~l1); lits.push(ls[i]);
            _solver->addClause(lits); lits.clear();
         }
         for (size_t i=0; i<ls.size(); ++i) {
            lits.push(~ls[i]);
         }
         lits.push(l1);
         _solver->addClause(lits); lits.clear();
      }

      void add2ANDCNF(Lit l1, Lit l2, Lit l3) {
         vec<Lit> lits;
         lits.push(~l1); lits.push(~l2); lits.push(l3);
         _solver->addClause(lits); lits.clear();
         lits.push(~l3); lits.push(l1);
         _solver->addClause(lits); lits.clear();
         lits.push(~l3); lits.push(l2);
         _solver->addClause(lits); lits.clear();
      }

      void step4_2CNF(vector<Var>& C1, vector<Var>& C2, Var k1, Var k2, vector<Var>& inter) {
         vec<Lit> lits, lc1, lc2, linter;
         Lit lk1 = Lit(k1);
         Lit lk2 = Lit(k2);
         for (size_t i=0; i<C1.size(); ++i) {
            lc1.push(Lit(C1[i]));
            lc2.push(Lit(C2[i]));
         }
         for (size_t i=0; i<inter.size(); ++i) {
            linter.push(Lit(inter[i]));
         }
         vec<Lit> tmp_vec;
         for (size_t i=0; i<C1.size(); ++i) {
            addORCNF(~lc1[i], lc2[i], linter[i*2]);
            addORCNF(lc1[i], ~lc2[i], linter[i*2+1]);
            tmp_vec.push(linter[i*2]);
            tmp_vec.push(linter[i*2+1]);
         }
         addANDCNF(linter[C1.size()*2], tmp_vec); // e <-> abcd
         add2ANDCNF(lk1, lk2, linter[C1.size()*2+1]); // k1k2 <-> f
         add2ANDCNF(~lk1, ~lk2, linter[C1.size()*2+2]); // k1'k2' <-> g
         addORCNF(linter[C1.size()*2+1], linter[C1.size()*2+2], linter[C1.size()*2+3]); // f+g <-> h

         lits.push(~linter[C1.size()*2]); lits.push(linter[C1.size()*2+3]); //e'+h
         _solver->addClause(lits); lits.clear();
      }

      void step4_3CNF(vector<Var>& Cn, vector<Var>& Cn1, Var var_k) {
         vec<Lit> lits;
         for (size_t i=0; i<Cn.size(); ++i) {
            lits.push(~Lit(Cn1[i])); lits.push(Lit(Cn[i])); lits.push(~Lit(var_k));
            _solver->addClause(lits); lits.clear();
            lits.push(Lit(Cn1[i])); lits.push(~Lit(Cn[i])); lits.push(~Lit(var_k));
            _solver->addClause(lits); lits.clear();
         } 
      }

      void step4_4CNF(vector<Var>& _kn, int node, vector<int> adj, vector<Var>& kn) {
         vec<Lit> lits;
         lits.push(~Lit(_kn[node]));
         lits.push(Lit(kn[node]));
         for (size_t i=0; i<adj.size(); ++i) {
            lits.push(Lit(kn[adj[i]]));
         }
         _solver->addClause(lits); lits.clear();
      }

      void step4_5CNF(vector<Var>& C_init, vector<Var>& C_new, vector<Var>& inter, vector<Var> Cn2, Var var_k) {
         vec<Lit> lits, linit, lnew, linter, tmp_vec, lc2;
         int color = C_init.size();
         Lit lk = Lit(var_k);
         for (size_t i=0; i<color; i++) {
            linit.push(Lit(C_init[i]));
            lnew.push(Lit(C_new[i]));
            lc2.push(Lit(Cn2[i]));
         }
         for (size_t i=0; i<inter.size(); ++i) {
            linter.push(Lit(inter[i]));
         }
         for (size_t i=0; i<color; ++i) {
            addORCNF(~lc2[i], lnew[i], linter[i*2]);
            addORCNF(lc2[i], ~lnew[i], linter[i*2+1]);
            tmp_vec.push(linter[i*2]);
            tmp_vec.push(linter[i*2+1]);
         }
         addANDCNF(linter[color*2], tmp_vec); // e <-> abcd
         add2ANDCNF(lk, linter[color*2], linter[color*2+1]); // Ke <-> f
         tmp_vec.clear();

         for (size_t i=0; i<C_init.size(); ++i) {
            addORCNF(~linit[i], lnew[i], linter[color*2+2+i*2]);
            addORCNF(linit[i], ~lnew[i], linter[color*2+2+i*2+1]);
            tmp_vec.push(linter[color*2+2+i*2]);
            tmp_vec.push(linter[color*2+2+i*2+1]);
         }
         addANDCNF(linter[color*4+2], tmp_vec); // k <-> ghij
         add2ANDCNF(~lk, linter[color*4+2], linter[color*4+3]); // K'k <-> l

         lits.push(linter[color*2+1]); lits.push(linter[color*4+3]);
         _solver->addClause(lits); lits.clear();
      }

      void step4_6CNF(vector<Var>& C, vector<string>& invalid) {
         vec<Lit> lits;
         for(size_t i=0; i<invalid.size(); ++i) {
            for(size_t j=0; j<C.size(); ++j) {
               if(invalid[i][j] == '1') lits.push(~Lit(C[j]));
               else lits.push(Lit(C[j]));
            } 
            _solver->addClause(lits); lits.clear();
         }
      }

      void step5CNF(vector<Var>& c, Var v1, Var v2) {
         vec<Lit> lits, lit1, lit2;
         Lit l1 = Lit(v1);
         Lit l2 = Lit(v2);
         for (size_t i=0; i<c.size(); ++i) {
            lit1.push(Lit(c[i]));
            lit2.push(~Lit(c[i]));
         }
         addANDCNF(l1, lit1);
         addANDCNF(l2, lit2);
         lits.push(l1); lits.push(l2);
         _solver->addClause(lits); lits.clear();
      }


      // For incremental proof, use "assumeSolve()"
      void assumeRelease() { _assump.clear(); }
      void assumeProperty(Var prop, bool val) {
         _assump.push(val? Lit(prop): ~Lit(prop));
      }
      bool assumpSolve() { return _solver->solve(_assump); }

      // For one time proof, use "solve"
      void assertProperty(Var prop, bool val) {
         _solver->addUnit(val? Lit(prop): ~Lit(prop));
      }
      bool solve() { _solver->solve(); return _solver->okay(); }

      // Functions about Reporting
      // Return 1/0/-1; -1 means unknown value
      int getValue(Var v) const {
         return (_solver->modelValue(v)==l_True?1:
                (_solver->modelValue(v)==l_False?0:-1)); }
      void printStats() const { const_cast<Solver*>(_solver)->printStats(); }

   private : 
      Solver           *_solver;    // Pointer to a Minisat solver
      Var               _curVar;    // Variable currently
      vec<Lit>          _assump;    // Assumption List for assumption solve
};

#endif  // SAT_H

