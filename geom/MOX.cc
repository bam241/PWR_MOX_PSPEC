#include "BinaryFormat2.hxx"
#include "MureHeaders.hxx"
#include "StringLine.hxx"
#include "ZAImox.hxx"

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iostream>
#include <sstream>
#include <sstream>
#include <string>
#include <vector>

using std::string;
using namespace std;
int step_to_skip = 0;

string s_OUT;

bool IsEvolution = true;
bool kcode = false;

double NAssembly = 157;  // CP 157 - P/P' 193 - N 205

double RealActiveHeight =
    3.66;  // 3.66 pour le palier CP, 4.267 pour les pallierrs N et P
double UsedActiveHeight = RealActiveHeight / 10.;

double CorePower = 2785 * 1.e6;  // Power in the Core - 2785 MW pour CP - 3817
                                 // MW pour P/P' - 4250 MW pour N
double AssemblyPower = CorePower / NAssembly;  // Power in an Assembly

double Power =  Power = AssemblyPower / RealActiveHeight *
               UsedActiveHeight;  // Real Power in the simulated Cube

double Efficiciency = 0.34;  // From NEA Benchmark
double PowerElecReactor = 1.5e9;  // From NEA Benchmark
double tHMReactor = 128.9;  // From NEA Benchmark 128.9t
double PowerDensity = PowerElecReactor / (Efficiciency * tHMReactor);

double U5_frac = 0.0025;
double Enricht = 0.05;

int NTotalCycle = 20;
int NInactiveCycle = 5;
int NPart = 10000;

int NCore = 1;

void WriteDB();

int ReadCommentVersion(ifstream& in, string filename) {
  if (!in.good()) {
    cerr << "ERROR in MureRead:: Cannot find ASCII file " << filename
         << " to Read" << endl;
    return -1;
  }
  //
  // Go throught the comments at the beginning of the file and Read the version
  // number of Data Format
  //
  string TheComment;
  int MureDataVersion;
  bool bComment = true;
  while (bComment) {
    in >> TheComment;
    if (TheComment == "%") {
      getline(
          in,
          TheComment);  // the "in>>" command keep the cursor at the end of line
    } else if (TheComment == "V") {
      in >> MureDataVersion;
      bComment = false;
    } else {
      cout << "No comments or format version at the beginning of data files"
           << endl;
      bComment = false;
      MureDataVersion = 0;
    }
  }
  return MureDataVersion;
}
string itoa(double num) {
  ostringstream os(ostringstream::out);
  os << "" << num;
  return os.str();
}

string dtoa(double num) {
  ostringstream os(ostringstream::out);
  os << setprecision(2) << num;
  return os.str();
}

string PerformDB(char** argv);

template <class T>
T random(T a, T b)  // peak random numebr between a and b
{
  double range = pow(2., 31);
  return (T)a + (T)(b - a) * rand() / range;
}

int main(int argc, char** argv) {
  PerformDB(argv);

  WriteDB();
}

