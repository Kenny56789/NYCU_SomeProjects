//[Project] => [Settings] => [LINK] => Library=winmm.lib
#pragma comment(lib,"winmm.lib")
#include <mmsystem.h>
#include <math.h>

#define       BUF_NUM           8
#define       MAX_BUF_LEN     1400  // 10ms*k/ ADC Sample Rate=8000Hz
//#define       Sample_Rate    8000
#define       DT_AMPLITUDE  15000
#define       DT_DUR_LEN    32000 // /8000 Sample/1 Channel=4 Second

#define       FREQ_NUM            12
#define       FFT_SIZE           512
typedef struct { float real, image; } COMPLEX;

//void CALLBACK Record_Proc(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
//void CALLBACK Play_Proc(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
/*
void CALLBACK Record_Proc(HWAVEIN hWaveIn, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	// ¦sÁn­µ
	int Len;
	switch (uMsg)
	{
	case WIM_DATA:
		if (lpCA->RStart)
		{
			lpCA->Record_Speech(Speech, &Len); g_No += Len;
			if ((Out != 0) && (Len == 240)) fwrite(Speech, Len, sizeof(short), Out);
			printf("%d\r", g_No++);
		}
		break;
	}
}
*/






class CAudio
{
public:

	// ##############################
	// ## for Audio Device Control ##
	// ##############################	
	short        RSBuf[BUF_NUM][MAX_BUF_LEN], PSBuf[BUF_NUM][MAX_BUF_LEN];
	WAVEFORMATEX WaveFormatex;
	WAVEHDR      RWaveHdr[BUF_NUM], PWaveHdr[BUF_NUM];
	HWAVEIN      RWaveIn;
	HWAVEOUT     PWaveOut;

	//FILE         *in1, *out1, *out2;  // ÀÉ®× Header
	LPSTR        ErrMsg; // ¿ù»~°T®§Åã¥Ü
	int          BUF_LEN, Sample_Rate;
	int          Audio_Out_ID, Audio_In_ID; // ­µ®Ä¥d¥N¸¹
	int          RFlag, PFlag; // Buffer Cursor:0~7(BUF_NUM)
	int          RStart, PStart; // Á×§K¹ï¤wÃö³¬ªº¸Ë¸m°µ¿ý©ñ­µ
	//int          PCursor; // ºX¼Ð¨Ï¥Î

	CAudio();
	int Init_Audio(int Buf_Len, int In_ID, int Out_ID, int SR);
    // 錄音 API
	int Open_Record(DWORD Hwnd, int Flag);// , void *Record_Proc1);
	int Start_Record();
	int Stop_Record();
	int Close_Record();
	// 放音 API
	int Open_Play(DWORD Hwnd, int Flag);// , void *Play_Proc1);
	int Start_Play();
	int Stop_Play();
	int Close_Play();

	int Play_Speech(short *Speech, int SLen);    // ¼½©ñÁn­µ
	int Record_Speech(short *Speech, int *SLen); // ¨ú±o¿ýÁn­µ


	// ###################################
	// ## For DTMF/Cadence Tone Process ##
	// ###################################
	int        USB_PlayTone(int Freq1, int Freq2, int OnTime, int OffTime, BOOL Enable, HWND m_hWnd);
	int        USB_Increase_Volume();
	int        USB_Decrease_Volume();

	// ===== Extend API Function(USA Format) =====
	int  USB_PlayDialTone(HWND m_hWnd);
	int  USB_PlayRingbackTone(HWND m_hWnd);
	int  USB_PlayBusyTone(HWND m_hWnd);
	int  USB_PlayDisconnectTone(HWND m_hWnd);
	int  USB_PlayDTMFTone(int Dig, HWND m_hWnd);
	int  USB_StopPlayTone(HWND m_hWnd);

	// ===== Global Variable =====
	HWND  g_Hwnd;
	BOOL            ToneMask, DTMFMask;

	// ===== For DTMF & Dual-Tone =====
	short         DT_WAV[2][DT_DUR_LEN];
	WAVEFORMATEX  DTpcmFormatex;
	WAVEHDR       DTWaveHdr[2];
	HWAVEOUT      DThWaveOut;
	DWORD         DTFlag, DTPlay, Buf_Num;

