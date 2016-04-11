#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <math.h>
using namespace std;

int noOfCells;
int noOfConnections;
vector< vector<int> > connectivityMatrix;

void formulateConnectivityMatrix(ifstream &benchMarkFile);
int randomlyChosenIndexFromVector(vector<int> *currentPartition);
int calculateGainAfterSwappingElements(int elementIndexA,int elementIndexB,vector<int> *initialCellVector);
void initialRandomSolution(int cellCount,vector<int> *initialCellVector);
bool acceptMove(int gain, int temperature);
int runSimulatedAnnealingAlgorithm(vector<int> *initialCellVector);
int getCutSetSize(vector<int> *initialCellVector);
void printConnectivityMatrix();
void printPartitions(vector<int> *initialCellVector);

int main(int argumentCount,char *fileName[]){
    vector<int> initialCellVector;
    vector<int> partition_A;
    vector<int> partition_B;
    int cutSet = 0;
    char inputFileName[5];
    char outputFileName[5];
    //cout<<"Please enter the input file name:"<<endl;
    //cin>>inputFileName;
    ifstream benchMarkFile(fileName[1]);
    //cout<<"Please enter output file name:"<<endl;
    //cin>>outputFileName;
    ofstream outputFile(fileName[2]);
	unsigned seed = time(0);
	/*if(fileName[0]=="B1"){
		seed = time(0);
	}elseif(fileName[0]=="B2"){
		seed = time(0);
	}elseif(fileName[0]=="B3"){
		seed = time(0);
	}elseifif(fileName[0]=="B4"){
		seed = time(0);
	}elseif(fileName[0]=="B5"){
		seed = time(0);
	}elseif(fileName[0]=="B6"){
		seed = time(0);
	}*/
	clock_t initialTime, finalTime;
    initialTime = clock();
    srand(seed);
	outputFile<<"seed: "<<seed<<endl;
    if(benchMarkFile.is_open()){
        formulateConnectivityMatrix(benchMarkFile);
        //printConnectivityMatrix();
        initialRandomSolution(noOfCells,&initialCellVector);
        cutSet = runSimulatedAnnealingAlgorithm(&initialCellVector);
	outputFile<<"cutsetSize: "<<cutSet<<endl;
        outputFile<<"final Partitions:"<<endl;
        //printPartitions(&initialCellVector);
        int initialCellVectorSize = initialCellVector.size();
        outputFile<<"partition A:"<<endl;
        for(int i=0; i<initialCellVectorSize;i++){
            if(initialCellVector.at(i)==0){
                outputFile<<i+1<<" ";
            }
        }
        outputFile<<endl<<"partition B:"<<endl;
        for(int i=0; i<initialCellVectorSize;i++){
            if(initialCellVector.at(i)==1){
                outputFile<<i+1<<" ";
            }
        }
        outputFile<<endl;
        finalTime = clock();
        outputFile<<"Execution Time: "<<(double)(finalTime-initialTime)/CLOCKS_PER_SEC<<" seconds." << endl;
    }else{
        cout<<"File could not be opened.\n";
    }
  return 0;
}

void formulateConnectivityMatrix(ifstream &benchMarkFile){
    int node1,node2;
    benchMarkFile>>noOfCells;
    benchMarkFile>>noOfConnections;
    //cout<<"noOfCells: "<<noOfCells<<" connections: "<<noOfConnections<<"\n";
    for(int i=0;i<noOfCells;i++){
        connectivityMatrix.push_back(vector<int>());
    }
    while(benchMarkFile >> node1 >> node2){
        connectivityMatrix[node1-1].push_back(node2-1);
        connectivityMatrix[node2-1].push_back(node1-1);
    }
}
void printConnectivityMatrix(){
    cout<<"connectivity Matrix:"<<endl;
    int rowSize = connectivityMatrix.size();
    int columnSize = 0;
    for(int i=0;i<rowSize;i++){
        columnSize = connectivityMatrix[i].size();
        cout<<i<<": ";
        for(int j=0;j<columnSize;j++){
            cout<<connectivityMatrix[i][j]<<" ";
            cout<<",";
        }
        cout<<endl;
    }
}

void initialRandomSolution(int cellCount,vector<int> *initialCellVector){
    for(int i=1;i<=cellCount/2;i++){
		initialCellVector->push_back(0);
		initialCellVector->push_back(1);
	}
	random_shuffle(initialCellVector->begin(), initialCellVector->end());
}

void printPartitions(vector<int> *initialCellVector){
    int initialCellVectorSize = initialCellVector->size();
    cout<<"partition A:"<<endl;
    for(int i=0; i<initialCellVectorSize;i++){
        if(initialCellVector->at(i)==0){
            cout<<i+1<<" ";
        }
    }
    cout<<endl<<"partition B:"<<endl;
    for(int i=0; i<initialCellVectorSize;i++){
        if(initialCellVector->at(i)==1){
            cout<<i+1<<" ";
        }
    }
    cout<<endl;
}

