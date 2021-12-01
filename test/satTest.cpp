#include <iostream>
#include <vector>
#include <cmath>
#include <stdlib.h>
#include <algorithm>
#include "sat.h"
#include <time.h>

using namespace std;

class Gate
{
public:
   Gate(unsigned i = 0): _gid(i) {}
   ~Gate() {}

   Var getVar() const { return _var; }
   void setVar(const Var& v) { _var = v; }

private:
   unsigned   _gid;  // for debugging purpose...
   Var        _var;
};

// 
//[0] PI  1 (a)
//[1] PI  2 (b)
//[2] AIG 4 1 2
//[3] PI  3 (c)
//[4] AIG 5 1 3
//[5] AIG 6 !4 !5
//[6] PO  9 !6
//[7] AIG 7 !2 !3
//[8] AIG 8 !7 1
//[9] PO  10 8
//
vector<Gate *> gates;

void
initCircuit()
{
   // Init gates
   for(int i=0; i<15; ++i) {
      gates.push_back(new Gate(i));
   }

   // POs are not needed in this demo example
}

void
genProofModel(SatSolver& s)
{
   // Allocate and record variables; No Var ID for POs
   // for (size_t i = 0, n = gates.size(); i < n; ++i) {
   //    Var v = s.newVar();
   //    gates[i]->setVar(v);
   // }

   // // Hard code the model construction here...
   // // [2] AIG 4 1 2 ==> [2] = [0] & [1]
   // s.addAigCNF(gates[2]->getVar(), gates[0]->getVar(), false,
   //             gates[1]->getVar(), false);
   // // [4] AIG 5 1 3 ==> [4] = [0] & [3]
   // s.addAigCNF(gates[4]->getVar(), gates[0]->getVar(), false,
   //             gates[3]->getVar(), false);
   // // [5] AIG 6 !4 !5 ==> [5] = ![2] & ![4]
   // s.addAigCNF(gates[5]->getVar(), gates[2]->getVar(), true,
   //             gates[4]->getVar(), true);
   // // [7] AIG 7 !2 !3 ==> [7] = ![1] & ![3]
   // s.addAigCNF(gates[7]->getVar(), gates[1]->getVar(), true,
   //             gates[3]->getVar(), true);
   // // [8] AIG 8 !7 1 ==> [8] = ![7] & [0]
   // s.addAigCNF(gates[8]->getVar(), gates[7]->getVar(), true,
   //             gates[0]->getVar(), false);
}

void reportResult(const SatSolver& solver, bool result, int num, int color, int steps)
{
   solver.printStats();
   cout << (result? "SAT" : "UNSAT") << endl;
   if (result) {
      // for (size_t i=0; i<steps+1; ++i) {
      //    cout << "STEP" << i << " : \n";
      //    for (size_t j=0; j<num; ++j) {
      //       cout << "C" << j << " : ";
      //       for (size_t k=0; k<color; ++k) {
      //          cout << solver.getValue(gates[i*num*color + j*color + k]->getVar());
      //       }
      //       cout << "\n";
      //    }
      //    cout << "\n";
      // }

      for (size_t i=0; i<steps; ++i) {
         for (size_t j=0; j<num; ++j) {
            if(solver.getValue(gates[(steps+1)*num*color + steps*color + i*num + j]->getVar()) == 1) cout << "C" << j;
         }
         cout << "(";
         for (size_t j=0; j<color; ++j) {
            cout << solver.getValue(gates[(steps+1)*num*color + i*color + j]->getVar());
         }
         cout << ")";
         if (i < steps-1) cout << " -> "; 
      }
      cout << "\n";

      // for(size_t i=0; i<4; ++i) {
      //    cout << "step" << i+1 << "\n";
      //    for (size_t j=0; j<num; ++j) {
      //       cout << solver.getValue(gates[i*(num*color +2*color+num) + num*color +2*color + j]->getVar()) << " ";
      //    }
      //    cout << "\n";
      //    for (size_t j=0; j<num; ++j) {
      //       cout << solver.getValue(gates[112+i*num+j]->getVar()) << " ";
      //    }
      //    cout << "\n";
      // }
   }
}

struct vertex {
   int idx;
   string color;
   vector<int> adj;
};


void addEdge(vector<int> vec[], int u, int v) {
   vec[u].push_back(v);
}

