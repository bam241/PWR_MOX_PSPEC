#include "ZAI.hxx"
#include <TGraph.h>
#include <TGraph.h>

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sstream>
#include <cmath>
#include <map>
#include <iostream>
#include <iomanip>

using namespace std;

string dtoa(double num)
{
	ostringstream os(ostringstream::out);
	os<<setprecision(3)<<num;
	return os.str();
}

string FilePath;
string DataPath;

vector<string> JobName;
vector<string> GoodJobName;

vector< vector<double> > fTime;				//Time vector of each depletion calculation (second)
vector<ZAI> fAllNuclei;						//All the nuclei present in the fuel

vector< vector<double> > fpu9;				//Keff evolution of all Evolution Datas


vector<IsotopicVector> fActinideCompoInit;	//Fresh fuel composition in atomic prop.

vector<double> fHMMass; //Vector of initial Heavy metal mass (every element should be egual or very very close !!)

vector<int> fNOfTimeStep; //number of time step in each Evolution data

string fEvolutionDataFolder = "";

map<ZAI,string> fMapName; // List of ZAI and their name to consider for model parametrization (must of the time Fuel composition @ t=0)

string fReactorType,fFuelType;

vector <ZAIStreamList> fStreamList; // Fisisle or Fertile and default proportions for all nuclei in fresh fuel

bool fIsAllNucleiAlreadyFill=false;			

void	CheckJob();
void	ReadAndFill(string jobname);
void    DumpForTestingNeuron(string filename);
void    DumpInputNeuron(string filename);
void 	FillMapName();
bool	UserSayYes();
string 	CreateInfoFile();
void 	ReadInfo(string InfoDBFile,string &ReactorType,string &FuelType,double &Power);
void 	ProgressBar(double loopindex, double totalindex);
vector<double> GetAllCompoOf(ZAI zai);

bool is_file_exist(const char *fileName);
void CompileTraining();
void FissileOrFertileList(int Z,int A,int I);
/* PRINT INFO */
void Generate_tmva_factory_input(double ProportionOfTraining);

void GetUserValue(int &Value);
void GetUserValue(double &Value);
void GetUserValue(string &Value);


