/**********************************************************/
//	Make the input file for the MLPs training
//
//	This programs reads a set of .dat files which are the
//	results of a depletion calculation (see manual and 
//  looks for XS_CLOSEST). From the reading it fills a 
//  TTree (Data) and write it in a file named
//	TrainingInput.root . 
// OUTPUTS :
// 1- File TrainingInput.cxx is the list of MLP outputs 
//    (cross sections)
// 2- File TrainingInput.root input datas (as a Root TTREE) for MLP training 
// 3- File Data_Base_Info.nfo Information file to be red by XSM_MLP 
//    (to be place in the weight folder which is build by MLP training)
//
//@author BaM, BaL
/**********************************************************/
#include "include/Generate_XSM.hxx"
#include <TH1F.h>
#include <TH2D.h>
#include <TFile.h>
#include <TTree.h>
#include "../../CLASS/source/external/StringLine.hxx"
#include <TString.h>
#include <string>
#include <cmath>
#include <math.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <sstream>
#include <numeric>
#include <functional>
#include <algorithm>    
#include <ctime>

using namespace std;

ZAIMass cZAIMass; //atomic masses

string ElNames[110]={"  ","H","He","Li","Be",
						"B","C","N","O","F","Ne","Na","Mg","Al","Si","P","S","Cl","Ar",
						"K","Ca","Sc","Ti","V ","Cr","Mn","Fe","Co","Ni","Cu","Zn","Ga","Ge",
						"As","Se","Br","Kr","Rb","Sr","Y","Zr","Nb","Mo","Tc","Ru","Rh","Pd",
						"Ag","Cd","In","Sn","Sb","Te","I","Xe","Cs","Ba","La","Ce","Pr","Nd",
						"Pm","Sm","Eu","Gd","Tb","Dy","Ho","Er","Tm","Yb","Lu","Hf","Ta","W",
						"Re","Os","Ir","Pt","Au","Hg","Tl","Pb","Bi","Po","At","Rn","Fr","Ra",
						"Ac","Th","Pa","U","Np","Pu","Am","Cm","Bk","Cf","Es","Fm","Md","No",
						"Lr","Rf","Db","Sg","Bh","Hs","Mt"};

