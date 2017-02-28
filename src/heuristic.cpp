/*
 * heuristic.cpp
 *
 *  Created on: 20 nov 2016
 *      Authors: group_35
 *
 *      Luca Capaccio
 *      Davide Cometa
 *      Christopher Dedominici
 *      Matias Morales
 *      Claudio Sava
 *
 */
#include <iostream>
#include <random>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "heuristic.h"
#include <thread>


double Percentage_Undone_Activities = 1.29; ///percentage of activities to be undone for neighborhood generation
                                            /// initially is set to 1.29 to easily adapt it to 0.99 or 0.29 on the basis of nCells (see below)

using namespace std;





Heuristic::Heuristic(string path){

    srand(time(NULL));

    this->hasSolution = false;
    string line;
    string word;

    Num_activities = 0;
    NumUndoneActivities = 0;

    ifstream iffN(path.c_str());

    if (!iffN.is_open()) {
        cout << "Impossible to open" << path << endl;
        cin.get();
        exit(1);
    }

    getline(iffN, line);
    std::replace(line.begin(), line.end(), ';', ' ');
    istringstream iss(line);
    iss >> word;
    this->nCells = atoi(word.c_str());
    iss >> word;
    this->nTimeSteps = atoi(word.c_str());
    iss >> word;
    this->nCustomerTypes = atoi(word.c_str());

    rndVect = new int[nCells];


    problem.costs = new double***[nCells];

    for(int k=0;k<10;k++) {
        objFun[k] = 0;
    }



    problem.costs = new double***[nCells];
    for (int i = 0; i < this->nCells; i++) {
        problem.costs[i] = new double**[nCells];

        rndVect[i] = i;
        for (int j = 0; j < this->nCells; j++) {
            problem.costs[i][j] = new double*[nCustomerTypes];

            for (int m = 0; m < this->nCustomerTypes; m++) {
                problem.costs[i][j][m] = new double[nTimeSteps];

            }
        }
    }


    problem.n = new int[nCustomerTypes];
    problem.activities = new int[nCells];

    problem.usersCell = new int***[nCells];
    problem.usersCellBackup=new int**[nCells];

    for (int i = 0; i < this->nCells; i++) {
        problem.usersCell[i] = new int**[nCustomerTypes];
        problem.usersCellBackup[i]=new int*[nCustomerTypes];
        for (int m = 0; m < this->nCustomerTypes; m++) {
            problem.usersCell[i][m] = new int*[nTimeSteps];
            problem.usersCellBackup[i][m]= new int [nTimeSteps];
            for(int t = 0; t < this->nTimeSteps; t++){
                problem.usersCell[i][m][t]=new int[ N_dim  + 2];
            }
        }
    }




    getline(iffN, line);
    getline(iffN, line);
    std::replace(line.begin(), line.end(), ';', ' ');
    istringstream issN(line);
    for (int m = 0; m < nCustomerTypes; m++) {
        issN >> word;
        problem.n[m] = atoi(word.c_str());
    }



    getline(iffN, line);
    for (int m = 0; m < nCustomerTypes; m++) {

        for (int t = 0; t < nTimeSteps; t++) {

            getline(iffN, line);// line with m and t
            for (int i = 0; i < nCells; i++) {

                getline(iffN, line);// line of matrix c_{ij} for fixed t and m
                istringstream issC(line);
                for (int j = 0; j < nCells; j++) {

                    issC >> word;
                    problem.costs[i][j][m][t] = atoi(word.c_str());

                }
            }
        }
    }

    getline(iffN, line);
    getline(iffN, line);
    std::replace(line.begin(), line.end(), ';', ' ');
    istringstream issA(line);
    for (int i = 0; i < nCells; i++) {
        issA >> word;
        problem.activities[i] = atoi(word.c_str());
        if(problem.activities[i] != 0){
            Num_activities++;
        }
    }

    ///Calculating the best value of Percentage_Undone_Activities

    Percentage_Undone_Activities -= (double) nCells / 100;
    if(Percentage_Undone_Activities < 0.3) {Percentage_Undone_Activities = 0.3;}

    NumUndoneActivities = ceil(Percentage_Undone_Activities * Num_activities);  //number of demands to be undone

    getline(iffN, line);
    for (int m = 0; m < nCustomerTypes; m++) {
        for (int t = 0; t < nTimeSteps; t++) {
            getline(iffN, line);
            getline(iffN, line);
            std::replace(line.begin(), line.end(), ';', ' ');
            istringstream issU(line);
            for (int i = 0; i < nCells; i++) {
                issU >> word;
                problem.usersCellBackup[i][m][t]=atoi(word.c_str());
                problem.usersCell[i][m][t][0] = atoi(word.c_str());
                problem.usersCell[i][m][t][ N_dim +1] = 0; //initialize to 0 the counter
            }
        }
    }
}
void Heuristic::create_C_Struct(int startP, int endP, Costs *cost_pointer){

    int temp_i_j_m_t[4];
    vector<int> move;
    for (int m = 0; m < nCustomerTypes; m++) {
        temp_i_j_m_t[0]=m;
        for (int t = 0; t < nTimeSteps; t++) {
            temp_i_j_m_t[1]=t;

            for (int i = 0; i < nCells; i++) {
                temp_i_j_m_t[2]=i;


                for (int j = startP; j < endP; j++) {
                    temp_i_j_m_t[3]=j;
                    if(problem.usersCell[i][m][t][0]!=0 && problem.activities[j]!=0) {

                        move.insert(move.begin(), temp_i_j_m_t, temp_i_j_m_t + 4);
                        cost_pointer->insert(Costs::value_type(problem.costs[i][j][m][t] / (double) problem.n[temp_i_j_m_t[0]],move));

                        move.erase(move.begin(), move.end());
                    }
                }
            }
        }
    }
}

