#ifndef _STRINGLINE_
#define _STRINGLINE_

#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>
using namespace std;
/*!
 \file
 \brief Header file for StingLine class. 
*/

//! Class extracting fields from a string / line.
/*!
 The aim of this class is to provide tools to extract fields ("word") from
 a string and convert a string in Upper/Lower case. 
 All methods are static so that it is not necessary to create object to use them
 
 example:
 \code
 string line="The temperature is : 300.6 K";
 int start;

 1st method: creation of StringLine

 start=0;
 StringLine SL;
 string the=SL.NextWord(line,start);
 string temperature_is=SL.NextWord(line,start,':');
 string colon=SL.NextWord(line,start);
 double T=atof(SL.NextWord(line,start).c_str());
 cout<<the<<endl<<temperature_is<<endl<<T<<endl;
 
 2nd method: "using" the static methods
 
 start=0;
 the=StringLine::NextWord(line,start);
 temperature_is=StringLine::NextWord(line,start,':');
 colon=StringLine::NextWord(line,start);
 T=atof(StringLine::NextWord(line,start).c_str());
 cout<<the<<endl<<temperature_is<<endl<<T<<endl;
 \endcode
 @author PTO
 @version 2.01
*/

class StringLine
{
 public:
	//! Find the next word in a line.
	/*!
	 Find Next word in a line starting from position "start" in the line. If an alternative
	 separator is given, the word length is defined by the first position of sep or alt_sep found.
	 The first value of start is in general 0 (i.e. the beginning of the Line)
	 \param Line : a line containing words
	 \param start : from where to start to find the begining of a word
	 \param sep : the separator between 2 words (default=space)
	 \param alt_sep : the alternative separator between 2 words (default='')
	*/
	static string NextWord(string Line,int &start,char sep=' ', char alt_sep='\0');
	//! Find the previous word in a line.
	/*!
	 Find Previous word in a line starting from position "start" in the line. If an alternative
	 separator is given, the word length is defined by the first position of sep or alt_sep found.
	 The first value of start is in general the end of the Line.
	 \param Line : a line containing words
	 \param start : from where to start to find the begining of a word
	 \param sep : the separator between 2 words (default=space)
	 \param alt_sep : the alternative separator between 2 words (default='')
	*/
	static string PreviousWord(string Line,int &start,char sep=' ', char alt_sep='\0');
 	static void ToLower(string &Line); //!< convert a string to Lower case
 	static void ToUpper(string &Line); //!< convert a string to Upper case

	//! Find \p search in \p Line from the begining.
	/*!
	 returns the position, starting from the begenning of the first occurence 
	 of \p search in \p Line if it is found, else returns -1 
	 \param search : a string to find
	 \param Line : where to search
	*/
	static int Find(const char *search,string Line);
	//! Find \p search in \p Line from the end.
	/*!
	 returns the position, starting from the end of the first occurence 
	 of \p search in \p Line if it is found, else returns -1 
	 \param search : a string to find
	 \param Line : where to search
	*/
	static int rFind(const char *search,string Line);
	 //! convert a input type (\p in_T) to another (\p out_T).
	/*!
	 Example: 	
	 \code
	 string s="32.12";
	 double t=StringLine::convert<double>(s);
	 string temperature=StringLine::convert<string>(300.);
	 \endcode
	 \param t : the input value
	*/
	template <class out_T, class in_T> static  out_T convert(const in_T & t);
	//! Find the start of a word in a line.
	/*!
	 \param Line : a line containing words
	 \param CurrentPosition : from where to start to find the begining of a word
	 \param sep : the separator between 2 words (default=space)
	 \param alt_sep : the alternative separator between 2 words (default='')
	*/
 	static int GetStartWord(string Line,int CurrentPosition,char sep=' ', char alt_sep='\0');
	//! Find the end of a word in a line.
	/*!
	 \param Line : a line containing words
	 \param CurrentPosition : from where to start to find the end of a word
	 \param sep : the separator between 2 words (default=space)
	 \param alt_sep : the alternative separator between 2 words (default='')
	*/
	static int GetEndWord(string Line,int CurrentPosition,char sep=' ', char alt_sep='\0');
	//! Replace a sub-string by an other in a string.
	/*!
	 \param InLine : the string  which contains the sub-string to replace
	 \param ToReplace : the sub-string to replace
	 \param By : the sub-string  ToReplace is replaced by the sub-string By in Inline
	*/
	static string ReplaceAll(string InLine, string ToReplace, string By);
};


