#include <Riostream.h>
#include "TCanvas.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TH2F.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TROOT.h"
#include "TApplication.h"
#include "TLine.h"

// global variables
int globall0tis=0, globall0tos=0;
double nsele(0);
double PI(3.14159);
string CombTechnique="NNet";
double nrt[20],nwt[20],nrtag[20],nwtag[20];
float nlm(0),nllm(0),nle(0),nlle(0),nlk(0),nlkS(0),nllk(0),nllkS(0),nidm(0)
     ,nide(0),nidk(0),nidkS(0); 
float nghost_m(0),nghost_e(0),nghost_k(0), nghost_kS(0), ghrate_k(0),gherror_k(0); 
float ghrate_kS(0),gherror_kS(0); 
float ghrate_m(0),gherror_m(0),ghrate_e(0),gherror_e(0);
int nrTisTis(0),nrTosTos(0),nrTisTos(0),nrTosTis(0),nrTob(0);
int ntruemu=0, ntrueel=0, ntruekO=0, ntruekS=0;
int ntruemutag=0, ntrueeltag=0, ntruekOtag=0, ntruekStag=0;
int ndirect_mu=0,ndirect_el=0,nbc_mu=0,nbc_el=0,nbc_kO=0,nbc_kS=0;
int isBs(0), DBG(0), askL0(0), askHlt1(0), askHlt2(0), askTis(0), askTos(0), askPtB(0), askWeightedch(0);
int askNNcomb(0), DBGNN(0);
string ROJO("\x1b[91m"),ROJO2("\x1b[31m"), VERDE("\x1b[32m"), AMARILLO("\x1b[33m"),
  AZUL("\x1b[94m"),AZULCLARO("\x1b[36m"), VIOLETA("\x1b[95m"),SUBROJO("\x1b[101m"),
  SUBVERDE("\x1b[102m"), SUBAMARILLO("\x1b[103m"), SUBAZUL("\x1b[104m"), 
  SUBVIOLETA("\x1b[105m"),SUBBLANCO("\x1b[107m"),BLANCO("\x1b[37m"), 
  BOLD("\x1b[1m"),ENDC("\x1b[m"),FAINT("\x1b[2m"),UNDERLINE("\x1b[4m"),
  BLINK("\x1b[5m"),CLEARSCREEN("\x1b[2J"), DEL1L("\x1b[2A\n\n");
double tag();

//functions////////////////////////////////////////////////////////////////
TString readString(TString varname) {

  ifstream indata; 
  indata.open("tag.cuts.txt"); // opens the file
  if(!indata) { // file couldn't be opened
      cerr << "Error: tag.cuts.txt could not be opened" << endl;
      exit(1);
  }
  TString a, egual, value="";
  while ( !indata.eof() ) { // keep reading until end-of-file
   indata >> a;
      if(a==varname) {
	indata >> egual;
	if(egual!= "=") {cout<<"missing spaced = sign after "<<a<<endl;exit(1);}
	indata >> value;
      }
  }
  indata.close();
  if(value=="") {
    cout<<"Input "<<varname<<" not found in tag.cuts.txt! Stop.\n";
    exit(1);
  }
  return value;
}
double read(TString varname) {
  
  ifstream indata; // indata is like cin
  indata.open("tag.cuts.txt"); // opens the file
  if(!indata) { // file couldn't be opened
      cerr << "Error: tag.cuts.txt could not be opened" << endl;
      exit(1);
  }
  double dvalue=-12345;
  TString a, egual, value;
  while ( !indata.eof() ) { // keep reading until end-of-file
   indata >> a;
      if(a==varname) {
	indata >> egual;
	if(egual!= "=") {cout<<"missing spaced = sign after "<<a<<endl;exit(1);}
	indata >> value;
	dvalue = atof(value);
	//cout<<"... Using cut on "<< varname<<" = "<<dvalue<<endl;
      }
  }
  indata.close();
  if(dvalue==-12345) {
    cout<<"Input "<<varname<<" not found in tag.cuts.txt! Stop.\n";
    exit(1);
  }
  //cout << "End-of-file reached.." << endl;
  return dvalue;
}

