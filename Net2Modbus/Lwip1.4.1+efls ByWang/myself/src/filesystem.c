
/*

  filesystem interface 
  by СARM�˲�
  QQ��925295580

*/

#include "efs.h"
#include "ls.h"
#include "sd.h"
#include "ui.h"
#include "filesystem.h"
#include "httpd.h"

#include <stdio.h>
#include <string.h>

#define DATA_LEN_MAX       512

EmbeddedFileSystem  myefs1, myefs2;
DirList             mylist1, mylist2;
EmbeddedFile        myfile_SD, myfile_CR;
 

euint8 FindMP3(euint32 FileSizeu,euint8* FileNameu)
{

 if(FileSizeu>0) //���ļ�
			 {
			 //�Ƿ�ΪMP3��
					 if((FileNameu[8]==0x4d)&&(FileNameu[8]==0x50)&&(FileNameu[8]==0x33))
					 {
					 		 
						  return 1;
					 
					 }
					 else
					 return 0;
			  }

}

 static euint32 MP3count=0; //MP3��������
static euint32 filecount=0;//�ļ�������

void FindMP3fileALL(euint8* filename)
{

	euint8 FileNameu[12];	
	euint32 FileSizeu;
	euint8 Attributeu;
	DirList   localist;
    EmbeddedFileSystem 	fts;
		 //root directory

		 if (efs_init(&fts, 0) != 0)
      {
        
		return;
      
	  }

      /* If Could not open the selected directory */
      if (ls_openDir(&localist, &fts.myFs, filename) != 0)
      {
        return;
      }
	  /* Print a list of all files and their filesize */
	   while(ls_getNext(&localist)==0)
	   {
			 //�ļ�/�ļ���
			 FileSizeu=0; //
			 Attributeu=0; //
			 memcpy(FileNameu,(void*)0,12);  
			 memcpy(FileNameu,&localist.currentEntry.FileName[0],12);
			 FileSizeu=localist.currentEntry.FileSize;
			 Attributeu=localist.currentEntry.Attribute ;

			 if((FileSizeu==0)&&(FileNameu!=0))//���ļ���-��ʾ����Ŀ¼
			 {

			    FindMP3fileALL((euint8*)&FileNameu); //����Ŀ¼
			 	filecount++;
//		
//
//
//
//
			 }
			 else
			 if(FileSizeu>0) //���ļ�
			 {
			 
				/* if(FindMP3(FileSizeu,&FileNameu[0])>0)//
					 {		 
						  MP3count++;
					 }	*/

			 
			 //�Ƿ�ΪMP3��
					 if((FileNameu[8]==0x4d)&&(FileNameu[9]==0x50)&&(FileNameu[10]==0x33))
					 {
					 		 
						   MP3count++;	 //MP3�ļ�
					 
					 }
					 
			  
			 }


	   }

	  return;

}


extern void Http_File_System_Init(void);
void FileSystemInit(void)
{
	// euint8 MP3count=0; //MP3����
	// euint8 filecount=0;//��Ŀ¼�ļ�������


	euint8 FileNameu[12];
	euint32 FileSizeu;
	euint8 Attributeu;
  /* If Could not open filesystem */
      if (efs_init(&myefs1, 0) != 0)
      {
        
		return;
      
	  }
			 //root directory
      /* If Could not open the selected directory */
      if (ls_openDir(&mylist1, &(myefs1.myFs), "/") != 0)
      {
        return;
      }
	 	 
	 // FindMP3fileALL("/"); //�Ӹ�Ŀ¼��ʼѰ�ҳ����е�MP3�ļ����ļ��е�������
	   /* If Could not open filesystem */
      if (efs_init(&myefs2, 0) != 0)
      {
        return;
      }
     // If Could not open the selected directory 
      if (ls_openDir(&mylist2, &(myefs2.myFs), "/") != 0)
      {
        return;
      }	

	   Http_File_System_Init();//��ʼ��HTTP�����ļ�ϵͳ




 }
//
 
 euint8  fsbufer[1024];
euint8 FileSysteRead(void)
{
	  
	 euint32 datalen;
	 
  /* If Could not open the file which will be return 0  */
  if (file_fopen(&myfile_SD, &myefs1.myFs, "\.mp3", 'r') != 0)
  {
    return 0;
  }
 datalen= file_read(&myfile_SD, DATA_LEN_MAX, fsbufer);

FileSystemClose(&myfile_SD,&myefs1.myFs); 

return 1;

}



euint8 FileSysteWrite(void)
{
	  
		euint32 datalen;
   /* If Could not open the file which will be return 0  */
  if (file_fopen(&myfile_CR, &myefs2.myFs, "test23.txt", 'w') != 0)
  {
    return 0;
  }
 /* write the received data to the file */
  datalen = file_write(&myfile_CR,sizeof("this is a filesystem test!"),"this is a filesystem test!");
 
 FileSystemClose(&myfile_CR,&myefs2.myFs);

 return 1;

}

void FileSystemClose(EmbeddedFile * file,FileSystem* efs)
 {
 
 file_fclose(file);
 fs_umount(efs);

 }

