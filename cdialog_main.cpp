#include "cdialog_main.h"

BEGIN_MESSAGE_MAP(CDialog_Main,CDialog)
 ON_WM_DESTROY()
 ON_COMMAND(IDC_BUTTON_CREATE_TAP,OnCommand_Button_CreateTap)
END_MESSAGE_MAP()

CDialog_Main::CDialog_Main(LPCTSTR lpszTemplateName, CWnd* pParentWnd):CDialog(lpszTemplateName,pParentWnd)
{
}

CDialog_Main::~CDialog_Main()
{
}

afx_msg void CDialog_Main::OnOK(void)
{
}

afx_msg BOOL CDialog_Main::OnInitDialog(void)
{
 //get the controls
 cEdit_ZeroMin=(CEdit*)GetDlgItem(IDC_EDIT_ZERO_MIN);
 cEdit_ZeroMax=(CEdit*)GetDlgItem(IDC_EDIT_ZERO_MAX);

 cEdit_OneMin=(CEdit*)GetDlgItem(IDC_EDIT_ONE_MIN);
 cEdit_OneMax=(CEdit*)GetDlgItem(IDC_EDIT_ONE_MAX);

 cEdit_LeadMin=(CEdit*)GetDlgItem(IDC_EDIT_LEAD_MIN);
 cEdit_LeadMax=(CEdit*)GetDlgItem(IDC_EDIT_LEAD_MAX);

 cEdit_Level=(CEdit*)GetDlgItem(IDC_EDIT_LEVEL);
 cEdit_LevelPoints=(CEdit*)GetDlgItem(IDC_EDIT_LEVEL_POINTS);

 cButton_UseLeftChannel=(CButton*)GetDlgItem(IDC_CHECK_USE_LEFT_CHANNEL);
 cButton_UseRightChannel=(CButton*)GetDlgItem(IDC_CHECK_USE_RIGHT_CHANNEL);

 cButton_LevelAuto=(CButton*)GetDlgItem(IDC_RADIO_LEVEL_AUTO);
 cButton_LevelManual=(CButton*)GetDlgItem(IDC_RADIO_LEVEL_MANUAL);

 //customize the controls
 cEdit_ZeroMin->SetLimitText(5);
 cEdit_ZeroMax->SetLimitText(5);
 cEdit_OneMin->SetLimitText(5);
 cEdit_OneMax->SetLimitText(5);
 cEdit_LeadMin->SetLimitText(5);
 cEdit_LeadMax->SetLimitText(5);

 cEdit_ZeroMin->SetWindowText("11");
 cEdit_ZeroMax->SetWindowText("75");
 cEdit_OneMin->SetWindowText("76");
 cEdit_OneMax->SetWindowText("110");
 cEdit_LeadMin->SetWindowText("111");
 cEdit_LeadMax->SetWindowText("150");

 cEdit_Level->SetWindowText("127");
 cEdit_Level->SetLimitText(5);
 cEdit_LevelPoints->SetWindowText("200");
 cEdit_LevelPoints->SetLimitText(100);

 cButton_UseLeftChannel->SetCheck(1);
 cButton_UseRightChannel->SetCheck(1);

 cButton_LevelAuto->SetCheck(1);
 cButton_LevelManual->SetCheck(0);


 return(CDialog::OnInitDialog());
}

afx_msg void CDialog_Main::OnDestroy(void)
{
 CDialog::OnDestroy();
}