//_________________________________________________________________________________
inline string StringLine::NextWord(string Line,int &start,char sep, char alt_sep)
{
	string Word="";
	if(start>=int(Line.size())) 
	{
		return Word;
	}
	start=GetStartWord(Line,start,sep,alt_sep);
	int wordlength=GetEndWord(Line,start,sep,alt_sep)-start;
	
	Word=Line.substr(start,wordlength);
	
	start+=wordlength;
	return Word;
}
//_________________________________________________________________________________
inline string StringLine::PreviousWord(string Line,int &start,char sep, char alt_sep)
{
	string Word="";
	if(start<=0) 
	{
		return Word;
	}
	int pos=Line.rfind(sep,start);
	int alt_pos=-1;
	int real_pos=pos;
	char real_sep=sep;
	if(alt_sep!='\0')
	{
		alt_pos=Line.rfind(alt_sep,start);
		real_pos=max(pos,alt_pos);
		if(real_pos!=pos)
			real_sep=alt_sep;
	}
	int wordlength=start-Line.rfind(real_sep,real_pos);
	if(real_pos<=0)
	{
		Word=Line.substr(0,start+1);
		start=0;
		return Word;
	}
	Word=Line.substr(real_pos+1,wordlength);
	
	start-=wordlength+1;
	return Word;
}
	
//_________________________________________________________________________________
inline void StringLine::ToLower(string &Line)
{
	transform (Line.begin(), Line.end(),	// source
				Line.begin(),				// destination
				(int(*)(int))tolower);		// operation
}

//_________________________________________________________________________________
inline void StringLine::ToUpper(string &Line)
{
	transform (Line.begin(), Line.end(),	// source
				Line.begin(),				// destination
				(int(*)(int))toupper);		// operation
}

//_________________________________________________________________________________
inline int StringLine::GetStartWord(string Line,int CurrentPosition,char sep, char alt_sep)
{
	int pos=Line.find(sep,CurrentPosition);
	int alt_pos=-1;
	if(alt_sep!='\0')
		alt_pos=Line.find(alt_sep,CurrentPosition);
	int real_pos=pos;
	char real_sep=sep;
	if(alt_pos>=0)
	{
		real_pos=min(pos,alt_pos);
		if(pos==int(string::npos))real_pos=alt_pos;
		if(real_pos!=pos)
			real_sep=alt_sep;
	}
	if(real_pos==int(string::npos)) return CurrentPosition;
	while(CurrentPosition<int(Line.size()) && Line[CurrentPosition]==real_sep)
		CurrentPosition++;
	return CurrentPosition;
}

//_________________________________________________________________________________
inline int StringLine::GetEndWord(string Line,int CurrentPosition,char sep, char alt_sep)
{
	int pos=Line.find(sep,CurrentPosition);
	int alt_pos=-1;
	if(alt_sep!='\0')
		alt_pos=Line.find(alt_sep,CurrentPosition);
	int real_pos=pos;
	if(alt_pos>=0)
	{
		real_pos=min(pos,alt_pos);
		if(pos==int(string::npos))real_pos=alt_pos;
	}
	if(real_pos==int(string::npos))
		return Line.size();
	return real_pos;
}

//_________________________________________________________________________________
inline int StringLine::Find(const char *search,string Line)
{
	size_t Pos=Line.find(search);
	if(Pos != string::npos ) return Pos;
	return -1;
}

//_________________________________________________________________________________
inline int StringLine::rFind(const char *search,string Line)
{
	size_t Pos=Line.rfind(search);
	if(Pos != string::npos) return Pos;
	return -1;
}

//_________________________________________________________________________________
template <class out_T, class in_T>
inline out_T StringLine::convert(const in_T & t)
{
	stringstream stream;
	stream << t; 		// insert value to stream
	out_T result; 		// store conversion's result here
 	stream >> result; 	// write value to result
	return result;
}

//_________________________________________________________________________________
inline string StringLine::ReplaceAll(string InLine, string ToReplace, string By)
{
	int start=0;
	int pos=InLine.find(ToReplace,start);
	while(pos!=int(string::npos))
	{
		InLine.replace(pos,ToReplace.size(),By);
		start=0;
		pos=InLine.find(ToReplace,start);
	}
	return InLine;
	
}
#endif