//////////////////////////////////////////////////////////////////////////
bool isNonResonantD(float id) {
  
  int aid = (int)fabs(id);
  if(aid==411 || aid==421 || aid==431) return true;

  return false;
}
//////////////////////////////////////////////////////////////////////////
bool isD(int id) {
  
  int aid = abs(id);
  if(aid>410&&aid<446)     return true;
  if(aid>4100&&aid<4556)   return true;
  if(aid>10410&&aid<10454) return true;
  if(aid>20410&&aid<20454) return true;

  return false;
}
double dPhi(double f1, double f2){
  double a = fabs(f1-f2);
  if(a>PI) a = 2*PI-a;
  return a;
}
//////////////////////////////////////////////////////////////////////////
TGraphErrors* plotVersus(TH1F* rh, TH1F* wh, TString name="graphic" ){ 

  TGraphErrors* ww = new TGraphErrors;
  ww->SetName(name);
  if(rh->GetNbinsX() != wh->GetNbinsX()) {
    cout<<"plotVersus Error!\n"; 
    return ww;
  }
  for(int i=0; i!=rh->GetNbinsX(); ++i) {
    float r = rh->GetBinContent(i);
    float w = wh->GetBinContent(i);
    float re= rh->GetBinError(i);
    float we= wh->GetBinError(i);
    ww->SetPoint(i, r, w );
    ww->SetPointError(i, re, we );
  }
  return ww;
}

//////////////////////////////////////////////////////////////////////////
TGraphErrors* asymmetry(TH1F* rh, TH1F* wh, TString name="asymplot" ){ 

  //rh and wh should have large nr of bins
  int bins=12;
  double tbin[13] =
    { 0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.2, 4.9, 6.0, 7.5, 10.0 };
  //---------

  double rhnew[100], whnew[100], total=0;
  for(int i=0; i!=100; ++i) {rhnew[i]=0; whnew[i]=0;}
  
  for(int i=0; i!=rh->GetNbinsX(); ++i) {
    //decide new bin:
    int newbin = 0;
    double xbin = rh->GetBinCenter(i);
    for( int j=0; j!=bins-1; ++j) { 
      if( xbin>=tbin[j] && xbin<tbin[j+1] ) {newbin=j; break;}
      if( xbin<tbin[0] ) newbin=0;
      if( xbin>tbin[bins-1] ) newbin=bins-1;
    }
    //cout<<xbin<< " is put in "<<newbin<<endl;

    //fill rhnew, whnew (rebin):
    double r = rh->GetBinContent(i);
    double w = wh->GetBinContent(i);
    rhnew[newbin] += r;
    whnew[newbin] += w;
    total += r;
    total += w;
  }

  //fill points:
  int ireal=0;
  Float_t ax[13],ay[13],aex[13],aey[13];
  for( int i=0; i!=bins; ++i) { 
    Float_t a = rhnew[i];
    Float_t b = whnew[i];
    Float_t sumsq = (a+b)*(a+b);
    if( sumsq>0 ) {
      Float_t xt= (tbin[i+1]+tbin[i])/2;
      Float_t dx= (tbin[i+1]-tbin[i])/3.46;//sqrt(12)
      Float_t As= (a-b)/(a+b);
      Float_t siga = a*(1-a/total);
      Float_t sigb = b*(1-b/total);
      Float_t term1= pow( 2*b/sumsq, 2 ) * siga;
      Float_t term2= pow(-2*a/sumsq, 2 ) * sigb;
      Float_t cov  = -a*b/total;
      Float_t term3= 2 * (2*b/sumsq) * (-2*a/sumsq) * cov;
      Float_t sigAs= sqrt( term1 + term2 + term3 );
      //Float_t sigAs= sqrt(a+b)/(a+b); //banale
           
      ax[ireal]=xt;
      ay[ireal]=As;
      aex[ireal]=dx;
      aey[ireal]=sigAs;
      ireal++;
    }
  }

  TGraphErrors* asymplot = new TGraphErrors(bins,ax,ay,aex,aey);
  asymplot->SetName(name);
  asymplot->SetTitle("; time/ps; Asymmetry");
  asymplot->SetLineColor(4);
  asymplot->SetMarkerColor(4);
  asymplot->SetMarkerStyle(20);
  asymplot->SetMarkerSize(0.6);
  asymplot->SetMinimum(-0.7); 
  asymplot->SetMaximum(0.7); 

  return asymplot;
}

