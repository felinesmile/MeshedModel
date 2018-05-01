
#include <math.h>
#include <fstream>
#include "Hvdc_PQMVA.h"
#include "Hvdc_SETT_IN_1_EXTRA.h"
#include "Hvdc_SETT_IN_1.h"
#include "Hvdc_INPUTS_TDM.h"
#include "Hvdc_INPUTS_SCALE_1.h"
#include "Hvdc_IN_CELL_SCALE.h"
#include "Hvdc_INPUTS_SCALE_2.h"
#include "Hvdc_INITIALIZE0.h"
#include "Hvdc_INITIALIZE.h"
#include "Hvdc_EXTIN1.h"
#include "Hvdc_IN_APPL_L4_1.h"
#include "Hvdc_DQ_TRANSFORM.h"
#include "Hvdc_ABNORM_IV_DET.h"
#include "Hvdc_ABNORM_UD_DET.h"
#include "Hvdc_TRANSIENT_DET.h"
#include "Hvdc_ABNORM_UAC_DET.h"
#include "Hvdc_UCONV_FILT.h"
#include "Hvdc_SETTING_CALC.h"
#include "Hvdc_UPCC_FILT.h"
#include "Hvdc_IV_FILT.h"
#include "Hvdc_IPRIM_FILT.h"
#include "Hvdc_PLL1.h"
#include "Hvdc_PLL2.h"
#include "Hvdc_PQUI_CALC.h"
#include "Hvdc_P_CTRL.h"
#include "Hvdc_UDC_CTRL.h"
#include "Hvdc_ENF_UAC_CTRL.h"
#include "Hvdc_UAC_CTRL.h"
#include "Hvdc_UAC_CTRL_WEAKAC.h"
#include "Hvdc_Q_CTRL.h"
#include "Hvdc_IREF_LIM.h"
#include "Hvdc_C_CTRL.h"
#include "Hvdc_V_CTRL.h"
#include "Hvdc_UREF_LIM.h"
#include "Hvdc_AC_CURR_LIM.h"
#include "Hvdc_OVERM_LIM.h"
#include "Hvdc_CHOP_TRANS_CTRL.h"
#include "Hvdc_CHOP_SUPV_CTRL.h"
#include "Hvdc_PROT.h"
#include "Hvdc_UVOUT.h"
#include "Hvdc_BLOCKEX.h"
#include "Hvdc_CURRENTINJECT.h"
#include "Hvdc_Warning.h"
#include "ConverterDataDefinitions.h"
#include "Hvdc_Light_Converter.h"

