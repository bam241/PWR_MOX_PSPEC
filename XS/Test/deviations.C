/***********************/
//  Root macro
//Plot & record distribution of mean (relative or absolute) 
// difference and the standard deviation of this difference
// between the MLP exectution and a set of known events
//Save the mean and the standard deviation in file :
//XS_accuracy.dat
// This file is a slighly modified version of the file
// $ROOTSYS/tmva/test/deviations.C
/***********************/
#include "TLegend.h"
#include "TText.h"
#include "TH2.h"
#include "tmvaglob.C"

enum HistType { MVAType = 0, ProbaType = 1, RarityType = 2, CompareType = 3 };

// input: - Input file (result from TMVA)
//        - use of TMVA plotting TStyle
void deviations( TString fin = "TMVAReg.root", 
                 HistType htype = MVAType, Bool_t showTarget, Bool_t useTMVAStyle = kTRUE ,Bool_t Absolute = kTRUE )
{
   // set style and remove existing canvas'
   TMVAGlob::Initialize( useTMVAStyle );
   gStyle->SetNumberContours(999);
 ofstream  Variance("XS_accuracy.dat",ios::app);

   // switches
   const Bool_t Save_Images     = kTRUE;

   // checks if file with name "fin" is already open, and if not opens one
   TFile* file = TMVAGlob::OpenFile( fin );  

   // define Canvas layout here!
   Int_t xPad = 1; // no of plots in x
   Int_t yPad = 1; // no of plots in y
   Int_t noPad = xPad * yPad ; 
   const Int_t width = 650;   // size of canvas

   // this defines how many canvases we need
   TCanvas* c[100];

   // counter variables
   Int_t countCanvas = 0;

   // search for the right histograms in full list of keys
   //    TList* methods = new TMap();

   TIter next(file->GetListOfKeys());
   TKey *key(0);   
   while ((key = (TKey*)next())) {

      if (!TString(key->GetName()).BeginsWith("Method_")) continue;
      if (!gROOT->GetClass(key->GetClassName())->InheritsFrom("TDirectory")) continue;

      TString methodName;
      TMVAGlob::GetMethodName(methodName,key);
      cout << "--- Plotting deviation for method: " << methodName << endl;

      TObjString *mN  = new TObjString( methodName );
      TDirectory* mDir = (TDirectory*)key->ReadObj();

      TList* jobNames = new TList();

      TIter keyIt(mDir->GetListOfKeys());
      TKey *titkey;
      while ((titkey = (TKey*)keyIt())) {

         if (!gROOT->GetClass(titkey->GetClassName())->InheritsFrom("TDirectory")) continue;

         TDirectory *titDir = (TDirectory *)titkey->ReadObj();

         TObjString *jN = new TObjString( titDir->GetName() );
         if (!jobNames->Contains( jN )) jobNames->Add( jN );
         else delete jN;
	    
         TString methodTitle;
         TMVAGlob::GetMethodTitle(methodTitle,titDir);

         TString hname = "MVA_" + methodTitle;
         TIter   dirKeyIt( titDir->GetListOfKeys() );
         TKey*   dirKey;

         Int_t countPlots = 0;
         while ((dirKey = (TKey*)dirKeyIt())){
            if (dirKey->ReadObj()->InheritsFrom("TH2F")) {
               TString s(dirKey->ReadObj()->GetName());
               if (s.Contains("_reg_") && 
                   ( (showTarget && s.Contains("_tgt")) || (!showTarget && !s.Contains("_tgt")) ) && 
                   s.Contains( (htype == CompareType ? "train" : "test" )))
               {
                  c[countCanvas] = new TCanvas( Form("canvas%d", countCanvas+1), 
                                                Form( "Regression output deviation versus %s for method: %s",
                                                      (showTarget ? "target" : "input variables"), methodName.Data() ),
                                                countCanvas*50+100, (countCanvas+1)*20, width, (Int_t)width*0.72 ); 
                  c[countCanvas]->SetRightMargin(0.10); // leave space for border

                  TH1* htmp = (TH1*)dirKey->ReadObj();

               int NbinX = htmp->GetNbinsX();
               int NbinY = htmp->GetNbinsY()*2;
               double XMin = htmp->GetXaxis()->GetBinLowEdge(0);
               double XMax = htmp->GetXaxis()->GetBinUpEdge(NbinX-1);

               double YMax =-1e10;
               double YMin =1e10;

               for(int binx=0;binx < htmp->GetNbinsX() ;binx++)
               {
                   double x = htmp->GetXaxis()->GetBinLowEdge(binx);

                   for(int biny=0;biny < htmp->GetNbinsY() ;biny++)
                  {
                        int Bin=htmp->GetBin(binx,biny);
                        double y = htmp->GetYaxis()->GetBinLowEdge(biny);
                        
                        if( (y/x > YMax) && (htmp->GetBinContent(Bin) !=0) )
                        {   YMax = y/x; }

                        if( (y/x < YMin)  && (htmp->GetBinContent(Bin) !=0) )
                        {   YMin = y/x ; }
                  }
               }

               double PerOneStep=0.004;
               NbinY = int( (YMax-YMin)/PerOneStep );

             //  TH2D* h=new TH2D("toto", "titi",NbinX, XMin , XMax,NbinY, YMin, YMax );

              TH2D* h=new TH2D("toto", "titi",50, XMin , XMax,20, YMin, YMax );

                for(int binx=0;binx < htmp->GetNbinsX() ;binx++)
                {
                        double x = htmp->GetXaxis()->GetBinLowEdge(binx);

                       for(int biny=0;biny < htmp->GetNbinsY() ;biny++)
                      {
                          int Bin=htmp->GetBin(binx,biny);

                          double y = htmp->GetYaxis()->GetBinLowEdge(biny);


                          h->Fill(x,y/x,htmp->GetBinContent(Bin));

                      }

                  }



                  h->SetTitle( Form("Relative Output deviation for method: %s (%s sample)", 
                                    hname.Data(), (htype == CompareType ? "training" : "test" )) );


                  htmp->SetTitle( Form("Output deviation for method: %s (%s sample)", 
                                    hname.Data(), (htype == CompareType ? "training" : "test" )) );


                  if(!Absolute)
                  {  
                      h->GetXaxis()->SetTitle(htmp->GetXaxis()->GetTitle());
                      h->GetYaxis()->SetTitle(htmp->GetYaxis()->GetTitle());

                        TH1D* h_py =  h->ProjectionY();
                        h_py->GetYaxis()->SetTitle(htmp->GetYaxis()->GetTitle());

                        h_py->Fit("gaus","M","",YMin,YMax);
                        TF1 *gfit= (TF1 *)h_py->GetFunction("gaus");
                        Variance<<htmp->GetXaxis()->GetTitle()<<" "<< gfit->GetParameter(1) <<" "<< gfit->GetParameter(2)<<endl;
                       // h_py->DrawCopy();

                   /*  h->Draw("colz");
                     TLine* l = new TLine( h->GetXaxis()->GetXmin(), 0, h->GetXaxis()->GetXmax(), 0 );
                     l->SetLineStyle(2);
                     l->Draw();
                    */ 
                  }
                  else
                  { 
                      htmp->Draw("colz");
                      TLine* l = new TLine( htmp->GetXaxis()->GetXmin(), 0, htmp->GetXaxis()->GetXmax(), 0 );
                      l->SetLineStyle(2);
                      l->Draw();
                  }  

                  // update and print
                  
                  cout << "plotting logo" << endl;
                  TMVAGlob::plot_logo(1.058);
                  c[countCanvas]->Update();

                  TString fname = Form( "plots/deviation_%s_%s_%s_c%i", 
                                        methodName.Data(), 
                                        (showTarget ? "target" : "vars"),
                                        (htype == CompareType ? "training" : "test" ), countPlots );
                 TMVAGlob::imgconv( c[countCanvas], fname );
              
                  //countPlots++;
                //  countCanvas++;
               }
            }
         }         
      }
   }
}