////////////////////////////////////////////////////////////////////////
TLorentzVector build4V(double p, double pt, double phi, double ID) { //in GeV
  double a = p*p - pt*pt;
  if(a<0) {
    cout<<"Error in build4V: negative value detected!"<<endl;
    return 0.0;
  }
  double m=0;
  //if(ID>4.0 && ID<7.0) m=ID;
  if(int(ID)!=ID) m=ID;//ID is the B mass 
  else if(abs((int)ID)==321)  m=0.493677;
  else if(abs((int)ID)==211)  m=0.13957;
  else if(abs((int)ID)==2212) m=0.938272;
  else if(abs((int)ID)==11)   m=0.000511;
  else if(abs((int)ID)==13)   m=0.105658;
  else { cout<<"Error in build4V: unknown ID="<<ID<<endl; return 0.0;}

  TLorentzVector v( pt*cos(phi), pt*sin(phi), sqrt(a), sqrt(p*p+m*m) );
  return v;
}
double calc_dQ(const TLorentzVector& BSpart, const TLorentzVector& ipart) {
  return (BSpart + ipart).M() - BSpart.M();
}

////////////////////////////////////////////////////////////////////////
void calculateOmega(TH1F* rh, TH1F* wh, TH1F* omega){ //used by plot_omega()
  for(int i=1; i!=rh->GetNbinsX(); ++i) {
    double r = rh->GetBinContent(i);
    double w = wh->GetBinContent(i);
    if(r) if(w){
      double om = w/(r+w);
      double omerr = sqrt(om*(1-om)/(r+w));
      omega->SetBinContent(i, om);
      omega->SetBinError(i, omerr);
    }
  }
  omega->SetLineColor(kBlue);
  omega->SetMinimum(0.0); 
  omega->SetMaximum(0.7);
}
////////////////////////////////////////////////////////////////////////
TH1F* plot_ratio(TString name, TH1F* rh, TH1F* wh, TH1F* extra=0){ 

  TCanvas* c= (TCanvas*)gROOT->GetListOfCanvases()->FindObject("c");  
  c->Clear(); 
  c->Divide(1,2);
  c->cd(1);

  TH1F* htot = (TH1F*) rh->Clone(); 
  htot->SetName("htotal");
  htot->Add(wh);
  htot->SetLineColor(2);
  htot->SetFillColor(2);
  htot->SetLineWidth(1); 
  htot->SetMinimum(0.0); 
  htot->SetTitle("; "   +name+"; Events");
  htot->Draw();

  rh->SetLineColor(0);
  rh->SetFillColor(3);
  rh->SetLineWidth(1); 
  rh->SetMinimum(0.0); 
  rh->Draw("same");

  
  if(extra) {
    extra->SetLineStyle(2);
    extra->SetLineColor(4);
    extra->SetLineWidth(1); 
    extra->Draw("same");
  }

  TH1F* hhh = (TH1F*) rh->Clone(); 
  hhh->SetName("_hratio");
  calculateOmega(wh, rh, hhh);
  hhh->SetTitle("; "   +name+"; Purity");
  hhh->SetMaximum(1.1);
  hhh->SetMinimum(0);
  c->GetPad(1)->RedrawAxis();
  c->cd(2); c->GetPad(2)->SetGrid();
  hhh->Fit("pol2");
  double p0= hhh->GetFunction("pol2")->GetParameter(0);
  double p1= hhh->GetFunction("pol2")->GetParameter(1);
  double p2= hhh->GetFunction("pol2")->GetParameter(2);
  cout<< "double prob_"<<name<<" = pol("<<name 
      <<", "<<p0<<", "<<p1<<", "<<p2<<");"<<endl;

  c->Print("pics/ratio_"+name+".gif"); c->cd();
  c->Update();

  return hhh;
}