//--------------------------------------------------------------------------------------------------
/*************************
 		MAIN
*************************/
int main(int argc, char ** argv){
	
	if(argc!=2)
	{
		cout << "Usage : Generate_XSM Path" << endl;
		cout << " Where Path is the path to the folder containing Evolution Datas" << endl;
		cout << " i.e the (.dat) files" << endl;
		exit(0);
	}	

	/*****************Preparation of working folders*************************/
	if(is_file_exist("Training_output_0.root" ))
	{
		cout<< "Trainining_output* detected. Delete these files and weights folder ? [y/n]"<<endl;
		if(UserSayYes())
			system( "rm -rf Training_output* weights"  );
		else
		{	cout << "Move this files elsewhere and run this program again"<<endl;
			exit(0);
		}
	}

	if(is_file_exist("_tmp/include_Train_XS/TrainingInput.cxx" ))
		system( "rm -rf _tmp"  );

	system("mkdir -p _tmp/scripts/subatech");
	system("mkdir -p _tmp/include_Train_XS");
	/**********************************************************************/

	fEvolutionDataFolder = argv[1];
	CheckJob();	// looks fot the .dat files in the fEvolutionDataFolder

	cout << endl;
	cout << "╭───────────────────────────────────────────────╮" << endl; 
	cout << "│      Load your EvolutionDatas to R.A.M        │" << endl; 
	cout << "╰───────────────────────────────────────────────╯" << endl; 
	cout<<endl;
	for(int i = 0; i < (int)JobName.size(); i++)
	{
		ReadAndFill(JobName[i]);
		ProgressBar(i,JobName.size());
	}
	ProgressBar(1,1);

	FillMapName();

	cout << "Proportion of data to be used for training ? [0-100]" <<endl;
	double ProportionOfTraining = 0 ;
	cin >> ProportionOfTraining ;
		DumpInputNeuron("_tmp/include_Train_XS/TrainingInput.root");
		Generate_tmva_factory_input(ProportionOfTraining);

	CreateInfoFile();

	cout << "╭─────────────────────────────────────────────────────────────────────╮" << endl; 
	cout << "│                            GENERATED FILES:                         │" << endl; 
	cout << "├─────────────────────────────────────────────────────────────────────┤" << endl; 
	cout << "│#1 Input for TMVA training: \033[36m_tmp/include_Train_XS/TrainingInput.root\033[0m │" << endl; 
	cout << "│#2 Target names for TMVA:   \033[36m_tmp/include_Train_XS/TrainingInput.cxx\033[0m  │" << endl; 
	cout << "│#3 Model Information for CLASS: \033[36mData_Base_Info.nfo\033[0m                   │" << endl; 
	cout << "╰─────────────────────────────────────────────────────────────────────╯" << endl; 
	cout << endl;
	cout << "╭────────────────────────────────────────────────╮" << endl; 
	cout << "│                  NEXT STEPS:                   │" << endl; 
	cout << "├────────────────────────────────────────────────┤" << endl; 
	cout << "│1. Train your MLPs with \033[36mTrain_XS.cxx\033[0m            │" << endl; 
	cout << "│2. Test MLPs performances using informations in:│" << endl; 
	cout << "│	     \033[36m../Test/EvaluateTrainingCommands.dat\033[0m│" << endl; 
	cout << "│3. Put the file #3 in \033[36mweights\033[0m folder then       │" << endl; 
    cout << "│ move this folder to $CLASS_PATH/DATA_BASES     │" << endl; 
	cout << "╰────────────────────────────────────────────────╯" << endl; 
	cout << endl; 
	cout << "=> Doing this steps for you. Do you want to train the MLPs on your local machine on one cpu ? It can take a while.  [y/n]" << endl; 

	if(UserSayYes())
	{
		GenerateScript_Sequential(0,fReactionCounter,"_tmp/scripts/Run_Sequential.sh");
		CompileTraining();
		cout << "Let's train locally ? [y/n]" <<endl;
		if(UserSayYes())
			Run_Sequential();
		else
		{
			cout << "You can run _tmp/scripts/Run_Sequential.sh later. Then read the following : "<<endl;
			PrintFinalSteps();
			exit(0);
		}
	}
	else
	{
		cout << "Ok so you want to run on a grid ?  [y/n]" << endl;
		if(UserSayYes())
		{

			int Threads = 4;
			cout << "On how many threads you want to run training ?" <<endl;
			cin >> Threads;
			GenerateScript_Parallel(Threads);
			cout << "Are you a Researcher from Subatech ? [y/n] " << endl;
			if(UserSayYes())
			{
				GenrateZubaScript(Threads);
				PrintFinalSteps();
			}
			else 
			{
				cout << "Nobody is perfect ... RUN THE SCRIPTS located in folder \"_tmp/scripts\" using your job submission program (e.g qsub)" << endl;
				cout << "Once training are finished follow the following steps :" << endl;
				PrintFinalSteps();
			}	


		}
		else
			cout <<" Ok , so follow advices in \"NEXT STEPS:\" " <<endl;
	
	}



}
//--------------------------------------------------------------------------------------------------
void GenrateZubaScript(int threads)
{
	cout << "╭────────────────────────────────────────────────╮" << endl; 
	cout << "│   GENERATE QSUB SCRIPT FOR SUBATECH NANSL      │" << endl; 
	cout << "╰────────────────────────────────────────────────╯" << endl; 

	string filepath = "_tmp/scripts/subatech/RunOnSubaGrid.sh";

	if(is_file_exist(filepath.c_str()))
		system( ("rm " + filepath).c_str()   );

	ofstream Script(filepath.c_str());
	Script << "#!/bin/bash"                                                                                                      << endl;
	Script << "cd .."                                                                                                            << endl;
	Script << "max_job=$((40))"                                                                                                  << endl;
	Script << "user=$(whoami)"                                                                                                   << endl;
	Script << "job=$((0))"                                                                                                       << endl;
	Script << "launchedjob=$((0))"                                                                                               << endl;
	Script << "currentJob=$((0))"                                                                                                << endl;
	Script << " for ((file=$((0)); file<$(("<< threads  <<")); file=file+1 )) ;"                                                 << endl;
	Script << " do "                                                                                                             << endl;
	Script << " 		Job_Launched=$((0))"                                                                                     << endl;
	Script << " 		while [ $Job_Launched -lt 1 ]"                                                                           << endl;
	Script << " 		do"                                                                                                      << endl;
	Script << "			launchedjob=$(( `qstat -u ${user} | grep \" R \" |grep \"XSM_MLP_Training_\"  | wc -l `))"               << endl;
	Script << " 			jobinactive=$(( `qstat | grep ${user} | grep  \" Q \" | wc -l `))"                                   << endl;
	Script << "			if [ $jobinactive -lt 5 ] ; then"                                                                        << endl;
	Script << "			   	../../include/QSUB -n \"XSM_MLP_Training_${currentJob}\" -c \"./Run_Parallel_${currentJob}.sh\""  << endl;
	Script << "				Job_Launched=$(( $Job_Launched +1 ))"                                                                << endl;
	Script << "				currentJob=$(( $currentJob + 1 ))"                                                                   << endl;
	Script << "				sleep 5s"                                                                                            << endl;
	Script << "			else"                                                                                                    << endl;
	Script << "				if [ $launchedjob -lt $max_job ]; then"                                                              << endl;
	Script << "					sleep 1m"                                                                                        << endl;
	Script << "				else"                                                                                                << endl;
	Script << "					sleep 5m"                                                                                        << endl;
	Script << "				fi"                                                                                                  << endl;
	Script << "			fi"	                                                                                                     << endl;
	Script << "		done"                                                                                                        << endl;
	Script << "done"                                                                                                             << endl;
	Script << "cd -"                                                                                                             << endl;
	Script.close();

	string CMD = "chmod u+x " + filepath ;
	cout << CMD <<endl;
	system(CMD.c_str());

	cout << "╭──────────────────────────────────────────────────────────────────╮" << endl; 
	cout << "│                        RUN ON NANSL GRID                         │" << endl; 
	cout << "│1.Syncrhonise your Utils folder and ssh to nansl3                 │" << endl; 
	cout << "│2. type : cd _tmp/scripts/subatech/                               │" << endl; 
	cout << "│3. then  nohup ./RunOnSubaGrid.sh > LogFile.log &                 │" << endl; 
	cout << "│3. wait                                                           │" << endl; 
	cout << "╰──────────────────────────────────────────────────────────────────╯" << endl; 



}

//--------------------------------------------------------------------------------------------------
void PrintFinalSteps()
{
	cout << "╭───────────────────────────────────────────────────────╮" << endl; 
	cout << "│                  NEXT STEPS:                          │" << endl; 
	cout << "├───────────────────────────────────────────────────────┤" << endl; 
	cout << "│1.(optional) Test MLPs performances using              │" << endl;
	cout << "│ informations in:                                      │" << endl; 
	cout << "│       \033[36m../Test/EvaluateTrainingCommands.dat\033[0m            │" << endl;
	cout << "│2. Put the file \033[36mData_Base_Info.nfo\033[0m in \033[36mweights\033[0m then     │" << endl;
    cout << "│ mkdir -p $CLASS_PATH/DATA_BASES/"<<fReactorType<<"/"<<fFuelType << "/XSModel/ChooseAName   │" << endl; 
    cout << "│ mv  weights $CLASS_PATH/DATA_BASES/"<<fReactorType<<"/"<<fFuelType << "/XSModel/ChooseAName│" << endl; 
	cout << "╰───────────────────────────────────────────────────────╯" << endl; 
}