void HVDC_LIGHT_CONVERTER_V2_0(int *iswx,float *fInternalStepx,int *iCalcModex, int *InitMethodUdc, ConverterDataDef *conv, float *fInternalTimex, int *ConvNrx)
{

char* strDetailedLight_ConverterVersionString="Hvdc_DetailedLight_Converter_V2.0";
static FILE *pfiles[20]; 
int FIRSTTIMEINSTEP=*iswx;  
int ConvNr=*ConvNrx; 
float fInternalStep=(float) *fInternalStepx; 
double fInternalTime= *fInternalTimex; 
int iCalcMode=*iCalcModex; 
char* sErrorMessage=conv->sErrorMessage; 
int INITMETHODUD= *InitMethodUdc;  
int iAutoInitLoadFlow=0; 
float DummyForPlots[iRealPlotMaxConv+1]; 
int STEPUDCP= (int)conv->OptionalParam[3]; 
int DOVOLTAGECHECK=1; 
int DOLIMITACCURRENT=1; 
float *pp=DummyForPlots+1; 
float *states=conv->fRealStore; 
int *statesInt=conv->iIntStore; 
const float Ts = 1.0e-4f ; 
const float Pi= 3.1415926536f;  
const float TwoTimesPi = 2.0f * Pi ; 
const float Pi2Ts=2*Pi*Ts ; 


int BLOCKEDIND=1- (conv->DEBLOCKIND); 


float   *ffeedback=conv->fFBSignalArray; 
int     *ifeedback=conv->iFBSignalArray; 
 
//*****************************************


//  Outputs fromPQMVA
float QTRAFOPRIM;
float ACTPWR;

//  Outputs fromSETT_IN_1_EXTRA
float QREFMIN;
float QREFMAX;
int CONVBROPIND;
int CONVBEARTHFLTTRIP;
int POLEABNVOLTBLOCKCHOPPOS;
int POLEABNVOLTBLOCKCHOPNEG;
float DIDREF1;
float DIQREF1;

//  Outputs fromSETT_IN_1
float KUPU;
float UBASE;
float RDCPU;
float ZBASE;
float UPRIMBAS;
 // conv->TRAFORATIO;
float KPRITOSEC;
float TRAFOXTP;
float QREFLIMH;
float QREFLIML;
float QACVCLIML;
float QACVCLIMH;
float OMEGAMULL;
float MMAX;
float UPCCLIMH;
float UPCCLIML;
float RPCKP;
float RPCKPTRAN;
float RPCTI;
float RPCTIWAC;
float UACKP;
float UACKPTRAN;
float UACTI;
float UACTITRAN;
float UACSLOPE;
float TRANPOSL;
float TRANTRANABL;
float PFNORM;
float KGFV;
float KG;
float CHOPCAPACITY;
float CHOPMARG;
float CHOPCOOLRATE;
float IVTRANSMAX;
float IVMAXREC;
float IVMAXINV;
float IBASEPRIM;
float QTRAFOPRIMFILT;
float PINPUT;
float PREFMIN;
float PREFMAX;
float MMAXWARN;

//  Outputs fromINPUTS_TDM
float UCONVALPHAKV;
float IPRIMALPHAAMP;
float IPRIMBETAAMP;
float UPCCALPHAKV;
float UPCCBETAKV;
float UCONVBETAKV;
float UDPKV;
float ICHOPV1AMP;
float UDNKV;
float ICHOPV2AMP;

//  Outputs fromINPUTS_SCALE_1
float IVALPHA;
float IVBETA;

//  Outputs fromIN_CELL_SCALE
float KXIUDC;
float UARMMEANUDPU;

//  Outputs fromINPUTS_SCALE_2
float IPRIMALPHA;
float IPRIMBETA;
float UCONVALPHA;
float UCONVBETA;
float UPCCALPHA;
float UPCCBETA;
float UDMEAN;
float UDMEANUDPU;

//  Outputs fromINITIALIZE0

//  Outputs fromINITIALIZE

//  Outputs fromEXTIN1
float ACVCQLIM;
float DELTAPOW;

//  Outputs fromIN_APPL_L4_1
float QPCCORD;
float PPCCORD;
float UDORD;
float UPCCACVCQLIMADD;
int CHOPONPERMIT;
float UPCCORD;

//  Outputs fromDQ_TRANSFORM
float IVD;
float IVQ;
float UCONVD;
float UCONVQ;
float IPRIMQ;
float UPCCPOS;
float UTRANS;

//  Outputs fromABNORM_IV_DET
int OVERIV;

//  Outputs fromABNORM_UD_DET
int UDCHI;
int UDCLO;
float UDCPPARTADD;
float KID;
int REDUTCONT;
float KPDC;

//  Outputs fromTRANSIENT_DET
int OPMOD;
int DISTURBOUT;
int POSSEQOUT;
int PDIFFHI;
int ABNORMUAC;

//  Outputs fromABNORM_UAC_DET
int ABNUACTRUE;
int ABNUACTRUPL;

//  Outputs fromUCONV_FILT
float UCONVQPOSF;
float UCONVMAGFILT;
float UCONVDPOS;
float UCONVDPOSF;
float UCONVQPOS;

//  Outputs fromSETTING_CALC
float IVMAX;
float IQUACLIML;
float IQLIMH;
float IQLIML;
float IQUACLIMH;

//  Outputs fromUPCC_FILT
float UPCCPOS1;
float UPCCPOSNOFIL;
float UPCCALPHAPOS1;
float UPCCBETAPOS1;

//  Outputs fromIV_FILT
float YDDAM;
float YQDAM;
float IVDDEFAL;
float IVQDEFAL;

//  Outputs fromIPRIM_FILT
float IPRIMALPHAPOS1;
float IPRIMBETAPOS1;

//  Outputs fromPLL1
float DELTFREQ;
float DELTAFG;

//  Outputs fromPLL2
float SINWT;
float COSWT;
float THETAA;
 // conv->DELTAFKOUT;

//  Outputs fromPQUI_CALC
float PPCCNOFIL;
int RECTOP;
float DROPPART;
 // conv->PPCC;
 // conv->QPCC;

//  Outputs fromP_CTRL
float DELTAUDC;
float P_ERR_FLT;

//  Outputs fromUDC_CTRL
float PORD;
 // conv->IVDORD;
float DIDREF;

//  Outputs fromENF_UAC_CTRL
int RPCACT;
float UACREFEXE;

//  Outputs fromUAC_CTRL
int QCTRLON;
 // conv->IVQORD;

//  Outputs fromUAC_CTRL_WEAKAC
float E;

//  Outputs fromQ_CTRL
float IQORDQC;
float DUACQC;

//  Outputs fromIREF_LIM
float DELTAIQREF;
float DELTAIDREF;
 // conv->IVDREF;
 // conv->IVQREF;

//  Outputs fromC_CTRL
float YQREF;
float YDREF;

//  Outputs fromV_CTRL
float YDREFP;
float YQREFP;

//  Outputs fromUREF_LIM
 // conv->UVOUTABSMAX;
 // conv->MODINDEX;
float YDPOSOUTX;
float YQPOSOUTX;
 // conv->OVERMOD;
int ENABUPH;
float MINDEXO;

//  Outputs fromAC_CURR_LIM
 // conv->LARGEVOLTAGESTEP;
 // conv->YDPOSOUT;
 // conv->YQPOSOUT;

//  Outputs fromOVERM_LIM
int DCVCINTFRZ;
int ACVCINTFRZ;
float DURLIMM;
float DIQLIMM;
int LIMMACT;

//  Outputs fromCHOP_TRANS_CTRL
 // conv->CHOPPERONP;
 // conv->CHOPPERONN;
int CHOPPERONPPERMIT;
int CHOPPERONNPERMIT;

//  Outputs fromCHOP_SUPV_CTRL
int CHOPPERON;
int CHOPLOADHIGH;
 // conv->ECHOP;

//  Outputs fromPROT
int TRIPORDER;

//  Outputs fromUVOUT
float UVOUTALFAPRE;
float UVOUTBETAPRE;

//  Outputs fromBLOCKEX
int TEMPBLOCK;
 // conv->UVOUTALFA;
 // conv->UVOUTBETA;

//  Outputs fromCURRENTINJECT
 // conv->IINJECTBETA;
 // conv->IINJECTALFA;
 // conv->PLOS;
 // conv->DCINJECTASP;
 // conv->PDCINJECT;
 // conv->IDCINJECT;
 // conv->ICHOPPU;

//  Outputs fromINITIALIZE

//  Outputs fromINITIALIZE0
float IVDORDINIT;
//*************Real feedbacks****************************

       PINPUT=*ffeedback;ffeedback++;
 conv->ICHOPPU=*ffeedback;ffeedback++;
       SINWT=*ffeedback;ffeedback++;
       COSWT=*ffeedback;ffeedback++;
       UPCCPOSNOFIL=*ffeedback;ffeedback++;
 conv->PPCC=*ffeedback;ffeedback++;
       PORD=*ffeedback;ffeedback++;
       DELTAIDREF=*ffeedback;ffeedback++;
       DIQLIMM=*ffeedback;ffeedback++;
       IQORDQC=*ffeedback;ffeedback++;
       DELTAIQREF=*ffeedback;ffeedback++;
       DURLIMM=*ffeedback;ffeedback++;
       DUACQC=*ffeedback;ffeedback++;
//*************Integer feedbacks****************************

       ENABUPH=*ifeedback;ifeedback++;
       OPMOD=*ifeedback;ifeedback++;
       PDIFFHI=*ifeedback;ifeedback++;
       POSSEQOUT=*ifeedback;ifeedback++;
       RECTOP=*ifeedback;ifeedback++;
       DCVCINTFRZ=*ifeedback;ifeedback++;
       ACVCINTFRZ=*ifeedback;ifeedback++;
       LIMMACT=*ifeedback;ifeedback++;
       CHOPPERON=*ifeedback;ifeedback++;
       CHOPLOADHIGH=*ifeedback;ifeedback++;
conv->NumberOfFBRealStates=ffeedback-conv->fFBSignalArray;

ffeedback=conv->fFBSignalArray; // Reset ffeedback to the start

conv->NumberOfFBIntegerStates=ifeedback-conv->iFBSignalArray;

ifeedback=conv->iFBSignalArray; // Reset ifeedback to the start

//*****************************************



 Hvdc_PQMVA(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,conv->IPRIMALPHAPU
        ,conv->UPCCBETAPU
        ,conv->UPCCALPHAPU
        ,conv->IPRIMBETAPU
        ,&QTRAFOPRIM
        ,&ACTPWR
        ,&states,&statesInt, &pp);    


 Hvdc_SETT_IN_1_EXTRA(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,PINPUT
        ,conv->BASEPOWER
        ,&QREFMIN
        ,&QREFMAX
        ,&CONVBROPIND
        ,&CONVBEARTHFLTTRIP
        ,&POLEABNVOLTBLOCKCHOPPOS
        ,&POLEABNVOLTBLOCKCHOPNEG
        ,&DIDREF1
        ,&DIQREF1
        ,&states,&statesInt, &pp);    


 Hvdc_SETT_IN_1(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,conv->USECNOM
        ,conv->N1TAP
        ,conv->N2TAP
        ,QTRAFOPRIM
        ,ACTPWR
        ,conv->REACTORLV
        ,conv->FUNDFREQ
        ,QREFMAX
        ,QREFMIN
        ,conv->TRAFOXT
        ,conv->RDCCABLE
        ,conv->BASEPOWER
        ,conv->UACBASERMS
        ,conv->UPRIMNOM
        ,&KUPU
        ,&UBASE
        ,&RDCPU
        ,&ZBASE
        ,&UPRIMBAS
        ,&conv->TRAFORATIO
        ,&KPRITOSEC
        ,&TRAFOXTP
        ,&QREFLIMH
        ,&QREFLIML
        ,&QACVCLIML
        ,&QACVCLIMH
        ,&OMEGAMULL
        ,&MMAX
        ,&UPCCLIMH
        ,&UPCCLIML
        ,&RPCKP
        ,&RPCKPTRAN
        ,&RPCTI
        ,&RPCTIWAC
        ,&UACKP
        ,&UACKPTRAN
        ,&UACTI
        ,&UACTITRAN
        ,&UACSLOPE
        ,&TRANPOSL
        ,&TRANTRANABL
        ,&PFNORM
        ,&KGFV
        ,&KG
        ,&CHOPCAPACITY
        ,&CHOPMARG
        ,&CHOPCOOLRATE
        ,&IVTRANSMAX
        ,&IVMAXREC
        ,&IVMAXINV
        ,&IBASEPRIM
        ,&QTRAFOPRIMFILT
        ,&PINPUT
        ,&PREFMIN
        ,&PREFMAX
        ,&MMAXWARN
        ,&states,&statesInt, &pp);    


 Hvdc_INPUTS_TDM(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,IBASEPRIM
        ,conv->ICHOPPU
        ,conv->PNOM
        ,KUPU
        ,conv->UCONVBETAPU
        ,conv->UCONVALPHAPU
        ,UPRIMBAS
        ,conv->UPCCBETAPU
        ,conv->UPCCALPHAPU
        ,conv->UDPU
        ,conv->IPRIMBETAPU
        ,conv->UDRATE
        ,conv->IPRIMALPHAPU
        ,&UCONVALPHAKV
        ,&IPRIMALPHAAMP
        ,&IPRIMBETAAMP
        ,&UPCCALPHAKV
        ,&UPCCBETAKV
        ,&UCONVBETAKV
        ,&UDPKV
        ,&ICHOPV1AMP
        ,&UDNKV
        ,&ICHOPV2AMP
        ,&states,&statesInt, &pp);    


 Hvdc_INPUTS_SCALE_1(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,conv->IVBETAPU
        ,conv->IVALPHAPU
        ,&IVALPHA
        ,&IVBETA
        ,&states,&statesInt, &pp);    


 Hvdc_IN_CELL_SCALE(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,conv->UDRATE
        ,UBASE
        ,conv->UDPU
        ,&KXIUDC
        ,&UARMMEANUDPU
        ,&states,&statesInt, &pp);    


 Hvdc_INPUTS_SCALE_2(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,UPRIMBAS
        ,KXIUDC
        ,conv->UDPU
        ,IBASEPRIM
        ,KUPU
        ,UCONVBETAKV
        ,UCONVALPHAKV
        ,IPRIMBETAAMP
        ,IPRIMALPHAAMP
        ,UPCCBETAKV
        ,UPCCALPHAKV
        ,conv->TRAFORATIO
        ,&IPRIMALPHA
        ,&IPRIMBETA
        ,&UCONVALPHA
        ,&UCONVBETA
        ,&UPCCALPHA
        ,&UPCCBETA
        ,&UDMEAN
        ,&UDMEANUDPU
        ,&states,&statesInt, &pp);    
 if(iCalcMode==0){


 Hvdc_INITIALIZE0(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,conv->FVCTRL
        ,conv->OPMODEXT
        ,UCONVBETA
        ,UCONVALPHA
        ,IVBETA
        ,IVALPHA
        ,conv->EXWEAKAC
        ,&conv->PMODTOTAL
        ,&POSSEQOUT
        ,&PDIFFHI
        ,&ENABUPH
        ,&OPMOD
        ,&DELTAIDREF
        ,&DCVCINTFRZ
        ,&ACVCINTFRZ
        ,&DELTAIQREF
        ,&DIQLIMM
        ,&LIMMACT
        ,&DURLIMM
        ,&THETAA
        ,&SINWT
        ,&COSWT
        ,&E
        ,&conv->IVQORD
        ,&IVDORDINIT
        ,&IQORDQC
        ,&DUACQC
        ,&TRIPORDER
        ,&states,&statesInt, &pp);    
 }
 if(iCalcMode==0){


 Hvdc_INITIALIZE(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,conv->DELTAUDCNOM
        ,INITMETHODUD
        ,conv->PCTRLON
        ,conv->FVCTRL
        ,IVDORDINIT
        ,conv->EXWEAKAC
        ,conv->BASEPOWER
        ,KPRITOSEC
        ,UACSLOPE
        ,conv->UDRATE
        ,RDCPU
        ,KXIUDC
        ,UARMMEANUDPU
        ,IPRIMBETA
        ,IPRIMALPHA
        ,UPCCBETA
        ,KUPU
        ,UPCCALPHA
        ,&conv->PPCC
        ,&conv->QPCC
        ,&conv->PREFMAIN
        ,&conv->QREFOMAIN
        ,&conv->UPCCORDJS
        ,&RECTOP
        ,&PORD
        ,&conv->UDORDMAIN
        ,&UDORD
        ,&conv->UDPU
        ,&DELTAUDC
        ,&states,&statesInt, &pp);    
 }


 Hvdc_EXTIN1(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,QREFMAX
        ,conv->PMODTOTAL
        ,conv->PREFMAIN
        ,conv->PCTRLON
        ,PREFMIN
        ,PREFMAX
        ,QREFMIN
        ,conv->ACVCTRLON
        ,QTRAFOPRIMFILT
        ,&ACVCQLIM
        ,&DELTAPOW
        ,&states,&statesInt, &pp);    


 Hvdc_IN_APPL_L4_1(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,conv->BASEPOWER
        ,conv->DELTAUPCCREFMAIN
        ,conv->ENABLECHOPSUP
        ,QREFMIN
        ,QREFMAX
        ,DELTAPOW
        ,conv->PREFMAIN
        ,conv->QREFOMAIN
        ,conv->UPCCORDJS
        ,ACVCQLIM
        ,conv->UDORDMAIN
        ,KUPU
        ,&QPCCORD
        ,&PPCCORD
        ,&UDORD
        ,&UPCCACVCQLIMADD
        ,&CHOPONPERMIT
        ,&UPCCORD
        ,&states,&statesInt, &pp);    


 Hvdc_DQ_TRANSFORM(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,UCONVALPHA
        ,SINWT
        ,COSWT
        ,IPRIMALPHA
        ,IPRIMBETA
        ,UPCCALPHA
        ,UPCCBETA
        ,IVBETA
        ,IVALPHA
        ,UCONVBETA
        ,&IVD
        ,&IVQ
        ,&UCONVD
        ,&UCONVQ
        ,&IPRIMQ
        ,&UPCCPOS
        ,&UTRANS
        ,&states,&statesInt, &pp);    


 Hvdc_ABNORM_IV_DET(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,IVBETA
        ,UPCCPOSNOFIL
        ,IVTRANSMAX
        ,conv->FVCTRL
        ,UPCCPOS
        ,IVMAXREC
        ,IVALPHA
        ,&OVERIV
        ,&states,&statesInt, &pp);    


 Hvdc_ABNORM_UD_DET(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,UARMMEANUDPU
        ,conv->PCTRLON
        ,conv->EXWEAKAC
        ,ENABUPH
        ,OPMOD
        ,STEPUDCP
        ,PDIFFHI
        ,POSSEQOUT
        ,&UDCHI
        ,&UDCLO
        ,&UDCPPARTADD
        ,&KID
        ,&REDUTCONT
        ,&KPDC
        ,&states,&statesInt, &pp);    


 Hvdc_TRANSIENT_DET(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,OVERIV
        ,conv->OPMODEXT
        ,conv->FVCTRL
        ,conv->PPCC
        ,UDMEAN
        ,TRANTRANABL
        ,UTRANS
        ,UPCCPOS
        ,TRANPOSL
        ,&OPMOD
        ,&DISTURBOUT
        ,&POSSEQOUT
        ,&PDIFFHI
        ,&ABNORMUAC
        ,&states,&statesInt, &pp);    


 Hvdc_ABNORM_UAC_DET(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,UPCCALPHA
        ,OVERIV
        ,UPCCBETA
        ,&ABNUACTRUE
        ,&ABNUACTRUPL
        ,&states,&statesInt, &pp);    


 Hvdc_UCONV_FILT(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,UCONVQ
        ,UCONVD
        ,&UCONVQPOSF
        ,&UCONVMAGFILT
        ,&UCONVDPOS
        ,&UCONVDPOSF
        ,&UCONVQPOS
        ,&states,&statesInt, &pp);    


 Hvdc_SETTING_CALC(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,RECTOP
        ,QACVCLIMH
        ,QACVCLIML
        ,QREFLIML
        ,QREFLIMH
        ,UCONVMAGFILT
        ,IVMAXINV
        ,IVMAXREC
        ,&IVMAX
        ,&IQUACLIML
        ,&IQLIMH
        ,&IQLIML
        ,&IQUACLIMH
        ,&states,&statesInt, &pp);    


 Hvdc_UPCC_FILT(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,UPCCBETA
        ,UPCCALPHA
        ,&UPCCPOS1
        ,&UPCCPOSNOFIL
        ,&UPCCALPHAPOS1
        ,&UPCCBETAPOS1
        ,&states,&statesInt, &pp);    


 Hvdc_IV_FILT(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,conv->FVCTRL
        ,IVMAXREC
        ,REDUTCONT
        ,IVQ
        ,IVD
        ,&YDDAM
        ,&YQDAM
        ,&IVDDEFAL
        ,&IVQDEFAL
        ,&states,&statesInt, &pp);    


 Hvdc_IPRIM_FILT(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,IPRIMBETA
        ,IPRIMALPHA
        ,&IPRIMALPHAPOS1
        ,&IPRIMBETAPOS1
        ,&states,&statesInt, &pp);    


 Hvdc_PLL1(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,UCONVQPOS
        ,PFNORM
        ,KGFV
        ,ABNUACTRUPL
        ,OVERIV
        ,ABNORMUAC
        ,KG
        ,PORD
        ,conv->PPCC
        ,conv->DEBLOCKIND
        ,conv->FVCTRL
        ,conv->EXWEAKAC
        ,UCONVDPOS
        ,&DELTFREQ
        ,&DELTAFG
        ,&states,&statesInt, &pp);    


 Hvdc_PLL2(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,DELTFREQ
        ,conv->TRESETOFFSETFROMLF
        ,DELTAFG
        ,conv->FVCTRL
        ,ENABUPH
        ,&SINWT
        ,&COSWT
        ,&THETAA
        ,&conv->DELTAFKOUT
        ,&states,&statesInt, &pp);    


 Hvdc_PQUI_CALC(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,UPCCBETAPOS1
        ,UPCCALPHAPOS1
        ,UACSLOPE
        ,KPRITOSEC
        ,IPRIMBETAPOS1
        ,IPRIMALPHAPOS1
        ,&PPCCNOFIL
        ,&RECTOP
        ,&DROPPART
        ,&conv->PPCC
        ,&conv->QPCC
        ,&states,&statesInt, &pp);    


 Hvdc_P_CTRL(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,conv->PPCC
        ,conv->DELTAUDCNOM
        ,STEPUDCP
        ,UDORD
        ,PPCCNOFIL
        ,UDCLO
        ,RDCPU
        ,OPMOD
        ,UDCHI
        ,BLOCKEDIND
        ,conv->FVCTRL
        ,conv->PCTRLON
        ,PPCCORD
        ,&DELTAUDC
        ,&P_ERR_FLT
        ,&states,&statesInt, &pp);    


 Hvdc_UDC_CTRL(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,INITMETHODUD
        ,UDCPPARTADD
        ,ABNUACTRUE
        ,UDMEANUDPU
        ,conv->FVCTRL
        ,IVDDEFAL
        ,IVMAXINV
        ,UDCLO
        ,UDORD
        ,UDCHI
        ,KXIUDC
        ,UARMMEANUDPU
        ,DELTAUDC
        ,conv->PCTRLON
        ,PPCCORD
        ,BLOCKEDIND
        ,DCVCINTFRZ
        ,KPDC
        ,KID
        ,IVMAXREC
        ,DELTAIDREF
        ,&PORD
        ,&conv->IVDORD
        ,&DIDREF
        ,&states,&statesInt, &pp);    


 Hvdc_ENF_UAC_CTRL(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,UPCCPOS1
        ,UPCCACVCQLIMADD
        ,DISTURBOUT
        ,QPCCORD
        ,conv->QPCC
        ,POSSEQOUT
        ,DROPPART
        ,UPCCORD
        ,UPCCLIML
        ,UPCCLIMH
        ,conv->ACVCTRLON
        ,&RPCACT
        ,&UACREFEXE
        ,&states,&statesInt, &pp);    


 Hvdc_UAC_CTRL(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,UPCCPOS1
        ,IQUACLIML
        ,DIQLIMM
        ,conv->EXWEAKAC
        ,UACTI
        ,UACTITRAN
        ,UACKP
        ,UACKPTRAN
        ,IVQDEFAL
        ,IQUACLIMH
        ,OPMOD
        ,IQORDQC
        ,DELTAIQREF
        ,conv->FVCTRL
        ,RPCACT
        ,ACVCINTFRZ
        ,BLOCKEDIND
        ,UACREFEXE
        ,UPCCPOSNOFIL
        ,&QCTRLON
        ,&conv->IVQORD
        ,&states,&statesInt, &pp);    


 Hvdc_UAC_CTRL_WEAKAC(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,DURLIMM
        ,LIMMACT
        ,conv->EXWEAKAC
        ,IPRIMQ
        ,TRAFOXTP
        ,KPRITOSEC
        ,DROPPART
        ,ABNORMUAC
        ,conv->FVCTRL
        ,DUACQC
        ,conv->DEBLOCKIND
        ,UPCCORD
        ,UPCCPOS1
        ,&E
        ,&states,&statesInt, &pp);    


 Hvdc_Q_CTRL(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,QPCCORD
        ,IQLIML
        ,RPCTIWAC
        ,IVQDEFAL
        ,conv->EXWEAKAC
        ,conv->FVCTRL
        ,IQLIMH
        ,RPCTI
        ,RPCKP
        ,RPCKPTRAN
        ,OPMOD
        ,conv->IVQORD
        ,QCTRLON
        ,DELTAIQREF
        ,ACVCINTFRZ
        ,BLOCKEDIND
        ,conv->QPCC
        ,&IQORDQC
        ,&DUACQC
        ,&states,&statesInt, &pp);    


 Hvdc_IREF_LIM(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,conv->IVDORD
        ,P_ERR_FLT
        ,conv->FVCTRL
        ,DIQREF1
        ,DIDREF1
        ,OPMOD
        ,IVMAX
        ,UPCCPOS1
        ,conv->IVQORD
        ,&DELTAIQREF
        ,&DELTAIDREF
        ,&conv->IVDREF
        ,&conv->IVQREF
        ,&states,&statesInt, &pp);    


 Hvdc_C_CTRL(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,UCONVDPOSF
        ,OPMOD
        ,UCONVQPOSF
        ,OMEGAMULL
        ,conv->IVQREF
        ,conv->IVDREF
        ,&YQREF
        ,&YDREF
        ,&states,&statesInt, &pp);    


 Hvdc_V_CTRL(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,IVDDEFAL
        ,conv->EXWEAKAC
        ,conv->DEBLOCKIND
        ,OMEGAMULL
        ,YQDAM
        ,DIDREF
        ,YDDAM
        ,IVQDEFAL
        ,E
        ,&YDREFP
        ,&YQREFP
        ,&states,&statesInt, &pp);    


 Hvdc_UREF_LIM(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,MMAXWARN
        ,conv->DEBLOCKIND
        ,conv->EXWEAKAC
        ,conv->FVCTRL
        ,ABNUACTRUE
        ,YQREFP
        ,YDREFP
        ,YDREF
        ,YQREF
        ,MMAX
        ,UDMEAN
        ,&conv->UVOUTABSMAX
        ,&conv->MODINDEX
        ,&YDPOSOUTX
        ,&YQPOSOUTX
        ,&conv->OVERMOD
        ,&ENABUPH
        ,&MINDEXO
        ,&states,&statesInt, &pp);    


 Hvdc_AC_CURR_LIM(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,YQPOSOUTX
        ,FIRSTTIMEINSTEP
        ,DOLIMITACCURRENT
        ,OMEGAMULL
        ,DOVOLTAGECHECK
        ,UPCCBETA
        ,UPCCALPHA
        ,UCONVQPOS
        ,YDPOSOUTX
        ,UCONVDPOS
        ,&conv->LARGEVOLTAGESTEP
        ,&conv->YDPOSOUT
        ,&conv->YQPOSOUT
        ,&states,&statesInt, &pp);    


 Hvdc_OVERM_LIM(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,conv->QPCC
        ,conv->PPCC
        ,UACTI
        ,UACKP
        ,MMAX
        ,OMEGAMULL
        ,ABNORMUAC
        ,conv->IVQREF
        ,conv->OVERMOD
        ,IVQ
        ,conv->IVDREF
        ,IVD
        ,MINDEXO
        ,conv->DEBLOCKIND
        ,&DCVCINTFRZ
        ,&ACVCINTFRZ
        ,&DURLIMM
        ,&DIQLIMM
        ,&LIMMACT
        ,&states,&statesInt, &pp);    


 Hvdc_CHOP_TRANS_CTRL(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,CHOPPERON
        ,CHOPONPERMIT
        ,CONVBROPIND
        ,ICHOPV2AMP
        ,ICHOPV1AMP
        ,CONVBEARTHFLTTRIP
        ,CHOPLOADHIGH
        ,conv->ENABLECHOPSUP
        ,POLEABNVOLTBLOCKCHOPNEG
        ,POLEABNVOLTBLOCKCHOPPOS
        ,UDNKV
        ,UDPKV
        ,&conv->CHOPPERONP
        ,&conv->CHOPPERONN
        ,&CHOPPERONPPERMIT
        ,&CHOPPERONNPERMIT
        ,&states,&statesInt, &pp);    


 Hvdc_CHOP_SUPV_CTRL(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,UDMEANUDPU
        ,CHOPCOOLRATE
        ,conv->CHOPPERONN
        ,conv->CHOPPERONP
        ,conv->ENABLECHOPSUP
        ,ICHOPV2AMP
        ,ICHOPV1AMP
        ,CHOPCAPACITY
        ,CHOPMARG
        ,UDPKV
        ,conv->DEBLOCKIND
        ,ABNORMUAC
        ,UDNKV
        ,&CHOPPERON
        ,&CHOPLOADHIGH
        ,&conv->ECHOP
        ,&states,&statesInt, &pp);    


 Hvdc_PROT(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,conv->UDPU
        ,BLOCKEDIND
        ,conv->FVCTRL
        ,conv->PPCC
        ,&TRIPORDER
        ,&states,&statesInt, &pp);    


 Hvdc_UVOUT(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,conv->YQPOSOUT
        ,conv->YDPOSOUT
        ,SINWT
        ,COSWT
        ,&UVOUTALFAPRE
        ,&UVOUTBETAPRE
        ,&states,&statesInt, &pp);    


 Hvdc_BLOCKEX(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,UVOUTBETAPRE
        ,UVOUTALFAPRE
        ,TRIPORDER
        ,BLOCKEDIND
        ,UDMEAN
        ,conv->UDPU
        ,UCONVBETA
        ,UCONVALPHA
        ,UPCCBETA
        ,UPCCALPHA
        ,KPRITOSEC
        ,&TEMPBLOCK
        ,&conv->UVOUTALFA
        ,&conv->UVOUTBETA
        ,&states,&statesInt, &pp);    


 Hvdc_CURRENTINJECT(fInternalStep,iAutoInitLoadFlow,iCalcMode,&(conv->iErrorCount),iErrorCountMaxConv,sErrorMessage 
        ,conv->CHOPPERONP
        ,CHOPPERON
        ,KXIUDC
        ,conv->BASEPOWER
        ,conv->PNOM
        ,conv->UDPU
        ,conv->PLOSSADJUST
        ,UCONVBETA
        ,conv->UVOUTBETA
        ,TEMPBLOCK
        ,conv->UVOUTALFA
        ,TRIPORDER
        ,BLOCKEDIND
        ,UDMEAN
        ,UCONVALPHA
        ,OMEGAMULL
        ,&conv->IINJECTBETA
        ,&conv->IINJECTALFA
        ,&conv->PLOS
        ,&conv->DCINJECTASP
        ,&conv->PDCINJECT
        ,&conv->IDCINJECT
        ,&conv->ICHOPPU
        ,&states,&statesInt, &pp);    
//*****************************************

//**************Store Real feedbacks***************************

 *ffeedback=PINPUT;ffeedback++;
 *ffeedback=conv->ICHOPPU;ffeedback++;
 *ffeedback=SINWT;ffeedback++;
 *ffeedback=COSWT;ffeedback++;
 *ffeedback=UPCCPOSNOFIL;ffeedback++;
 *ffeedback=conv->PPCC;ffeedback++;
 *ffeedback=PORD;ffeedback++;
 *ffeedback=DELTAIDREF;ffeedback++;
 *ffeedback=DIQLIMM;ffeedback++;
 *ffeedback=IQORDQC;ffeedback++;
 *ffeedback=DELTAIQREF;ffeedback++;
 *ffeedback=DURLIMM;ffeedback++;
 *ffeedback=DUACQC;ffeedback++;
//**************Store Integer feedbacks***************************

 *ifeedback=ENABUPH;ifeedback++;
 *ifeedback=OPMOD;ifeedback++;
 *ifeedback=PDIFFHI;ifeedback++;
 *ifeedback=POSSEQOUT;ifeedback++;
 *ifeedback=RECTOP;ifeedback++;
 *ifeedback=DCVCINTFRZ;ifeedback++;
 *ifeedback=ACVCINTFRZ;ifeedback++;
 *ifeedback=LIMMACT;ifeedback++;
 *ifeedback=CHOPPERON;ifeedback++;
 *ifeedback=CHOPLOADHIGH;ifeedback++;
//*****************************************

 int nf=states-conv->fRealStore; 
 int ni=statesInt-conv->iIntStore; 
 int np=pp-DummyForPlots-1; 
 int errmax=iErrorCountMaxConv; 
  if(iCalcMode==0){ 
  conv->NumberOfRealStates=nf; 
  conv->NumberOfIntegerStates=ni;
   if(nf > iRealStoreMaxConv){
   char *mess="Non matching array dimension for real states";
   int lenmess=strlen(mess);
   HVDC_WARNING(mess,lenmess,&(conv->iErrorCount),&errmax,(conv->sErrorMessage),132);
   }
   if(ni> iIntStoreMaxConv){
   char *mess="Non matching array dimension for integer states";
   int lenmess=strlen(mess);
   HVDC_WARNING(mess,lenmess,&(conv->iErrorCount),&errmax,(conv->sErrorMessage),132);
   }
   if(np> iRealPlotMaxConv ){
   char *mess="Non matching array dimension in plotvariables";
   int lenmess=strlen(mess);
   HVDC_WARNING(mess,lenmess,&(conv->iErrorCount),&errmax,(conv->sErrorMessage),132);
   }
  int nfbf=ffeedback-conv->fFBSignalArray;
  int nfbi=ifeedback-conv->iFBSignalArray;
   if( nfbf > iFBSignalArrayFlt){
   char *mess="Non matching array dimension in real feedback variables";
   int lenmess=strlen(mess);
   HVDC_WARNING(mess,lenmess,&(conv->iErrorCount),&errmax,(conv->sErrorMessage),132);
   }
   if( nfbi > iFBSignalArrayInt){
   char *mess="Non matching array dimension in integer feedback variables";
   int lenmess=strlen(mess);	
   HVDC_WARNING(mess,lenmess,&(conv->iErrorCount),&errmax,(conv->sErrorMessage),132);
   }
  }
 if(conv->iInspect==1||conv->iInspect==-7971){ 
  if(conv->iInspectFileOpen==0){
  char names[80];
 #ifndef strcpy_s
  strcpy(names,"Converter_No_");
  strcat(names,conv->sConverterId);
   if(conv->iInspect==-7971)
   strcat(names,"_inspectFile.txt");
   else
   strcat(names,"_inspectFile.ins");
 #else
  strcpy_s(names,80,"Converter_No_");
  strcat_s(names,80,conv->sConverterId);
   if(conv->iInspect==-7971)
   strcat_s(names,80,"_inspectFile.txt");
   else
   strcat_s(names,80,"_inspectFile.ins");
 #endif
   if(conv->iInspect==-7971)
   pfiles[ConvNr-1]=fopen(names,"w");
   else
   pfiles[ConvNr-1]=fopen(names,"wb");
   if(pfiles[ConvNr-1]==NULL){
   conv->iInspectFileOpen=0;
   char *mess="Can't open Inspect file";
   perror(mess);
   int lenmess=strlen(mess);	
   HVDC_WARNING(mess,lenmess,&(conv->iErrorCount),&errmax,(conv->sErrorMessage),132);
   }else{
    if(conv->iInspect==-7971){
    fprintf(pfiles[ConvNr-1],"%d %s \n",np,strDetailedLight_ConverterVersionString);
    }else{
    fwrite(&np, sizeof(int), 1, pfiles[ConvNr-1]);
    int lenDetailedLight_ConverterVersionString = strlen(strDetailedLight_ConverterVersionString);
    fwrite(&lenDetailedLight_ConverterVersionString, sizeof(int), 1, pfiles[ConvNr-1]);
    fwrite(strDetailedLight_ConverterVersionString, sizeof(char), lenDetailedLight_ConverterVersionString, pfiles[ConvNr-1]);
    }
   conv->iInspectFileOpen=1;
   }
  }
 }
  if(iCalcMode>=0&&conv->iInspect==1&&conv->iInspectFileOpen==1){ 
  DummyForPlots[0]=(float) fInternalTime;
  fwrite(&DummyForPlots, sizeof(float), np+1, pfiles[ConvNr-1]);
  }
  if(iCalcMode>=0&&conv->iInspect==-7971&&conv->iInspectFileOpen==1){
  int i=1;
  fprintf(pfiles[ConvNr-1],"%f ",fInternalTime);
   while(i < np+1){
   fprintf(pfiles[ConvNr-1],"%f ", DummyForPlots[i]);
   i++;
   }
  fprintf(pfiles[ConvNr-1], "\n");
  }
}
// End of HVDC_LIGHT_CONVERTER_V2_0.                :-