void Heuristic::CostsStructureGeneration() {

    //Costs structure generation
    cost_1=new Costs;
    cost_2=new Costs;

    std::thread thread_1( &Heuristic::create_C_Struct, this, 0, nCells/2, cost_1 );
    std::thread thread_2( &Heuristic::create_C_Struct, this, nCells/2, nCells, cost_2 );

    thread_1.join();
    thread_2.join();




}


void Heuristic::generateInitialSolution(){




    srand(time(NULL));

    if(firstStep==false)
        random_shuffle(&rndVect[0], &rndVect[nCells - 1]);


    for(int i=0;i<nCells;i++){

        int demand;
        int currentCell;

        if(firstStep == false){

            demand=problem.activities[rndVect[i]];
            currentCell=rndVect[i];

        }else{

            demand=problem.activities[i];
            currentCell=i;
        }

        if(currentCell < nCells/2)
            cost=cost_1;
        else
            cost=cost_2;


        vector<int> singleMove;//to store single move of the group related to current cell
        MoveGroup groupOfmoves;//to store group of moves related to current cell
        int temp[5];//in order to have a precise positioning of i j m t

        if (demand != 0) { //usefull in order to avoid no sense cycle at the end of the greedy algorithm

            bool notSatisfied = true; //flag which allow us to know when the demand is satisfied

            Costs::const_iterator D; //now we analyze the cost substructure created before

            solveDemand:
            for (D = cost->begin(); D != cost->end() && notSatisfied; D++) {//from this structure

                if (D->second[3] == currentCell) {//select just the rows relative to the current cell
                    //if we are here it is beacuse we have found a move to solve these tasks

                    int NumberOfPeople=problem.usersCell[D->second[2]][D->second[0]][D->second[1]][0]; //number of people in cell i of type m at time t

                    if(NumberOfPeople!=0 && D->second[2]!=D->second[3]){//if there are people in the cell and i!=j

                        temp[0]=D->second[2];//i
                        temp[1]=D->second[3];//j
                        temp[2]=D->second[0];//m
                        temp[3]=D->second[1];//t


                        if(demand > (NumberOfPeople*problem.n[D->second[0]])){ //in this case we need to take each person

                            temp[4]=NumberOfPeople;//number of people to be moved

                            singleMove.insert(singleMove.begin(),temp,temp+5);//we insert this single move

                            groupOfmoves.push_back(singleMove);//in the group related to the current cell

                            singleMove.erase(singleMove.begin(),singleMove.end());//this is necessary in order to avoid a bad insertion

                            problem.usersCell[D->second[2]][D->second[0]][D->second[1]][0]=0;//in this case we need to completely clear the userCell

                            demand -= NumberOfPeople*problem.n[D->second[0]];

                            objFun[0]+=NumberOfPeople*problem.costs[D->second[2]][D->second[3]][D->second[0]][D->second[1]];

                        }
                        else if(demand <= (NumberOfPeople*problem.n[D->second[0]])){

                            int roundUp=floor((double)demand/(double)problem.n[D->second[0]]);

                            temp[4]=roundUp;//Solution

                            singleMove.insert(singleMove.begin(),temp,temp+5);

                            groupOfmoves.push_back(singleMove);

                            singleMove.erase(singleMove.begin(),singleMove.end());

                            problem.usersCell[D->second[2]][D->second[0]][D->second[1]][0]-=roundUp;

                            demand -= roundUp*problem.n[D->second[0]];

                            if(demand == 0){notSatisfied = false;}

                            objFun[0]+=roundUp*problem.costs[D->second[2]][D->second[3]][D->second[0]][D->second[1]];


                        }

                        if(notSatisfied==false){
                            solCurrent.insert(Solution::value_type(currentCell,groupOfmoves));//this is here because map allows to insert the key just one time,so when the group of moves related to it is complete,we add it
                            groupOfmoves.erase(groupOfmoves.begin(),groupOfmoves.end());//this is necessary in order to avoid bad following insertion
                        }

                    }
                }

            }
            if(demand > 0){goto solveDemand;}

        }

    }



}