////////////////////////////////////////////////////////////////////////
TH1F* plot_omega(TH1F* rh, TH1F* wh, TString name, int opts=1){ 
  //opts is the polinomial degree.

  TCanvas* c= (TCanvas*)gROOT->GetListOfCanvases()->FindObject("c");  
  c->Clear(); 
  c->Divide(1,2);
  c->cd(1);
  
  if(name=="sameside"){ if(isBs) name="kS"; else name="pS"; }

  if(opts==0) rh->SetLineColor(1);rh->SetFillColor(3);rh->SetFillStyle(3001);
  if(opts==1) rh->SetLineColor(0);
  if(opts==0) rh->SetTitle(";NNET Output for "   +name+";Events");
  if(opts==1) rh->SetTitle("; "+name+";Events");
  rh->SetLineWidth(1); 
  rh->SetMinimum(0.0); 
  rh->Draw();

  if(opts==0) wh->SetLineColor(1);wh->SetFillColor(2);wh->SetFillStyle(3001);
  if(opts==1) wh->SetLineColor(0);
  wh->SetLineWidth(1); 
  wh->SetMinimum(0.0); 
  wh->Draw("same");

  TH1F* hhh = (TH1F*) rh->Clone(); 
  if(opts==0) hhh->SetName("hom");
  if(opts==1) hhh->SetName("homp");
  calculateOmega(rh, wh, hhh);
  if(opts==0) hhh->SetTitle(";NNET Output for "   +name+";Wrong tag fraction");
  if(opts==1) hhh->SetTitle("; "+name+";TRUE OMEGA");
  c->GetPad(1)->RedrawAxis();

  c->cd(2); c->GetPad(2)->SetGrid();
  if(opts==3) {
    hhh->Fit("pol3");
    double p0= hhh->GetFunction("pol3")->GetParameter(0);
    double p1= hhh->GetFunction("pol3")->GetParameter(1);
    double p2= hhh->GetFunction("pol3")->GetParameter(2);
    double p3= hhh->GetFunction("pol3")->GetParameter(3);
    ofstream out;
    out.open("NN_FitParameters.txt", ios::app);
    if(out){ 
      out<< name <<"  "<<p0<<"  "<<p1<<"  "<<p2<<"  "<<p3<<endl;
      out.close();
    } else cout<<"ERROR: do touch ./NN_FitParameters.txt"<<endl;
  } 
  if(opts==1) hhh->Fit("pol1"); 
  TLine* a=new TLine(0,0, .6,.6);
  if(opts==1) a->Draw(); a->SetLineColor(4); a->SetLineStyle(4); a->SetLineWidth(2);
  c->Print("pics/nn_"+name+".gif"); c->cd();
  c->Update();

  return hhh;
}

