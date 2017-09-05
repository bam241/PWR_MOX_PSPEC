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
#include "include/Generate_MLP_Kinf.hxx"
#include <TH1F.h>
#include <TH2D.h>
#include <TFile.h>
#include <TTree.h>
#include "../CLASS/source/external/StringLine.hxx"
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
int main(int argc, char ** argv)
{
	
	if(argc!=2)
	{
		cout << "Usage : Generate_MLP_Kinf Path" << endl;
		cout << " Where Path is the path to the folder containing Evolution Datas" << endl;
		cout << " i.e the (.dat) files" << endl;
		exit(0);
	}	

	/*****************Preparation of working folders*************************/
	if(is_file_exist("Training_output_k_inf.root" ))
	{
		cout<< "\033[33m Trainining_output* detected. Delete these files and weights folder ? [y/n]\033[0m"<< endl;
		if(UserSayYes())
			system( "rm -rf Training_output* weights"  );
		else
		{	cout << "Move this files elsewhere and run this program again"<< endl;
			exit(0);
		}
	}

	if(is_file_exist("_tmp/include_Train/TrainingInput.root" ))
		system( "rm -rf _tmp"  );

	system("mkdir -p _tmp/include_Train");
	/**********************************************************************/

	fEvolutionDataFolder = argv[1];
	CheckJob();	// looks fot the .dat files in the fEvolutionDataFolder

	cout << endl;
	cout << "╭───────────────────────────────────────────────╮" << endl; 
	cout << "│      Load your EvolutionDatas to R.A.M        │" << endl; 
	cout << "╰───────────────────────────────────────────────╯" << endl; 
	cout<< endl;
	for(int i = 0; i < (int)JobName.size(); i++)
	{
		ReadAndFill(JobName[i]);
		ProgressBar(i,JobName.size());
	}
	ProgressBar(1,1);

	FillMapName();

	cout << "Proportion of data to be used for training ? ]0-100[" << endl;
	double ProportionOfTraining = 0 ;

	GetUserValue( ProportionOfTraining);
		DumpInputNeuron("_tmp/include_Train/TrainingInput.root");
		Generate_tmva_factory_input(ProportionOfTraining);

	string ModelName = CreateInfoFile();

	cout << "╭─────────────────────────────────────────────────────────────────────╮" << endl; 
	cout << "│                            GENERATED FILES:                         │" << endl; 
	cout << "├─────────────────────────────────────────────────────────────────────┤" << endl; 
	cout << "│#1 Input for TMVA training: \033[36m_tmp/include_Train/TrainingInput.root\033[0m    │" << endl; 
    cout << "│#3 Model Information for CLASS: \033[36m"+ ModelName +".nfo\033[0m   │" << endl; 
	cout << "╰─────────────────────────────────────────────────────────────────────╯" << endl; 
	cout << endl;
	cout << "╭────────────────────────────────────────────────╮" << endl; 
	cout << "│                  NEXT STEPS:                   │" << endl; 
	cout << "├────────────────────────────────────────────────┤" << endl; 
	cout << "│1. Train your MLPs with \033[36mTrain.cxx\033[0m               │" << endl; 
	cout << "│2. Test MLPs performances using informations in:│" << endl; 
	cout << "│	     \033[36mUtils/XSM/MLP/Test/EvaluateTrainingCommands.dat\033[0m│" << endl; 
	cout << "│3. Put the file #3 in \033[36mweights\033[0m folder then       │" << endl; 
    cout << "│ move this folder to $CLASS_PATH/DATA_BASES"<<fReactorType<<"/"<<fFuelType << "/EQModel/ChooseAName  │" << endl; 
	cout << "╰────────────────────────────────────────────────╯" << endl; 
	cout << endl; 
	cout << "=> Doing the first step for you. Do you want to train the MLP on your local machine ? [y/n]" << endl; 

	if(UserSayYes())
	{
		CompileTraining();
		cout << "Let's train locally ? [y/n]" << endl;
		if(UserSayYes())
			system("./train");
		else
		{
			cout << "You can run it latter with  ./Train" << endl;
			exit(0);
		}
	}	

	


}