void Heuristic::generateNeighbors(){

    random_shuffle(&rndVect[0], &rndVect[nCells-1]);
    int DeletedDemand = 0;
    int DemandCell[NumUndoneActivities];

    int currentObj=objFun[0];

    Solution tempNeighSolStructure;


    for(int i=0; i < nCells && DeletedDemand != NumUndoneActivities; i++){
        int iRand = rndVect[i];


        if(problem.activities[iRand] != 0) {
            //remove any move associated to this

            MoveGroup moves;
            moves = solCurrent[iRand];
            MoveGroup::const_iterator m;
            for(m=moves.begin(); m!=moves.end(); m++) { //for each move of extracted group
                vector<int> singleMove;
                singleMove = (*m); //extract the single move
                int j = singleMove[0];
                int m = singleMove[2];
                int t = singleMove[3];
                int Users = singleMove[4];

                problem.usersCell[j][m][t][0] += Users;
                currentObj -= Users * problem.costs[j][iRand][m][t];
            }
            solCurrent.erase(iRand);
            DemandCell[DeletedDemand] = iRand;
            DeletedDemand++;
        }
    }

    for(int k = 1; k <=  N_dim ; k++){
        objFun[k] = currentObj;
    }


    for(int NumGeneratedSol = 0; NumGeneratedSol <  N_dim ; NumGeneratedSol++) {
        globalCounter++;


        random_shuffle(&DemandCell[0], &DemandCell[NumUndoneActivities - 1]);

        bool tryrandom =false;

        if(rand() % 100 < NumUndoneActivities*0.20){tryrandom = true;} ///Only for the 20% of Activities to be redone we can try to take worse moves
        for (int k = NumUndoneActivities; k != 0; k--) {
            MoveGroup groupOfmoves;
            int currentCell = DemandCell[k - 1];
            int demand = problem.activities[currentCell];
            int temp[5];
            int NumberOfPeople = 0;

            bool notSatisfied = true; //flag which allow us to know when the demand is satisfied


            if(currentCell < nCells/2)
                cost=cost_1;
            else
                cost=cost_2;


            Costs::const_iterator D; //now we construct the SubCostsMap used in the algorithm

            solveDemand:
            for (D = cost->begin(); D != cost->end() && notSatisfied; D++) { //from the global costs Multimap
                int UserType = 0;
                if(tryrandom){
                    if (rand() % 100 < 16) { UserType = 2;}///If this move is choosen to be a worsening one, only for the 16% of times it will really be a worsening solution, otherwise solve as usual
                }

                if (D->second[3] == currentCell && D->second[2] != D->second[3] && D->second[0] >= UserType) {//select just the rows relative to the current cell and i != j

                    temp[0] = D->second[2]; //origin cell
                    temp[1] = currentCell;  //destination cell
                    temp[2] = D->second[0]; //type m
                    temp[3] = D->second[1]; //time t

                    if(problem.usersCell[D->second[2]][D->second[0]][D->second[1]][ N_dim +1] < globalCounter){

                        NumberOfPeople = problem.usersCell[D->second[2]][D->second[0]][D->second[1]][0];

                    }else{

                        NumberOfPeople = problem.usersCell[D->second[2]][D->second[0]][D->second[1]][NumGeneratedSol+1];

                    }


                    vector<int> singleMove;

                    if (NumberOfPeople != 0) {//if there are people in the cell

                        if (demand > (NumberOfPeople * problem.n[D->second[0]])) { //in this case we need to take each person


                            temp[4] = NumberOfPeople;//number of people to be moved

                            singleMove.insert(singleMove.begin(), temp, temp + 5);//we insert this single move

                            groupOfmoves.push_back(singleMove);//in the group related to the current cell

                            singleMove.erase(singleMove.begin(), singleMove.end());//this is necessary in order to avoid a bad insertion

                            problem.usersCell[D->second[2]][D->second[0]][D->second[1]][NumGeneratedSol+1] = 0;//in this case we need to completely clear the userCell
                            problem.usersCell[D->second[2]][D->second[0]][D->second[1]][ N_dim +1] = globalCounter;

                            demand -= NumberOfPeople * problem.n[D->second[0]];

                            objFun[NumGeneratedSol+1] += NumberOfPeople * problem.costs[D->second[2]][D->second[3]][D->second[0]][D->second[1]];


                        } else if (demand <= NumberOfPeople * problem.n[D->second[0]]) {

                            int truncation = floor((double) demand / (double) problem.n[D->second[0]]);

                            problem.usersCell[D->second[2]][D->second[0]][D->second[1]][NumGeneratedSol+1] = NumberOfPeople - truncation;

                            problem.usersCell[D->second[2]][D->second[0]][D->second[1]][ N_dim +1] = globalCounter;

                            demand -= truncation * problem.n[D->second[0]];
                            objFun[NumGeneratedSol+1] += truncation * problem.costs[D->second[2]][D->second[3]][D->second[0]][D->second[1]];

                            temp[0] = D->second[2];
                            temp[1] = currentCell;
                            temp[2] = D->second[0];
                            temp[3] = D->second[1];
                            temp[4] = truncation;

                            singleMove.insert(singleMove.begin(), temp, temp + 5);//we insert this single move

                            groupOfmoves.push_back(singleMove);//in the group related to the current cell

                            singleMove.erase(singleMove.begin(), singleMove.end());

                            if (demand <= 0) {
                                notSatisfied = false;
                                tempNeighSolStructure.insert(Solution::value_type(currentCell, groupOfmoves));//this is here because map allows to insert the key just one time,so when the group of moves related to it is complete,we add it
                                groupOfmoves.erase(groupOfmoves.begin(), groupOfmoves.end());
                            }
                        }
                    }


                }

            }

            if(demand > 0){goto solveDemand;}
            tryrandom = true;
            if(rand() % 100 < 50 ){tryrandom = false;}


        }


        NeighborMap.insert(NeighborhoodStructure::value_type(NumGeneratedSol+1,tempNeighSolStructure));
        tempNeighSolStructure.erase(tempNeighSolStructure.begin(), tempNeighSolStructure.end());
    }

}