	// ===== Variable For Audio Process =====
	UINT          Audio_Out, Audio_In;


	// #######################################
	// ## FFT/wav
	// #######################################
	// ==== FFT Definition ====
	COMPLEX x[FFT_SIZE], Cexp[FFT_SIZE / 2];
	short   bit_reverse[FFT_SIZE / 2][2];
	float   hamming[FFT_SIZE];
	int     NO_SWAP;

	int  Freq_Tab[FREQ_NUM] = { 1209, 1336, 1477, 1633,  697,  770,  852,  941,  350,  440,  480,  620 };
	int  Freq_Div[FREQ_NUM];

	int  Init_FFT(int FFT_Size);
	void REALFFT(short *IData, float *PSD, int FFT_Size, int Window_Size);
	int  Add_Wav(char *File1, char *File3, int SR11);
};

CAudio::CAudio()
{
	BUF_LEN = 240;
	Sample_Rate = 8000;
	Audio_In_ID =  WAVE_MAPPER; // -1=WAVE_MAPPER
	Audio_Out_ID = WAVE_MAPPER;
}

int CAudio::Init_Audio(int Buf_Len,int In_ID,int Out_ID,int SR)
{
	BUF_LEN = Buf_Len;
	Sample_Rate = SR;
	Audio_In_ID = In_ID;
	Audio_Out_ID = Out_ID;
	return 0;
}


// ##############################
// ## for Audio Device Control ##
// ##############################
int CAudio::Open_Record(DWORD Hwnd, int Flag)
{
	int i, j;

	// ³]©w­µ®Ä¥d°Ñ¼Æ
	WaveFormatex.wFormatTag = WAVE_FORMAT_PCM;
	WaveFormatex.nChannels = 1; // 1:Mono, 2:Stereo
	WaveFormatex.nSamplesPerSec = Sample_Rate;
	WaveFormatex.nAvgBytesPerSec = Sample_Rate * 2;
	WaveFormatex.nBlockAlign = (WORD)2;
	WaveFormatex.wBitsPerSample = (WORD)16;

	// ³]©w¿ý­µ°Ñ¼Æ
	if (i = waveInOpen(&RWaveIn, Audio_In_ID, &WaveFormatex, (DWORD)Hwnd, NULL, Flag)) return -1;
	if (i = waveInReset(RWaveIn)) {	waveInGetErrorText(i, ErrMsg, 100);	return -1;	}
	for (j = 0; j < BUF_NUM; j++)
	{
		RWaveHdr[j].dwBufferLength = BUF_LEN * 2;
		RWaveHdr[j].dwBytesRecorded = BUF_LEN * 2;
		RWaveHdr[j].dwFlags = 0L;
		RWaveHdr[j].dwLoops = 0L;
		RWaveHdr[j].lpData = (char *)&RSBuf[j][0];
		if (i = waveInPrepareHeader(RWaveIn, &RWaveHdr[j], sizeof(WAVEHDR))) return -1;
	}
	return 0;
}
int CAudio::Open_Play(DWORD Hwnd, int Flag)
{
int i,j;

    // ³]©w­µ®Ä¥d°Ñ¼Æ
 	WaveFormatex.wFormatTag=WAVE_FORMAT_PCM;
    WaveFormatex.nChannels=1; // 1:Mono, 2:Stereo
    WaveFormatex.nSamplesPerSec= Sample_Rate;
    WaveFormatex.nAvgBytesPerSec= Sample_Rate*2;
    WaveFormatex.nBlockAlign=(WORD)2;
    WaveFormatex.wBitsPerSample=(WORD)16;
   	
  	// ³]©w©ñ­µ°Ñ¼Æ
    if (i = waveOutOpen(&PWaveOut, Audio_Out_ID, &WaveFormatex, (DWORD)Hwnd, NULL, Flag)) return -1;
    if (i=waveOutReset(PWaveOut))   {   return -1; }
	for (j=0;j<BUF_NUM;j++)
	{   PWaveHdr[j].dwBufferLength=BUF_LEN*2;
        PWaveHdr[j].dwBytesRecorded=BUF_LEN*2;
        PWaveHdr[j].dwFlags=0L;
        PWaveHdr[j].dwLoops=j;
        PWaveHdr[j].lpData=(char *)&PSBuf[j][0]; 
        if (i=waveOutPrepareHeader(PWaveOut,&PWaveHdr[j],sizeof(WAVEHDR)))	{   return -1; }
	}
	return 0;
}