//--------------------------------------------------------------------------------------------------
void GetUserValue(int &Value)
{
	string sValue ;
	std::getline(std::cin, sValue);

	if(StringLine::IsDouble(sValue))
	{
		double dValue = atof(sValue.c_str());
		double iValue = floor(dValue);

		if( dValue != iValue )
		{
			cout << "It must be an INTEGER" << endl;
			GetUserValue(Value);

		}
		else
			Value = iValue;
	}
	
	else
	{
		cout << "It must be an INTEGER" << endl;
		GetUserValue(Value);
	}

}
void GetUserValue(double &Value)
{
	string sValue ;
	std::getline(std::cin, sValue);

	if(StringLine::IsDouble(sValue))
		Value = atof(sValue.c_str());
	else
	{
		cout << "It must be a DOUBLE" << endl;
		GetUserValue(Value);
	}
}
void GetUserValue(string &Value)
{
	std::getline(std::cin, Value);
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
	if(is_file_exist("Train"))
		system("rm Train");

	string CMD = "g++ -o Train  `root-config --cflags` Train.cxx `root-config --glibs` -lTMVA";
	cout << CMD << endl;
	system(CMD.c_str());

	if(!is_file_exist("Train"))
	{
		cout <<"\033[31m  COMPILATION FAILED !!! May be not the good compilator name nor the good path of file to compile ? \033[0m " << endl;
		exit(1);
	}
	else
		cout <<"\t \033[32m Done \033[0m " << endl;
	

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
	cout<< endl;
	cout<< "╭───────────────────────────────────────────────╮"<< endl;
	cout<< "│               SET UP MLPs INPUT               │"<< endl;
	cout<< "│default : composition @ t=0 + Irradiation time │"<< endl;
	cout<< "╰───────────────────────────────────────────────╯"<< endl;
	cout<< endl;

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
			cout<< "Add this nuclei with this name [y/n] ?  (if you don't know type yes to all)  "<< endl;
		else
			cout<< "Add [y/n] ?  "<< endl;

		cout<< "[Z\tA\tI\tName]"<< endl;
		cout<<"\033[36m"<<Z<<"\t"<<A<<"\t"<<I<<"\t"<<ssname.str()<<"\033[0m"<< endl;
		if (UserSayYes())
		{	
			fMapName.insert(pair<ZAI,string> ( ZAI(Z,A,I) , ssname.str()) );
			FissileOrFertileList(Z,A,I);
		}	
	}

	bool UserWantToAdd = true;
	while(UserWantToAdd)
	{
		cout<< "Do you want to add additional nuclei  [y/n] ? (if you don't know type no) "<< endl;
		if(UserSayYes())
		{	
			int Z = 0;
			int A = 0;
			int I = 0;
			string Name;
			cout << " Z -> "; 
			GetUserValue( Z);
			cout <<" A -> ";
			GetUserValue( A);
			cout <<" I -> ";
			GetUserValue( I);
			cout <<" Name -> ";
			GetUserValue( Name);

			fMapName.insert(pair<ZAI,string> ( ZAI(Z,A,I) ,Name) );

			FissileOrFertileList(Z,A,I);
		}

		else
			UserWantToAdd = false;
	}	


}

//--------------------------------------------------------------------------------------------------
void FissileOrFertileList(int Z,int A,int I)
{

			cout << "Is this nuclei is considered to be a part of a fissile element ? [y/n]" << endl;

			if (UserSayYes())	
				fStreamList.push_back( ZAIStreamList(Z,A,I,"Fissile",1) );
			else
			{
				cout<<"So it is considered as fertile\n What is the default proportion of this nucleus in the fertile vector ? [0-1] " << endl;
				double Default_Proportion = 0;
				GetUserValue( Default_Proportion);
				fStreamList.push_back( ZAIStreamList(Z,A,I,"Fertile",Default_Proportion) );
			}
			cout<< endl;
}