void Heuristic::Cooling_Schedules(){

    SA_parameters.T0=10*(objFun[0]/2.0);

    SA_parameters.TF=0.0001*(objFun[0]/0.69);

    SA_parameters.alpha=0.5;

    SA_parameters.Plateau=15*N_dim;


}

int Heuristic::NeighborRandomChoice() {
    int min=objFun[1];
    int p_min=1;
    for(int i=2;i<=N_dim;i++){
        if(objFun[i]<min) {
            min = objFun[i];
            p_min = i;
        }
    }
    if(min<=objFun[0])
        return p_min;
    else {

        if (notChoosen == true) {

            notChoosen = false;


            int newChoice;


            newChoice = rand() % (N_dim);



            return newChoice+1;



        }else {
            int newChoice = rand() % (N_dim);

            return newChoice + 1;
        }
    }
}

void Heuristic::StructuresUpdating(){



    objFun[0]=objFun[ChoosenNeighbor];//the current objFun is updated to the choosen one

    Solution tempSol;
    tempSol = NeighborMap.at(ChoosenNeighbor);

    Solution::const_iterator D;
    MoveGroup moveGroup;

    for (D = tempSol.begin(); D != tempSol.end(); D++) {
        moveGroup = D->second;
        solCurrent.insert(Solution::value_type(D->first, moveGroup));//this is here because map allows to insert the key just one time,so when the group of moves related to it is complete,we add it

        MoveGroup::const_iterator m;
        for(m=moveGroup.begin(); m!=moveGroup.end(); m++){ //for each group of moves of the extracted cell
            vector<int> singleMove;

            singleMove=(*m); //extract the single move
            problem.usersCell[singleMove[0]][singleMove[2]][singleMove[3]][0]-=singleMove[4];

            singleMove.erase(singleMove.begin(),singleMove.end());
        }
        moveGroup.erase(moveGroup.begin(), moveGroup.end());
    }



    NeighborMap.erase(NeighborMap.begin(),NeighborMap.end());//deleting the content of NeighborMap



    if(objFun[0]<objFun[N_dim+1]){//in this case it is necessary to update best parameters valutare l uguale

        objFun[N_dim+1]=objFun[0];
        bestSolution.erase(bestSolution.begin(),bestSolution.end());//because new solution could have less moves

        bestSolution=solCurrent;

    }





}

