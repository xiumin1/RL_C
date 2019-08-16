#pragma once
#include <iostream>
#include<stdlib.h>
#include <time.h>
#include<vector>
#include<tuple>
using namespace std;

#define MACRO_RL _declspec(dllexport)

class MyRL_QLearning
{
	int state_dim;
	int action_dim;
	int grid_rows;
	int grid_cols;
	int target_state;

	int obst1, obst2;

	double alpha; //0.1
	double gamma; //0.6
	double epsilon; //0.1

					//tuple<double, double> pos_target;
					//Point pos_agent_init;
					//Point pos_groud;
	double** Qtable;
	double** Rtable;

public:
	//MyRL_QLearning(int State_dim, int Action_dim, int grid_rows, int grid_cols, double Alpha, double Gamma, double Epsilon, tuple<double, double> Pos_target);
	MyRL_QLearning(int State_dim, int Action_dim, int Grid_rows, int Grid_cols, double Alpha, double Gamma, double Epsilon, int* target_s, int* obt1, int* obt2);
	void Create_Qtable_Rtable();

	void ArgumentMax(int state, double* qvalue, int* act);
	int Greedy_action(int st);
	void Run_env(int state, int act, int* next_state, double* reward, bool* done);
	//return next_state,reward,done
	int Reset_env(); //return a state number
	void RL_train();
	void RL_run();
	~MyRL_QLearning();
};

