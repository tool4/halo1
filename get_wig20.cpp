#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "main.h"
#include "get_wig20.h"

int comment = 0;
int tag = 0;
int in_script = 0;
int in_comment = 0;

// if found ftell will point at end of found string
bool search_for(char *token, FILE* plik)
{
	int position = 0;
    int c = 0;
	bool ret = true;
	
    if(!plik)
	    return false;
	
	position = ftell( plik );
	
	for( unsigned int i = 0; i < strlen( token ); i++)
	{
		c = fgetc( plik );
		if( tolower( c ) != tolower( *(token++) )  )
		{
			ret = false;
			break;
		}
	}
	
	if( !ret )
	{
		fseek( plik, position, SEEK_SET );
	}
	return ret;
}

int parse_txt_to_txt( char* filein, char* fileout)
{
	FILE* plik1;
	FILE* plik2;

	int c=0,lastc=0;
	char buf[BUFSIZE];
	char buf2[BUFSIZE];
	
	plik1=fopen(filein,"rt");
	if(!plik1)
	{
		printf("cannot open file %s for reading", filein);
		return 1;
	}
	
	plik2=fopen(fileout,"wt");
	if(!plik2)
	{
	    fclose(plik1);
	    printf("cannot open file %s for writting", fileout);
	    return 1;
	}
	else
	{
	    printf( "file %s opened \n", fileout);
	}
	int count =0;
	while(!feof(plik1))
	{
		fgets(buf, BUFSIZE, plik1);
		if( !strncmp( buf, "Data notowania:", 5) )
		{
			fgets(buf2, BUFSIZE, plik1);  //skip 1st line
			fgets(buf2, BUFSIZE, plik1);
// 			fputs(buf2, plik2);
		}
		if( !strncmp( buf, "warto", 5) )
		{
			count++;
			fgets(buf, BUFSIZE, plik1);  //skip 2 lines
			fgets(buf, BUFSIZE, plik1);
		}
		//		remove(buf,-62);
		//		remove(buf,-96);
		//		remove(buf,'\t');
		
		if(count && count < 21)
		{
			buf[ strlen(buf)-1 ] = ' ' ; // ending '\n' -> ' ' 
			strncat( buf, buf2, BUFSIZE - strlen(buf) - 2); 
			fputs(buf, plik2);
			count++;
		}
		lastc=c;
	}
	fclose (plik1);
	fclose (plik2);
	return 0;

}

int parse_html_to_txt( char* filein, char* fileout)
{
	
    FILE* plik1;
	FILE* plik2;

	int c=0,lastc=0,lastf=0;
	int in=0;
	
	plik1=fopen(filein,"rt");
	if(!plik1)
	{
	    printf("cannot open file %s for reading", filein);
	    return 1;
	}
	
	
	plik2=fopen(fileout,"wt");
	if(!plik2)
	{
	    fclose(plik1);
	    printf("cannot open file %s for writting", fileout);
	    return 1;
	}
	
	int a = 1;

	while(!feof(plik1))
	{
	    lastc=c;
	    c=fgetc(plik1);
		
	    if( in_comment )
	    {
			if( search_for( (char*)"-->",plik1 ) )
			{
				in_comment = false;
				--in;
			}
			else
			{
				continue;
			}
	    }
		
		if( lastc == '<' && 
			c == 'a' )
		{
			a = 1 - a;
			if( !a )
				fputc('\n',plik2);  //add newline on every second <a>
		}

	    if(c=='<')
	    {
	        if( search_for( (char*)"!--",plik1 ) )
	    	{
	    	    in_comment = true;
				in++;// = 1;
				continue;
			}
			
			if( !in_script )
			{
				if( search_for( (char*)"script",plik1 ) )
				{
					in_script = true;
				}
				++in;
			}
			else
			{
				if( search_for( (char*)"/script>",plik1 ) )
				{
					in_script = false;
					--in;
					continue;
				}
			}
	    }
	    if( in_script )
	    {
		if( c=='>')
			--in;
		continue;
	    }
		if(c==' ' && lastc==' ')
			continue;
		if(c==' ' && lastc=='\n')
			continue;
		if(c=='\n' && lastc=='\n')
			continue;

	    if(c=='&')
	    {
	        if( search_for( (char*)"nbsp;",plik1 ) )
			{
				fgetc(plik1);
				fgetc(plik1);
 				fputc('\n',plik2);
				continue;
			}
		}
// 		if( lastc == ' ' && 
// 			c <= 'Z' &&
// 			c >= 'A'  )
// 		{
// 			fputc('*',plik2);
// 		}
		
 		if(c==13)
 			continue;
		if(c=='\n' && lastc==' ')
			continue;
		if(c=='>')
		{
			--in;
			if( in < 0 )
				in = 0;
			
 			if(lastf != '\n')
 				fputc(' ',plik2);
			lastf='\n';
			continue;
		}
		
		if(in==0)
		{
			lastf=c;
// 			if( c==' ')
// 				fputc('.',plik2);
// 			else
			fputc(c,plik2);
		}
		
	}
	fclose(plik1);
	fclose(plik2);
    return 0;
}

void get_wig20()
{
    printf( "Getting stock fromm millenniumdm.pl:\n" );
	
	//    system( "wget http://notowania.pb.pl/stocktable/WIG20 -O files/wig20.html" );
	system( "wget http://www.millenniumdm.pl/no_gpw_ciagle_wig20.html -O files/wig20.html" );
	
    parse_html_to_txt( (char*)"files/wig20.html",
					   (char*)"files/wig20.tmp" );
	
    parse_txt_to_txt( (char*)"files/wig20.tmp",
					  (char*)"files/wig20.txt" );
}