void Heuristic::AcceptancePhase() {

    if(objFun[ChoosenNeighbor]<=objFun[N_dim+1]){//if choosen obj fun is less or equal the best one

        Heuristic::StructuresUpdating();//it is accepted and it is necessary to update parameters and strutucture for the following iterations

    } else{//otherwise the acceptance depends on the probabilty criteria
        double p=0;
        double actual_p=0;
        p= (double)(rand()) / ((double)RAND_MAX + 1.0);
        double y=(objFun[ChoosenNeighbor]-objFun[0])/SA_parameters.T;
        actual_p=exp(-y);
        if(p<=actual_p){//is accepted

            Heuristic::StructuresUpdating();

        }
        else{//it is not accepted
            notChoosen=true;//in order to redo the random choice of a neighbor in the same neighborhood
            iterationCounter++;
        }

    }

}

void Heuristic::ResetStructures(){

    firstStep=false;//it is not first step anymore
    notChoosen=false;

    for(int i=0;i<N_dim+1;i++){//reset to 0 for the following choices each objFUN but the best one
        objFun[i]=0;
    }

    solCurrent.erase(solCurrent.begin(),solCurrent.end());

    NeighborMap.erase(NeighborMap.begin(),NeighborMap.end());//deleting the content of NeighborMap
    for(int j=0;j<=N_dim+1;j++) {
        for (int m = 0; m < nCustomerTypes; m++) {
            for (int t = 0; t < nTimeSteps; t++) {


                for (int i = 0; i < nCells; i++) {


                    problem.usersCell[i][m][t][j] = 0;


                }
            }
        }
    }

    //Initial User Cell generation
    for (int t = 0; t < nTimeSteps; t++) {
        for (int m = 0; m < nCustomerTypes; m++) {

            for (int i = 0; i < nCells; i++) {


                problem.usersCell[i][m][t][0] = problem.usersCellBackup[i][m][t];


            }
        }
    }

}