//--------------------------------------------------------------------------------------------------
void Run_Sequential()
{
	cout << "╭────────────────────────────────────────────────╮" << endl; 
	cout << "│  RUNNING TRAINING ON ONE CPU (Please wait)     │" << endl; 
	cout << "╰────────────────────────────────────────────────╯" << endl; 

	system("cd _tmp/scripts/ ; ./Run_Sequential.sh ; cd -");

	cout << "╭────────────────────────────────────────────────╮" << endl; 
	cout << "│              TRAINING FINISHED                 │" << endl; 
	cout << "╰────────────────────────────────────────────────╯" << endl; 


}
//--------------------------------------------------------------------------------------------------
bool is_file_exist(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}
//--------------------------------------------------------------------------------------------------
void CompileTraining()
{
	cout << "╭────────────────────────────────────────────────╮" << endl; 
	cout << "│     COMPLILING TMVA TRAINING PROGRAM           │" << endl; 
	cout << "╰────────────────────────────────────────────────╯" << endl; 
	if(is_file_exist("Train_XS"))
		system("rm Train_XS ");

	string CMD = "g++ -o Train_XS  `root-config --cflags` Train_XS.cxx `root-config --glibs` -lTMVA";
	cout << CMD <<endl;
	system(CMD.c_str());

	if(!is_file_exist("Train_XS"))
	{
		cout <<"\033[31m  COMPILATION FAILED !!! May be not the good compilator name nor the good path of file to compile ? \033[0m " << endl;
		exit(1);
	}
	else
		cout <<"\t \033[32m Done \033[0m " << endl;
	

}

//--------------------------------------------------------------------------------------------------
void GenerateScript_Parallel(int threads)
{
	cout << "╭────────────────────────────────────────────────╮" << endl; 
	cout << "│  GENERATE BASH SCRIPTS FOR PARALLEL TRAINING   │" << endl; 
	cout << "╰────────────────────────────────────────────────╯" << endl; 

	double JobPerProcessor = double (fReactionCounter) / double (threads);
	int    JobPerProcessor_IntegerPart = floor(JobPerProcessor);
	double Rest = (JobPerProcessor - JobPerProcessor_IntegerPart) * threads;

	for (int proc = 0 ; proc < threads -1 ; proc++)
	{
		stringstream ssScriptName ;
		ssScriptName<< "_tmp/scripts/Run_Parallel_" <<proc <<".sh";
		GenerateScript_Sequential( proc * JobPerProcessor_IntegerPart , (proc + 1) * JobPerProcessor_IntegerPart - 1 ,ssScriptName.str().c_str(),false);
	}

	stringstream ssScriptName ;
	ssScriptName<< "_tmp/scripts/Run_Parallel_" << threads-1 <<".sh";
	if (Rest > 0)
		GenerateScript_Sequential( (threads-1) * JobPerProcessor_IntegerPart , threads * JobPerProcessor_IntegerPart + Rest , ssScriptName.str().c_str(),false);

	else
		GenerateScript_Sequential( (threads-1) * JobPerProcessor_IntegerPart , threads * JobPerProcessor_IntegerPart + Rest , ssScriptName.str().c_str(),false);

	cout << "\t \033[32m => Scripts buit in _tmp/scripts \033[0m " << endl; 

}

//--------------------------------------------------------------------------------------------------
void GenerateScript_Sequential(int begin, int end, string filepath ,bool print)
{

	if(print)
	{
		cout << "╭────────────────────────────────────────────────╮" << endl; 
		cout << "│ GENERATE BASH SCRIPT FOR SEQUENTIAL TRAINING   │" << endl; 
		cout << "╰────────────────────────────────────────────────╯" << endl; 
	}

	ofstream Script(filepath.c_str());
	Script << "#!/bin/bash" << endl;
	Script <<endl;
	Script << "# Script to train MLPs" << endl;
	Script << "#@author BaL" << endl;
	Script << "#" << endl;
	Script <<  endl;
	Script << "cd ../.."<<endl;
	Script << "echo \"----------------------------------\"" << endl;
	Script << "echo \"--- Run Training from MLP " << begin<< " to " << end << "---\"" << endl;
	Script << "echo \"----------------------------------\"" << endl;
	Script << endl;
	Script << "Start=$(("<<  begin  << "))" << endl;
	Script << "End=$(("  << end+1 << "))" << endl;
	Script << endl;
	Script << "for ((reaction=$Start; reaction<$End; reaction=reaction+1 )) ;"<< endl;
	Script << "do"<<endl;
	Script << "     ./Train_XS $reaction"<< endl;
	Script << "done" << endl;
	Script << "cd -"<<endl;

	Script.close();

	stringstream ssCMD ;
	ssCMD << "chmod u+x "<< filepath  <<endl;
	cout << ssCMD.str() << endl;

	system(ssCMD.str().c_str());

}

//--------------------------------------------------------------------------------------------------
//Convert int to string
string itoa(int num)
{
	ostringstream os(ostringstream::out);
	os << num;
	return os.str();
}
//--------------------------------------------------------------------------------------------------
//Give a name to a cross section 
//!!!!!!!!!!!!!!!!!!!!!!!!!!//
// YOU MUST KEEP THE FORMAT :
// XS_Z_A_I_fis	(fission cross section)
// XS_Z_A_I_cap (n,gamma cross section)
// XS_Z_A_I_n2n	(n,2n cross section)
//!!!!!!!!!!!!!!!!!!!!!!!!!!//
string NameXS(ZAI act,string xs)
{
	stringstream Name;
	Name<<"XS_"<<act.Z<<"_"<<act.A<<"_"<<act.I<<"_"<<xs;

	return Name.str();
}