int runSimulatedAnnealingAlgorithm(vector<int> *initialCellVector){
    int initialTemperature = 400000;
    int lowestTemperature = 10;
    int temperature;
    int gain;
    int elementIndexA =0;
    int elementIndexB =0;
    int tempForSwap;
    int noOfStepsPerIteration = 400000;
    int cutSet =0;
    bool accept = false;
    temperature = initialTemperature;
    cutSet = getCutSetSize(initialCellVector);
    //cout<<endl<<"initial cutsetSize: "<<cutSet<<endl;
    while(temperature >=lowestTemperature){
        for(int i=0;i<noOfStepsPerIteration;i++){
            elementIndexA = randomlyChosenIndexFromVector(initialCellVector);
            elementIndexB = randomlyChosenIndexFromVector(initialCellVector);
            while(initialCellVector->at(elementIndexA) == initialCellVector->at(elementIndexB)){
                elementIndexB = randomlyChosenIndexFromVector(initialCellVector);
            }
            gain = calculateGainAfterSwappingElements(elementIndexA,elementIndexB,initialCellVector);
            accept = acceptMove(gain,temperature);
            if(accept){
                tempForSwap = initialCellVector->at(elementIndexA);
                initialCellVector->at(elementIndexA) = initialCellVector->at(elementIndexB);
                initialCellVector->at(elementIndexB) = tempForSwap;
                cutSet = cutSet-gain;
            }
        }
        temperature = 0.99*temperature;
        //cout<<"temperature:"<<temperature<<" cutset:"<<cutSet<<endl;
	//cout<<temperature<<","<<cutSet<<endl;
        //noOfStepsPerIteration = noOfStepsPerIteration+10;
    }
    return cutSet;
}

int calculateGainAfterSwappingElements(int elementIndexA,int elementIndexB,vector<int> *initialCellVector){
    int Ea = 0;
    int Eb = 0;
    int Ia = 0;
    int Ib = 0;
    int Cab = 0;
    int Da = 0;
    int Db = 0;
    int Gab =0;
    int elementIndexARowSize = 0;
    int elementIndexBRowSize = 0;
    elementIndexARowSize = connectivityMatrix[elementIndexA].size();
    elementIndexBRowSize = connectivityMatrix[elementIndexB].size();
    for(int i=0;i<elementIndexARowSize;i++){
        if(initialCellVector->at(elementIndexA)==initialCellVector->at(connectivityMatrix[elementIndexA][i])){
            Ia += 1;
        }else{
            Ea += 1;
        }
    }
    for(int i=0;i<elementIndexBRowSize;i++){
        if(initialCellVector->at(elementIndexB)==initialCellVector->at(connectivityMatrix[elementIndexB][i])){
            Ib += 1;
        }else{
            Eb += 1;
        }
    }
    if(elementIndexARowSize<elementIndexBRowSize){
        for(int i=0;i<elementIndexARowSize;i++){
            if(connectivityMatrix[elementIndexA][i] == elementIndexB){
                Cab += 1;
            }
        }
    }else{
        for(int i=0;i<elementIndexBRowSize;i++){
            if(connectivityMatrix[elementIndexB][i] == elementIndexA){
                Cab += 1;
            }
        }
    }
    //cout<<"a:"<<elementIndexA<<" b:"<<elementIndexB<<"Ea:"<<Ea<<" Ia:"<<Ia<<" Eb:"<<Eb<<" Ib:"<<Ib<<" Cab:"<<Cab<<endl;
    Da = Ea-Ia;
    Db = Eb-Ib;
    Gab = Da+Db-2*Cab;
    return Gab;
}


int randomlyChosenIndexFromVector(vector<int> *currentPartition){
	int randomElementIndex = ((double)rand()*(currentPartition->size()-1))/(double)RAND_MAX;
	return randomElementIndex;
}

int getCutSetSize(vector<int> *initialCellVector){
    int cutSetSize = 0;
    int elementOfPartition_A;
    int elementOfPartition_B;
    int initialCellVectorSize = initialCellVector->size();
    int rowSize = 0;
    vector<int> tempRowVector;
    for(int i=0;i<initialCellVectorSize;i++){
        elementOfPartition_A = initialCellVector->at(i);
        tempRowVector = connectivityMatrix.at(i);
        rowSize = tempRowVector.size();
        for(int j=0;j<rowSize;j++){
            elementOfPartition_B = initialCellVector->at(tempRowVector.at(j));
            if(elementOfPartition_A!=elementOfPartition_B){
                cutSetSize = cutSetSize+1;
            }
        }
    }
    return cutSetSize/2;
}

bool acceptMove(int gain, int temperature){
	double boltz;
	double random;
	double exponent;
	double kT;
	const double k = 1.380658e-23;
	if(gain<0){
		kT = k*temperature;
		exponent = gain/kT;
		boltz = exp(exponent);
		random = ((double)rand())/(double)RAND_MAX;
		if(random<boltz)
            return true;
	 	return false;
	}
	return true;
}
