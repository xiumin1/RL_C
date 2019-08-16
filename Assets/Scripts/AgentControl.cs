using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using System.Runtime.InteropServices;
using System;
public class AgentControl : MonoBehaviour
{
    // Start is called before the first frame update
    int state_dim=100;
    int action_dim=4;
    int grid_rows=10;
    int grid_cols=10;
    int target_state;

    int obstacle_state1;
    int obstacle_state2;
    //float example=0.1f
    //double example=0.1d
    double alpha=0.1d;
    double gamma=0.1d;
    double epsilon=0.6d;


    Transform agent;
    Transform target;

    Transform obstacle1;
    Transform obstacle2;

    private const string pluginname = "RL_c++";

    [DllImport(pluginname)]
    extern static IntPtr Create_MyRL_QLearning(int State_dim, int Action_dim, int Grid_rows, int Grid_cols, double Alpha, double Gamma, double Epsilon, ref int target_state, ref int obstacle_state1, ref int obstacle_state2);

    [DllImport(pluginname)]
    extern static void Get_RL_train(IntPtr myrl);

    [DllImport(pluginname)]
    extern static void Get_ArgumentMax(IntPtr myrl, int state, ref double Q_max, ref int act_max );

    [DllImport(pluginname)]
    extern static int Get_Reset_env(IntPtr myrl);

    [DllImport(pluginname)]
    extern static void Get_Run_env(IntPtr myrl, int state, int act, ref int next_state, ref double reward, ref bool done);

    IntPtr myrl_QLearning;

    double reward, rewards;
    List<double> REWARDS=new List<double>();
    bool done;
    int next_state, state, action;
    double Q_max;
    float x, y, z;
// int reset_counter = 0;
    float timer = 0;
    void Start()
    {
        agent = GameObject.Find("agent").transform;
        target = GameObject.Find("target").transform;
        obstacle1 = GameObject.Find("obstacle1").transform;
        obstacle2 = GameObject.Find("obstacle2").transform;

       // target.position= new Vector3(2.5f, 0.01f, 3.5f);

        myrl_QLearning = Create_MyRL_QLearning(state_dim, action_dim, grid_rows, grid_cols, alpha, gamma, epsilon, ref target_state, ref obstacle_state1, ref obstacle_state2);
        Debug.Log("init state = " + state + ", target_state = " + target_state + ", obstacle_state1 = " + obstacle_state1 + ", obstacle_state2 = " + obstacle_state2);
        Get_RL_train(myrl_QLearning);
        Debug.Log("init state = " + state + ", target_state = " + target_state + ", obstacle_state1 = " + obstacle_state1 + ", obstacle_state2 = " + obstacle_state2);
        done = false;
        state = Get_Reset_env(myrl_QLearning);
       // Debug.Log("test");
        Debug.Log("init state = "+state+", target_state = "+target_state+", obstacle_state1 = "+obstacle_state1+", obstacle_state2 = "+obstacle_state2);
        agent.position = StateToPosition(state);
        rewards = 0;

        target.position = StateToPosition(target_state);
        target.position = new Vector3(target.position.x, 0.01f, target.position.z);
        obstacle1.position = StateToPosition(obstacle_state1);
        obstacle2.position = StateToPosition(obstacle_state2);

    }

    // Update is called once per frame
    void Update()
    {
        timer += Time.time;
        if (timer > 100.0f)
        {
            RL_run();
            timer = 0;
        }
            

        //StartCoroutine(waitforseconds(2.0f));
    }