void FileSystemDelete(FileSystem *fs,euint8* filename)
{

   if(  rmfile(fs,filename)!=0)
   {
   	  // not found
   
   }

}

 void FileSystemThread()
 {
 
 	 /// FileSysteRead() ;
	  FileSysteWrite();
 }
 
char  FS_Write(int f,char *p, unsigned int size)
{
   	 #ifdef USED_FILESYSTEM
		euint32 datalen;

  if(f==1)
  {

  // д��TCP dump
  if (file_fopen(&myfile_CR, &myefs2.myFs, "ReportFs.txt", 'a') != 0)
  {										
    return 0;
  }

  }
  else
  if(f==2)
  {
	     //д���¶�����
	  if (file_fopen(&myfile_CR, &myefs2.myFs, "RepoTemp.txt", 'a') != 0)
	  {										
	    return 0;
	  }




  }
 // write the received data to the file
  datalen = file_write(&myfile_CR,size,p);
 
  FileSystemClose(&myfile_CR,&myefs2.myFs);

  return 1;

   #endif
}  

#ifdef TEMP_WRITE_HTML	 
#include <stdio.h>
#include "file.h"
#define STRINGLENTH 6
unsigned int FindFileString(char *str);
char filebuf[512];
/*
>xxx.xx<a name="cgqa" id="cgqa"></a></div>
>yyy.yy<a name="cgqb" id="cgqb"></a></div>
*/
char strA[]="<a name=\"cgqa\"";//ƥ���ַ�A ǰ���6���ַ�������xxx.xxֻҪ���¶������滻�ͺ�
char strB[]="<a name=\"cgqb\"";//ƥ���ַ�B ǰ���6���ַ�������yyy.yyֻҪ���¶������滻�ͺ�

char  FS_Writehtml(char *p, unsigned int size)
{
 #ifdef USED_FILESYSTEM
   	static unsigned int deepsizeA=0,deepsizeB=0;//��־�ַ����ļ��е�λ��
	     //��HTML�ļ�
  static char tempt[14];
	 if(memcmp (tempt, p+17,6)==0)return 0;
	   tempt[0]= *(p+17);
	   tempt[1]= *(p+18);
	   tempt[2]= *(p+19);
	   tempt[3]= *(p+20);
	   tempt[4]= *(p+21);
	   tempt[5]= *(p+22);

	   tempt[6]= *(p+24);
	   tempt[7]= *(p+25);
	   tempt[8]= *(p+26);
	   tempt[9]= *(p+27);
	   tempt[10]= *(p+28);
	   tempt[11]= *(p+29);

     	if((deepsizeA==0))//�����һ�ζ��ļ�������ô��Ҫ���в��ң��Ժ�Ͳ����ˡ�
	{
				  if (file_fopen(&myfile_CR, &myefs2.myFs, "index.txt", 'r') != 0)
				  {										
				    return 0;
				  }
			
			
			deepsizeA= FindFileString(strA);//����A�ַ���A��λ��
			file_fread(&myfile_CR,deepsizeA-STRINGLENTH,STRINGLENTH,filebuf);
			FileSystemClose(&myfile_CR,&myefs2.myFs);
  }

  	if((deepsizeB==0))//�����һ�ζ��ļ�������ô��Ҫ���в��ң��Ժ�Ͳ����ˡ�
	{
				  if (file_fopen(&myfile_CR, &myefs2.myFs, "index.txt", 'r') != 0)
				  {										
				    return 0;
				  }
			
			
			deepsizeB= FindFileString(strB);//����A�ַ���A��λ��
			file_fread(&myfile_CR,deepsizeB-STRINGLENTH,STRINGLENTH,filebuf);
			FileSystemClose(&myfile_CR,&myefs2.myFs);
  }

  if (file_fopen(&myfile_CR, &myefs2.myFs, "index.txt", 'w') != 0)
	  {										
	    return 0;
	  }
 file_fwrite(&myfile_CR,deepsizeB-STRINGLENTH,STRINGLENTH,tempt);
 file_fwrite(&myfile_CR,deepsizeA-STRINGLENTH,STRINGLENTH,&tempt[6]);

 FileSystemClose(&myfile_CR,&myefs2.myFs);
  return 1;

   #endif
}  
 
unsigned int FindFileString(char *str)
{
		int len=0,index=0;
		char *p;
	//  FILE *streamA=(FILE *)&myfile_CR;
	
	do
	{
	   len = file_read(&myfile_CR,512,(unsigned char*)filebuf);

		 if(strstr(filebuf,str)!=0)
		 {
		  	p= strstr(filebuf,str);
			 len=(int)p-(int)filebuf;
		 	 return len+(index*512);
		 }
			  index++;
	}while(len==512);//�ļ������������������ض�С��512�ֽ�

	return 0;//û���ҵ�ƥ���ַ�
}
 /*
char *efgets(char *s, int n,  char*stream)

               {

                    register int c;

                    register char *cs;

                    cs=s;

                    while(--n>0 &&(c = egetc(stream))!=EOF)

                         if ((*cs++=  c) =='\n')

                           break;

                     *cs ='\0';

                     return (c == EOF && cs == s) ?NULL :s ;

                   }


 char egetc(char *stream)
 {
 		//char c;
 
 	  //return c ;
 }
	*/

#endif
