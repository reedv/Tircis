/**
 * main.cpp
 * main program to start up the Tircis_Process software
 **/

#include "mainprocess.h"
#include <iostream>
using namespace std ;

int main(int argc, char *argv[])
{
	char str[240] ;
	if (argc < 2) 
	{
		cout << "Usage: tircis_process_cmd newproc.txt" << endl ;
		exit (-1) ;
	}
	else 
		strcpy (str, *++argv) ;
	MainProcess *mp = new MainProcess () ;
	mp->readProcessFile (str) ; 
	mp->run() ;
}