void Heuristic::Memetic_Algorithm(vector<double> &stat, double timeLimit) {

    clock_t tStart = clock();

    Heuristic::CostsStructureGeneration();


    while(((double)(clock() - tStart) / CLOCKS_PER_SEC)<=timeLimit) {


            if(firstStep==true) { //At the first step we suppose that the first initial solution is the best one
                Heuristic::generateInitialSolution();
                objFun[N_dim + 1] = objFun[0];//best updating
            bestSolution.erase(bestSolution.begin(),bestSolution.end());
            bestSolution = solCurrent;
            Heuristic::Cooling_Schedules();//cooling schedules set up
            SA_parameters.T=SA_parameters.T0;//actual temperature is equal to initial temperature
        }
        else if(firstStep==false){


            Heuristic::generateInitialSolution();
            Heuristic::Cooling_Schedules();//cooling schedules set up
            SA_parameters.T=SA_parameters.T0;//actual temperature is equal to initial temperature
            if( objFun[0]<objFun[N_dim+1] ){

                objFun[N_dim + 1] = objFun[0];//best updating
                bestSolution.erase(bestSolution.begin(),bestSolution.end());
                bestSolution=solCurrent;
            }
        }
        ///Simulated Annealing
        while( SA_parameters.T>SA_parameters.TF && ((double)(clock() - tStart) / CLOCKS_PER_SEC)<=timeLimit) {
            iterationCounter=0;


            while(iterationCounter<SA_parameters.Plateau && ((double)(clock() - tStart) / CLOCKS_PER_SEC)<=timeLimit) {
                if (notChoosen == true)
                    goto newChoice;

                Heuristic::generateNeighbors();
                iterationCounter++;

                newChoice:
                ChoosenNeighbor = Heuristic::NeighborRandomChoice();

                Heuristic::AcceptancePhase();
            }
            SA_parameters.T = SA_parameters.alpha * SA_parameters.T;
        }
        Heuristic::ResetStructures();

    }


    cout<<"\n Best objective funtion:"<<objFun[N_dim+1]<<endl;

    stat.push_back(objFun[N_dim+1]);
    stat.push_back((double)(clock() - tStart) / CLOCKS_PER_SEC);


    hasSolution=true;
}

void Heuristic::writeKPI(string path, string instanceName, vector<double> stat){
    if (!hasSolution)
        return;

    ofstream fileO(path, ios::app);
    if(!fileO.is_open())
        return;

    fileO << instanceName << ";" << stat[1] << ";" << stat[0];
    for(int i=2; i<stat.size(); i++)
        fileO <<  ";" << stat[i];
    fileO << endl;

    fileO.close();

}

void Heuristic::writeSolution(string path) {
    if (!hasSolution)
        return;

    ofstream fileO(path);
    if(!fileO.is_open())
        return;

    fileO << this->nCells << "; " << this->nTimeSteps << "; " << this->nCustomerTypes << endl;

    Solution::const_iterator i;
    for(i=bestSolution.begin();i!=bestSolution.end();i++){//for each cell in solution structure

        int key=i->first;
        MoveGroup moves;

        moves=bestSolution[key]; //extract its group of moves

        MoveGroup::const_iterator m;
        for(m=moves.begin(); m!=moves.end(); m++){ //for each group of moves of the extracted cell
            vector<int> singleMove;

            singleMove=(*m); //extract the single move

                        //i                     //j                     //m                     //t               //solution[i][j][m][t]
            fileO << singleMove[0] << ";" << singleMove[1] << ";" << singleMove[2] << ";" << singleMove[3] << ";" << singleMove[4] << endl;
            singleMove.erase(singleMove.begin(),singleMove.end());
        }

    }

    fileO.close();
}

