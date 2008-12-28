#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "main.h"

void log( const char* fmt, ... )
{
	va_list ap;
	
	va_start(ap, fmt);         /* Initialize the va_list */

	FILE* plik = fopen( "files/makler_log.txt","a");
	if(plik)
	{
		vfprintf( plik, fmt, ap );
		fclose(plik);
	}
	va_end(ap);                /* Cleanup the va_list */
}

void logp( const char* fmt, ... )
{
	va_list ap;
	
	va_start(ap, fmt);         /* Initialize the va_list */

	FILE* plik = fopen( "files/makler_log.txt","a");
	if(plik)
	{
		vprintf( fmt, ap );
		vfprintf( plik, fmt, ap );
		fclose(plik);
	}
	va_end(ap);                /* Cleanup the va_list */
}

void logf( const char* filename, const char* fmt, ... )
{
	va_list ap;
	
	va_start(ap, fmt);         /* Initialize the va_list */

	FILE* plik = fopen( filename ,"a");
	if(plik)
	{
		vfprintf( plik, fmt, ap );
		fclose(plik);
	}
	va_end(ap);                /* Cleanup the va_list */
}
void logfp( const char* filename, const char* fmt, ... )
{
	va_list ap;
	
	va_start(ap, fmt);         /* Initialize the va_list */

	FILE* plik = fopen( filename ,"a");
	if(plik)
	{
		vprintf( fmt, ap );
		vfprintf( plik, fmt, ap );
		fclose(plik);
	}
	va_end(ap);                /* Cleanup the va_list */
}

//replace each occurence of char a in string str with char b
void replace(char *str, char a, char b)
{
	char* p;
	p=strchr(str,a);
	while(p!=NULL)
	{
		*p=b;
		p=strchr(str,a);
	}
}

//replace each occurence of char a in string str with char b
void remove(char *str, char a)
{
	char* p;
	p=strchr(str,a);
	while(p!=NULL)
	{
		strcpy( p, p+1);
		p=strchr(str,a);
	}
}

// cut from str substring starting with char, ending with char b
void cut(char *str, char a, char b)
{
	char* p;
	char* r;
	p=strchr(str,a);
	if( p )
	{
		r=strchr(p,b);
		if( r )
		{
			strcpy( p, r+1);
		}
	}
}