class kami_graph {
   public:
      kami_graph(int n1, int n2, int n3, int n4, vector<string> c, vector<vector<int>> a, vector<vector<int>> e) {
         num = n1;
         num_color = n2;
         num_e = n3;
         num_steps = n4;
         colors = c;
         adjnodes = a;
         edges = e;
      }
      int num;
      int num_color;
      int num_e;
      int num_steps;

      vector<string> colors;
      vector<vector<int>> adjnodes;
      vector<vector<int>> edges;
   private:
      
};


void random_node (vector<vector<int>> &nodes, int target, vector<int>& ans, vector<string> &valid, string& color) {
   for (size_t i=0; i<ans.size(); ++i) {
      if (target == ans[i]) {
         return ;
      }
   }
   ans.push_back(target);
   int ran;
   for (size_t i=0; i<nodes[target].size(); ++i) {
      ran = 3* ((double) rand() / (RAND_MAX + 1) + 1);
      if (ran % 3 == 0 && valid[nodes[target][i]] != color) {
         random_node(nodes, nodes[target][i], ans, valid, color);
      }
   }
   return ;
}

kami_graph gen_testcase (int num = -1, int num_color = -1, int num_e = -1) {
   srand(time(NULL));
   if (num == -1) {
      num = (20 - 1)* ((double) rand() / (RAND_MAX + 1) + 1) + 1;
   }
   if (num_color == -1) {
      num_color = (6 - 3)* ((double) rand() / (RAND_MAX + 1) + 1) + 3;
   }
   if (num_e == -1) {
      num_e == (2*num -3)* ((double) rand() / (RAND_MAX + 1) + 1) + 3;
   }
   int ran, ran1, ran2;
   int max_steps = 10;
   vector<int>::iterator it;
   vector<vector<int>> adjnodes = {};
   for (size_t i=0; i<num; i++) {
      vector<int> tmp;
      adjnodes.push_back(tmp);
   }
   for (size_t i=0; i<num_e; i++) {
      ran1 = (num + 1)* ((double) rand() / (RAND_MAX + 1) + 1);
      ran2 = (num + 1)* ((double) rand() / (RAND_MAX + 1) + 1);
      if (ran1 < num && ran2 < num) {
         it = find(adjnodes[ran1].begin(), adjnodes[ran1].end(), ran2);
         if (ran1 != ran2 && it == adjnodes[ran1].end()) {
            adjnodes[ran1].push_back(ran2);
            adjnodes[ran2].push_back(ran1);
            num_e ++;
         }
      }
   }
   cout << "edges: " << num_e << "\n";

   for (size_t i=0; i<num; i++) {
      sort(adjnodes[i].begin(), adjnodes[i].end());
   }

   vector<vector<int>> edges = {};
   for (size_t i=0; i<num; i++) {
      vector<int> tmp;
      edges.push_back(tmp);
   }
   
   for (size_t i=0; i<adjnodes.size(); i++) {
      for (size_t j=0; j<adjnodes[i].size(); j++) {
         if(adjnodes[i][j] > i) {
            edges[i].push_back(adjnodes[i][j]);
         }
      } 
   }

   vector<string> colors = {};
   vector<string> valid_colors = {};
   int color = log2(num_color);
   if (log2(num_color) > color) color++;
   int valid_color, valid_count;
   string valid_str;
   for (int i=0; i<num_color; ++i) {
      valid_color = i;
      valid_count = color;
      while (valid_color >= 0 && valid_count > 0) {
         if(valid_color >= pow(2, valid_count-1)) {
            valid_str += "1";
            valid_color -= pow(2, valid_count-1);
            valid_count--;
         } else {
            valid_str += "0";
            valid_count--;
         } 
      }
      valid_colors.push_back(valid_str);
      valid_str = "";
   }
   for (size_t i=0; i<num; i++) {
      colors.push_back(valid_colors[0]);
   }
   vector<int> record;
   ran = (max_steps - 5)* ((double) rand() / (RAND_MAX + 1) + 1) + 5;
   int num_steps = ran;
   for (size_t i=0; i<ran; ++i) {
      ran1 = (num-1)* ((double) rand() / (RAND_MAX + 1) + 1);
      ran2 = (num_color-1)* ((double) rand() / (RAND_MAX + 1) + 1);
      if(colors[ran1] == valid_colors[ran2]) {
        ran2 = (ran2 == num_color-1) ? ran2-1 : ran2+1; 
      }
      random_node(adjnodes, ran1, record, valid_colors, valid_colors[ran2]);
      for (size_t i=0; i<record.size(); i++) {
         colors[ran1] = valid_colors[ran2];
      }
      record.clear();
   }

   kami_graph kami(num, num_color, num_e, num_steps, colors, adjnodes, edges);
   return kami;
   
}