int CAudio::Close_Record()
{int  i;

    if (RStart == 1) {	Stop_Record(); RStart = 0; }
    // Ãö³¬¿ý­µ¸Ë¸m
    if (i=waveInStop(RWaveIn))
    {   return -1; }
    if (i=waveInReset(RWaveIn))
    {   return -1; }
    if (i=waveInClose(RWaveIn))
    {   return -1; }
	return 0;
}

int CAudio::Close_Play()
{int  i;

    if (PStart == 1) { Stop_Play(); PStart = 0; }

	// Ãö³¬©ñ­µ¸Ë¸m
    if (i=waveOutReset(PWaveOut))
    {   return -1; }
    if (i=waveOutClose(PWaveOut))
    {   return -1; }
	return 0;
}

int CAudio::Start_Record()
{
int i,Flag;
//char S1[100];
    // Start up Record
	for (Flag=0;Flag<BUF_NUM;Flag++)
	{   if (i=waveInAddBuffer(RWaveIn,&RWaveHdr[Flag],sizeof(WAVEHDR))) return -1; }  
	waveInStart(RWaveIn);
    RFlag=0; RStart=1;
	return 0;
}

int CAudio::Start_Play()
{
int i,Flag;
	
    // Start up Play
	for (Flag=0;Flag<BUF_NUM;Flag++)
	{   if (i=waveOutWrite(PWaveOut,&PWaveHdr[Flag],sizeof(WAVEHDR))) {   return -1; }  }
	PFlag=0; PStart=1; //PCursor=0;
	return 0;
}

int CAudio::Stop_Record()
{
    RStart=0;
	return 0;
}

