#pragma once

#define BUFSIZE 255

#define WOLUMENS 2

struct SIndexes{
		double nasdaq;
		double djia;
		double s_p_500;
		double nikkei;
		double hangseng;
		double cac_40;
		double ftse;
};


//log to file files/makler_log.txt
void log( const char* fmt, ... );

//log to file filename
void logf( char* filename, char* fmt, ... );

//replace each occurence of char a in string str with char b
void replace(char *str, char a, char b);

//replace each occurence of char a in string str with char b
void remove(char *str, char a);

// cut from str substring starting with char, ending with char b
void cut(char *str, char a, char b);

char* gettime();