    Vector3 StateToPosition(int state)
    {
        int row, col;
        float pos_x, pos_z;
        row = state / grid_cols+1;
        col = state % grid_cols + 1;

        pos_x = col * 1.0f - 0.5f + (-5.0f);
        pos_z = row * 1.0f - 0.5f + (-5.0f);

        return new Vector3(pos_x,0.5f,pos_z);

    }
    void RL_run()
    {
        if (!done)
        {
            x = agent.position.x;
            y = agent.position.y;
            z = agent.position.z;

            Get_ArgumentMax(myrl_QLearning, state, ref Q_max, ref action);
            Debug.Log("state = " + state + ", action = " + action);
            if (action == 0) agent.position = new Vector3(x, y, z+1.0f);
            if (action == 1) agent.position = new Vector3(x, y, z-1.0f);
            if (action == 2) agent.position = new Vector3(x-1.0f, y, z);
            if (action == 3) agent.position = new Vector3(x+1.0f, y, z);

            Get_Run_env(myrl_QLearning, state, action, ref next_state, ref reward, ref done);
           // Debug.Log("state = "+state+" action = "+action+" next_state = "+next_state);
            state = next_state;
            rewards += reward;

        }
        /*
        if (done && reset_counter < 10)
        {
            reset_counter++;
            REWARDS.Add(rewards);
            rewards = 0;
            done = false;
            state = Get_Reset_env(myrl_QLearning);
            agent.position = StateToPosition(state);
        }
        if (reset_counter == 10)
        {
            for (int i = 0; i < REWARDS.Count; i++)
                Debug.Log(REWARDS[i]);
        }
        */
    }
    IEnumerator waitforseconds(float t)
    {
        yield return new WaitForSeconds(t);
        RL_run();
    }
}





/*
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using System.Runtime.InteropServices;
using System;

public class sample1 : MonoBehaviour
{
    // Start is called before the first frame update
    private const string pluginname = "mydll";

    public delegate void mydelegate(int value);

    public static mydelegate mycall;

    [DllImport(pluginname)]
    public static extern int Add(int a, int b);


    [DllImport(pluginname)]
    public static extern void CallMe(Action<int> action); //this is a callback

    [DllImport(pluginname)]
    extern static IntPtr CreatesharedAPI(int id);

    [DllImport(pluginname)]
    extern static int GetMyIdPlusTen(IntPtr api);

    [DllImport(pluginname)]
    extern static void DeletesharedAPI(IntPtr api);


    //extern static void Name();

    [DllImport(pluginname)]
    static extern IntPtr GetArray(IntPtr api);

    [DllImport(pluginname)]
    static extern void DeleteArray(IntPtr array);

    Text text;
    Transform agent, target;

    //mydelegate Iwascalled;
    IntPtr sharedAPI1, sharedAPI2;
    void Start()
    {
        text = GetComponent<Text>();

        Debug.Log("test output");
        sharedAPI1 = CreatesharedAPI(1);
        sharedAPI2 = CreatesharedAPI(2);
        //mydelegate.action += Iwascalled;
        //mydelegate cback=new mydelegate(Iwascalled);
        //CallMe(Iwascalled);

        // Name();
        //  text.text = "test";

        // mycall += Iwascalled;
        // mycall(15);
        // text.text = Test(13).ToString();
        //text.text = Add(1, 1).ToString();

        // agent = GameObject.Find("agent").transform;
        // target = GameObject.Find("target").transform;

        IntPtr array = GetArray(sharedAPI1);
        string mystuff="";
        unsafe
        {
            Debug.Log("unsafe test");
            int** myarray= (int**)array.ToPointer();
            Debug.Log(array);
            for(int i=0;i<10;i++)
            {
                for (int j = 0; j < 10; j++)
                {
                    mystuff += myarray[i][j].ToString()+" ";
                }
                mystuff += "\n";
                
            }
        }

        text.text = mystuff;
    }
    /*
    void Iwascalled(int value)
    {
        text.text = value.ToString();
    }
    
    // Update is called once per frame
    void Update()
    {
        // text.text = Add(1, 2).ToString();
        if (UnityEngine.Random.Range(0.0f, 1.0f) < 0.5f)
        {
            text.text = GetMyIdPlusTen(sharedAPI1).ToString();
        }
        else
        {
            text.text = GetMyIdPlusTen(sharedAPI2).ToString();
        }

    }

    public void OnDestroy()
    {
        DeletesharedAPI(sharedAPI1);
        DeletesharedAPI(sharedAPI2);
    }

    */