int CAudio::Stop_Play()
{
    PStart=0;
	return 0;
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// À³¸Ó­n¦Ò¼{: ­Y SLen¤£¬O BUF_LENªº¾ã¼Æ­¿®É¡AÀ³¸Ó±N´Ý¦s¯dµ¹¤U¤@¦¸¥Î
int CAudio::Play_Speech(short *Speech, int SLen)
{
	int i, j;
	if (PStart == 0) return -1;
	memcpy(&PSBuf[PFlag][0], Speech, SLen * sizeof(short));
	i = waveOutWrite(PWaveOut, &PWaveHdr[PFlag], sizeof(WAVEHDR));
	PFlag++; PFlag %= BUF_NUM;
	return 0;
}

int CAudio::Record_Speech(short *Speech, int *SLen)
{
	int i;// , j;
	if (RStart == 0) return -1;
	
	*SLen = BUF_LEN;
	memcpy(Speech,&RSBuf[RFlag][0], BUF_LEN * 2);
	i = waveInAddBuffer(RWaveIn, &RWaveHdr[RFlag], sizeof(WAVEHDR));
	RFlag++; RFlag %= BUF_NUM;
	return 0;
}

// ###################################
// ## For DTMF/Cadence Tone Process ##
// ###################################
int  CAudio::USB_PlayDialTone(HWND m_hWnd)
{
     if (ToneMask==TRUE) USB_StopPlayTone(m_hWnd); 
	 Buf_Num=2;
	 USB_PlayTone(350,440,4000,0,TRUE,m_hWnd); 
	 return 0;
}
int  CAudio::USB_PlayRingbackTone(HWND m_hWnd)
{
     if (ToneMask==TRUE) USB_StopPlayTone(m_hWnd); 
	 Buf_Num=2;
	 USB_PlayTone(440,480,1000,3000,TRUE,m_hWnd); 
	 return 0;
}
int  CAudio::USB_PlayBusyTone(HWND m_hWnd)
{
     if (ToneMask==TRUE) USB_StopPlayTone(m_hWnd); 
	 Buf_Num=2;
	 USB_PlayTone(480,620,500,500,TRUE,m_hWnd); 
	 return 0;
}
int  CAudio::USB_PlayDisconnectTone(HWND m_hWnd)
{
     if (ToneMask==TRUE) USB_StopPlayTone(m_hWnd); 
	 Buf_Num=2;
	 USB_PlayTone(480,620,250,250,TRUE,m_hWnd); 
	 return 0;
}
int  CAudio::USB_PlayDTMFTone(int Dig,HWND m_hWnd)
{
WORD DTMF_Tab3[16][2]={ 
	{941, 1336}, {697, 1209}, {697, 1336}, {697, 1477},   // 0,1,2,3
	{770, 1209}, {770, 1336}, {770, 1477}, {852, 1209},   // 4,5,6,7
	{852, 1336}, {852, 1477}, {697, 1633}, {770, 1633},   // 8,9,A,B
	{852, 1633}, {941, 1633}, {941, 1209}, {941, 1477} }; // C,D,*,#

     if (ToneMask==TRUE) USB_StopPlayTone(m_hWnd); 
	 Buf_Num=1;
	 USB_PlayTone(DTMF_Tab3[Dig][0],DTMF_Tab3[Dig][1],300,100,TRUE,m_hWnd); 
	 DTPlay=0; Buf_Num=2; // ==== Play 1 Cycle/1 Buffer Only ====
	 return 0;
}


int  CAudio::USB_StopPlayTone(HWND m_hWnd)
{
     USB_PlayTone(1,1,1,1,FALSE,m_hWnd);
     return 0;
}

int CAudio::USB_PlayTone(int Freq1, int Freq2,int OnTime,int OffTime,BOOL Enable,HWND m_hWnd)
{
int     i,j;
double  Period1,Period2;
//char S1[100];

       g_Hwnd=m_hWnd;
       Audio_Out=0;
	   if ((OnTime+OffTime)*(Sample_Rate/1000)>DT_DUR_LEN) 
	   {  //sprintf(ErrMsg,"OnTime+OffTime must Smaller than 4000 ms"); 
	      return -1; }
	   if (Enable==TRUE)
	   {  if (ToneMask==TRUE) return 0; else ToneMask=TRUE;
		  // ³]©w½ÆÀW­µ
		  DTPlay=1;
          // Generate Dial Tone Waveform
	      Period1=(double)Sample_Rate/(double)Freq1; 
	      Period2=(double)Sample_Rate/(double)Freq2;
		  // x(t)=g*cos(2*pi*f*t)
		  // t=n/SR;
		  // x[n]=g*cos(2*pi*f*n/SR)
	      for (j=0;j<OnTime*(Sample_Rate/1000);j++)
		  {   DT_WAV[0][j] =DT_WAV[1][j]=(short)((double)DT_AMPLITUDE*
		           (cos((double)j*6.28/Period1)*0.5+cos((double)j*6.28/Period2)));    }
	      for (j=OnTime*(Sample_Rate/1000);j<(OnTime+OffTime)*(Sample_Rate/1000);j++)
		  {   DT_WAV[0][j]=DT_WAV[1][j]=0;  }

	      // ³]©w¿ý©ñ­µ°Ñ¼Æ
	      DTpcmFormatex.wFormatTag=WAVE_FORMAT_PCM;
          DTpcmFormatex.nChannels=1;
          DTpcmFormatex.nSamplesPerSec= Sample_Rate;
          DTpcmFormatex.nAvgBytesPerSec=Sample_Rate*2L;
          DTpcmFormatex.nBlockAlign=2;
          DTpcmFormatex.wBitsPerSample=16;
          if (i=waveOutOpen(&DThWaveOut,Audio_Out,&DTpcmFormatex,(DWORD)m_hWnd,(DWORD)NULL,CALLBACK_WINDOW))
		  {  
		  
		     return -1; }
 
	      for (j=0;j<(int)Buf_Num;j++)
		  {   DTWaveHdr[j].dwBufferLength=(OnTime+OffTime)*(Sample_Rate/1000)*2L; // 2byte
              DTWaveHdr[j].dwBytesRecorded=(OnTime+OffTime)*(Sample_Rate/1000)*2L;
              DTWaveHdr[j].dwFlags=0L;
              DTWaveHdr[j].dwLoops=0L;
              DTWaveHdr[j].lpData=(char *)&DT_WAV[j][0]; 
              if (i=waveOutPrepareHeader(DThWaveOut,&DTWaveHdr[j],sizeof(WAVEHDR)))
			  {  
			     return -1; }
              if (i=waveOutWrite(DThWaveOut,&DTWaveHdr[j],sizeof(WAVEHDR))) 
			  { 
			     return -1; }  
		  }
	      DTFlag=0; 
	   }
	   else
	   {  if (ToneMask==FALSE) return 0; else ToneMask=FALSE;
		  DTPlay=0;
          waveOutReset(DThWaveOut); 
	      waveOutClose(DThWaveOut);
	   }
	
	   return 0;
}

int  CAudio::USB_Increase_Volume()
{
DWORD Volume;
MMRESULT i;
     i=waveOutGetVolume(DThWaveOut,&Volume);
	 // ==== Increase Right Channel ====
	 if (Volume<0xF0000000) Volume+=0x10000000; 
	 else                   Volume|=0xFFFF0000;
	 // ==== Increase Left Channel ====
	 if ((Volume&0x0000FFFF)<0x0000F000) Volume+=0x00001000; 
	 else                                Volume|=0x0000FFFF;
	 // ==== Set Volume ====
	 i=waveOutSetVolume(DThWaveOut,Volume);
     return 0;
}

int  CAudio::USB_Decrease_Volume()
{
DWORD Volume;
MMRESULT i;
     i=waveOutGetVolume(DThWaveOut,&Volume);
	 // ==== Increase Right Channel ====
	 if (Volume>=0x10000000) Volume-=0x10000000; 
	 else                    Volume&=0x0000FFFF;
	 // ==== Increase Left Channel ====
	 if ((Volume&0x0000FFFF)>=0x00001000) Volume-=0x00001000; 
	 else                                 Volume&=0xFFFF0000;
	 // ==== Set Volume ====
	 i=waveOutSetVolume(DThWaveOut,Volume);
	 return 0;
}

int CAudio::Init_FFT(int FFT_Size)
{
int  i,j,k,l;

//hsfout=fopen("c:/as.fft","wb");
     if (FFT_Size>FFT_SIZE) return -1;

     for(i=0; i<FFT_Size/2; i++)
	 {  Cexp[i].real  = (float)cos(i*6.28318/FFT_Size);
	    Cexp[i].image = (float)sin(i*6.28318/FFT_Size); }
         
     k = 0;  j = 0;
     for ( i=1; i<FFT_Size/2; i++)
     {   l=FFT_Size/2;
         while ( j + l >= FFT_Size/2)   l = l/2;
	     j=(j%l)+l;
		 if (j>i) { bit_reverse[k][0]=i; bit_reverse[k][1]=j; k++; }	 }
	 NO_SWAP=k;
     for (i=0; i<FFT_Size; i++) hamming[i]=(float)0.54-(float)0.46*(float)cos(6.28318*i/(FFT_Size-1));
     for (i=0;i<FREQ_NUM;i++) Freq_Div[i]=(int)((float)Freq_Tab[i]*0.035+0.5);

	 return 0;
}


void CAudio::REALFFT(short *IData,float *PSD,int FFT_Size,int Window_Size)//COMPLEX *x)//,COMPLEX *Cexp,short bit_reverse[][2],int NO_SWAP)
{
float      sinN, cosN, Realsum, Imagsum, Realdif, Imagdif;
COMPLEX    T,c0,c1;
int        i,j,I,J,K,L,MM,N,TWOL;

 //==== initial ====//
 for (i=0;i<(Window_Size>>1);i++) { x[i].real=(float)IData[i*2]*hamming[i*2]; x[i].image=(float)IData[i*2+1]*hamming[i*2+1]; }
 for (i=(Window_Size>>1);i<(FFT_Size>>1);i++) x[i].real =x[i].image=0;

 // ==== Bit Reverse ====
 for (I=0;I<NO_SWAP;I++) { MM=bit_reverse[I][0]; N=bit_reverse[I][1]; T=x[MM]; x[MM]=x[N]; x[N]=T; }

 // ====transform argument ====
 L = 1; TWOL = 2; N = FFT_Size>>1;
 while ( L<(FFT_Size>>1))
 {   for ( J=0; J<L; J++)
     {   for ( K=J; K<(FFT_Size>>1); K+=TWOL)
         {   c0 = x[K+L];  c1 = Cexp[J*N]; 
             T.real = c0.real * c1.real - c0.image * c1.image;
             T.image = c0.real * c1.image + c0.image * c1.real;
             c0 = x[K];
			 x[K+L].real  = c0.real - T.real;
             x[K+L].image = c0.image - T.image;
             x[K].real    = c0.real + T.real;
             x[K].image   = c0.image + T.image;    }   }
     L = TWOL; TWOL = L*2; N = N/2;  } 

  // Spectrum Envelope Multiply with 2.0
  for (i=1; i<(FFT_Size>>2); i++)
  {   cosN = Cexp[i].real;
      sinN = Cexp[i].image;
      j = (FFT_Size>>1) - i;
      Realsum = /*(float)0.5*/( x[i].real + x[j].real );
      Imagsum = /*(float)0.5*/( x[i].image + x[j].image );
      Realdif = /*(float)0.5*/( x[i].real - x[j].real );
      Imagdif = /*(float)0.5*/( x[i].image - x[j].image );
      x[i].real = Realsum + cosN*Imagsum + sinN*Realdif;
      x[i].image = Imagdif + sinN*Imagsum - cosN*Realdif;
      x[j].real = Realsum - cosN*Imagsum - sinN*Realdif;
      x[j].image = -Imagdif + sinN*Imagsum - cosN*Realdif;
  }
  x[0].real += x[0].image;
  x[0].real*=2; x[0].image*=2;

  for (i=0;i<(FFT_Size>>1);i++) PSD[i]=x[i].real*x[i].real+x[i].image*x[i].image;
}
// %%%%%%%%%%%%%%%%%%%%%%%%%%
// #### ¼W¥[ WAV Header ####
int CAudio::Add_Wav(char *File1,char *File3, int SR11)
{
FILE *in1,*out1;
errno_t tt;
int  i,Len;
unsigned char UCBuf[100];
short         UCBuf1[256];

     if ((tt=fopen_s(&in1,File1,"rb"))!=NULL) return -1;
	 fseek(in1,0,SEEK_END); Len=ftell(in1);
	 
	 // 寫入 *.wav Header
     UCBuf[0]='R'; UCBuf[1]='I'; UCBuf[2]='F'; UCBuf[3]='F';
	 i=Len-8; memcpy(&UCBuf[4],&i,4);
     UCBuf[8]='W'; UCBuf[9]='A'; UCBuf[10]='V'; UCBuf[11]='E';

	 UCBuf[12]='f'; UCBuf[13]='m'; UCBuf[14]='t'; UCBuf[15]=' ';
	 UCBuf[16]=0x10; UCBuf[17]=0x00; UCBuf[18]=0x00; UCBuf[19]=0x00;
	 UCBuf[20]=0x01; UCBuf[21]=0x00; UCBuf[22]=0x01; UCBuf[23]=0x00;
     i=SR11; memcpy(&UCBuf[24],&i,4);
	 i=SR11*2; memcpy(&UCBuf[28],&i,4);
	 UCBuf[32]=0x02; UCBuf[33]=0x00; UCBuf[34]=0x10; UCBuf[35]=0x00;
	 UCBuf[36]='d';  UCBuf[37]='a';  UCBuf[38]='t';  UCBuf[39]='a';
	 i=Len; memcpy(&UCBuf[40],&i,4);
     if ((tt=fopen_s(&out1,File3,"wb"))!=NULL) return -1;

	 fwrite(UCBuf,44,1,out1);
	 fseek(in1,0,0);
	 while (!feof(in1) )
	 {  fread(UCBuf1,256,2,in1);
	    fwrite(UCBuf1,256,2,out1);  }
	 fclose(in1);
	 fclose(out1);
     return 0;
}