//--------------------------------------------------------------------------------------------------
void  FillMapName()
{
	cout<<endl;
	cout<< "╭───────────────────────────────────────────────╮"<<endl;
	cout<< "│               SET UP MLPs INPUT               │"<<endl;
	cout<< "│default : composition @ t=0 + Irradiation time │"<<endl;
	cout<< "╰───────────────────────────────────────────────╯"<<endl;
	cout<<endl;

	vector<ZAI> ZAI_T0 =  fActinideCompoInit[0].GetNonZeroZAIList();

	for(int zai = 0 ; zai < ZAI_T0.size() ; zai++ )
	{	stringstream ssname;
		int Z = ZAI_T0[zai].Z;
		int A = ZAI_T0[zai].A;
		int I = ZAI_T0[zai].I;
		string sI="";
		if(I == 1)
			sI = "m";
		else if(I == 2) 
			sI = "2m";
		else if(I == 3) 
			sI = "3m";

		ssname<<A<<ElNames[Z]<<sI;
		if(zai == 0)
			cout<< "Add this nuclei with this name [y/n] ?  (if you don't know type yes to all)  "<<endl;
		else
			cout<< "Add [y/n] ?  "<<endl;

		cout<< "[Z\tA\tI\tName]"<<endl;
		cout<<"\033[36m"<<Z<<"\t"<<A<<"\t"<<I<<"\t"<<ssname.str()<<"\033[0m"<<endl;

		if (UserSayYes())
			fMapName.insert(pair<ZAI,string> ( ZAI(Z,A,I) , ssname.str()) );

	}

	bool UserWantToAdd = true;
	while(UserWantToAdd)
	{
		cout<< "Do you want to add additional nuclei  [y/n] ? (if you don't know type no) "<<endl;
		if(UserSayYes())
		{	
			int Z = 0;
			int A = 0;
			int I = 0;
			string Name;
			cout << " Z -> "; cin >> Z ; cout <<" A -> "; cin >> A ; cout <<" I -> "; cin >> I ;cout <<" Name -> "; cin >> Name;
			fMapName.insert(pair<ZAI,string> ( ZAI(Z,A,I) ,Name) );
		}

		else
			UserWantToAdd = false;
	}	


}
//--------------------------------------------------------------------------------------------------
void Generate_tmva_factory_input(double ProportionOfTraining)
{

	ofstream  InputNetwork("_tmp/include_Train_XS/InputVariables.cxx");
	for(map<ZAI,string>::iterator it = fMapName.begin() ; it != fMapName.end() ; it++ )
		InputNetwork <<"factory->AddVariable( \"" << it->second  << "\" , \"" << it->second << "\", \"IsotopicFraction\", 'F' );"<<endl; 
    InputNetwork <<"factory->AddVariable( \"Time\" , \"Time\"     , \"seconds\", 'F' );"<<endl<<endl;

	ProportionOfTraining /=100;
    InputNetwork <<"double PropTraining = "<< ProportionOfTraining << ";" << endl;

    InputNetwork.close();
}

//--------------------------------------------------------------------------------------------------
bool UserSayYes()
{
		bool AnswerIsNotGiven = true;
		bool isYES = false;
		while(AnswerIsNotGiven)
		{
			string answer;
			std::getline(std::cin, answer);

	
			if(answer == "y" || answer == "yes" || answer == "Yes" || answer == "Y")
			{
				isYES = true;
				AnswerIsNotGiven = false;
			}
			else if (answer == "n" || answer == "no" || answer == "No" || answer == "N")
			{
				isYES = false;
				AnswerIsNotGiven = false;
			}

			else{
				cout << "Yes OR No ? "<<endl;
			}
		}
	return isYES;	
}

