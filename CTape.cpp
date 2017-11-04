#include "ctape.h"

CTape::CTape()
{
 TapeLength=0; //signal length on tape
 TapeData=NULL; //tape data
 Speed=1; //playback speed
 HeadPosition=0; //current position of the "head" of the recorder
 UseLeftChannel=true; //permission to use the left channel
 UseRightChannel=true; //permission to use the right channel
 Channels=0;
}

CTape::~CTape()
{
 Eject();
}

bool CTape::LoadTape(char *FileName)
{
 Eject();

 HMMIO hwav;
 MMCKINFO parent,child;
 WAVEFORMATEX wfmtx;
 parent.ckid=(FOURCC)0;
 parent.cksize=0;
 parent.fccType=(FOURCC)0;
 parent.dwDataOffset=0;
 parent.dwFlags=0;
 child=parent;
 if ((hwav=mmioOpen(FileName,NULL,MMIO_READ|MMIO_ALLOCBUF))==NULL) return(false);
 parent.fccType=mmioFOURCC('W','A','V','E');
 if (mmioDescend(hwav,&parent,NULL,MMIO_FINDRIFF))
 {
  mmioClose(hwav,0);
  return(false);
 }
 child.ckid=mmioFOURCC('f','m','t',' ');
 if (mmioDescend(hwav,&child,&parent,0))
 {
  mmioClose(hwav,0);
  return(false);
 }
 if (mmioRead(hwav,(char *)&wfmtx,sizeof(wfmtx))!=sizeof(wfmtx))
 {
  mmioClose(hwav,0);
  return(false);
 }
 if (wfmtx.wFormatTag!=WAVE_FORMAT_PCM)
 {
  mmioClose(hwav,0);
  return(false);
 } 
 if (mmioAscend(hwav,&child,0))
 {
  mmioClose(hwav,0);
  return(false);
 }
 child.ckid=mmioFOURCC('d','a','t','a');
 if (mmioDescend(hwav,&child,&parent,MMIO_FINDCHUNK))
 {
  mmioClose(hwav,0);
  return(false);
 }

 //check the file format
 Channels=wfmtx.nChannels; //number of channels
 if (wfmtx.nSamplesPerSec!=44100) return(false); //check sample rate 44100
 if (wfmtx.nBlockAlign!=wfmtx.nChannels) return(false); //only 8 bit

 //load sound data
 TapeData=new unsigned char[child.cksize];
 TapeLength=child.cksize;
 mmioRead(hwav,(char *)TapeData,child.cksize);
 TapeLength/=Channels; //make the length of one channel
 mmioClose(hwav,0);
 return(true);
}

bool CTape::SetSpeed(long double speed)
{
 if (Speed<=0) return(false);
 Speed=speed;
 return(true);
}

bool CTape::ReadTape(unsigned char &volume)
{
 if (TapeData==NULL) return(false);
 unsigned long pos=(unsigned long)HeadPosition;
 if (pos>=TapeLength) return(false); 
 unsigned long left=pos;
 unsigned long right=pos+1;
 if (right>=TapeLength) right=pos;

 //do a linear interpolation of the signal between neighboring points
 if (Channels==1) //for mono
 {
  long double k=((long double)(TapeData[right])-(long double)(TapeData[left]));
  double v=(double)(TapeData[left])+(HeadPosition-(long double)(pos))*k;
  volume=(unsigned char)v;
 }
 if (Channels==2) //for stereo
 {
  double ch_level=0;
  int ch_summ=0;
  if (UseLeftChannel==true)
  {
   long double k=((long double)(TapeData[right*2])-(long double)(TapeData[left*2]));
   ch_level+=(double)(TapeData[left*2])+(HeadPosition-(long double)(pos))*k;
   ch_summ++;  
  } 
  if (UseRightChannel==true)
  {
   long double k=((long double)(TapeData[right*2+1])-(long double)(TapeData[left*2+1]));
   ch_level+=(double)(TapeData[left*2+1])+(HeadPosition-(long double)(pos))*k;
   ch_summ++;
  } 
  if (ch_summ==0) ch_summ=1;
  ch_level/=ch_summ;
  volume=(unsigned char)ch_level;
 }
 return(true);
}

bool CTape::MoveHead(void)
{
 HeadPosition+=Speed;
 unsigned long pos=(unsigned long)HeadPosition;
 if (pos>=TapeLength) return(false); 
 return(true);
}

bool CTape::ResetHead(void)
{
 HeadPosition=0;
 return(true);
}

unsigned long CTape::GetTapeLength(void)
{
 return(TapeLength);
}

void CTape::Eject(void)
{
 if (TapeData!=NULL) delete(TapeData);
 HeadPosition=0;
 TapeData=NULL;
 TapeLength=0;
}

bool CTape::ApplyFilter(void)
{
 /*
 unsigned long n;
 int Filter=TapeData[0];
 for(n=0;n<TapeLength;n++)
 {
  Filter=Filter+TapeData[n];
  Filter/=2;
  TapeData[n]=Filter;
 }
 */
 return(true);
}

long double CTape::GetSpeed(void)
{
 return(Speed);
}

bool CTape::EndOfTape(void)
{
 unsigned char volume;
 if (ReadTape(volume)==false) return(true); //tape finished
 return(false); //tape not finished
}

unsigned char CTape::GetLevel(int Step)
{
 unsigned long h_p=(unsigned long)HeadPosition;
 int n;
 unsigned long level=0;
 int max=0;
 int min=255;
 if (h_p+Step>=TapeLength) h_p=TapeLength-Step;
 if (TapeLength<Step)
 {
  h_p=0;
  Step=TapeLength;
 }
 for(n=0;n<Step;n++,h_p++)
 {
  unsigned char volume;
  if (Channels==1) volume=TapeData[h_p]; //for mono
  if (Channels==2)
  {
   int ch_level=0;
   int ch_summ=0;
   if (UseLeftChannel==true)
   {
	ch_level+=TapeData[h_p*2];
    ch_summ++;  
   }
   if (UseRightChannel==true)
   {
	ch_level+=TapeData[h_p*2+1];
    ch_summ++;  
   }
   if (ch_summ==0) ch_summ=1;
   ch_level/=ch_summ;
   volume=ch_level;
  }
  level+=volume;
  if (volume>max) max=volume;
  if (volume<min) min=volume;
 }
 if (n!=0) level/=n;
 int center=(max+min)/2;
 level=(level+center)/2;
 return((unsigned char)level);
}

bool CTape::SetUseChannel(bool use_left_channel,bool use_right_channel)
{
 UseLeftChannel=use_left_channel;
 UseRightChannel=use_right_channel;
 return(true);
}