afx_msg void CDialog_Main::OnCommand_Button_CreateTap(void)
{
 //read settings
 float ZeroMin;
 float ZeroMax;
 float OneMin;
 float OneMax;
 float LeadMin;
 float LeadMax;

 char string[255];
 cEdit_ZeroMin->GetWindowText(string,255);
 ZeroMin=(float)atof(string);
 cEdit_ZeroMax->GetWindowText(string,255);
 ZeroMax=(float)atof(string);

 cEdit_OneMin->GetWindowText(string,255);
 OneMin=(float)atof(string);
 cEdit_OneMax->GetWindowText(string,255);
 OneMax=(float)atof(string);
 
 cEdit_LeadMin->GetWindowText(string,255);
 LeadMin=(float)atof(string);
 cEdit_LeadMax->GetWindowText(string,255);
 LeadMax=(float)atof(string);

 bool use_left_channel=false;
 bool use_right_channel=false;
 if (cButton_UseLeftChannel->GetCheck()) use_left_channel=true;
 if (cButton_UseRightChannel->GetCheck()) use_right_channel=true;

 if (ZeroMin>ZeroMax)
 {
  MessageBox("The minimum boundry for Zero is greater than the maximum!","Error",MB_OK);
  return;
 }
 if (OneMin>OneMax)
 {
  MessageBox("The minimum boundry for One is greater than the maximum!","Error",MB_OK);
  return;
 }
 if (LeadMin>LeadMax)
 {
  MessageBox("The minimum boundry for Start tone is greater than the maximum!","Error",MB_OK);
  return;
 }
 cEdit_Level->GetWindowText(string,255);
 Level=atoi(string);
 cEdit_LevelPoints->GetWindowText(string,255);
 LevelPoints=atoi(string);
 if (cButton_LevelManual->GetCheck())
 {
  LevelAuto=false;
  if (Level<0 || Level>255)
  {
   MessageBox("The level must be between 0 and 255!","Error",MB_OK);
   return;
  }
 }
 if (cButton_LevelAuto->GetCheck())
 {
  LevelAuto=true;
  if (LevelPoints<=0)
  {
   MessageBox("The number of points must be greater than zero!","Error",MB_OK);
   return;
  }
 } 

 CFileDialog cFileDialog(TRUE,"","",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"*.wav|*.wav||",this);
 char Title[256];
 strcpy(Title,"Choose a WAV file for conversion");
 cFileDialog.m_ofn.lpstrTitle=Title;
 if (cFileDialog.DoModal()!=IDOK) return;
 CTape cTape;
 cTape.SetUseChannel(use_left_channel,use_right_channel);
 char *FileName=new char[strlen(cFileDialog.GetFileName())+1];
 strcpy(FileName,cFileDialog.GetFileName());
 if (cTape.LoadTape(FileName)==false)
 {
  delete(FileName);
  MessageBox("File cannot be opened. Check if the file is in use by another program or if the file format is correct (uncompressed WAV, 44100Hz, 8-bit).","Error",MB_OK);
 }
 char *FileTitle=new char[strlen(cFileDialog.GetFileTitle())+1];
 strcpy(FileTitle,cFileDialog.GetFileTitle());
 cTape.ResetHead();
 cTape.SetSpeed(0.2);//5 times slower

 //process the file
 if (CreateTapFile(FileTitle,&cTape,ZeroMin,ZeroMax,OneMin,OneMax,LeadMin,LeadMax)==false)
 {
  MessageBox("Cannot create text file!","Error",MB_OK);
  delete(FileName);
  delete(FileTitle);
  return;
 }
 delete(FileName);
 delete(FileTitle);
 MessageBox("Files were created successfully!","Ready",MB_OK);
}

int CDialog_Main::GetSignalLength(CTape *cTape,unsigned char &CurrentSignal)
{ 
 int max=(int)(100.0f/cTape->GetSpeed()); //limit the cycles for changing the signal value
 unsigned char level=Level;
 if (LevelAuto==true) level=cTape->GetLevel(LevelPoints);
 for(int n=0;n<max;n++)
 {
  unsigned char volume;
  if (cTape->ReadTape(volume)==false) break; //data end
  //compare with given level
  unsigned char signal=CurrentSignal;  
  if (volume>level) signal=1;
               else signal=0;
  cTape->MoveHead();
  if (signal!=CurrentSignal)
  {
   CurrentSignal=signal;
   return(n);
  }
 }
 return(-1);
}