int main()
{
   // initCircuit();

   SatSolver solver;
   solver.initialize();
   int a, b, c;
   cout << "Type num of nodes | num of color | num of edges\n";
   cin >> a >> b >> c;
   kami_graph kami = gen_testcase(a, b, c);
   

   int num = kami.num, num_color = kami.num_color, num_e = kami.num_e, steps = kami.num_steps-1;
   vector<string> colors = kami.colors;
   vector<vector<int>> adjnodes = kami.adjnodes;
   vector<vector<int>> edges = kami.edges;
   vector<vertex> graph;

   // vector<vector<int>> edges = {};
   // for (size_t i=0; i<34; i++) {
   //    vector<int> tmp;
   //    edges.push_back(tmp);
   // }
   // int count = 0, count2 = 0;
   // for (size_t i=0; i<adjnodes.size(); i++) {
   //    for (size_t j=0; j<adjnodes[i].size(); j++) {
   //       count ++;
   //       if(adjnodes[i][j] > i) {
   //          edges[i].push_back(adjnodes[i][j]);
   //       }
   //    } 
   // }


   vector<string> invalid_colors;
   for (int i=0; i<num; ++i) {
      vector<int> new_vec;
      vertex new_item {i+1, colors[i], new_vec};
      graph.push_back(new_item);
   }
   for (int i=0; i<edges.size(); ++i) {
      for (int j=0; j<edges[i].size(); ++j) {
         graph[i].adj.push_back(edges[i][j]);
      }
   }
   int now_step = 0;
   int color = log2(num_color);
   if(log2(num_color) > color) color++;
   int invalid_color, invalid_count;
   string invalid_str;
   for (int i=num_color; i<pow(2,color); ++i) {
      invalid_color = i;
      invalid_count = color;
      while (invalid_color >= 0 && invalid_count > 0) {
         if(invalid_color >= pow(2, invalid_count-1)) {
            invalid_str += "1";
            invalid_color -= pow(2, invalid_count-1);
            invalid_count--;
         } else {
            invalid_str += "0";
            invalid_count--;
         } 
      }
      invalid_colors.push_back(invalid_str);
      invalid_str = "";
   }
  
   vector<Gate *> gates_1[steps+1];
   vector<Gate *> gates_2[steps];
   vector<Gate *> gates_4_1[steps];
   vector<Gate *> gates_4_2[steps];
   vector<Gate *> gates_4_5[steps];


   // STEP 1 give input color   
   // num of clauses : num*color
   // num of variables : num*color

   
   for(size_t i=0; i<num*color; ++i) {
      gates_1[now_step].push_back(new Gate(i));
      Var v = solver.newVar();
      gates_1[now_step][i]->setVar(v);
   }

   vector<Var> vv_1;
   vector<bool> vb_1;
   for (size_t i=0; i<num; ++i) {
      for (size_t j=0; j<color; ++j) {
         if (colors[i][j] == '0') {
            vb_1.push_back(true);
         } else {
            vb_1.push_back(false);
         }
      }
   }
   for (size_t i=0; i<num*color; ++i) {
      solver.addOnelitCNF(gates_1[now_step][i]->getVar(), vb_1[i]);
   }
   

   while (now_step < steps) {

      // STEP 2 only one change  Cn1 : clicked color   Cn2 : changed color 
      // num of clauses : num*color*2*3 = 6*num*color
      // num of variables : new colors + Cn1 + Cn2 + k1~kn = num*color + 2*color + num

      for (size_t i=0; i<num*color+color*2+num; ++i) {
         gates_2[now_step].push_back(new Gate(i));
         Var v = solver.newVar();
         gates_2[now_step][i]->setVar(v);
      }

      vector<Var> vars_2, vars_1, vars_Cn1, vars_Cn2;
      for (size_t i=0; i<color; ++i) {
         vars_Cn1.push_back(gates_2[now_step][num*color+i]->getVar());
         vars_Cn2.push_back(gates_2[now_step][num*color+color+i]->getVar());
      }
      for (size_t i=0; i<num; ++i) {
         for (size_t j=0; j<color; ++j) {
            vars_2.push_back(gates_2[now_step][i*color+j]->getVar());
            vars_1.push_back(gates_1[now_step][i*color+j]->getVar());
         }
         solver.step2_1CNF(vars_2, vars_1, gates_2[now_step][num*color+2*color+i]->getVar());
         solver.step2_2CNF(vars_1, vars_Cn1, gates_2[now_step][num*color+2*color+i]->getVar());
         solver.step2_2CNF(vars_2, vars_Cn2, gates_2[now_step][num*color+2*color+i]->getVar());
         vars_2.clear();
         vars_1.clear();
      }

      // STEP 3 check only one kn is 1   
      // num of clauses : num*(num-1)/2 + 1
      // num of variables : 0

      vector<Var> vars_k;
      for (size_t i=0; i<num; ++i) {
         vars_k.push_back(gates_2[now_step][num*color + 2*color + i]->getVar());
      }
      solver.step3_1CNF(vars_k);


      // STEP 4 change adjacent nodes (with same color)
      // num of clauses : 
      // num of variables : 


      // STEP 4.1 create new _k and initialize 1 to certain _k
      // num of clauses : num*num
      // num of variables : num

      for (size_t i=0; i<num*num; ++i) {
         gates_4_1[now_step].push_back(new Gate(i));
         Var v = solver.newVar();
         gates_4_1[now_step][i]->setVar(v);
      }
      

      // STEP 4.2 set adjacent node the same value of k
      // num of clauses : num_e*(2*color*3+2*color+1+9+1) = num_e*(8*color+11)
      // num of variables : (2*color+4)*num_e


      vector<Var> C_init[num], Cn1, Cn2;
      vector<Var> inter[num*num_e];
      int num_inter = 2*color+4;
      for (size_t i=0; i<num; ++i) {
         for (size_t j=0; j<num_inter*num_e; ++j) {
            gates_4_2[now_step].push_back(new Gate(i*num_inter*num_e+j));
            Var v = solver.newVar();
            gates_4_2[now_step][i*num_inter*num_e+j]->setVar(v);
         }
      }
      for (size_t i=0; i<num; ++i) {
         for (size_t j=0; j<color; ++j) {
            C_init[i].push_back(gates_1[now_step][i*color+j]->getVar());
         }
      }
      for(size_t i=0; i<color; ++i) {
         Cn1.push_back(gates_2[now_step][num*color+i]->getVar());
         Cn2.push_back(gates_2[now_step][num*color+color+i]->getVar());
      }

      for (size_t i=0; i<num; ++i) {
         for (size_t j=0; j<num_e; ++j) {
            for (size_t k=0; k<num_inter; ++k) {
               inter[i*num_e+j].push_back(gates_4_2[now_step][(num_inter*num_e)*i+num_inter*j+k]->getVar());
            }
         }
      }


      size_t adj_idx, count = 0;
      for (size_t i=0; i<num; ++i) {
         count = 0;
         for (size_t j=0; j<num; ++j) {
            for (size_t k=0; k<graph[j].adj.size(); ++k) {
               adj_idx = graph[j].adj[k];
               solver.step4_2CNF(C_init[j], C_init[adj_idx], 
                  gates_4_1[now_step][num*i+j]->getVar(), gates_4_1[now_step][num*i+adj_idx]->getVar(), inter[num_e*i+count]);
               count ++;
            } 
         }
      }


     

      // STEP 4.3 set the kn whose color is different from the clicked color to 0
      // num of clauses : num
      // num of variables : 0

      // STEP 4.4 set the kn which is surrounded by 0's to 0 (it is not 1)
      // num of clauses : num
      // num of variables : 0

      vector<Var> _vars_k;

      for (size_t k=0; k<num; ++k) {
         _vars_k.clear();
         for (size_t i=0; i<num; ++i) {
            _vars_k.push_back(gates_4_1[now_step][k*num+i]->getVar());
         }
         solver.step4_1CNF(vars_k, _vars_k);
         for (size_t i=0; i<num; ++i) {
            solver.step4_3CNF(C_init[i], Cn1, gates_4_1[now_step][i+k*num]->getVar());
         }
         for (int i=0; i<num; ++i) {
            solver.step4_4CNF(_vars_k, i, adjnodes[i], vars_k);
         }
         vars_k.clear();
         vars_k = _vars_k;
      }

      // STEP 4.5 change the color with k = 1
      // num of clauses :
      // num of variables : num*color + (4*color+4)*num

      vector<Var> inter_2[num];
      vector<Var> C_new[num];
      for (size_t i=0; i<num*color; ++i) {
         gates_1[now_step+1].push_back(new Gate(i));
         Var v = solver.newVar();
         gates_1[now_step+1][i]->setVar(v);
      }

      for (size_t i=0; i<(4*color+4)*num; ++i) {
         gates_4_5[now_step].push_back(new Gate(i));
         Var v = solver.newVar();
         gates_4_5[now_step][i]->setVar(v);
      }

      for (size_t i=0; i<num; ++i) {
         for (size_t j=0; j<color; ++j) {
            C_new[i].push_back(gates_1[now_step+1][i*color+j]->getVar());
         }
      }

      for (size_t i=0; i<num; ++i) {
         for (size_t j=0; j<4*color+4; ++j) {
            inter_2[i].push_back(gates_4_5[now_step][i*(4*color+4)+j]->getVar());
         }
      }

      for (size_t i=0; i<num; ++i) {
         solver.step4_5CNF(C_init[i], C_new[i], inter_2[i], Cn2, _vars_k[i]);
      }

      // // STEP 4.6 check all colors are valid(C' and C'') 
      // // num of clauses : invalid*2*num
      // // num of variables : 0

      vector<Var> all_color[num*2];

      for (size_t i=0; i<num; ++i) {
         for (size_t j=0; j<color; ++j) {
            all_color[i].push_back(gates_2[now_step][i*color+j]->getVar());
            all_color[num+i].push_back(gates_1[now_step+1][i*color+j]->getVar());
         }
      }
      for (size_t i=0; i<2*num; ++i) {
         solver.step4_6CNF(all_color[i], invalid_colors);
      }

      now_step ++;
   }

   // STEP 5 all color are the same
   // num of clauses : color
   // num of variables : 2*color

   vector<Var> final_color[color];

   for (size_t i=0; i<2*color; ++i) {
      gates_1[steps].push_back(new Gate(i));
      Var v = solver.newVar();
      gates_1[steps][num*color+i]->setVar(v);
   }

   for (size_t i=0; i<color; ++i) {
      for (size_t j=0; j<num; ++j) {
         final_color[i].push_back(gates_1[steps][i+j*color]->getVar());
      }
   }
   for (size_t i=0; i<color; ++i) {
      solver.step5CNF(final_color[i], gates_1[steps][num*color+i*2]->getVar(), gates_1[steps][num*color+i*2+1]->getVar());
   }








   // collect gates_n to gates

   // for (size_t i=0; i<steps; ++i) {
   //    for (size_t j=0; j<gates_2[i].size(); ++j) {
   //       gates.push_back(gates_2[i][j]);
   //    }
   // }

   // for (size_t i=0; i<steps; ++i) {
   //    for (size_t j=0; j<gates_4_1[i].size(); ++j) {
   //       gates.push_back(gates_4_1[i][j]);
   //    }
   // }

   
   for (size_t i=0; i<steps; ++i) {
      for (size_t j=0; j<gates_1[i].size(); ++j) {
         gates.push_back(gates_1[i][j]);
      }
   }
   // for (size_t i=0; i<steps; ++i) {
   //    for (size_t j=0; j<gates_2[i].size(); ++j) {
   //       gates.push_back(gates_2[i][j]);
   //    }
   // }
   // for (size_t i=0; i<steps; ++i) {
   //    for (size_t j=0; j<gates_4_1[i].size(); ++j) {
   //       gates.push_back(gates_4_1[i][j]);
   //    }
   // }
   // for (size_t i=0; i<steps; ++i) {
   //    for (size_t j=0; j<gates_4_2[i].size(); ++j) {
   //       gates.push_back(gates_4_2[i][j]);
   //    }
   // }
   // for (size_t i=0; i<steps; ++i) {
   //    for (size_t j=0; j<gates_4_5[i].size(); ++j) {
   //       gates.push_back(gates_4_5[i][j]);
   //    }
   // }
   for (size_t j=0; j<num*color; ++j) {
      gates.push_back(gates_1[steps][j]);
   }

   for (size_t i=0; i<steps; ++i) {
      for (size_t j=0; j<color; ++j) {
         gates.push_back(gates_2[i][num*color+color+j]);
      }
   }

   for (size_t i=0; i<steps; ++i) {
      for (size_t j=0; j<num; ++j) {
         gates.push_back(gates_2[i][num*color+2*color+j]);
      }  
   }
  


   bool result;

   Var newV = solver.newVar();
   solver.assumeRelease();  // Clear assumptions
   solver.assumeProperty(newV, true);  // k = 1
   result = solver.assumpSolve();
   reportResult(solver, result, num, color, steps);
}