//--------------------------------------------------------------------------------------------------
void CreateInfoFile()
{

	/**************************************************/
	// GETTING USER INFO
	/**************************************************/
	double sum = 0 ;
	for (int i=0 ; i < fHMMass.size() ; i++)
			sum+=fHMMass[i];

    double MeanHMMass = sum / (double)fHMMass.size();
    string Author,Mail,XSBase,HLCut,EnergyDisc,FPYBase,SABase,Geom,AddInfo,DepCode ;
    double Power = 0;


	int start=0;
	string AnInfoFile = JobName[0];
	int pos=AnInfoFile.find(".dat",start);
	AnInfoFile.replace(pos,4,".info");

    ReadInfo(AnInfoFile,fReactorType,fFuelType,Power);

	cout<<endl;
	cout<<endl;
	cout<<"╭───────────────────────────────────────────────╮"<<endl;
	cout<<"│        XSM_MLP .NFO FILE GENERATOR            │"<<endl;
	cout<<"╰───────────────────────────────────────────────╯"<<endl;
	cout<<" Answer following questions "<<endl;
	cout<<"-> Author(s) name(s) : "<<endl;
	std::getline(std::cin, Author);
	cout<<endl;

	cout<<"-> email adress(es) : "<<endl;
	std::getline(std::cin, Mail);	
	cout<<endl;

	cout<<"-> Depletion code used : "<<endl;
	std::getline(std::cin, DepCode);
	cout<<endl;

	cout<<"-> Cross section data base (e.g ENSDF7.1) : "<<endl;
	std::getline(std::cin, XSBase);
	cout<<endl;

	cout<<"-> Fission yield data base (e.g ENSDF7.1) : "<<endl;
	std::getline(std::cin, FPYBase);
	cout<<endl;

	cout<<"-> S(alpha,beta) data base (e.g ENSDF7.1) : "<<endl;
	std::getline(std::cin, SABase);
	cout<<endl;

	cout<<"-> Geometry simulated  (e.g Cubic Assembly with mirror boundary) : "<<endl;
	std::getline(std::cin, Geom);
	cout<<endl;

	cout<<"-> Half life cut [s] (if any) : "<<endl;
	std::getline(std::cin, HLCut);	
	cout<<endl;

	cout<<"-> Multi group treatment (yes/no if yes give the number of groups) : "<<endl;
	std::getline(std::cin, EnergyDisc);
	cout<<endl;	

	cout<<"-> Additional informations : "<<endl;
	std::getline(std::cin, AddInfo);	
	cout<<endl;	


	cout<<"-> Reactor type (e.g PWR, FBR,...) : "<<endl;
	cout<<"Found in a .info file :"<<endl;
	cout<<"\033[36m"<<fReactorType<<"\033[0m"<<endl;
	cout<< "Is that corect ? [y/n] "<<endl;
	if(!UserSayYes())
	{	cout<<"\t So what it is ?"<<endl;
		cin>> fReactorType;
	}	
	cout<<endl;

	cout<<"-> Fuel type (e.g UOX, MOX,...) : "<<endl;
	cout<<"Found in a .info file :"<<endl;
	cout<<"\033[36m"<<fFuelType<<"\033[0m"<<endl;
	cout<< "Is that corect ? [y/n] "<<endl;
	if(!UserSayYes())
	{	cout<<"\t So what it is ?"<<endl;
		cin>> fFuelType;
	}	
	cout<<endl;

	cout<<"-> Simulated heavy metal mass (tons) : "<<endl;
	cout<< "\t According your evolution datas the AVERAGE heavy metal mass is : "<<endl;
	cout<<"\033[36m"<<MeanHMMass<<"\033[0m"<<" tons"<<endl;
	cout<< "Is that corect ? [y/n] "<<endl;
	if(!UserSayYes())
	{	cout<<"\t So what it is ?"<<endl;
		cin>> MeanHMMass;
	}
	cout<<endl;

	cout<<"-> Simulated thermal power (W) : "<<endl;
	cout<<"Found in a .info file :"<<endl;
	cout<<"\033[36m"<<Power<<"\033[0m"<<endl;
	cout<< "Is that corect ? [y/n] "<<endl;
	if(!UserSayYes())
	{	cout<<"\t So what it is ?"<<endl;
		cin>> Power;
	}	
	cout<<endl;

	/**************************************************/
	// BUILDING FILE
	/**************************************************/
	ofstream InfoFile("Data_Base_Info.nfo");

  	InfoFile <<"============================================" << endl;
  	InfoFile <<"    Informations needed by XSM_MLP model    " <<endl;
  	InfoFile <<"============================================" << endl;
	InfoFile << endl;
	InfoFile << "Reactor Type :"<<endl;
	InfoFile << "K_REACTOR "<< fReactorType <<endl;
	InfoFile << endl;
	InfoFile << "Fuel Type :"<<endl;
	InfoFile << "K_FUEL "<< fFuelType <<endl;
	InfoFile << endl;
	InfoFile << "Heavy Metal [t] :"<<endl;
	InfoFile << "K_MASS  "<< MeanHMMass <<endl;
	InfoFile << endl;
	InfoFile << "Thermal Power [W] :"<<endl;
	InfoFile << "K_POWER  "<< Power <<endl;
	InfoFile << endl;
	InfoFile << "Irradiation time steps [s] :"<<endl;
	InfoFile << "K_TIMESTEP";
		for( int t = 0 ; t < fNOfTimeStep[0] ; t++ ) /// DANGER IF NOT THE SAME TIME BINNING FOR EACH EVOLUTION DATA => Futher work : get the vector with the shortest irrariaiton time to avoid extrapolation in CLASS
			InfoFile <<" "<<fTime[0][t];
	InfoFile << endl<<endl;
	InfoFile << "Z A I Name (input MLP) :"<<endl;
	for(map<ZAI,string>::iterator it = fMapName.begin() ; it != fMapName.end() ; it++ )
			InfoFile <<"K_ZAINAME "<<it->first.Z <<" " <<it->first.A <<" " <<it->first.I<<" " << it->second <<endl ;
	InfoFile <<endl;
	InfoFile << "Fuel range (Z A I min max) :"<<endl;
	for(map<ZAI,string>::iterator it = fMapName.begin() ; it != fMapName.end() ; it++ )
	{	vector <double> AllCompoOfZAI = GetAllCompoOf(it->first);
		vector <double>::iterator Min = std::min_element(AllCompoOfZAI.begin(),AllCompoOfZAI.end()); 
		vector <double>::iterator Max = std::max_element(AllCompoOfZAI.begin(),AllCompoOfZAI.end());
		InfoFile <<"K_ZAIL "<<it->first.Z <<" " <<it->first.A <<" " <<it->first.I<<" " << *Min << " "  << *Max <<endl ;
  	}
    InfoFile << endl;
  	InfoFile <<"============================================" << endl;
  	InfoFile <<"     Data base generation informations      " <<endl;
  	InfoFile <<"============================================" << endl;
  	InfoFile << endl;
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t ); 
    InfoFile <<" Date: "<< now->tm_mday<< '/'<<  (now->tm_mon + 1) << '/'  <<(now->tm_year + 1900)<< endl;
	InfoFile <<" Author(s): "<< Author <<endl;
	InfoFile <<" Author(s) contact: "<< Mail <<endl;
	InfoFile <<" Depletion code: "<< DepCode <<endl;
	InfoFile <<" Simulated geometry: "<<  Geom <<endl;
	InfoFile <<" Nuclear data used in "<< DepCode <<endl;
	InfoFile <<"\tCross section library: "<< XSBase <<endl;
	InfoFile <<"\tFission yield library: "<< FPYBase <<endl;
	InfoFile <<"\tS(alpha,beta) library: "<< SABase <<endl;
	InfoFile <<" Half life cut [s] : "<<  HLCut <<endl;
	InfoFile <<" Multi-group treatment: "<<  EnergyDisc <<endl;
	InfoFile <<" Additional informations: "<< endl << AddInfo <<endl;


}
//--------------------------------------------------------------------------------------------------
vector<double> GetAllCompoOf(ZAI zai)
{
	vector<double> AllCompoOfZAI;

		for( int b = 0 ; b < fActinideCompoInit.size() ; b++ )
			AllCompoOfZAI.push_back(fActinideCompoInit[b].GetZAIIsotopicQuantity(zai)); 

return AllCompoOfZAI;
}
//--------------------------------------------------------------------------------------------------
void ProgressBar(double loopindex, double totalindex)
{
	// Reset the line
	//for(int i = 0; i < 22; i++)
	//	cout << "  ";
	cout << flush ;

	cout << "\033[42m";
	for(int i = 0; i < (int)(loopindex/totalindex*44.0); i++)
		cout << " ";
	cout<<" \033[41m";
	for(int i = 44; i >= (int)(loopindex/totalindex*44.0); i--)
		cout << " ";
	cout<<"\033[0m";
	cout << (int)(loopindex/totalindex*100) << "%\r";
	cout << flush;
	//cout << endl;
}
//--------------------------------------------------------------------------------------------------
void ReadInfo(string InfoDBFile,string &ReactorType,string &FuelType,double &Power)
{	
	ifstream InfoDB(InfoDBFile.c_str());				// Open the File
	if(!InfoDB)
	{
		cout << "\033[31m !!ERROR!! !!!EvolutionData!!! \n Can't open \"" << InfoDBFile << "\"\033[0m \n" << endl;
	}
	else
	{
		int start = 0;
		string line;
		getline(InfoDB, line);
		string Next = StringLine::NextWord(line, start, ' ');
		StringLine::ToLower(Next) ;
		if ( Next ==  "reactor")
			ReactorType = StringLine::NextWord(line, start, ' ');
		
		start = 0;
		getline(InfoDB, line);
		Next = StringLine::NextWord(line, start, ' ');
		StringLine::ToLower(Next) ;
		if ( Next ==  "fueltype")
			FuelType = StringLine::NextWord(line, start, ' ');
	
		start = 0;
		getline(InfoDB, line);
		Next = StringLine::NextWord(line, start, ' ');
		StringLine::ToLower(Next) ;
		if ( Next  ==  "cycletime")
			double cycletime = atof(StringLine::NextWord(line, start, ' ').c_str());
	
		getline(InfoDB, line); // Assembly HM Mass DONT TRUST THIS ONE CALCULATED WITH A instead of real atomic mass
	
		start = 0;
		getline(InfoDB, line);
		Next = StringLine::NextWord(line, start, ' ');
		StringLine::ToLower(Next) ;
		if ( Next ==  "constantpower")
			Power = atof(StringLine::NextWord(line, start, ' ').c_str());
		InfoDB.close();
	}

}


