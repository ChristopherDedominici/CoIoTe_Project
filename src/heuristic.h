/*
 * heuristic.h
 *
 *  Created on: 20 nov 2016
 *      Author: group_35
 */


#ifndef HEURISTIC_H_
#define HEURISTIC_H_

#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <list>
#include <map>
#include <unordered_map>

using namespace std;


typedef multimap<double,vector<int>> Costs;
typedef  vector<vector<int>> MoveGroup;
typedef  unordered_map<int,MoveGroup> Solution;
typedef map<int,Solution> NeighborhoodStructure;





struct Data {
    /**
     * The costs ijkt
     */
    double**** costs;

    /**
     * number of activity done by one user of type k
     */
    int* n;

    /**
     * Activities to be done in node i during the given time horizon
     */
    int* activities;


    /**
     * Number of users of type m in i during time period t (ikt)
     */
    int**** usersCell;
    int*** usersCellBackup;



};

struct SimulatedAnnealingParameters{

    double T0; //initial temperature
    double TF; //final temperature

    double T;

    double alpha;



    double Plateau; //# iterations before annealing the temperature
};

enum eFeasibleState {
    FEASIBLE,
    NOT_FEASIBLE_DEMAND,
    NOT_FEASIBLE_USERS
};

class Heuristic{
private:
    /**
     * Number of periods
     */
    int nTimeSteps;

    /**
     * Number of customer types
     */
    int nCustomerTypes;

    /**
     * Number of cells
     */
    int nCells;


    /**
     * Problem structure for parameters
     */
    Data problem;

    /**
     * Flag equals to true if the problems has a solution
     */
    bool hasSolution;

    /**
     * Variables of the problem (X in the model)
     */


    SimulatedAnnealingParameters SA_parameters;



    int Num_activities; //stores the total number of activities to be solved in current problem
    int NumUndoneActivities;


    Costs *cost;
    Costs *cost_1;
    Costs *cost_2;

    int* rndVect;




    Solution solCurrent; //to store the current solution
    Solution bestSolution;


    NeighborhoodStructure NeighborMap;

    const static int N_dim=20;

    double objFun[N_dim+2];//20 neighbors + current and best obj Fun

    long int globalCounter=0;



    int iterationCounter;
    int ChoosenNeighbor;

    bool notChoosen = false;
    bool firstStep = true;






public:
    /**
     * Default constructor
     * @return Heuristic object
     */
    Heuristic(){};

    /**
     * Constructor from external file
     * @param path path of the external file cotaining the instance of the problem
     * @return
     */
    Heuristic(string path);

    /**
     * Function to CHANGE!!! This function only makes a very bad solution for the problem
     * @param stat Array of statistics. In position 0 put the objVal, in position 1 the computational time
     * @param timeLimit Time limit for computation
     * @param verbose
     */




    void generateInitialSolution();


    void generateNeighbors();

    void create_C_Struct(int startP, int endP, Costs *cost_pointer);
    void CostsStructureGeneration();
    void Cooling_Schedules();

    int NeighborRandomChoice();

    void StructuresUpdating();

    void AcceptancePhase();

    void ResetStructures();

    void Memetic_Algorithm(vector<double> &stat, double timeLimit = -1);

    /**
     * Puts KPIs in the statistics' array. Call this only if problem has a solution
     * @param stat Array of statistics
     */
    void getStatSolution(vector<double>& stat);

    /**
     * Write KPIs on a file
     * @param path path of the file
     * @param nameInstance name of the instance
     * @param stat array of statistics
     */
    void writeKPI(string path, string nameInstance, vector<double> stat);

    /**
     * Write the detailed solution on a file
     * @param path path of the file
     */
    void writeSolution(string path);

    /**
     * Check the feasibility of the problem
     * @param path path of the solution file
     * @return a state of the check (i.e. FEASIBLE if the solution is feasible)
     */
    eFeasibleState isFeasible(string path);
};


#endif /* HEURISTIC_H_ */