////////////////////////////////////////////////////////////////////////
void calculateEffEff(double nsele, TH1F* rh, TH1F* wh, TH1F* effeff, 
		     TString direction="left2right" ){ 

  for(int i=0; i!=rh->GetNbinsX()+1; ++i) {

    double rtag=0, wtag=0;
    if(direction == "left2right") {
      for(int j=i; j!=rh->GetNbinsX()+1; j++)  { 
	rtag += rh->GetBinContent(j);
	wtag += wh->GetBinContent(j);
      }
    } else if(direction == "right2left") {
      for(int j=1; j!=i+1; j++)  { 
	rtag += rh->GetBinContent(j);
	wtag += wh->GetBinContent(j);
      }
    } else cout<<"Error: unknown option "<<direction<<endl;

    if(rtag) if(wtag) {
      double utag = nsele-rtag-wtag;              // untagged
      double omtag = wtag/(rtag+wtag);
      double eftag = (rtag+wtag)/nsele;           // tagging efficiency
      double epsil = eftag*pow(1-2*omtag,2);      // effective efficiency
      if(rtag<wtag) epsil= -epsil;
      double epsilerr = sqrt((pow(rtag - wtag,2)*
			      (-(pow(rtag - wtag,2)*(rtag +wtag))+nsele
			       *(pow(rtag,2) +14*rtag*wtag+ pow(wtag,2))))
			     /(pow(rtag+wtag+utag,3)*pow(rtag + wtag,3)));
      effeff->SetBinContent(i, epsil*100);
      effeff->SetBinError(i, epsilerr*100);
    }
  }
  rh->SetMinimum(0);
  wh->SetMinimum(0);
}
// ////////////////////////////////////////////////////////////////////////
// TH1F* calculateEffEff(const TH1F* rh, const TH1F* wh, 
// 		      TString direction="left2right" ){ 

//   TH1F* effeff = new TH1F(*rh);
//   effeff->Reset();
//   effeff->SetName("heffec");

//   for(int i=1; i!=rh->GetNbinsX(); ++i) {

//     double rtag=0, wtag=0;
//     if(direction == "left2right") {
//       for(int j=i; j!=rh->GetNbinsX()+1; j++)  { 
// 	rtag += rh->GetBinContent(j);
// 	wtag += wh->GetBinContent(j);
//       }
//     } else if(direction == "right2left") {
//       for(int j=1; j!=i+1; j++)  { 
// 	rtag += rh->GetBinContent(j);
// 	wtag += wh->GetBinContent(j);
//       }
//     } else cout<<"calculateEffEff Error: unknown option "<<direction<<endl;

//     if(rtag) if(wtag) {
//       double utag = nsele-rtag-wtag;              // untagged
//       double omtag = wtag/(rtag+wtag);
//       double eftag = (rtag+wtag)/nsele;// tagging efficiency
//       double epsil = eftag*pow(1-2*omtag,2);      // effective efficiency
//       if(rtag<wtag) epsil= -epsil;
//       double epsilerr = sqrt((pow(rtag - wtag,2)*
// 			      (-(pow(rtag - wtag,2)*(rtag +wtag))+nsele
// 			       *(pow(rtag,2) +14*rtag*wtag+ pow(wtag,2))))
// 			     /(pow(rtag+wtag+utag,3)*pow(rtag + wtag,3)));
//       effeff->SetBinContent(i, epsil*100);
//       effeff->SetBinError(i, epsilerr*100);
//     }
//   }
//   effeff->SetMinimum(0.2);

//   return effeff;
// }

//======================================================================
void decode(const int flags, int& a, int& b, int& c) {
  a = int(float(flags)/100);
  b = int(float(flags-100*a)/10);
  c = int(float(flags-100*a-10*b)/1);
}
void decode(const int flags, int& a, int& b, int& c, int& d, int& e, int& f){
  a = int(float(flags)/100000);
  b = int(float(flags-100000*a)/10000);
  c = int(float(flags-100000*a-10000*b)/1000);
  d = int(float(flags-100000*a-10000*b-1000*c)/100);
  e = int(float(flags-100000*a-10000*b-1000*c-100*d)/10);
  f = int(float(flags-100000*a-10000*b-1000*c-100*d-10*e)/1);
}
void PrintAdvance(int n, float nmax) {
  if(!n) cout<<ROJO2<<" |<<<<<<<<<<<<<<  ";
  float r= float(n)/int(nmax/15.);
  if(r==int(r)) cout<<ROJO2<<"\b\b| \b"<<flush<<ENDC;
}