//--------------------------------------------------------------------------------------------------
void DumpInputNeuron(string filename)
{
	TFile*   fOutFile = new TFile(filename.c_str(),"RECREATE");
	TTree*   fOutT = new TTree("Data", "Data");


/**********************INITIALISATIONNN********************/

	////////////////////////////////////////////////////////
	// INIT FRESH FUEL COMPOSITION and TIME
	////////////////////////////////////////////////////////

	double *FreshCompo = new double[fMapName.size()]; 

	for(int i = 0 ; i < fMapName.size() ; i++ )
		FreshCompo[i] = 0;

	////////////////////////////////////////////////////////
	double Time 		     = 0;	

/**********************init map********************/
	map < ZAI,vector<double> > mAllXS;
	//map < ZAI, vector<double>  > mAllInventories;
	//for(int act=0;act<int(fAllNuclei.size());act++ )	
	//{	 
	//	vector<double>  InitVect;
	//	for(int Tstep=0 ;Tstep<fNOfTimeStep;Tstep++)
	//	{
	//		InitVect.push_back(0);
	//	}
	//	mAllInventories.insert( pair<ZAI,vector<double> >(fAllNuclei[act],InitVect) );
	//}

	for(int act=0;act<int(fAllNuclei.size());act++ )	
	{	
		vector< double>  InitVect;
		for(int xs=0;xs<3;xs++)
		{
			
			InitVect.push_back(0);
				
		}

		mAllXS.insert(pair<ZAI,vector< double> >(fAllNuclei[act], InitVect) );
	}


/**********************BRANCHING**************************************************/
/**********************Fresh fuel**************************************************/

	map<ZAI,string>::iterator it;
	int index = 0;
	for(it = fMapName.begin() ; it != fMapName.end() ; it++ )
	{	string Name = it->second + "/D"; 
		fOutT->Branch( it->second.c_str() , &FreshCompo[index] , Name.c_str());
		index++;
	}

	////////////////////////////////////////////////////////
	fOutT->Branch(	"Time"			,&Time			,"Time/D"			);


/**********************cross section**************************************************/

	string XSType[3]={"fis","cap","n2n"}; //!!!!!!DO NOT TOUCH THIS

	for(int act=0;act<int(fAllNuclei.size());act++ )
	{	
		for(int xs=0;xs<3;xs++)
		{
				string NamedXS= NameXS(fAllNuclei[act],XSType[xs]);
				string NameXSBis=NamedXS+"/D";

		if(	xs==0 && fXSFis[0][fAllNuclei[act]].size()!=0)	//Check if the reaction we want to branch exists
			 if(fXSFis[0][fAllNuclei[act]][0]!=0) 
			 		fOutT->Branch(	NamedXS.c_str() ,  &mAllXS[fAllNuclei[act]][xs],   NameXSBis.c_str()	);			

		if(xs==1 && fXSCap[0][fAllNuclei[act]].size()!=0)
			 if(fXSCap[0][fAllNuclei[act]][0]!=0) 
			 		fOutT->Branch(	NamedXS.c_str() ,  &mAllXS[fAllNuclei[act]][xs],   NameXSBis.c_str()	);				

		if(xs==2 && fXSN2N[0][fAllNuclei[act]].size()!=0)
			 if(fXSN2N[0][fAllNuclei[act]][0]!=0) 
					fOutT->Branch(	NamedXS.c_str() ,  &mAllXS[fAllNuclei[act]][xs],   NameXSBis.c_str()	);				


		}
	}
/**********************FILLING THE TTREE**************************************************/
		cout<<endl;
		cout<<endl;
		cout<<"╭───────────────────────────────────────────────╮"<<endl;
		cout<<"│                 FILLING TTREE                 │"<<endl;
		cout<<"│         (building TrainingInput.root)         │"<<endl;		         
		cout<<"╰───────────────────────────────────────────────╯"<<endl;
		cout<<endl;
	//File containing all the output of the networks to train
	 ofstream  InputNetwork("_tmp/include_Train_XS/TrainingInput.cxx");

	 int NumOfBase=fActinideCompoInit.size();
	for(int b=0;b<NumOfBase;b++) 
	{ 

		ProgressBar(b,NumOfBase);

		///////////////////////////////////////////////////////
		int index =0;
		for(it = fMapName.begin() ; it != fMapName.end() ; it++ )
		{	
			int Z = it->first.Z;
			int A = it->first.A;
			int I = it->first.I;
			FreshCompo[index] = fActinideCompoInit[b].GetZAIIsotopicQuantity(ZAI(Z,A,I));

			index++;
		}	
		///////////////////////////////////////////////////////

			for(int Tstep=0 ;Tstep<fNOfTimeStep[b];Tstep++ )	
			{	
	 			Time=fTime[b][Tstep];
				for(int act=0;act<int(fAllNuclei.size());act++)
				{
	
					if(fXSFis[b][fAllNuclei[act]].size()!=0) // reaction may not be present
					{	
						if(fXSFis[b][fAllNuclei[act]][Tstep] !=0 )
						{	mAllXS[fAllNuclei[act]][0]   = 	fXSFis[b][fAllNuclei[act]][Tstep];
							if(b==0 && Tstep==0){
								InputNetwork<<"OUTPUT.push_back(\""<<NameXS(fAllNuclei[act],XSType[0])<<"\");"<<endl;
								fReactionCounter++;
							}
						}
					}		
					if(fXSCap[b][fAllNuclei[act]].size()!=0)
				 	{	
				 		if(fXSCap[b][fAllNuclei[act]][Tstep] !=0) 
				 		{	mAllXS[fAllNuclei[act]][1]   = 	fXSCap[b][fAllNuclei[act]][Tstep];
							if(b==0 && Tstep==0){		 			
				 				InputNetwork<<"OUTPUT.push_back(\""<<NameXS(fAllNuclei[act],XSType[1])<<"\");"<<endl;
								fReactionCounter++;
							}
						}
				 	}	
				 	if( fXSN2N[b][fAllNuclei[act]].size()!=0)
				 	{	
				 		if(fXSN2N[b][fAllNuclei[act]][Tstep] !=0)
				 		{	mAllXS[fAllNuclei[act]][2]   = 	fXSN2N[b][fAllNuclei[act]][Tstep];
							if(b==0 && Tstep==0){
				 				InputNetwork<<"OUTPUT.push_back(\""<<NameXS(fAllNuclei[act],XSType[2])<<"\");"<<endl;
								fReactionCounter++;
							}
						}
				 	}
				 	
				 } 
				 fOutT->Fill();	
			}		
	}
	ProgressBar(1,1);

	fOutFile->Write();
	delete fOutT;
	fOutFile-> Close();
	delete fOutFile;

}