string PerformDB(char** argv) {
  double Pi = 3.14159;

  //==========================================================================================
  // Compo
  //==========================================================================================

  U5_frac = atof(argv[1]);
  Enricht = atof(argv[2]);
  double N_tot = 0;
  double N_add = 0;
  double N_U5 = (1 - Enricht) * U5_frac;  // 3
  double N_U8 = (1 - Enricht) * (1 - U5_frac);  // 5
  N_tot += N_U8 + N_U5;

  double N_Pu6 = Enricht * atof(argv[3]);
  double N_Pu8 = Enricht * atof(argv[4]);
  double N_Pu0 = Enricht * atof(argv[5]);
  double N_Pu1 = Enricht * atof(argv[6]) * atof(argv[7]);
  double N_Pu2 = Enricht * atof(argv[8]);

  N_add += N_Pu6 + N_Pu8 + N_Pu0 + N_Pu1+ N_Pu2;

  double N_Am1 = Enricht * atof(argv[6]) * (1 - atof(argv[7]));
  double N_Am2 = 0;//Enricht * atof(argv[9]);
  double N_Am3 = 0;//Enricht * atof(argv[10]);
  N_add += N_Am1 + N_Am2 + N_Am3;

  double N_Np7 = 0;//Enricht * atof(argv[11]);
  N_add += N_Np7;

  double N_Cm2 = 0;//Enricht * atof(argv[12]);
  double N_Cm3 = 0;//Enricht * atof(argv[13]);
  double N_Cm4 = 0;//Enricht * atof(argv[14]);
  double N_Cm5 = 0;//Enricht * atof(argv[15]);
  double N_Cm6 = 0;//Enricht * atof(argv[16]);
  N_add += N_Cm2 + N_Cm3 + N_Cm4 + N_Cm5 + N_Cm6;
  cout << "!!!!!!!!!! N_Add !!!!!!!!!!!" << N_add/Enricht << endl;
  double N_Pu9 = (1-N_add/Enricht) * Enricht;
  N_add += N_Pu9;
  N_tot += N_add;


  PowerDensity = atof(argv[9]);


  // !!! Should have  Sum of HN = 1
  std::cout << "N_U5 "  << N_U5 << " "
       << "N_U8 "  << N_U8 << " "
       << "N_Pu8 " << N_Pu8 << " "
       << "N_Pu6 " << N_Pu6 << " "
       << "N_Pu9 " << N_Pu9 << " "
       << "N_Pu0 " << N_Pu0 << " "
       << "N_Pu1 " << N_Pu1 << " "
       << "N_Pu2 " << N_Pu2 << " "
       << "N_Am1 " << N_Am1 << " "
       //<< "N_Am2 " << N_Am2 << " "
       //<< "N_Am3 " << N_Am3 << " "
       //<< "N_Np7 " << N_Np7 << " "
       //<< "N_Cm2 " << N_Cm2 << " "
       //<< "N_Cm3 " << N_Cm3 << " "
       //<< "N_Cm4 " << N_Cm4 << " "
       //<< "N_Cm5 " << N_Cm5 << " "
       //<< "N_Cm6 " << N_Cm6 << " "
       << "N_tot " << N_tot << std::endl;

  // Normalisation
  N_U5  = N_U5  / N_tot;
  N_U8  = N_U8  / N_tot;

  N_Pu6 = N_Pu6 / N_tot;
  N_Pu8 = N_Pu8 / N_tot;
  N_Pu9 = N_Pu9 / N_tot;
  N_Pu0 = N_Pu0 / N_tot;
  N_Pu1 = N_Pu1 / N_tot;
  N_Pu2 = N_Pu2 / N_tot;

  N_Am1 = N_Am1 / N_tot;
  N_Am2 = N_Am2 / N_tot;
  N_Am3 = N_Am3 / N_tot;

  N_Np7 = N_Np7 / N_tot;

  N_Cm2 = N_Cm2 / N_tot;
  N_Cm3 = N_Cm3 / N_tot;
  N_Cm4 = N_Cm4 / N_tot;
  N_Cm5 = N_Cm5 / N_tot;
  N_Cm6 = N_Cm6 / N_tot;

  s_OUT = "";
  s_OUT +="U5_"  + dtoa(N_U5 *100) + "_";
  s_OUT +="8_"   + dtoa(N_U8 *100) + "_";
  s_OUT +="P6_" + dtoa(N_Pu6*100) + "_";
  s_OUT +="8_"   + dtoa(N_Pu8*100) + "_";
  s_OUT +="9_"   + dtoa(N_Pu9*100) + "_";
  s_OUT +="0_"   + dtoa(N_Pu0*100) + "_";
  s_OUT +="1_"   + dtoa(N_Pu1*100) + "_";
  s_OUT +="2_"   + dtoa(N_Pu2*100) + "_";
  s_OUT +="A1_" + dtoa(N_Am1*100) + "_";
  //s_OUT +="2_"   + dtoa(N_Am2*100) + "_";
  //s_OUT +="3_"   + dtoa(N_Am3*100) + "_";
  //s_OUT +="N7_" + dtoa(N_Np7*100) + "_";
  //s_OUT +="C2_" + dtoa(N_Cm2*100) + "_";
  //s_OUT +="3_"   + dtoa(N_Cm3*100) + "_";
  //s_OUT +="4_"   + dtoa(N_Cm4*100) + "_";
  //s_OUT +="5_"   + dtoa(N_Cm5*100) + "_";
  //s_OUT +="6_"   + dtoa(N_Cm6*100) + "_";
  s_OUT +="P_"   + dtoa(PowerDensity*1e-6);
  //==========================================================================================
  // Miscellaneous for MURE
  //==========================================================================================

  gMURE->SetVolumeNPS(20000000);
  gMURE->SetFissionReleasedFile("Fission.dat");

  gMURE->SetAutoXSDIR();
  gMURE->SetRemove_r_files();
  gMURE->SetMCNPRunDirectory(s_OUT.c_str(), "CREATE");

  string PATHCMD =
      "ln -sf $DATAPATH DATAPATH; ln -fs $DATAPATH " + s_OUT + "/DATAPATH";

  // gMURE->SetMCNPRunDirectory("TEST","CREATE");
  // string PATHCMD = "\\rm DATAPATH; ln -s $DATAPATH DATAPATH; ln -s $DATAPATH
  // TEST/DATAPATH";
  system(PATHCMD.c_str());

  gMURE->SetComment("REP Assembly Calculation MOX - Cube");

  gMURE->SetMCNPExec("mcnp6 xsdir=xsdir");

  // gMURE->SetDATADIR("/scratch/spallati/soft/SHARE/PROG/MURE/SL3/data/");
  // gMURE->SetDATADIR("/scratch/spallati/soft/SHARE/PROG/MURE/SL3/data.ENDFB.7.1/");
  gMURE->SetDATADIR(
//    "/home/mouginot/work/app/MURE/data.JEFF_3.1.1");
     "/tmp/mouginot/opt/mure/data.JEFF_3.1.1");


  BasePriority* BP = new BasePriority();
  // BP->SetBuilderPriority("JOY");
  BP->SetBasePriority(0, "JEFF");
  BP->SetVersionPriority(0, "3.1.1", 0);
  // BP->SetBasePriority(0,"ENDF");
  // BP->SetVersionPriority(0,"7.1",0);
  BP->SetBasePriority(1, "THERMAL");
  gMURE->SetBasePriority(BP);
  gMURE->SetSpectrumType("thermal");

  gMURE->SetShortestHalfLife(32400);
  gMURE->SetReactionThreshold(1e-6);
  gMURE->GetTemperatureMap()->SetDeltaTPrecision(1500);


  gMURE->UseMultiGroupTallies();
  gMURE->UsePreviousRunSource();

  gMURE->SetOMP(NCore);

  //==========================================================================================
  // Temperature in Kelvin
  //==========================================================================================

  //==========================================================================================
  // Geometric data size
  //==========================================================================================

  // The bath   /bain
  double Bath_H = UsedActiveHeight;

  // The vessel  / vessel partie qui contient les assemblages correspond a la
  // hauteur d un assemblage
  double Vessel_H = UsedActiveHeight;

  // The assembly    /assemblage
  double Ass_Structure_Thickness = 0.00475 - 0.0041;

  // The Pin   /crayon de combustible
  double Pin_ext = 0.01262;  // exterieur du carre
  double Pin_R_ext = 0.0041;  // exterieur combustible
  double Pin_Void_Thickness = 0;  // epaisseur air entre combustible et gaine
  double Pin_Steel_Thickness = 0.00475 - 0.0041;  // gaine

  // Calculated parameters
  double Ass_in = Pin_ext * 17;
  double Vessel_H_In = Vessel_H;

  double fGuideTubeInnerRadius = 0.01110 / 2.0;
  double fGuideTubeOuterRadius = 0.01232 / 2.0;

  // Miscellaneous
  int nb_pin = 0;

  //==========================================================================================
  // Shapes
  //==========================================================================================

  // Assembly
  Shape_ptr s_Assembly(new Brick(Ass_in / 2., Ass_in / 2., (Bath_H) / 2., -1));
  Shape_ptr s_exterior(!s_Assembly);
  s_Assembly->SetMirrorBoundary();

  // Pins
  // The generator
  Shape_ptr s_Pin_Generator(
      new Brick(Pin_ext / 2., Pin_ext / 2., (Bath_H) / 2., -1));
  s_Pin_Generator->SetUniverse();
  s_Pin_Generator >> s_Assembly;

  Shape_ptr s_Pin_Cladding(
      new Tube((Bath_H) / 2., Pin_R_ext + Pin_Steel_Thickness));
  s_Pin_Cladding->SetUniverse();

  Shape_ptr s_Pin_Pellet(new Tube((Bath_H) / 2., Pin_R_ext));
  s_Pin_Pellet->SetUniverse(s_Pin_Cladding->GetUniverse());

  s_Pin_Pellet >> s_Pin_Cladding >> s_Pin_Generator;
  Shape_ptr s_Pin;
  s_Pin = s_Pin_Pellet | s_Pin_Cladding;

  // GuideTube
  Shape_ptr s_Tube_Cladding(new Tube(Vessel_H / 2., fGuideTubeOuterRadius));
  s_Tube_Cladding->SetUniverse();

  Shape_ptr s_Tube_Void_in(new Tube(Vessel_H / 2., fGuideTubeInnerRadius));
  s_Tube_Void_in->SetUniverse(s_Tube_Cladding->GetUniverse());

  s_Tube_Void_in >> s_Tube_Cladding;
  Shape_ptr s_Tube;
  s_Tube = s_Tube_Void_in | s_Tube_Cladding;
  Shape_ptr s_Whole(new Node(-1));

  s_Whole->SetUniverse();

  //==========================================================================================
  // Densities
  //==========================================================================================



  // Fuel
  double T_Fuel = 923;
  double Rho_Fuel = 10.02;  // T MOX at 900 K -> from chaudire....
  Material* m_Fuel = new Material();
  m_Fuel->SetTemperature(T_Fuel);
  m_Fuel->SetDensity(Rho_Fuel);
  // Uranium
  m_Fuel->AddNucleus(92, 238, 0, N_U8);
  m_Fuel->AddNucleus(92, 235, 0, N_U5);
  // Plutonium
  m_Fuel->AddNucleus(94, 238, 0, N_Pu8);
  m_Fuel->AddNucleus(94, 239, 0, N_Pu9);
  m_Fuel->AddNucleus(94, 240, 0, N_Pu0);
  m_Fuel->AddNucleus(94, 241, 0, N_Pu1);
  m_Fuel->AddNucleus(94, 242, 0, N_Pu2);
  // Americium
  m_Fuel->AddNucleus(95, 241, 0, N_Am1);
  m_Fuel->AddNucleus(95, 242, 1, N_Am2);
  m_Fuel->AddNucleus(95, 243, 0, N_Am3);
  // Neptunium
  m_Fuel->AddNucleus(93, 237, 0, N_Np7);
  // Curium
  m_Fuel->AddNucleus(96, 242, 0, N_Cm2);
  m_Fuel->AddNucleus(96, 243, 0, N_Cm3);
  m_Fuel->AddNucleus(96, 244, 0, N_Cm4);
  m_Fuel->AddNucleus(96, 245, 0, N_Cm5);
  m_Fuel->AddNucleus(96, 246, 0, N_Cm6);
  // Oxygen
  m_Fuel->AddNucleus(8, 16, 2);
  m_Fuel->SetFuel();
  if (IsEvolution) m_Fuel->SetEvolution();

  // Caloporteur
  double Bore_uma = 11009305.406e-6 * 0.801 + 10012936.992e-6 * 0.199;
  double H2O_uma = 18.0153;
  double Bore_MassicFraction = 600. / 1000000;
  double H2O_MassicFraction = 1 - Bore_MassicFraction;

  double T_Coolant = 578;
  double Rho_Coolant = 0.716;
  Material* m_Coolant = new Material();
  m_Coolant->SetTemperature(T_Coolant);
  m_Coolant->SetDensity(Rho_Coolant);
  m_Coolant->AddNucleus(8, 16, 1 * H2O_MassicFraction / H2O_uma, kpMOL);

  m_Coolant->AddNucleus(1, 1, 2 * H2O_MassicFraction / H2O_uma, kpMOL, "H2O");
  //	m_Coolant->GetNucleus(1,1)->SetModeratorName("lwtr.16t");
  // m_Coolant->GetNucleus(1,1)->SetModeratorName("lwtr.04t");

  m_Coolant->AddNucleus(5, 11, 0.801 * Bore_MassicFraction / Bore_uma, kpMOL);
  m_Coolant->AddNucleus(5, 10, 0.199 * Bore_MassicFraction / Bore_uma, kpMOL);
  m_Coolant->SetTemperatureEvolution();
  m_Coolant->SetCoolant();
  m_Coolant->SetModerator();

  // Acier MT5 zircaloy
  // Pin
  double Rho_Steel_Pin = 5.762925;
  double T_Steel_Pin = 577;  // 470°C
  Material* m_Steel_Pin = new Material();
  m_Steel_Pin->SetTemperature(T_Steel_Pin);
  m_Steel_Pin->SetDensity(Rho_Steel_Pin);
  m_Steel_Pin->AddNucleus(8, 16, 0.00125, kpMASS);
  m_Steel_Pin->AddNucleus(24, 50, 0.001 * 0.0431, kpMASS);
  m_Steel_Pin->AddNucleus(24, 52, 0.001 * 0.83789, kpMASS);
  m_Steel_Pin->AddNucleus(24, 53, 0.001 * 0.09501, kpMASS);
  m_Steel_Pin->AddNucleus(24, 54, 0.001 * 0.02365, kpMASS);
  m_Steel_Pin->AddNucleus(26, 54, 0.00210 * 0.05845, kpMASS);
  m_Steel_Pin->AddNucleus(26, 56, 0.00210 * 0.9172, kpMASS);
  m_Steel_Pin->AddNucleus(26, 57, 0.00210 * 0.022, kpMASS);
  m_Steel_Pin->AddNucleus(26, 58, 0.00210 * 0.0028, kpMASS);
  m_Steel_Pin->AddNucleus(40, 90, 0.98115 * 0.5145, kpMASS);
  m_Steel_Pin->AddNucleus(40, 91, 0.98115 * 0.1122, kpMASS);
  m_Steel_Pin->AddNucleus(40, 92, 0.98115 * 0.1715, kpMASS);
  m_Steel_Pin->AddNucleus(40, 94, 0.98115 * 0.1738, kpMASS);
  m_Steel_Pin->AddNucleus(40, 96, 0.98115 * 0.028, kpMASS);
  m_Steel_Pin->AddNucleus(50, 120, 0.0145, kpMASS);  // Fe
  m_Steel_Pin->SetTemperatureEvolution();
  m_Steel_Pin->SetCladding();

  //==========================================================================================
  // Cells  //remplis avec le motif defini et verifie qu il peut remplir, a
  // modifier
  //==========================================================================================

  //==========================================================================================
  // & The source
  //==========================================================================================

  MCNPSource* s = new MCNPSource(NPart);
  s->SetKcode(NTotalCycle, NInactiveCycle, 1.0);

  // exterior
  Cell* c_exterior = new Cell(s_exterior, 0, 0);
  c_exterior->SetComment("The exterior");

  // Assembly
  Cell* c_Assembly = new Cell(s_Assembly);
  c_Assembly->SetVolume(16.846e-3);
  c_Assembly->SetComment("The Assembly");

  // Pins lattice    //maillageIsHexagonInHexagon
  int N_range_pin = int(Ass_in / (Pin_ext) + 1);

  Cell* c_Pin_Generator = new Cell(s_Pin_Generator);
  c_Pin_Generator->Lattice(1, -N_range_pin / 2, N_range_pin / 2,
                           -N_range_pin / 2, N_range_pin / 2);

  for (int i = -N_range_pin / 2; i <= N_range_pin / 2; i++) {
    for (int j = -N_range_pin / 2; j <= N_range_pin / 2; j++) {
      int pos[3] = {i, j, 0};
      double xt = Pin_ext * i;
      double yt = Pin_ext * j;
      double X[2] = {xt, yt};
      if (fabs(X[0]) <= (Ass_in / 2 - Pin_ext / 2) &&
          fabs(X[1]) <= (Ass_in / 2 - Pin_ext / 2))  // si le centre du crayon
                                                     // est au moins eloigne de
                                                     // la taille du crayon/2 a
                                                     // la limite interieure de
                                                     // l assemblage
      {
        if ((abs(i) == 6 && abs(j) == 6) /*|| (abs(i)==0 && abs(j)==0)*/) {
          c_Pin_Generator->FillLattice(s_Pin_Cladding, pos);
          nb_pin++;
          Tube* SourceTube = new Tube(Bath_H / 2., Pin_R_ext);
          SourceTube->Translate(xt, yt, 0);
          s->AddTubeSource(SourceTube, "2e6");  // neutron de 2Mev repartie de
                                                // façon homogène dans les
                                                // crayons
        } else if (((abs(i) % 3 == 0 && abs(j) % 3 == 0) ||
                    (abs(i) == 5 && abs(j) == 5)))
          c_Pin_Generator->FillLattice(s_Tube_Cladding, pos);
        else {
          c_Pin_Generator->FillLattice(s_Pin_Cladding, pos);
          nb_pin++;
          Tube* SourceTube = new Tube(Bath_H / 2., Pin_R_ext);
          SourceTube->Translate(xt, yt, 0);
          s->AddTubeSource(SourceTube, "2e6");  // neutron de 2Mev repartie de
                                                // façon homogène dans les
                                                // crayons
        }
      } else
        c_Pin_Generator->FillLattice(s_Whole, pos);
    }
  }

  gMURE->SetSource(s);

  Cell* c_Pin_Cladding = new Cell(s_Pin_Cladding, m_Steel_Pin);
  c_Pin_Cladding->SetComment("The Pin Cladding - Acier MT5 zircaloy");

  Cell* c_Pin_Pellet = new Cell(s_Pin_Pellet, m_Fuel);
  c_Pin_Pellet->SetVolume(5.12108e-3);
  c_Pin_Pellet->SetComment("The Fuel Pellet");

  Cell* c_Pin_Ext = new Cell(!s_Pin, m_Coolant);
  c_Pin_Ext->SetComment("The Pin Ext");

  Cell* c_Tube_Cladding = new Cell(s_Tube_Cladding, m_Steel_Pin);
  c_Tube_Cladding->SetComment("The Tube Cladding");

  Cell* c_Tube_Void_in = new Cell(s_Tube_Void_in, m_Coolant);
  c_Tube_Void_in->SetComment("The Tube In Void - not present ");

  Cell* c_Tube_Ext = new Cell(!s_Tube, m_Coolant);
  c_Tube_Ext->SetComment("The Tube Ext");

  Cell* c_Pin_Void = new Cell(s_Whole, m_Coolant);
  c_Pin_Void->SetComment("Empty - no Tube no Pin");

  //==========================================================================================
  // Build and run the calculation //idem
  //==========================================================================================


  if (kcode == true)
  {
    gMURE->RunMCNP();
    gMURE->BuildMCNPFile();
  }
  if (IsEvolution == false) exit(1);

  //==========================================================================================
  // The Evolution For Cross section or Core Evolution
  //==========================================================================================

  vector<double> Time;  // vecteur divisant les pas en temps
  double BurnupMax = 75e9;
  double Tdays = 24. * 3600.;
  int NMCNPIrradiationStep = 80;

  //***************Equ Xénon identique pour chaque cas******************///
  int NMCNPXenon = 6;
  int TimeStepXenon = 1;  // pas de 1 jour pour le Xénon

  double TEnd = (BurnupMax) / (PowerDensity)*Tdays;
  double TXenon = (NMCNPXenon - 1) * Tdays;

  for (int i = 0; i < NMCNPXenon; i++) {
    int Time_day = TimeStepXenon * i;
    Time.push_back(Time_day * Tdays);
    cout << Time.back() << endl;
  }

  double TimeStep = (TEnd - TXenon) / (NMCNPIrradiationStep - NMCNPXenon);

  int k = 1;
  for (int i = NMCNPXenon; i < NMCNPIrradiationStep; i++) {
    Time.push_back(k * TimeStep + Time[NMCNPXenon - 1]);
    k++;
    cout << Time.back() << endl;
  }
  // Evolution du cooling
  gMURE->SetPower(PowerDensity * gMURE->GetInitialHNMass());
  Power = PowerDensity * gMURE->GetInitialHNMass();
  gMURE->Evolution(Time);

  return s_OUT;
}