bool wait() {
  cout<<"--> Hit return to continue.  ";
  char *s = new char[1];
  char *bb = fgets(s, 1, stdin); bb=0;
  if(*s=='q' || *s=='.') { delete s; return true; }
  cout<<endl; delete s;
  return false;
}

//======================================================================
double pol(double x, double a0, 
	   double a1=0.0, double a2=0.0, double a3=0.0, double a4=0.0) {
  double res = a0;
  if(a1) res += a1*x;
  if(a2) res += a2*x*x;
  if(a3) res += a3*x*x*x;
  if(a4) res += a4*x*x*x*x;
  return res;
}

////////////////////////////////////////////////////////////////////////
float PrintPerformance(){ 
   
  if(nsele==0) cout<<"No events selected.\n";
  if(nsele==0) return 0;
  if(ntruekO) {
    cout <<BOLD<<
      "\n=========================================================\n";
    cout<<"Fraction in event sample of true particles from B:"<<endl;
    cout<<ENDC<<setprecision(3)<<"mu= "<<float(ntruemu)/nsele*100
	<< "  %\t (sel. as tagger:"<<float(ntruemutag)/nsele*100<<"%)"<<endl;
    cout<<"el= "<<float(ntrueel)/nsele*100
	<< "  %\t (sel. as tagger:"<<float(ntrueeltag)/nsele*100<<"%)"<<endl;
    cout<<"kO= "<<float(ntruekO)/nsele*100
	<< "  %\t (sel. as tagger:"<<float(ntruekOtag)/nsele*100<<"%)"<<endl;
    if(isBs)cout<<"kS= "<<float(ntruekS)/nsele*100
	<< "  %\t (sel. as tagger:"<<float(ntruekStag)/nsele*100<<"%)"<<endl;
    cout<<BOLD<<"Fraction in tagger sample of:\n"<<ENDC;
    cout<<"b->mu= "<<int(float(ndirect_mu)/(nrtag[1]+nwtag[1])*100)<<"%\t";
    cout<<"b->c->mu= "<<int(float(nbc_mu)/(nrtag[1]+nwtag[1])*100)<<"%\n";
    cout<<"b->e = "<<int(float(ndirect_el)/(nrtag[2]+nwtag[2])*100)<<"%\t";
    cout<<"b->c->e = "<<int(float(nbc_el)/(nrtag[2]+nwtag[2])*100)<<"%\n";
    cout<<"b->K = "<<int(float(nbc_kO)/(nrtag[3]+nwtag[3])*100)<<"%\t";
    if(isBs)cout<<"b->Ksame= "<<int(float(nbc_kS)/(nrtag[4]+nwtag[4])*100)<<"%";
    cout<<BOLD<<"\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<ENDC;
  }

  // calculate effective efficiency in various categories
  double rtt=0, wtt=0;
  double rtag,wtag,utag;
  double ef_tot=0, effe_tot=0;
  double epsilerr, epsilerrtot=0;
  float den_m = (float) (nrtag[1]+nwtag[1]);
  float den_e = (float) (nrtag[2]+nwtag[2]);
  float den_k = (float) (nrtag[3]+nwtag[3]);
  float den_kS= (float) (nrtag[4]+nwtag[4]);

  if(ntruekO) {
    if(den_m) ghrate_m = nghost_m/den_m*100;
    if(den_m) gherror_m= sqrt(nghost_m)/den_m*100;
    if(den_e) ghrate_e = nghost_e/den_e*100;
    if(den_e) gherror_e= sqrt(nghost_e)/den_e*100;
    if(den_k) ghrate_k = nghost_k/den_k*100;
    if(den_k) gherror_k= sqrt(nghost_k)/den_k*100;
    if(den_kS)ghrate_kS = nghost_kS/den_kS*100;
    if(den_kS)gherror_kS= sqrt(nghost_kS)/den_kS*100;

    if(den_m) cout<<"\nMuon Ghosts = "<<ghrate_m<<" +- "<<gherror_m<<" %"<<endl;
    if(den_e) cout<<  "Elec        = "<<ghrate_e<<" +- "<<gherror_e<<" %"<<endl;
    if(den_k) cout<<  "Kaon_opp    = "<<ghrate_k<<" +- "<<gherror_k<<" %"<<endl;
    if(nghost_kS && den_kS)
      cout<<  "Kaon_same   = "<<ghrate_kS<<" +- "<<gherror_kS<<" %"<<endl;
    cout <<BOLD<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    cout <<setprecision(3)<<"PID purity (in r+w), and efficiency (P>5GeV)\n"<<ENDC;
    if(den_m) cout <<"muon:       "<<(nidm/den_m*100)
		   <<"       "<<(nllm/nlm*100)<<" %"<<endl;
    if(den_e) cout <<"elec:       "<<(nide/den_e*100)
		   <<"       "<<(nlle/nle*100)<<" %"<<endl;
    if(den_k) cout <<"kaon:       "<<(nidk/den_k*100)
		   <<"       "<<(nllk/nlk*100)<<" %"<<endl;
    if(isBs && den_kS)
      cout <<"kaonS:      "<<(nidkS/den_kS*100)
	   <<"       "<<(nllkS/nlkS*100)<<" %";
  }
  cout<<BOLD<<"\n=========================================================";
  cout<< "\n Category            EFF.          Etag         Wrong TF"
      << FAINT <<"      r       w\n"<<ENDC;

  for( int it=1; it < 19; it++ ) {
    rtag = wtag = 0; 
    string cats;
    if(it== 1) cats =  "   mu only";
    if(it== 2) cats =  "    e only";
    if(it== 3) cats =  "    k only";
    if(it== 4) cats =  "    mu + k";
    if(it== 5) cats =  "     e + k";
    if(it== 6) cats =  "  vtx only";
    if(it== 7) cats =  "     ps/ks";
    if(it== 8) cats =  "   mu + ks";
    if(it== 9) cats =  "    e + ks";
    if(it==10) cats =  "    k + ks";
    if(it==11) cats =  "   mu+k+ks";
    if(it==12) cats =  "    e+k+ks";
    if(it==13){cats =  "  OS muons"; rtag = nrtag[1]; wtag = nwtag[1]; }
    if(it==14){cats =  "  OS elect"; rtag = nrtag[2]; wtag = nwtag[2]; }
    if(it==15){cats =  "  OS kaons"; rtag = nrtag[3]; wtag = nwtag[3]; }
    if(it==16){cats =  "  SS pions"; rtag= nrtag[4]; wtag= nwtag[4]; }
    if(isBs)if(it==16){cats ="  SS kaons"; rtag= nrtag[4]; wtag= nwtag[4]; }
    if(it==17){cats =  "  VertexCh"; rtag = nrtag[5]; wtag = nwtag[5]; }
    if(CombTechnique=="NNet" && it<13) cats =  "  NNet    "; 
    if(CombTechnique=="PID"  && it<13) cats =  "  PID     "; 
    else if(it==13) 
      cout<<"---------------------------------------------------------\n";

    if(it<13) { rtag = nrt[it]; wtag = nwt[it]; }

    if(rtag+wtag == 0) continue; //empty category

    utag = nsele-rtag-wtag;       // untagged
    double omtag = wtag/(rtag+wtag);
    double eftag = (rtag+wtag)/nsele;           // tagging efficiency
    double epsil = eftag*pow(1-2*omtag,2);      // effective efficiency
    if(rtag<wtag) epsil= -epsil;

    if(it<13){
      rtt      += rtag;
      wtt      += wtag;
      ef_tot   += eftag;
      effe_tot += epsil;
    }

    //errors on efficiency and omega
    double eftag_err= sqrt((rtag*utag + utag*wtag)/nsele)/nsele;
    double omtag_err= sqrt( rtag*wtag /(rtag+wtag) ) / (rtag+wtag);
    epsilerr = sqrt((pow(rtag - wtag,2)*
                     (-(pow(rtag - wtag,2)*(rtag +wtag))+nsele
                      *(pow(rtag,2) +14*rtag*wtag+ pow(wtag,2))))
                    /(pow(rtag+wtag+utag,3)*pow(rtag + wtag,3)));
    if(it<13) epsilerrtot = sqrt(pow(epsilerrtot,2)+pow(epsilerr,2));

    //PRINT: ----------------------------------
    cout.setf(ios::fixed);
    if(it<13) cout<<VIOLETA<<setw(2)<< it; else cout<<AZUL<<"**";
    cout<<BOLD<< cats<<setprecision(2)
        <<" "<<setw(8)<< epsil*100 << "+-" << epsilerr*100 
        <<" "<<setw(8)<< eftag*100 << "+-" <<eftag_err*100
        <<" "<<setw(8)<< omtag*100 << "+-" <<omtag_err*100
        <<" "<<ENDC<<FAINT<<setw(7)<< (int) rtag
        <<" "<<setw(7)<< (int) wtag<<ENDC
	<< endl;
  }

  //calculate global tagging performances -------------------------------

  //equivalent value of the wrong tag fraction 
  double avw_invert=(1-sqrt(fabs(effe_tot)/ef_tot))/2;
  if(effe_tot<0) avw_invert= 1-avw_invert;
  double utt = nsele-rtt-wtt;
  double eftot_err= sqrt((rtt*utt + utt*wtt)/nsele)/nsele;
  double avw_invert_err= sqrt( rtt*wtt /(rtt+wtt) ) / (rtt+wtt);

  cout << "---------------------------------------------------------\n";
  cout << "Tagging efficiency =  "<<setw(5)
       << ef_tot*100 << " +- "<<eftot_err*100<< " %"<< endl;       
  cout << "Wrong Tag fraction =  "<<setw(5)
       << avw_invert*100 << " +- " <<avw_invert_err*100 << " %"<< endl;
  cout << ROJO <<BOLD<< "EFFECTIVE COMB. TE =  "<<setw(5)
       << effe_tot*100 << " +- "<<epsilerrtot*100<< " %" <<ENDC<<FAINT
       << "    (Total events = " << int(nsele) <<")"<<ENDC<< endl;
  cout <<BOLD<< 
    "=========================================================\n\n"<<ENDC;
  return effe_tot*100;
}
//=====================================================================
#include <fstream>
#include <iostream>
#include <string>

#ifdef __CINT__
#undef __attribute__
#define __attribute__(X)
#endif
#include <dirent.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
std::vector<TString> getFiles(string dir) {

  std::vector<TString> filelist(0);
  ifstream fin;
  string filepath;
  DIR *dp;
  struct dirent *dirp;
  struct stat filestat;

  dp = opendir( dir.c_str() );
  if (dp == NULL) {
    cout << "Error opening " << dir << endl;
    return filelist;
  }
  while ((dirp = readdir( dp ))) {
    filepath = dir + "/" + dirp->d_name;
    // If the file is a directory (or is in some way invalid) we'll skip it 
    if (stat( filepath.c_str(), &filestat )) continue;
    if (S_ISDIR( filestat.st_mode ))         continue;
    const char* astr = "root";//check that filepath word contains root
    const char* conroot = strstr( filepath.c_str(), astr );
    if( !conroot ) continue;
    //cout<<conroot<<"file  "<<filepath<<endl;
    filelist.push_back(filepath);
  }
  closedir( dp );
  std::sort(filelist.begin(), filelist.end());
  return filelist;
}