//--------------------------------------------------------------------------------------------------
void CheckJob()
{	//LOAD THE LIST OF EvolutionData

		cout<<endl;
		cout<<"╭───────────────────────────────────────────────╮"<<endl;
		cout<<"    Scanning :                                   "<<endl;
		cout<<  fEvolutionDataFolder                             <<endl;	
		cout<<"          for EvolutionData (.dat files)         "<<endl;
		cout<<"╰───────────────────────────────────────────────╯"<<endl;

	string Command = "find "+ fEvolutionDataFolder + " -name \"*.dat\" > JOB.tmp";
	system(Command.c_str());

	ifstream JOB("JOB.tmp");

	if(JOB.peek() == std::ifstream::traits_type::eof()) // if file is empty 
	{	cout << endl <<  "\033[31mERROR: No .dat found in folder : " << fEvolutionDataFolder << "\033[0m" << endl;
		exit(1);
	}

	if (JOB.is_open())
	{
		while (!JOB.eof())
		{
			string tmp;
			getline(JOB,tmp);
			JobName.push_back(tmp);
		}
	} JOB.close(); JobName.pop_back();
	
	// Remove temporary files...
	Command = "\\rm -f JOB.tmp";
	system(Command.c_str());
	random_shuffle(JobName.begin(), JobName.end());
	cout << "\033[32m Scan complete \033[0m" <<endl; 
}