void WriteDB() {
  // =========================================================================================
  //  VARIABLES
  // =========================================================================================
  string sPWD = getenv("PWD");

  string s_tmp = "";
  int i_tmp = 0;
  double tmp = 0;
  string DataASCIIFile = "DATA_";
  int NDataASCIIFile = 0;
  string DataBinFile = "BDATA_";
  int NDataBinFile = 0;
  string DataFile = "";
  int NDataFile = 0;
  string LastDataFile = "";

  string OutDataFile = "";
  string OutDataFileInfo = "";
  string OutLOG = "";

  bool IsThereASCCIFile = false;
  bool IsThereBinFile = false;

  vector<int> vCellNumber;
  vector<string> vCellComment;

  int NumberOfCells = 1;

  vector<double> vTime;
  vector<double> vKeff;
  vector<double> vFlux;
  vector<vector<double> > vFlux1;

  vector<ZAImox> zai1;
  vector<vector<ZAImox> > zai2;
  vector<vector<vector<ZAImox> > > zai3;

  vector<ZAImox> zai_FS_1;
  vector<vector<ZAImox> > zai_FS_2;
  vector<vector<vector<ZAImox> > > zai_FS_3;

  vector<ZAImox> zai_SC_1;
  vector<vector<ZAImox> > zai_SC_2;
  vector<vector<vector<ZAImox> > > zai_SC_3;

  vector<ZAImox> zai_SN_1;
  vector<vector<ZAImox> > zai_SN_2;
  vector<vector<vector<ZAImox> > > zai_SN_3;

  vector<bool> HasToBePrint;

  // =========================================================================================
  //  WELCOME !
  // =========================================================================================

  string DBPath = sPWD + "/" + s_OUT;
  string OutName = s_OUT;
  string ReactorType = "REP";
  string FuelType = "MOX";

  double CutOff = 1e-10;
  double NormalizationFactor = 1;
  int WantedCell = 0;
  int StepToSkip = 0;
  // Name of the output file
  OutDataFile = "./" + OutName + ".dat";
  OutDataFileInfo = "./" + OutName + ".info";
  OutLOG = "./" + OutName + ".log";

  ofstream OutputLog(OutLOG.c_str());

  // Check how many DATAxxx and BDATAxxx files there are
  string Command =
      "find " + DBPath + " -name \"" + DataASCIIFile + "*\" > ASCII.tmp";
  system(Command.c_str());
  Command = "find " + DBPath + " -name \"" + DataBinFile + "*\" > BIN.tmp";
  system(Command.c_str());

  ifstream ASCIITMP("ASCII.tmp");
  if (ASCIITMP.is_open()) {
    while (!ASCIITMP.eof()) {
      getline(ASCIITMP, s_tmp);
      i_tmp++;
    }
  }
  ASCIITMP.close();
  i_tmp--;
  NDataASCIIFile = i_tmp;

  i_tmp = 0;
  ifstream BINTMP("BIN.tmp");
  if (BINTMP.is_open()) {
    while (!BINTMP.eof()) {
      getline(BINTMP, s_tmp);
      i_tmp++;
    }
  }
  BINTMP.close();
  i_tmp--;
  NDataBinFile = i_tmp;

  // Remove temporary files...
  Command = "\\rm -f ASCII.tmp BIN.tmp";
  system(Command.c_str());

  // Say if there is Binary or ASCII file
  if (NDataASCIIFile > 0) {
    IsThereASCCIFile = true;
    DataFile = DataASCIIFile;
    NDataFile = NDataASCIIFile;
  } else if (NDataBinFile > 0) {
    IsThereBinFile = true;
    DataFile = DataBinFile;
    NDataFile = NDataBinFile;
  } else {
    cout << endl
         << "There is no MURE DATA file in the given path... EXIT!" << endl
         << endl;
    exit(1);
  }

  // =========================================================================================
  //  READ (B)DATA files...
  // =========================================================================================

  OutputLog << endl;
  OutputLog << "===================================================" << endl;
  OutputLog << "---------------------------------------------------" << endl;
  if (IsThereBinFile)
    OutputLog << endl << "Binary file detected... " << endl << endl;
  else
    cout << endl << "ASCII file detected... " << endl << endl;
  //	/*sleep(1)*/;

  for (int t = 0; t < NDataFile; t++) {
    string SFX = "";
    if (t <= 9)
      SFX = "00" + itoa(t);
    else if (t >= 10 && t <= 99)
      SFX = "0" + itoa(t);
    else if (t >= 100 && t <= 999)
      SFX = "" + itoa(t);
    else {
      cout << endl
           << "there is more than 1000 DATA files, not yet implemented... EXIT!"
           << endl
           << endl;
      exit(1);
    }
    string s_File = DBPath + "/" + DataFile + SFX;
    if (t % 10 == 0) {
      string ff = DataFile + SFX;
      cout << "Reading file " << ff << endl; /*sleep(1)*/
      ;
    }
    LastDataFile = DataFile + SFX;

    // READ BINARY FILE
    if (IsThereBinFile) {
      ifstream in(s_File.c_str(), ios::binary);
      if (!in.good()) {
        cout << "Cannot find Binary file " << s_File << endl;
        exit(1);
      }
      //
      // Read The version number of writing Mure evolving Data Format
      //
      char TextV;
      int MureDataVersion = 0;
      in.read((char*)&TextV, sizeof(char));

      if (TextV == 'V')
        in.read((char*)&MureDataVersion, sizeof(int));
      else {
        in.close();
        in.clear();
        in.open(s_File.c_str(), ios::binary);
      }

      string TheComment;

      // Read the File Header
      FileHeader FH;
      FH.read(in);
      NumberOfCells = FH.NCells;

      // --------------------------- TIME ----------------------
      vTime.push_back(FH.Time);
      vKeff.push_back(FH.K);

      CellHeader CH;
      NucleusRecord NR;
      bool STOP2READ = false;

      for (int c = 0; c < NumberOfCells; c++) {
        if (STOP2READ) break;
        CH.read(in);
        vFlux.push_back(CH.Flux);

        // read the Cell Comment
        int StringSize;
        in.read((char*)&StringSize, sizeof(int));
        TheComment.resize(StringSize);
        char tmp[StringSize + 1];
        in.read(tmp, (StringSize + 1) * sizeof(char));
        tmp[StringSize] = '\0';
        TheComment = tmp;

        bool SkipCell = false;
        if (CH.CellNumber < 0 && TheComment != "WasteStorage") SkipCell = true;
        if (TheComment == "WasteStorage") STOP2READ = true;

        if (t == 0) {
          vCellNumber.push_back(CH.CellNumber);
          vCellComment.push_back(TheComment);
        }

        // read the spatial variables
        int NSpatialVariables;
        in.read((char*)&NSpatialVariables, sizeof(int));

        vector<double> SpatialVariables(NSpatialVariables);
        vector<string> SpatialVariableNames;

        for (int j = 0; j < NSpatialVariables; j++)
          in.read((char*)&SpatialVariables[j], sizeof(double));

        // read the spatial variable names
        for (int j = 0; j < NSpatialVariables; j++) {
          int StringSize;
          in.read((char*)&StringSize, sizeof(int));
          s_tmp = "";
          s_tmp.resize(StringSize);
          char tmp[StringSize + 1];
          in.read(tmp, (StringSize + 1) * sizeof(char));
          tmp[StringSize] = '\0';
          s_tmp = tmp;
          SpatialVariableNames.push_back(s_tmp);
        }

        for (int i = 0; i < CH.NNucleusRecords; i++) {
          // Read the Nucleus Record
          NR.read(in);

          if (!SkipCell) {
            ZAImox zai(NR.Z, NR.A, NR.I, NR.Proportion);
            zai1.push_back(zai);
            // if (t==0 && NR.Proportion>=2e+25) cout << NR.Z << "  " << NR.A <<
            // "  " << NR.I << "  " << NR.Proportion << endl;
          }

          ReactionRecord RR;
          for (int k = 0; k < NR.NReactionRecords; k++) {
            RR.read(in);
            if (RR.Sigma == 0.0) RR.Sigma = 1e-10;

            if (!SkipCell) {
              ZAImox zai(NR.Z, NR.A, NR.I, RR.Sigma);
              if (RR.Code == 18) zai_FS_1.push_back(zai);
              if (RR.Code == 102) zai_SC_1.push_back(zai);
              if (RR.Code == 16) zai_SN_1.push_back(zai);
            }
          }
        }
        if (!SkipCell) {
          zai2.push_back(zai1);
          zai1.clear();
          zai_FS_2.push_back(zai_FS_1);
          zai_FS_1.clear();
          zai_SC_2.push_back(zai_SC_1);
          zai_SC_1.clear();
          zai_SN_2.push_back(zai_SN_1);
          zai_SN_1.clear();
        }
      }
      in.close();
      zai3.push_back(zai2);
      zai2.clear();
      zai_FS_3.push_back(zai_FS_2);
      zai_FS_2.clear();
      zai_SC_3.push_back(zai_SC_2);
      zai_SC_2.clear();
      zai_SN_3.push_back(zai_SN_2);
      zai_SN_2.clear();

      vFlux1.push_back(vFlux);
      vFlux.clear();

    }
    // READ ASCII FILE
    else if (IsThereASCCIFile) {
      ifstream in(s_File.c_str());
      int TranspCode = 1;
      if (!in.good()) {
        if (!in.good()) {
          OutputLog << "Cannot find ASCII file " << s_File << endl;
          exit(0);
        }
      }
      //
      // Read The version number of writing Mure evolving Data Format
      //
      int MureDataVersion = ReadCommentVersion(in, s_File);
      if (MureDataVersion == 0) {
        in.close();
        in.clear();
        in.open(s_File.c_str());
      } else if (MureDataVersion < 0) {
        OutputLog << "Old version of MURE... Check!!! exit(1)";
        exit(0);
      }

      // Read out the TIME the KEFF and the KEFF ERROR
      double Time, Keff, Keff_Err;
      in >> Time >> Keff >> Keff_Err;
      in >> NumberOfCells;

      // --------------------------- TIME ----------------------
      vTime.push_back(Time);
      vKeff.push_back(Keff);

      bool STOP2READ = false;

      for (int c = 0; c < NumberOfCells; c++) {
        if (STOP2READ) break;
        int CellNumber;
        double Volume, Flux, FluxErr;
        in >> CellNumber >> Volume >> Flux >> FluxErr;

        vFlux.push_back(Flux);

        int NNucleusRecords;
        in >> NNucleusRecords;

        // read the Cell Comment and Spatial Variables
        string TheComment;
        getline(in, TheComment);  // the "in>>" command keep the cursor at the
                                  // end of line
        getline(in, TheComment);  // so two "getline" commands are requested

        bool SkipCell = false;
        if (CellNumber < 0 && TheComment != "WasteStorage") SkipCell = true;
        if (TheComment == "WasteStorage") STOP2READ = true;

        if (t == 0) {
          vCellNumber.push_back(CellNumber);
          vCellComment.push_back(TheComment);
        }

        int NSpatialVariables;
        in >> NSpatialVariables;

        vector<double> SpatialVariables;
        vector<string> SpatialVariableNames;

        for (int j = 0; j < NSpatialVariables; j++) {
          string TheString;
          double val;
          in >> TheString >> val;
          SpatialVariableNames.push_back(TheString);
          SpatialVariables.push_back(val);
        }

        for (int i = 0; i < NNucleusRecords; i++) {
          // Read the Nucleus Record
          int Z, A, I;
          double Mass, Proportion;
          in >> Z >> A >> I >> Mass >> Proportion;
          cout << Z << endl;
          if (!SkipCell) {
            ZAImox zai(Z, A, I, Proportion);
            zai1.push_back(zai);
          }

          int NReactionRecords;
          in >> NReactionRecords;
          for (int k = 0; k < NReactionRecords; k++) {
            int Code;
            double Sigma, SigmaErr;
            in >> Code >> Sigma >> SigmaErr;
            if (!SkipCell) {
              ZAImox zai(Z, A, I, Proportion);
              if (Code == 18) zai_FS_1.push_back(zai);
              if (Code == 102) zai_SC_1.push_back(zai);
              if (Code == 16) zai_SN_1.push_back(zai);
            }

            if (Sigma == 0.0) Sigma = 1e-10;
          }
        }
        if (!SkipCell) {
          zai2.push_back(zai1);
          zai1.clear();
          zai_FS_2.push_back(zai_FS_1);
          zai_FS_1.clear();
          zai_SC_2.push_back(zai_SC_1);
          zai_SC_1.clear();
          zai_SN_2.push_back(zai_SN_1);
          zai_SN_1.clear();
        }
      }
      in.close();
      zai3.push_back(zai2);
      zai2.clear();
      zai_FS_3.push_back(zai_FS_2);
      zai_FS_2.clear();
      zai_SC_3.push_back(zai_SC_2);
      zai_SC_2.clear();
      zai_SN_3.push_back(zai_SN_2);
      zai_SN_2.clear();
      vFlux1.push_back(vFlux);
      vFlux.clear();
    }
    // else ERROR
    else {
      OutputLog << endl
                << "DATA files are neither binary nor ASCII?? ... EXIT!"
                << endl;
      exit(1);
    }
  }
  OutputLog << endl << "Last file read : " << LastDataFile << endl << endl;
  OutputLog << "All (B)DATA files have been read..." << endl << endl;
  OutputLog << "---------------------------------------------------" << endl;
  OutputLog << "===================================================" << endl;

  // =========================================================================================
  //  Manage several Cells
  // =========================================================================================

  // Time bins
  int Nt = zai3.size();
  // Number of cells
  int Nc = zai3[0].size();

  // Number of nuclides
  int Ni = zai3[0][0].size();

  double CycleTime = (vTime[vTime.size() - 1] - vTime[0]) / 3600. / 24 / 365.4;

  // Manage the cells...
  bool SumOfCell = false;

  if (vCellNumber.size() >= 2) {
    OutputLog << endl
              << "===================================================" << endl;
    OutputLog << "-------------- WARNING ----------------------------" << endl;
    OutputLog << "===================================================" << endl
              << endl;
    OutputLog << "THERE IS MORE THAN ONE CELL... Cells are : " << endl
              << endl; /*sleep(1)*/
    ;
    for (int i = 0; i < vCellNumber.size(); i++) {
      OutputLog << "index : " << i << " - Cell number  : " << vCellNumber[i]
                << endl;
      OutputLog << "Cell comment : " << vCellComment[i] << endl << endl;
    }
    if (WantedCell == 0) SumOfCell = true;

    if (!SumOfCell && WantedCell > Nc) {
      OutputLog << "The index you choose is not defined... EXIT!" << endl;
      exit(1);
    }

    zai2.clear();
    zai_FS_2.clear();
    zai_SC_2.clear();
    zai_SN_2.clear();
    vFlux.clear();
    for (int t = 0; t < Nt; t++) {
      for (int i = 0; i < Ni; i++) {
        double SUM = 0;
        for (int c = 0; c < Nc; c++) SUM += zai3[t][c][i].Prop();
        ZAImox zai(zai3[t][0][i].Z(), zai3[t][0][i].A(), zai3[t][0][i].I(),
                   SUM);
        zai1.push_back(zai);
      }
      for (int i = 0; i < (int)zai_FS_3[0][0].size(); i++) {
        double SUM = 0;
        for (int c = 0; c < Nc; c++) SUM += zai_FS_3[t][c][i].Prop() / Nc;
        ZAImox zai(zai_FS_3[t][0][i].Z(), zai_FS_3[t][0][i].A(),
                   zai_FS_3[t][0][i].I(), SUM);
        zai_FS_1.push_back(zai);
      }
      for (int i = 0; i < (int)zai_SC_3[0][0].size(); i++) {
        double SUM = 0;
        for (int c = 0; c < Nc; c++) SUM += zai_SC_3[t][c][i].Prop() / Nc;
        ZAImox zai(zai_SC_3[t][0][i].Z(), zai_SC_3[t][0][i].A(),
                   zai_SC_3[t][0][i].I(), SUM);
        zai_SC_1.push_back(zai);
      }
      for (int i = 0; i < (int)zai_SN_3[0][0].size(); i++) {
        double SUM = 0;
        for (int c = 0; c < Nc; c++) SUM += zai_SN_3[t][c][i].Prop() / Nc;
        ZAImox zai(zai_SN_3[t][0][i].Z(), zai_SN_3[t][0][i].A(),
                   zai_SN_3[t][0][i].I(), SUM);
        zai_SN_1.push_back(zai);
      }

      double SUM = 0;
      for (int j = 0; j < (int)vFlux1[t].size(); j++) SUM += vFlux1[t][j];

      vFlux.push_back(SUM);

      zai2.push_back(zai1);
      zai1.clear();
      zai_FS_2.push_back(zai_FS_1);
      zai_FS_1.clear();
      zai_SC_2.push_back(zai_SC_1);
      zai_SC_1.clear();
      zai_SN_2.push_back(zai_SN_1);
      zai_SN_1.clear();
    }
    OutputLog << endl
              << "---------------------------------------------------" << endl;
    OutputLog << "-------------- Sum of cells done ------------------" << endl;
    OutputLog << "---------------------------------------------------" << endl
              << endl;
  } else
    WantedCell = 0;

  // =========================================================================================
  //  Manage the cutoff and calculate total N and M at t=0
  // =========================================================================================

  double NTotal = 0;
  double MTotalFissile = 0;
  if (SumOfCell) {
    for (int i = 0; i < Ni; i++) {
      double Ni = zai2[0][i].Prop();
      NTotal += Ni;
      if (zai2[0][i].Z() >= 90) MTotalFissile += Ni * zai2[0][i].A() / 6.023e23;
    }
  } else {
    for (int i = 0; i < Ni; i++) {
      double Ni = zai3[0][WantedCell][i].Prop();
      NTotal += Ni;
      if (zai3[0][WantedCell][i].Z() >= 90)
        MTotalFissile += Ni * zai3[0][WantedCell][i].A() / 6.023e23;
    }
  }
  CutOff = CutOff * NTotal;

  for (int t = 0; t < Nt; t++) {
    for (int i = 0; i < Ni; i++) {
      double Value = 0;
      if (SumOfCell)
        Value = zai2[t][i].Prop();
      else
        Value = zai3[t][WantedCell][i].Prop();

      if (t == 0) {
        if (Value >= CutOff)
          HasToBePrint.push_back(true);
        else
          HasToBePrint.push_back(false);
      } else {
        if (!HasToBePrint[i] && Value >= CutOff) {
          HasToBePrint.erase(HasToBePrint.begin() + i);
          HasToBePrint.insert(HasToBePrint.begin() + i, true);
        }
      }
    }
  }

  // =========================================================================================
  //  Write the DATABASE and convert Number to Mass - Manage Normalization
  //  Factor
  // =========================================================================================

  ofstream Output(OutDataFile.c_str());
  Output.precision(16);
  Output << "time";
  for (int t = StepToSkip; t < vTime.size(); t++)
    Output << " " << vTime.at(t) - vTime.at(StepToSkip);
  Output << endl;

  Output << "keff";
  for (int t = StepToSkip; t < vKeff.size(); t++) Output << " " << vKeff.at(t);
  Output << endl;

  Output << "flux";
  if (SumOfCell) {
    for (int t = StepToSkip; t < vTime.size(); t++)
      Output << " " << vFlux.at(t);
    Output << endl;
  } else {
    for (int t = StepToSkip; t < vTime.size(); t++)
      Output << " " << vFlux1[t][WantedCell];
    Output << endl;
  }
  int NPrinted = 0;
  for (int i = 0; i < Ni; i++) {
    ///		if (HasToBePrint[i])
    {
      if (SumOfCell) {
        Output << "Inv " << zai2[0][i].Z() << " " << zai2[0][i].A() << " "
               << zai2[0][i].I() << " ";
        for (int t = StepToSkip; t < vTime.size(); t++) {
          double Val = zai2[t][i].Prop() * NormalizationFactor;
          Output << Val << " ";
          if (t == StepToSkip) NPrinted++;
        }
        Output << endl;
      } else {
        Output << "Inv " << zai3[0][0][i].Z() << " " << zai3[0][0][i].A() << " "
               << zai3[0][0][i].I() << " ";
        for (int t = StepToSkip; t < vTime.size(); t++) {
          double Val = zai3[t][WantedCell][i].Prop() * NormalizationFactor;
          Output << Val << " ";
          if (t == StepToSkip) NPrinted++;
        }
        Output << endl;
      }
    }
  }
  for (int i = 0; i < (int)zai_FS_3[0][0].size(); i++) {
    if (SumOfCell) {
      Output << "XSFis " << zai_FS_2[0][i].Z() << " " << zai_FS_2[0][i].A()
             << " " << zai_FS_2[0][i].I() << " ";
      for (int t = StepToSkip; t < vTime.size(); t++) {
        double Val = zai_FS_2[t][i].Prop();
        Output << Val << " ";
      }
      Output << endl;
    } else {
      Output << "XSFis " << zai_FS_3[0][0][i].Z() << " "
             << zai_FS_3[0][0][i].A() << " " << zai_FS_3[0][0][i].I() << " ";
      for (int t = StepToSkip; t < vTime.size(); t++) {
        double Val = zai_FS_3[t][WantedCell][i].Prop();
        Output << Val << " ";
      }
      Output << endl;
    }
  }

  for (int i = 0; i < (int)zai_SC_3[0][0].size(); i++) {
    if (SumOfCell) {
      Output << "XSCap " << zai_SC_2[0][i].Z() << " " << zai_SC_2[0][i].A()
             << " " << zai_SC_2[0][i].I() << " ";
      for (int t = StepToSkip; t < vTime.size(); t++) {
        double Val = zai_SC_2[t][i].Prop();
        Output << Val << " ";
      }
      Output << endl;
    } else {
      Output << "XSCap " << zai_SC_3[0][0][i].Z() << " "
             << zai_SC_3[0][0][i].A() << " " << zai_SC_3[0][0][i].I() << " ";
      for (int t = StepToSkip; t < vTime.size(); t++) {
        double Val = zai_SC_3[t][WantedCell][i].Prop();
        Output << Val << " ";
      }
      Output << endl;
    }
  }
  for (int i = 0; i < (int)zai_SN_3[0][0].size(); i++) {
    if (SumOfCell) {
      Output << "XSn2n " << zai_SN_2[0][i].Z() << " " << zai_SN_2[0][i].A()
             << " " << zai_SN_2[0][i].I() << " ";
      for (int t = StepToSkip; t < vTime.size(); t++) {
        double Val = zai_SN_2[t][i].Prop();
        Output << Val << " ";
      }
      Output << endl;
    } else {
      Output << "XSn2n " << zai_SN_3[0][0][i].Z() << " "
             << zai_SN_3[0][0][i].A() << " " << zai_SN_3[0][0][i].I() << " ";
      for (int t = StepToSkip; t < vTime.size(); t++) {
        double Val = zai_SN_3[t][WantedCell][i].Prop();
        Output << Val << " ";
      }
      Output << endl;
    }
  }
  Output.close();

  ofstream OutputInfo(OutDataFileInfo.c_str());
  OutputInfo << "Reactor " << ReactorType << endl;
  OutputInfo << "Fueltype " << FuelType << endl;
  OutputInfo << "CycleTime " << CycleTime << endl;
  OutputInfo << "AssemblyHeavyMetalMass " << MTotalFissile << " g" << endl;
  OutputInfo << "ConstantPower " << Power << " W" << endl;

  OutputInfo << "CutOff " << CutOff / NTotal << endl;
  OutputInfo << "Nnuclei " << NPrinted << endl;
  OutputInfo << "NormalizationFactor " << NormalizationFactor << endl;
  OutputInfo << "FinalHeavyMetalMass " << MTotalFissile * NormalizationFactor
             << " g" << endl;

  OutputInfo.close();

  // =========================================================================================
  //  BYE!
  // =========================================================================================
  OutputLog << "===================================================" << endl;
  OutputLog << "---------------------------------------------------" << endl;

  OutputLog << endl
            << "The database " << OutDataFile << " has been generated..."
            << endl;
  OutputLog << "The database information " << OutDataFileInfo
            << " has been generated..." << endl
            << endl;
  OutputLog << NPrinted << " nuclides have been written" << endl << endl;

  OutputLog << "---------------------------------------------------" << endl;
  OutputLog << "===================================================" << endl;
}

//==========================================================================================
// Compilation
//==========================================================================================
/*

 g++  -o MOX MOX.cc -I$MURE_include -I$MURE_ExternalPkg -L$MURE_lib -lMUREpkg -lvalerr -lmctal -fopenmp


 */
