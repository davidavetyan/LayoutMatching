#pragma once

#include <iostream>

constexpr int M = 6;
constexpr int N = 6;

bool bipartiteGraph[M][N] = //A graph with M applicant and N jobs
{    
    {0, 1, 1, 0, 0, 0},
    {1, 0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0, 0},
    {0, 0, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1}
};

bool bipartiteMatch(int u, bool visited[], int assign[]) 
{
    for (int v = 0; v < N; ++v) 
    {    
        //for all jobs 0 to N-1
        if (bipartiteGraph[u][v] && !visited[v]) // when job v is not visited and u is interested
        {
            //mark as job v is visited
            visited[v] = true;    

            //when v is not assigned or previously assigned
            if (assign[v] < 0 || bipartiteMatch(assign[v], visited, assign)) 
            { 
                //assign job v to applicant u
                assign[v] = u;   
                return true;
            }
        }
    }
    return false;
}

int maxMatch() 
{
    //an array to track which job is assigned to which applicant
    int assign[N];    
    for(int i = 0; i<N; ++i)
        //initially set all jobs are available
        assign[i] = -1;  

    int jobCount = 0;

    for (int u = 0; u < M; u++) 
    {    
        //for all applicants
        bool visited[N];
        for(int i = 0; i<N; i++)
            //initially no jobs are visited
            visited[i] = false;    
        if (bipartiteMatch(u, visited, assign))
            //when u get a job
            jobCount++;
    }
    return jobCount;
}