//--------------------------------------------------------------------------------------------------
void ReadAndFill(string jobname)
{	//Read a .dat file and fill XS maps and the fuel initial composition

	vector<double>	vT;
	vector<int>	Z;
	vector<int>	A;
	vector<int>	I;
	vector<double>	Q;
	
	ifstream DecayDB(jobname.c_str());							// Open the File
	if(!DecayDB)
	{
		cout << "\033[33m !!Warning!! !!!EvolutiveProduct!!! \n Can't open \"" << jobname << "\"\033[0m\n" << endl;
	}
	
	string line;
	int start = 0;
	
	getline(DecayDB, line);
	
	/******Getting Time vecotr ....******/
	if( StringLine::NextWord(line, start, ' ') != "time")
	{
		cout << "\033[31m!!Bad Trouble!! !!!EvolutiveProduct!!! Bad Database file : " <<  jobname << "\033[0m" << endl;
		exit (1);
	}
	
	while(start < (int)line.size())
		vT.push_back(atof(StringLine::NextWord(line, start, ' ').c_str()));

	fTime.push_back(vT);

	fNOfTimeStep.push_back(int(vT.size()));
	/****Getting Inventories***/	
	getline(DecayDB, line);
	do
	{	
		start = 0;
		int z;
		string tmp2 = StringLine::NextWord(line, start, ' ');
		if (tmp2 == "Inv") {
			z = atoi(StringLine::NextWord(line, start, ' ').c_str());
		}
		else z = atoi(tmp2.c_str());
		int a = atoi(StringLine::NextWord(line, start, ' ').c_str());
		int i = atoi(StringLine::NextWord(line, start, ' ').c_str());
		
		if(a!=0 && z!=0)
		{
			
			ZAI zaitmp(z, a, i);
			Z.push_back(z);
			A.push_back(a);
			I.push_back(i);
			if(!fIsAllNucleiAlreadyFill)
			{	
				fAllNuclei.push_back(zaitmp);
			}	

			long double q = atof(StringLine::NextWord(line, start, ' ').c_str());
			Q.push_back(q);	

		}

		getline(DecayDB, line);
		start = 0;
		tmp2 = StringLine::NextWord(line, start, ' ');
		
		if(line == "" || line == "CrossSection" || tmp2 == "XSFis" || tmp2 == "XSCap" || tmp2 == "XSn2n") break;
	}while (!DecayDB.eof() );

	if(fAllNuclei.size()!=0)
	 fIsAllNucleiAlreadyFill=true;

	//XS  
	 map<ZAI, vector <double> > mapFistmp;
	 map<ZAI, vector <double> > mapCaptmp;
	 map<ZAI, vector <double> > mapN2Ntmp;
	do
	{
		
		start = 0;
		int z;
		string tmp2 = StringLine::NextWord(line, start, ' ');
		if (tmp2 == "XSFis") 
		{
			z = atoi(StringLine::NextWord(line, start, ' ').c_str());
			int a = atoi(StringLine::NextWord(line, start, ' ').c_str());
			int i = atoi(StringLine::NextWord(line, start, ' ').c_str());
		
			if(a!=0 && z!=0)
			{
				
				ZAI zaitmp(z, a, i);

				vector<double> XSTime;
				for(int i = 0; i < (int)vT.size(); i++)
				{	long double q = atof(StringLine::NextWord(line, start, ' ').c_str());
					XSTime.push_back(q);
				}
				mapFistmp.insert( pair<ZAI,vector<double> >(zaitmp,XSTime));		
			}
		}
		else if (tmp2 == "XSCap") 
		{
			z = atoi(StringLine::NextWord(line, start, ' ').c_str());
			int a = atoi(StringLine::NextWord(line, start, ' ').c_str());
			int i = atoi(StringLine::NextWord(line, start, ' ').c_str());
		
			if(a!=0 && z!=0)
			{
				
				ZAI zaitmp(z, a, i);
				vector<double> XSTime;
				for(int i = 0; i < (int)vT.size(); i++)
				{	long double q = atof(StringLine::NextWord(line, start, ' ').c_str());
					XSTime.push_back(q);
				}
				mapCaptmp.insert( pair<ZAI,vector<double> >(zaitmp,XSTime));	
				
			}
		}	
		else if (tmp2 == "XSn2n") 
		{
			z = atoi(StringLine::NextWord(line, start, ' ').c_str());
			int a = atoi(StringLine::NextWord(line, start, ' ').c_str());
			int i = atoi(StringLine::NextWord(line, start, ' ').c_str());
		
			if(a!=0 && z!=0)
			{		
				ZAI zaitmp(z, a, i);
				vector<double> XSTime;
				for(int i = 0; i < (int)vT.size(); i++)
				{	long double q = atof(StringLine::NextWord(line, start, ' ').c_str());
					XSTime.push_back(q);
				}
				mapN2Ntmp.insert( pair<ZAI,vector<double> >(zaitmp,XSTime));					
			}
		}
		getline(DecayDB, line);
		start = 0;
		tmp2 = StringLine::NextWord(line, start, ' ');
		
		if(line == "") break;
	}while (!DecayDB.eof() );
	
	fXSFis.push_back(mapFistmp);
	fXSCap.push_back(mapCaptmp);
	fXSN2N.push_back(mapN2Ntmp);
	
	DecayDB.close();
	
	double N = 0;
	for(int i=0; i < (int)Z.size()-2; i++)
	{
		if(  Z[i]>89 ) 
			N += Q[i];
	}
	

	IsotopicVector CompoBasei; 
	IsotopicVector CompoBaseiUnormalize;

	for(int i=0; i < (int)Z.size()-2; i++)
	{
		if(Z[i]>89)
		{
			ZAI zai = ZAI(Z[i], A[i], I[i]);
			CompoBasei.IVquantity.insert(pair<ZAI,double>(zai,Q[i]/N));
			CompoBaseiUnormalize.IVquantity.insert(pair<ZAI,double>(zai,Q[i]));
		}	
	}
	
	fActinideCompoInit.push_back(CompoBasei);
	double MassOfThisFuel = cZAIMass.GetMass(CompoBaseiUnormalize);
	//cout<<MassOfThisFuel<<endl;
	fHMMass.push_back(MassOfThisFuel);

GoodJobName.push_back(jobname);

}
/*-------------------------------------------------------------------------------------------------
COMPILATION :

g++ -o Generate_XSM Generate_XSM.cxx `root-config --cflags` `root-config --libs`


*/
