#ifndef LOG4CXXBENCHMARKER_H
#define LOG4CXXBENCHMARKER_H

#include <string>

#include <log4cxx/logger.h>

class log4cxxbenchmarker
{
private:
	log4cxxbenchmarker();

	static log4cxx::LoggerPtr resetLogger();

public:
	/**
	 * Given a conversion pattern, send a number of log messages to the logger.  INFO level.
	 *
	 * Log message format: "Hello logger: msg number " << x
	 *
	 * @param conversionPattern
	 * @param howmany
	 */
	static void logWithConversionPattern( std::string conversionPattern, int howmany );

	/**
	 * Log with the LOG4CXX_INFO_FMT macro to see how long it takes.
	 *
	 * @param howmany
	 */
	static void logWithFMT( int howmany );

	/**
	 * Log a string that doesn't use operator<< any place.
	 *
	 * String to log: "This is a static string to see what happens"
	 * @param howmany
	 */
	static void logString( int howmany );

       /**
	* Log a string that doesn't use operator<< any place, but uses libfmt.
	*
	* String to log: "This is a static string to see what happens"
	* @param howmany
	*/
	static void logStringFMT( int howmany );

	static void logDisabledDebug( int howmany );

	static void logDisabledTrace( int howmany );

	static void logEnabledDebug( int howmany );

	static void logEnabledTrace( int howmany );
};

#endif // LOG4CXXBENCHMARKER_H
