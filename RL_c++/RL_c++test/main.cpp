#include "MyRL_QLearning_test.h"
#include "iostream"
#include "time.h"
#include"stdlib.h"
using namespace std;
int main()
{
	//srand((int)time(0));
	int target_st;
	int obst1;
	int obst2;
	MyRL_QLearning ql = *new MyRL_QLearning(100, 4, 10, 10, 0.1, 0.6, 0.1, &target_st, &obst1, &obst2);
	cout << "starget_state = " << target_st << endl;
	cout << "obst1_state = " << obst1 << endl;
	cout << "obst2_state = " << obst2 << endl;
	ql.RL_train();
	ql.RL_run();
	getchar();
	return 0;
}