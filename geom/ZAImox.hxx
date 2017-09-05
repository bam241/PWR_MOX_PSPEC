#ifndef _ZAImox_
#define _ZAImox_ 

#include <math.h>
#include <vector>
#include <iostream>
#include <string>
using namespace std;

class ZAImox
{
 public:

	ZAImox() {fZ=0;fA=0;fI=0;fProp=0;fHalfLifeTime=0;} //!< Default constructor
	ZAImox(int Z, int A, int I) {fZ=Z;fA=A;fI=I;fProp=0.0;fHalfLifeTime=0;}
	ZAImox(int Z, int A, int I, double Prop) {fZ=Z;fA=A;fI=I;fProp=Prop;fHalfLifeTime=0;}
//	~ZAI();


	int  Z(){return fZ;} //!< returns the number of protons
	int  A(){return fA;} //!< returns the number of nucleons
	int  I(){return fI;} //!< returns the Isomeric state (Ground State, ith excited)
	int  N(){return fA-fZ;} //!< returns the number of neutrons
	double Prop(){return fProp;}//!< get the proportion of a ZAI

	void Set(int Z, int A, int I, double p) {fZ=Z;fA=A;fI=I;fProp=p;}
	void SetZ(int Z){fZ=Z;} //!< Set the proton number
	void SetA(int A){fA=A;} //!< Set the nucleon number
	void SetI(int I){fI=I;} //!< Set the isomeric state (0=gs,...)
	void SetProp(double p){fProp=p;}//!< set the proportion of a ZAI

	void SetHalfLifeTime(double T_12){fHalfLifeTime=T_12;} //!< Set the Decay Half life [s]
	double GetHalfLifeTime(){return fHalfLifeTime;} //!< returns the Decay Half life [s] 
	void SetDecayConstant(double lambda){fHalfLifeTime=log(2.)/lambda;} //!< Set the Decay Constant
	double GetDecayConstant(){if (fHalfLifeTime)return log(2.)/fHalfLifeTime;return 0.;} //!< returns the Decay Half life
	
	void Stable(){fStable=true;}	//!< Set that a ZAI is stable to decay
	bool IsStable(){return fStable;}//!< Return whether a ZAI is stable

 protected:
	
	int	fZ;		//!< number of protons
	int	fA;		//!< number of nucleons (A=0 means natural isotopes) 
	int	fI;		//!< Isomeric state (Ground State, ith excited)
	double	fProp;	//!< Mass of a ZAI (from the BaseSummary.dat file
	double	fHalfLifeTime;	//!< Decay Half life constant 
	bool	fStable;	//!< whether a ZAI is stable

};
#endif
