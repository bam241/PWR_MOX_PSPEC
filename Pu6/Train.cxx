//
// This program train and test a MLP using TMVA from a training data in a form of a TTRee
//
#include <cstdlib>
#include <iostream> 
#include <map>
#include <string>
#include <sstream>
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"


#if not defined(__CINT__) || defined(__MAKECINT__)
#include "TMVA/Tools.h"
#include "TMVA/Factory.h"
#endif

using namespace TMVA;
   
int main(int argc, char const *argv[])
{

   //---------------------------------------------------------------
   // This loads the library
   TMVA::Tools::Instance();
   // ---------------------------------------------------------------

   std::string TargetName = "pu6_e";


   std::cout << std::endl;
   std::cout << "==> Start TMVARegression" << std::endl;


   // Create a new root output file
   TString outfileName( "Training_output_"+TargetName+".root" );
   TFile* outputFile = TFile::Open( outfileName, "RECREATE" );//UPDATE


   // The first argument is the base of the name of all the
   // weightfiles in the directory weight/ 
   //
   // The second argument is the output file for the training results
   // All TMVA output can be suppressed by removing the "!" (not) in 
   // front of the "Silent" argument in the option string

   TMVA::Factory *factory = new TMVA::Factory( "TMVARegression", outputFile, 
                                               "!V:!Silent:Color:DrawProgressBar" );


   #include "_tmp/include_Train/InputVariables.cxx"

   // Add the variable carrying the regression target
   factory->AddTarget(TargetName);

   // It is also possible to declare additional targets for multi-dimensional regression, ie:
   // -- factory->AddTarget( "fvalue2" );
   // BUT: this is currently ONLY implemented for MLP

   // Read training and test data (see TMVAClassification for reading ASCII files)
   // load the signal and background event samples from ROOT trees
   TFile *input(0);
   TString fname = "_tmp/include_Train/TrainingInput.root"; //Training Data input file
   if (!gSystem->AccessPathName( fname )) 
      input = TFile::Open( fname ); // check if file in local directory exists 
   
   if (!input) {
      std::cout << "ERROR: could not open data file" << std::endl;
      exit(1);
   }
   std::cout << "--- TMVARegression           : Using input file: " << input->GetName() << std::endl;

   // --- Register the regression tree

   TTree *regTree = (TTree*)input->Get("Data");

   // global event weights per tree (see below for setting event-wise weights)
   Double_t regWeight  = 1.0;   

   // You can add an arbitrary number of regression trees
   factory->AddRegressionTree( regTree, regWeight );

   // This would set individual event weights (the variables defined in the 
   // expression need to exist in the original TTree)
  // factory->SetWeightExpression( "var1", "Regression" );

   // Apply additional cuts on the signal and background samples (can be different)
   TCut mycut = ""; // for example: TCut mycut = "abs(var1)<0.5 && abs(var2-0.5)<1";

   Long64_t NEvents   = regTree->GetEntries();
   Long64_t NTraining = PropTraining * NEvents ; 
   Long64_t NTesting  = NEvents - NTraining ; 

   std::stringstream Samples_Parameters ;
   Samples_Parameters <<  "nTrain_Regression=" << NTraining <<":"<< "nTest_Regression=" << NTesting <<":SplitMode=Random:NormMode=NumEvents:!V";


   // tell the factory to use all remaining events in the trees after training for testing:
   factory->PrepareTrainingAndTestTree( mycut, 
                                        "nTrain_Regression=0:nTest_Regression=0:SplitMode=Random:NormMode=NumEvents:!V" );

   // If no numbers of events are given, half of the events in the tree are used 
   // for training, and the other half for testing:

   // ---- Book MVA methods
   //
   TString NType    = "sigmoid";
   TString Arch     = "18,21,9";
   TString Sampl    = "0.5";
   TString LearRate = "0.1";
   
   TString ParamNN = "!H:V:VarTransform=Norm:NeuronType=" + NType + ":NCycles=16000:HiddenLayers=" + Arch + ":TestRate=6:TrainingMethod=BFGS:LearningRate=" + LearRate + ":Sampling=" + Sampl + ":SamplingEpoch=0.8:ConvergenceImprove=1e-6:ConvergenceTests=15:!UseRegulator";
   
   std::stringstream Name;
   Name<<"MLP_"<<TargetName; 
   // Neural network (MLP)                                                                                 
   factory->BookMethod( TMVA::Types::kMLP, Name.str().c_str(), ParamNN );
   // --------------------------------------------------------------------------------------------------

   // ---- Now you can tell the factory to train, test, and evaluate the MVAs

   // Train MVAs using the set of training events
   factory->TrainAllMethods();

   // ---- Evaluate all MVAs using the set of test events
   factory->TestAllMethods();

   // ----- Evaluate and compare performance of all configured MVAs
   factory->EvaluateAllMethods();    

   // --------------------------------------------------------------
   
   // Save the output
   outputFile->Close();

   std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
   std::cout << "==> TMVARegression is done!" << std::endl;      

   delete factory;


}
/*

g++ -o Train `root-config --cflags` Train.cxx `root-config --glibs` -lTMVA -I$ROOTSYS/tmva/test/
*/
