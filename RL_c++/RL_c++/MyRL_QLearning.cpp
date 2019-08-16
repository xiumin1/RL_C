#include "MyRL_QLearning.h"
#include<tuple>
MyRL_QLearning::MyRL_QLearning(int State_dim, int Action_dim, int Grid_rows, int Grid_cols, double Alpha, double Gamma, double Epsilon, int* target_s, int* obt1, int* obt2)
{
	grid_rows = Grid_rows;
	grid_cols = Grid_cols;
	state_dim = State_dim;
	action_dim = Action_dim;
	alpha = Alpha;
	gamma = Gamma;
	epsilon = Epsilon;
	//pos_agent_init=Pos_agent_init;
	Create_Qtable_Rtable();
	*target_s=target_state;
	*obt1 = obst1;
	*obt2 = obst2;
}

void MyRL_QLearning::Create_Qtable_Rtable()
{
	// to create the tables
	//cout<<"Create_Qtable_Rtable 0"<<"\n";
	srand((int)time(0));
	obst1 = rand() % state_dim;
	obst2 = rand() % state_dim;
	while (obst2 == obst1)
	{
		obst2 = rand() * state_dim;
	}
	target_state = rand() % state_dim;
	while (target_state == obst1 || target_state == obst2)
	{
		target_state = rand() % state_dim;
	}

	Qtable = new double*[state_dim];
	for (int i = 0; i < state_dim; ++i)
		Qtable[i] = new double[action_dim];
	/* states
	90 91 92 ...99
	.
	.
	.
	0  1. 2  ... 9
	*/
	Rtable = new double*[state_dim];
	for (int i = 0; i<state_dim; ++i)
	{
		Rtable[i] = new double[action_dim];
	}
	/*
	action : 0. -> up
	action : 1  ->down
	action : 2. -> left
	action : 3  -> right
	*/
	//to initialize Qtable
	for (int i = 0; i<state_dim; ++i)
	{
		for (int j = 0; j<action_dim; ++j)
		{
			Qtable[i][j] = 0;
		}
	}
	// to assign values to Rtable
	// int grid_rows=10;
	//int grid_cols=10;
	// target grid location: (target_col, target_row)

	//int target_col = (int)((pos_target_x - (-5.0f) + 0.5f) / 1.0f) ;
	//int target_row = (int)((pos_target_z - (-5.0f) + 0.5f) / 1.0f) ;

	int target_col = (int)(target_state % grid_cols + 1);
	int target_row = (int)(target_state / grid_cols + 1);

	//cout << "x = " << x << ", z = " << z << "\n";
	cout << "target_col = " << target_col << ", target_row = " << target_row << "\n";

	//target_state = (target_row - 1)*grid_cols + target_col - 1;
	//cout << "target_state = " << target_state << "\n";

	for (int i = 0; i<state_dim; ++i)
	{
		//initialize the r value to -0.01f for time cost peneties for all grids
		for (int j = 0; j<action_dim; ++j)
		{
			Rtable[i][j] = -0.01f;
		}

		// initialize the r value to 1 when arrive the target, here only the 4 sourounding grids can reach the target.
		//target: (col, row)
		// up: (col, row+1)
		// down: (col, row-1)
		//left: (col-1, row)
		//right: (col+1, row)
		int target_up_grid = ((target_row + 1) - 1)*grid_cols + target_col;
		int target_down_grid = ((target_row - 1) - 1)*grid_cols + target_col;
		int target_left_grid = (target_row - 1)*grid_cols + (target_col - 1);
		int target_right_grid = (target_row - 1)*grid_cols + (target_col + 1);

		//initialize the r value to -0.1 when accross the wall

		if (i<grid_rows)
		{
			Rtable[i][1] = -0.1f; // down wall (grid: 0-9)
			target_down_grid = -1;
		}
		if (i >= (grid_rows - 1)*grid_cols && i<grid_rows*grid_cols)
		{
			Rtable[i][0] = -0.1f; // up wall (grid: 90-99)
			target_up_grid = -1;
		}
		if (i%grid_cols == 0)
		{
			Rtable[i][2] = -0.1f; //left wall (grad: 0,10,20,...,90)
			target_left_grid = -1;
		}
		if ((i + 1) % grid_cols == 0)
		{
			Rtable[i][3] = -0.1f; // right wall (grid: 9,19,29,...,99)
			target_right_grid = -1;
		}

		// when arriving the target
		if (i == target_up_grid - 1) Rtable[i][1] = 1.0f; // take the down action
		if (i == target_down_grid - 1) Rtable[i][0] = 1.0f; // take the up action
		if (i == target_left_grid - 1) Rtable[i][3] = 1.0f; // take the right action
		if (i == target_right_grid - 1) Rtable[i][2] = 1.0f; // take the left action

	}

	// this part use to assign the Rtable value for the 4 states around obstacles
	int obstacles[] = { obst1,obst2 };

	for (int i = 0; i < 2; i++)
	{
		if (obstacles[i] == 0) // obstacle in the bottom left corner
		{
			Rtable[obstacles[i] + grid_cols][1] = -0.1f;
			Rtable[obstacles[i] + 1][2] = -0.1f;
		}
		else
			if (obstacles[i] == grid_cols * grid_rows - 1) // obstacle in the up right corner
			{
				Rtable[obstacles[i] -grid_cols][0] = -0.1f;
				Rtable[obstacles[i] -1][3] = -0.1f;
			}
			else
				if (obstacles[i] < grid_rows-1) // obstacle in the bottom line
				{
					Rtable[obstacles[i] + grid_cols][1] = -0.1f; // down wall (grid: 1-8)
					Rtable[obstacles[i] - 1][3] = -0.1f;
					Rtable[obstacles[i] + 1][2] = -0.1f;
				}
				else
					if (obstacles[i] > (grid_rows - 1)*grid_cols && i<grid_rows*grid_cols-1) // obstacle in the upper line
					{
						Rtable[obstacles[i] - grid_cols][0] = -0.1f; // up wall (grid: 91-98)
						Rtable[obstacles[i] - 1][3] = -0.1f;
						Rtable[obstacles[i] + 1][2] = -0.1f;
					}
					else // obstacle in the center area
					{
						Rtable[obstacles[i] + grid_cols][1] = -0.1f;
						Rtable[obstacles[i] - grid_cols][0] = -0.1f; 
						Rtable[obstacles[i] - 1][3] = -0.1f;
						Rtable[obstacles[i] + 1][2] = -0.1f;
					}
	}

	// print the initial R table for checking

	for (int i = 0; i<state_dim; i++)
	{
		cout << "state " << i << ": ";
		for (int j = 0; j<action_dim; j++)
		{
			cout << Rtable[i][j] << " ";
		}
		cout << "\n";
	}

	//cout<<"Create_Qtable_Rtable 1"<<"\n";
}
void MyRL_QLearning::ArgumentMax(int state, double* qvalue, int* act)
{
	//cout<<"ArgumentMax 0"<<"\n";
	double Q_max = -1000000;
	int act_max;
	for (int i = 0; i<action_dim; i++)
	{
		if (Qtable[state][i]>Q_max)
		{
			Q_max = Qtable[state][i];
			act_max = i;
		}
	}
	//cout<<"ArgumentMax 1"<<"\n";
	*qvalue = Q_max;
	*act = act_max;
	//return { Q_max, act_max };
}
int MyRL_QLearning::Greedy_action(int state)
{
	//cout<<"Greedy_action 0"<<"\n";
	//srand((int)time(0));
	int act_select;
	double Q_max;
	//int act_max;
	double random_ratio = (double)rand() / double(RAND_MAX); //[0,1]
															 //cout<<"random_ratio = "<<random_ratio<<"\n";
	if (random_ratio<epsilon)
	{
		act_select = rand() % action_dim;
	}
	else
	{
		ArgumentMax(state, &Q_max, &act_select);
		//act_select = act_max; // to get the action index
	}
	//cout<<"Greedy_action 1"<<"\n";
	return act_select;
}
void MyRL_QLearning::Run_env(int state, int act, int* next_state, double* reward, bool* done)
{
	//know the current state information
	//cout<<"Run_env 0"<<"\n";
	*next_state = -1;
	//double reward;
	*done = false;
	int row_num, col_num;
	row_num = state / grid_cols + 1;
	col_num = state % grid_cols + 1;


	if (act == 0)
	{
		if (row_num<grid_rows)
			*next_state = ((row_num - 1) + 1)*grid_cols + (col_num - 1);
		else *next_state = state;
	}
	if (act == 1)
	{
		if (row_num>1)
			*next_state = ((row_num - 1) - 1)*grid_cols + (col_num - 1);
		else *next_state = state;
	}
	if (act == 2)
	{
		if (col_num>1)
			*next_state = (row_num - 1)*grid_cols + (col_num - 1) - 1;
		else *next_state = state;
	}
	if (act == 3)
	{
		if (col_num<grid_cols)
			*next_state = (row_num - 1)*grid_cols + (col_num - 1) + 1;
		else *next_state = state;
	}
	*reward = Rtable[state][act];
	if (*next_state == target_state) *done = true;

	//cout<<"Run_env 1"<<"\n";
	//return { next_state, reward, done };

}
int MyRL_QLearning::Reset_env()
{
	//cout<<"Reset_env 0"<<"\n";
	int state;
	//srand((int )time(0));

	state = rand() % state_dim;
	while (state == obst1 || state == obst2 || state == target_state)
	{
		state = rand() % state_dim;
	}
	//cout<<"Reset_env 1"<<"\n";
	return state;
}
void MyRL_QLearning::RL_train()
{
	cout << "RL_train 0" << "\n";
	srand((int)time(0));

	int epoch = 100000;
	int steps = 1000;

	int state;
	int action;
	int next_state;
	double reward, rewards;
	vector<double> REWARDS;
	bool done = false;

	//tuple<int, double, bool> data;
	double Q_max;
	int act_max;
	cout << "state_dim = " << state_dim << ", action_dim = " << action_dim << "\n";
	for (int i = 0; i<epoch; i++)
	{
		state = Reset_env();
		//cout<<"state init = "<<state<<"\n";
		int step = 0;
		rewards = 0;
		done = false;
		while (!done && step<steps)
		{
			action = Greedy_action(state);
			//cout<<"state = "<<state<<", action = "<<action<<"\n";
			Run_env(state, action, &next_state, &reward, &done);
			//next_state = get<0>(data);
			//reward = get<1>(data);
			//done = get<2>(data);

			ArgumentMax(next_state, &Q_max, &act_max);
			//Q_max = get<0>(ArgumentMax(next_state));
			//cout<<"next_state = "<<next_state<<", Q_max = "<<Q_max<<"\n";
			//update the Q-table
			Qtable[state][action] = (1 - alpha)*Qtable[state][action] + alpha * (reward + gamma * Q_max);

			state = next_state;
			step++;
			rewards += reward;
		}
		if (i % 1000 == 0)
		{
			REWARDS.push_back(rewards);
			cout << "epoch " << i << ": " << rewards << "\n";
		}
	}
	cout << "RL_train 1" << "\n";

	// print the initial R table for checking

	for (int i = 0; i<state_dim; i++)
	{
		cout << "state " << i << ": ";
		for (int j = 0; j<action_dim; j++)
		{
			cout << Qtable[i][j] << " ";
		}
		cout << "\n";
	}

}
/*
void MyRL_QLearning::RL_run()
{
	//to run the trained q table
	//srand((int)time(0));
	double reward, rewards;
	vector<double> REWARDS;
	bool done = false;

	tuple<int, double, bool> data;


	int state, next_state;
	int action;
	for (int i = 0; i<10; i++)
	{
		done = false;
		state = Reset_env();
		//state=31;
		cout << "epoch " << i << ", init state = " << state << "\n";
		rewards = 0;
		while (!done)
		{
			//cout<<"inside"<<"\n";
			action = get<1>(ArgumentMax(state));
			data = Run_env(state, action);

			next_state = get<0>(data);
			reward = get<1>(data);
			done = get<2>(data);
			cout << "done = " << done << "\n";

			cout << "state = " << state << ", action = " << action << ", next_state = " << next_state << "\n";

			state = next_state;
			rewards += reward;

		}
		REWARDS.push_back(rewards);

	}
	for (int i = 0; i<REWARDS.size(); i++)
		cout << REWARDS[i] << "\n";

}
*/
MyRL_QLearning::~MyRL_QLearning()
{

}