//--------------------------------------------------------------------------------------------------
void Generate_tmva_factory_input(double ProportionOfTraining)
{

	ofstream  InputNetwork("_tmp/include_Train/InputVariables.cxx");
	for(map<ZAI,string>::iterator it = fMapName.begin() ; it != fMapName.end() ; it++ )
		InputNetwork <<"factory->AddVariable( \"" << it->second  << "\" , \"" << it->second << "\", \"IsotopicFraction\", 'F' );"<< endl; 
    InputNetwork <<"factory->AddVariable( \"Time\" , \"Time\"     , \"seconds\", 'F' );"<< endl<< endl;

	ProportionOfTraining /=100;
    InputNetwork <<"double PropTraining = "<< ProportionOfTraining <<";"<< endl;

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
			GetUserValue( answer);

	
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
				cout << "Yes OR No ? "<< endl;
			}
		}
	return isYES;	
}

//--------------------------------------------------------------------------------------------------
string CreateInfoFile()
{

	/**************************************************/
	// GETTING USER INFO
	/**************************************************/
	double sum = 0 ;
	for (int i=0 ; i < fHMMass.size() ; i++)
			sum+=fHMMass[i];

    double MeanHMMass = sum / (double)fHMMass.size();
    string Author,Mail,XSBase,HLCut,EnergyDisc,FPYBase,SABase,Geom,AddInfo,DepCode ;
    double Power = 0, FissileFirstGuess = 0 , MaxFisisle = 0;


	int start=0;
	string AnInfoFile = JobName[0];
	int pos=AnInfoFile.find(".dat",start);
	AnInfoFile.replace(pos,4,".info");

    ReadInfo(AnInfoFile,fReactorType,fFuelType,Power);

	cout<< endl;
	cout<< endl;
	cout<<"╭───────────────────────────────────────────────╮"<< endl;
	cout<<"│        XSM_MLP .NFO FILE GENERATOR            │"<< endl;
	cout<<"╰───────────────────────────────────────────────╯"<< endl;
	cout<<" Answer following questions "<< endl;
	cout<<"-> Author(s) name(s) : "<< endl;
	GetUserValue( Author);
	cout<< endl;

	cout<<"-> email adress(es) : "<< endl;
	GetUserValue( Mail);	
	cout<< endl;

	cout<<"-> Depletion code used : "<< endl;
	GetUserValue( DepCode);
	cout<< endl;

	cout<<"-> Cross section data base (e.g ENSDF7.1) : "<< endl;
	GetUserValue( XSBase);
	cout<< endl;

	cout<<"-> Fission yield data base (e.g ENSDF7.1) : "<< endl;
	GetUserValue( FPYBase);
	cout<< endl;

	cout<<"-> S(alpha,beta) data base (e.g ENSDF7.1) : "<< endl;
	GetUserValue( SABase);
	cout<< endl;

	cout<<"-> Geometry simulated  (e.g Cubic Assembly with mirror boundary) : "<< endl;
	GetUserValue( Geom);
	cout<< endl;

	cout<<"-> Half life cut [s] (if any) : "<< endl;
	GetUserValue( HLCut);	
	cout<< endl;

	cout<<"-> Multi group treatment (yes/no if yes give the number of groups) : "<< endl;
	GetUserValue( EnergyDisc);
	cout<< endl;	

	cout<<"-> Additional informations : "<< endl;
	GetUserValue( AddInfo);	
	cout<< endl;	


	cout<<"-> Reactor type (e.g PWR, FBR,...) : "<< endl;
	cout<<"Found in a .info file :"<< endl;
	cout<<"\033[36m"<<fReactorType<<"\033[0m"<< endl;
	cout<< "Is that corect ? [y/n] "<< endl;
	if(!UserSayYes())
	{	cout<<"\t So what it is ?"<< endl;
		GetUserValue( fReactorType);
	}	
	cout<< endl;

	cout<<"-> Fuel type (e.g UOX, MOX,...) : "<< endl;
	cout<<"Found in a .info file :"<< endl;
	cout<<"\033[36m"<<fFuelType<<"\033[0m"<< endl;
	cout<< "Is that corect ? [y/n] "<< endl;
	if(!UserSayYes())
	{	cout<<"\t So what it is ?"<< endl;
		GetUserValue( fFuelType);

	}	
	cout<< endl;

	cout<<"-> Simulated heavy metal mass (tons) : "<< endl;
	cout<< "\t According your evolution datas the AVERAGE heavy metal mass is : "<< endl;
	cout<<"\033[36m"<<MeanHMMass<<"\033[0m"<<" tons"<< endl;
	cout<< "Is that corect ? [y/n] "<< endl;
	if(!UserSayYes())
	{	cout<<"\t So what it is ?"<< endl;
		GetUserValue( MeanHMMass);		
	}
	cout<< endl;

	cout<<"-> Simulated thermal power (W) : "<< endl;
	cout<<"Found in a .info file :"<< endl;
	cout<<"\033[36m"<<Power<<"\033[0m"<< endl;
	cout<< "Is that corect ? [y/n] "<< endl;
	if(!UserSayYes())
	{	cout<<"\t So what it is ?"<< endl;
		GetUserValue( Power);		
	}	
	cout<< endl;

	cout<<"-> First guess of fissile content in fresh fuel (for algorithm initialization): ]0-1[ "<< endl;
	GetUserValue( FissileFirstGuess);		

	cout<<"-> Maximal fissile content in the fresh fuel  (for algorithm max range): ]0-1[ "<< endl;
	GetUserValue( MaxFisisle );		

	cout<< endl;


	/**************************************************/
	// BUILDING FILE
	/**************************************************/
	stringstream ssModelName;
	ssModelName << fReactorType << "_" << fFuelType << "_" << Power/(MeanHMMass*1e6) << "Wg";


	ofstream InfoFile( (ssModelName.str() + ".nfo").c_str());

	InfoFile << "To be used with : EQM_PWR_MLP_Kinf.cxx "    << endl;
	InfoFile << "with constructor : EQM_PWR_MLP_Kinf(string TMVAWeightPath,  int NumOfBatch, string InformationFile, double CriticalityThreshold)" << endl;
  	InfoFile <<"============================================" << endl;
  	InfoFile <<"    Informations needed by EQM_MLP_Kinf model    " << endl;
  	InfoFile <<"============================================" << endl;
  	InfoFile << endl;
	InfoFile << "Reactor Type :"<< endl;
	InfoFile << "K_REACTOR "<< fReactorType << endl;
	InfoFile << endl;
	InfoFile << "Fuel Type :"<< endl;
	InfoFile << "K_FUEL "<< fFuelType << endl;
	InfoFile << endl;
	InfoFile << "Specific Power (W/gHM) :"<< endl;
	InfoFile << "K_SPECPOWER  "<<  Power/(MeanHMMass*1e6) << endl;
	InfoFile << endl;

	double IrrTimeMax = 0;
	for(int i = 0 ; i < fTime.size() ; i++ )
	{
		if (IrrTimeMax < fTime[i].back())
			IrrTimeMax = fTime[i].back();
	}
	double Bu_Max = Power*1e-9 * (IrrTimeMax/3600./24.) / MeanHMMass;
	InfoFile << "Maximal Burnup:"<< endl;
	InfoFile << "K_MAXBURNUP "<< Bu_Max * 1.3 << endl;
	cout << endl;
	
	InfoFile << "Maximal fissile content (algo use) :" << endl;
	InfoFile << "K_MAXFISCONTENT "<< MaxFisisle  << endl;
	InfoFile << endl;


	InfoFile << "Z A I Name (input MLP) :"<< endl;
	for(map<ZAI,string>::iterator it = fMapName.begin() ; it != fMapName.end() ; it++ )
			InfoFile <<"K_ZAINAME "<<it->first.Z <<" " <<it->first.A <<" " <<it->first.I<<" " << it->second << endl ;
	InfoFile << endl;

	InfoFile << "Name of stream for fabrication (fissile or fertile):" << endl;
	InfoFile << "Z A I Default_Proportion" << endl;
	for( int l = 0 ; l < (int) fStreamList.size() ; l++ )
			InfoFile <<"K_LIST "<<fStreamList[l].fZAI.Z <<" " << fStreamList[l].fZAI.A <<" " <<fStreamList[l].fZAI.I<<" " << fStreamList[l].fListName <<" "<< fStreamList[l].fDefaultProportion<< endl ;
	InfoFile << endl;

	InfoFile << "Starting fissile content in fuel for equivalence model calculation :"<< endl;
	InfoFile << "k_firstguesscontent Fissile " << FissileFirstGuess << endl;
	InfoFile << "k_firstguesscontent Fertile " << 1 - FissileFirstGuess << endl;
	InfoFile << endl;

	InfoFile << "Fuel range (Z A I min max) :"<< endl;
	for(map<ZAI,string>::iterator it = fMapName.begin() ; it != fMapName.end() ; it++ )
	{	vector <double> AllCompoOfZAI = GetAllCompoOf(it->first);
		vector <double>::iterator Min = std::min_element(AllCompoOfZAI.begin(),AllCompoOfZAI.end()); 
		vector <double>::iterator Max = std::max_element(AllCompoOfZAI.begin(),AllCompoOfZAI.end());
		InfoFile <<"K_ZAIL "<<it->first.Z <<" " <<it->first.A <<" " <<it->first.I<<" " << *Min << " "  << *Max << endl ;
  	}
    InfoFile << endl;
  	InfoFile <<"============================================" << endl;
  	InfoFile <<"     Data base generation informations      " << endl;
  	InfoFile <<"============================================" << endl;
  	InfoFile << endl;
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t ); 
    InfoFile <<" Date: "<< now->tm_mday<< '/'<<  (now->tm_mon + 1) << '/'  <<(now->tm_year + 1900)<< endl;
	InfoFile <<" Author(s): "<< Author << endl;
	InfoFile <<" Author(s) contact: "<< Mail << endl;
	InfoFile <<" Depletion code: "<< DepCode << endl;
	InfoFile <<" Simulated geometry: "<<  Geom << endl;
	InfoFile <<" Nuclear data used in "<< DepCode << endl;
	InfoFile <<"\tCross section library: "<< XSBase << endl;
	InfoFile <<"\tFission yield library: "<< FPYBase << endl;
	InfoFile <<"\tS(alpha,beta) library: "<< SABase << endl;
	InfoFile <<" Half life cut [s] : "<<  HLCut << endl;
	InfoFile <<" Multi-group treatment: "<<  EnergyDisc << endl;
	InfoFile <<" Additional informations: "<< endl << AddInfo << endl;