eFeasibleState Heuristic::isFeasible(string path) {

    string line;
    string word;
    int nCellsN;
    int nTimeStepsN;
    int nCustomerTypesN;
    int i, j, m, t;


    ifstream iffN(path.c_str());

    if (!iffN.is_open()) {
        cout << "Impossible to open" << path << endl;
        exit(1);
    }

    getline(iffN, line);
    std::replace(line.begin(), line.end(), ';', ' ');
    istringstream iss(line);
    iss >> word; // nCells
    nCellsN = atoi(word.c_str());
    iss >> word; // nTimeSteps
    nTimeStepsN = atoi(word.c_str());
    iss >> word; // nCustomerTypes
    nCustomerTypesN = atoi(word.c_str());

    int**** solutionN = new int***[nCells];
    for (i = 0; i < nCellsN; i++) {
        solutionN[i] = new int**[nCells];
        for (j = 0; j < nCellsN; j++) {
            solutionN[i][j] = new int*[nCustomerTypes];
            for (m = 0; m < nCustomerTypesN; m++) {
                solutionN[i][j][m] = new int[nTimeSteps];
                for ( t = 0; t < nTimeStepsN; t++) {
                    solutionN[i][j][m][t] = 0;
                }
            }
        }
    }

    while (getline(iffN, line)) {
        std::replace(line.begin(), line.end(), ';', ' ');
        istringstream iss(line);
        iss >> word; // i
        i = atoi(word.c_str());
        iss >> word; // j
        j = atoi(word.c_str());
        iss >> word; // m
        m = atoi(word.c_str());
        iss >> word; // t
        t = atoi(word.c_str());
        iss >> word; // value
        solutionN[i][j][m][t] = atoi(word.c_str());
    }

    // Demand
    bool feasible = true;
    int expr;
    for (int i = 0; i < nCells; i++) {
        expr = 0;
        for (int j = 0; j < nCells; j++)
            for (int m = 0; m < nCustomerTypes; m++)
                for (int t = 0; t < nTimeSteps; t++)
                    expr += problem.n[m] * solutionN[j][i][m][t];
        if (expr < problem.activities[i])
            feasible = false;
    }

    if (!feasible)
        return NOT_FEASIBLE_DEMAND;

    // Max Number of users
    for (int i = 0; i < nCells; i++)
        for (int m = 0; m < nCustomerTypes; m++)
            for (int t = 0; t < nTimeSteps; t++) {
                expr = 0;
                for (int j = 0; j < nCells; j++)
                    expr += solutionN[i][j][m][t];
                if (expr > problem.usersCell[i][m][t][0])
                    feasible = false;
            }

    if(!feasible)
        return NOT_FEASIBLE_USERS;

    return FEASIBLE;
}

void Heuristic::getStatSolution(vector<double>& stat) {
    if (!hasSolution)
        return;

    int* tipi = new int[nCustomerTypes];
    for (int m = 0; m < nCustomerTypes; m++)
        tipi[m] = 0;

    Solution::const_iterator i;
    for(i=bestSolution.begin();i!=bestSolution.end();i++){//for each cell in solution structure
        int key=i->first;
        MoveGroup moves;

        moves=bestSolution[key]; //extract its group of moves

        MoveGroup::const_iterator m;
        for(m=moves.begin(); m!=moves.end(); m++){ //for each move of extracted group
            vector<int> singleMove;

            singleMove=(*m); //extract the single move

            tipi[singleMove[2]] += singleMove[4];
                //i                     //j                     //m                     //t               //solution[i][j][m][t]
            //singleMove[0]         singleMove[1]           singleMove[2]            singleMove[3]             singleMove[4]
            singleMove.erase(singleMove.begin(),singleMove.end());
        }

    }

    for (int m = 0; m < nCustomerTypes; m++)
        stat.push_back(tipi[m]);

}