MACRO_RL MyRL_QLearning* Create_MyRL_QLearning(int State_dim, int Action_dim, int Grid_rows, int Grid_cols, double Alpha, double Gamma, double Epsilon, int* target_s, int* obt1, int* obt2)
{
	return new MyRL_QLearning( State_dim, Action_dim, Grid_rows, Grid_cols, Alpha, Gamma, Epsilon, target_s, obt1, obt2);
}
MACRO_RL void Get_RL_train(MyRL_QLearning* myrl)
{
	myrl->RL_train();
}
MACRO_RL void Get_ArgumentMax(MyRL_QLearning* myrl, int state, double* Q_max, int* act_max)
{
	myrl->ArgumentMax(state, Q_max, act_max);
}
MACRO_RL int Get_Reset_env(MyRL_QLearning* myrl)
{
	return myrl->Reset_env();
}
MACRO_RL void Get_Run_env(MyRL_QLearning* myrl, int state, int act, int* next_state, double* reward, bool* done)
{
	myrl->Run_env(state, act, next_state, reward, done);
}



/*
#include "sharedAPI.h"
#include <iostream>
#include<string>
using namespace std;

SHAREDAPI int Add(int a, int b)
{
return a + b;
}


SHAREDAPI void CallMe(void(*action)(int value))
{
action(32);

}

SHAREDAPI sharedAPI* CreatesharedAPI(int id)
{
return new sharedAPI(id);
}

SHAREDAPI int GetMyIdPlusTen(sharedAPI* api)
{
return api->myidplusTen();
}

SHAREDAPI void DeletesharedAPI(sharedAPI* api)
{
delete api;

}

SHAREDAPI int** GetArray(sharedAPI* api)
{
return api->GetArray();
}

SHAREDAPI void DeleteArray(int** myarray)
{
for (int i = 0; i < 10; i++)
{
delete[] myarray[i];
}
delete[] myarray;
}

sharedAPI::sharedAPI(int id) : id(id)
{

}

int sharedAPI::myidplusTen()
{
return Add(id, 10);
}

int** sharedAPI::GetArray()
{
int** myarray = new int*[10];
for (int i = 0; i < 10; i++)
{
myarray[i] = new int[10];

for (int j = 0; j < 10; j++)
{
myarray[i][j] = i * j + 10;
}
}
return myarray;
}

sharedAPI::~sharedAPI()
{
}

*/