return ssModelName.str();

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
	double k_inf 			 = 0;


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
	fOutT->Branch(	"k_inf"			,&k_inf			,"k_inf/D"			);


/**********************FILLING THE TTREE**************************************************/
		cout<< endl;
		cout<< endl;
		cout<<"╭───────────────────────────────────────────────╮"<< endl;
		cout<<"│                 FILLING TTREE                 │"<< endl;
		cout<<"│         (building TrainingInput.root)         │"<< endl;		         
		cout<<"╰───────────────────────────────────────────────╯"<< endl;
		cout<< endl;

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
	 			k_inf=fkeff[b][Tstep];
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

		cout<< endl;
		cout<<"╭───────────────────────────────────────────────╮"<< endl;
		cout<<"    Scanning :                                   "<< endl;
		cout<<  fEvolutionDataFolder                             << endl;	
		cout<<"          for EvolutionData (.dat files)         "<< endl;
		cout<<"╰───────────────────────────────────────────────╯"<< endl;

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
	cout << "\033[32m Scan complete \033[0m" << endl; 
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

	/****Getting Keff***/	
	start = 0;
	getline(DecayDB, line);
	string tmp = StringLine::NextWord(line, start, ' ');
	vector<double> vKeff;
	if ( tmp == "keff"  || tmp == "Keff" )
	{
		while(start < (int)line.size())
		vKeff.push_back(atof(StringLine::NextWord(line, start, ' ').c_str()));
	}
	fkeff.push_back(vKeff);

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
	//cout<<MassOfThisFuel<< endl;
	fHMMass.push_back(MassOfThisFuel);

GoodJobName.push_back(jobname);

}
/*-------------------------------------------------------------------------------------------------
COMPILATION :

g++ -o Generate_MLP_Kinf Generate_MLP_Kinf.cxx `root-config --cflags` `root-config --libs`


*/
