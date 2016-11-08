#pragma once

// player.h

#ifndef LOGGER_CSV_H
#define LOGGER_CSV_H

class CvLoggerCSV
{
public:

		CvLoggerCSV() {};
		~CvLoggerCSV() {};

		//Logging Functions
		static void WriteCSVLog(const char* strLogName, const char* strHeader);
		static void DeleteCSV(const char* strLogName);

};

#endif