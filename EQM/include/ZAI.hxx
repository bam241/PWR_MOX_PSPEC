#ifndef MN_ZAI_H_
#define MN_ZAI_H_

#include <map>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

class ZAI {
	public :
		int Z;
		int A;
		int I;

	
		ZAI(int z, int a, int i=0) { Z=z;A=a; I=i;}
		ZAI(){;}
	
		bool operator <(const ZAI& zai) const	{return (Z != zai.Z)?  
									(Z < zai.Z) : ( (A != zai.A)?
												 (A < zai.A) : (I < zai.I) );}
};

class IsotopicVector {
	public :
	
	IsotopicVector() {}	///< Normal Constructor.
 	~IsotopicVector() {};	 ///< Normal Destructor.
	double	GetZAIIsotopicQuantity(const ZAI& zai) const
	{
		map<ZAI ,double> IsotopicQuantity = IVquantity;
		
		map<ZAI ,double>::iterator it;
		it = IsotopicQuantity.find(zai);
		
		if ( it != IsotopicQuantity.end() )
			return it->second;
		else
			return 0;
	}
	vector<ZAI> GetNonZeroZAIList()
	{	
		vector<ZAI> vZAI;
		map<ZAI,double>::iterator it;
		for(it = IVquantity.begin(); it != IVquantity.end(); ++it)
		{	if(it->second != 0 ) 
			 vZAI.push_back(it->first);
		}
		return vZAI;	
	}

	map<ZAI,double> IVquantity;

};


class ZAIMass
{
 public:
	ZAIMass()
	{
		string  CLASSPATH = getenv("CLASS_PATH");
		string	MassDataFile = CLASSPATH + "/data/Mass.dat";
	
		ifstream infile(MassDataFile.c_str());	
	
		if(!infile.good())
		{	
			std::cout << " ZAIMass Error.\n can't find/open file " << MassDataFile << std::endl;
			exit(1);
		}
	
		int Z,A;
		string Name;
		double MassUnity,MassDec,error;
		while (infile>>Z>>A>>Name>>MassUnity>>MassDec>>error)
		{
			double Masse = MassUnity + MassDec * 1e-6;
			fZAIMass.insert( pair< ZAI,double >( ZAI(Z,A,0), Masse ) );
		}
	
		infile.close();
	}	
	~ZAIMass()
	{
	fZAIMass.clear();
	}

	double GetMass(ZAI zai )  //!< get with ZAI
	{
		map<ZAI,double>::iterator  MassIT = fZAIMass.find( ZAI(zai.Z, zai.A, 0) );

		if(MassIT == fZAIMass.end())
			return zai.A;
		else
		   return MassIT->second;

	}
	
	double GetMass(IsotopicVector & IV)     //return Mass of IV [t]
	{
		double AVOGADRO = 6.02214129e23;
		double TotalMass = 0;

		for( map<ZAI,double>::iterator it = IV.IVquantity.begin(); it != IV.IVquantity.end(); it++)
		{
			TotalMass += it->second/AVOGADRO * GetMass( it->first ) ;
		}
	
		return TotalMass*1e-6;
	}


	private:
		map<ZAI, double> fZAIMass; //! ZAI mass list

};

class ZAIStreamList {
	public :
	
	ZAI fZAI;
	string fListName;
	double fDefaultProportion;

	ZAIStreamList(int Z, int A, int I, string ListName, double DefaultProportion )
	{
		fZAI = ZAI(Z,A,I);
		fListName = ListName;
		fDefaultProportion = DefaultProportion;
	}	///< Normal Constructor.
 	~ZAIStreamList() {};	 ///< Normal Destructor.


};




#endif //MN_ZAI_H_