bool CDialog_Main::CreateTapFile(char *FileTitle,CTape *cTape,float ZeroMin,float ZeroMax,float OneMin,float OneMax,float LeadMin,float LeadMax)
{
 char *FileName_Tap=new char[strlen(FileTitle)+255];
 char *FileName_Txt=new char[strlen(FileTitle)+255];
 sprintf(FileName_Tap,"%s.tap",FileTitle);
 sprintf(FileName_Txt,"%s.txt",FileTitle);
 FILE *file_tap=fopen(FileName_Tap,"wb"); 
 FILE *file_txt=fopen(FileName_Txt,"wb"); 
 delete(FileName_Tap);
 delete(FileName_Txt);
 if (file_tap==NULL || file_txt==NULL) return(false);

 int Mode=0; //not accepting anything
 int LeadCounter=0; //start tone is not accepted
 int BlockLength=0;
 int BytePtr=0;
 unsigned char Byte=0;
 unsigned char *BlockData=NULL;
 int BlockMax=0;
 unsigned char Signal=0;
 int Column=0; //printing column
 float scale=0;


 while(cTape->EndOfTape()==false)
 {
  //get the length of the tape
  int SignalLength=GetSignalLength(cTape,Signal);
  unsigned char type=0; //signal type
  //analyze the signal
  if (Signal==0)
  {
   //identify the signal
   if (SignalLength>=LeadMin && SignalLength<=LeadMax) type=1; //start tone
   if (SignalLength>=OneMin && SignalLength<=OneMax) type=2; //one
   if (SignalLength>=ZeroMin && SignalLength<=ZeroMax) type=3; //zero or sync

   //process the signal
   if (Mode==0 && type==1) //start tone
   {
    Mode=1;   
    continue;
   }
   if (Mode==1 && type==1) //start tone
   {
    Mode=1;
    LeadCounter++;
    continue;
   }
   if (Mode==1 && type==3) //clock signal
   {
    if (LeadCounter>100)
    {
     Mode=2; 
     fprintf(file_txt,"\r\n-----Start of file (sync signal)-----\r\n");
	 Column=0;
     BlockLength=0;
     BytePtr=0;
     Byte=0;
     continue;
    }
   }
   if (Mode==2 && type==3) //zero
   {
    Byte=Byte<<1;
    Byte|=0;
    BytePtr++;
    if (BytePtr==8)
    {
     if (BlockLength>=BlockMax)
	 {
      BlockMax+=16384;
      unsigned char *new_data=new unsigned char[BlockMax];
	  if (BlockData!=NULL)
	  {
	   memcpy(new_data,BlockData,BlockLength);
	   delete(BlockData);
	  }
	  BlockData=new_data;
	 }
     BlockData[BlockLength]=Byte;
	 if (Column==30)
	 {
	  fprintf(file_txt,"\r\n");
	  Column=0;
	 }
     fprintf(file_txt,"%02x ",Byte);
	 Column++;
     BytePtr=0;
     Byte=0;
     BlockLength++;
    }
    continue;
   }
   if (Mode==2 && (type==2 || type==1)) //one
   {
    Byte=Byte<<1;
    Byte|=1;
    BytePtr++;
    if (BytePtr==8)
    {
     if (BlockLength>=BlockMax)
	 {
      BlockMax+=16384;
      unsigned char *new_data=new unsigned char[BlockMax];
	  if (BlockData!=NULL)
	  {
	   memcpy(new_data,BlockData,BlockLength);
	   delete(BlockData);
	  }
	  BlockData=new_data;
	 }
     BlockData[BlockLength]=Byte;
	 if (Column==30)
	 {
	  fprintf(file_txt,"\r\n");
	  Column=0;
	 }
     fprintf(file_txt,"%02x ",Byte);
	 Column++;
     BytePtr=0;
     Byte=0;
     BlockLength++;
    }
    continue;
   }
  }
  else
  {
   if (Signal==0) SignalLength=-1;
  }
  if (SignalLength<0) //signal ended
  {
   if (Mode==2) //take the file, write it
   {
    if (BlockLength>0)
    {
     fprintf(file_txt,"\r\nEnd of file. Read %i bytes.\r\n",BlockLength);
     unsigned char b;
     b=(BlockLength)%256;
     fwrite(&b,1,1,file_tap);
     b=(BlockLength)/256;
     fwrite(&b,1,1,file_tap);
     unsigned char h=0;
     for(int m=0;m<BlockLength;m++)
     {     
      b=BlockData[m];
      fwrite(&b,1,1,file_tap);
      if (m!=BlockLength-1) h^=b;	 
     }
     if (BlockData[BlockLength-1]==h) fprintf(file_txt,"\r\nThere were no errors.\r\n");
	                             else fprintf(file_txt,"\r\nError reading!\r\n");
    }
   } 
   Mode=0; //not accepting anything
   LeadCounter=0; //start tone is not accepted
   if (BlockData!=NULL) delete(BlockData);
   BlockData=NULL;
   BlockMax=0;
   BlockLength=0;
   BytePtr=0;
   Byte=0;
   continue;
  }
 }
 fclose(file_txt);
 fclose(file_tap);
 return(true);
}
