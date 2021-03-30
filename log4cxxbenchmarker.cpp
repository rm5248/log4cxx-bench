#include "log4cxxbenchmarker.h"

#include <log4cxx/logger.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/appenderskeleton.h>

#include <fmt/format.h>

namespace log4cxx {

class NullWriterAppender : public log4cxx::AppenderSkeleton {
public:
	DECLARE_LOG4CXX_OBJECT(NullWriterAppender)
	BEGIN_LOG4CXX_CAST_MAP()
	LOG4CXX_CAST_ENTRY(NullWriterAppender)
	LOG4CXX_CAST_ENTRY_CHAIN(AppenderSkeleton)
	END_LOG4CXX_CAST_MAP()

	NullWriterAppender(){}

	virtual void close(){}

	virtual bool requiresLayout() const {
		return true;
	}

	virtual void append(const spi::LoggingEventPtr& event, log4cxx::helpers::Pool& p){
		// This gets called whenever there is a valid event for our appender.
	}

	virtual void activateOptions(log4cxx::helpers::Pool& /* pool */) {
		// Given all of our options, do something useful(e.g. open a file)
	}

	virtual void setOption(const LogString& option, const LogString& value){
	}
};

IMPLEMENT_LOG4CXX_OBJECT(NullWriterAppender)

LOG4CXX_PTR_DEF(NullWriterAppender);

}

log4cxxbenchmarker::log4cxxbenchmarker()
{

}

log4cxx::LoggerPtr log4cxxbenchmarker::resetLogger(){
	log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "bench_logger" );

	logger->removeAllAppenders();
	logger->setAdditivity( false );
	logger->setLevel( log4cxx::Level::getInfo() );

	log4cxx::PatternLayoutPtr pattern( new log4cxx::PatternLayout() );
	pattern->setConversionPattern( "%m%n" );

	log4cxx::NullWriterAppenderPtr nullWriter( new log4cxx::NullWriterAppender );
	nullWriter->setLayout( pattern );

	logger->addAppender( nullWriter );

	return logger;
}

void log4cxxbenchmarker::logWithConversionPattern( std::string conversionPattern, int howmany ){
	log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "bench_logger" );

	logger->removeAllAppenders();
	logger->setAdditivity( false );
	logger->setLevel( log4cxx::Level::getInfo() );

	log4cxx::PatternLayoutPtr pattern( new log4cxx::PatternLayout() );
	pattern->setConversionPattern( conversionPattern );

	log4cxx::NullWriterAppenderPtr nullWriter( new log4cxx::NullWriterAppender );
	nullWriter->setLayout( pattern );

	logger->addAppender( nullWriter );

	for( int x = 0; x < howmany; x++ ){
		LOG4CXX_INFO( logger, "Hello logger: msg number " << x);
	}
}

void log4cxxbenchmarker::logWithFMT(int howmany){
	log4cxx::LoggerPtr logger = resetLogger();

	for( int x = 0; x < howmany; x++ ){
		LOG4CXX_INFO_FMT( logger, "Hello logger: msg number {}", x);
	}
}

void log4cxxbenchmarker::logString( int howmany ){
	log4cxx::LoggerPtr logger = resetLogger();

	for( int x = 0; x < howmany; x++ ){
		LOG4CXX_INFO( logger, "This is a static string to see what happens");
	}
}

void log4cxxbenchmarker::logStringFMT( int howmany ){
	log4cxx::LoggerPtr logger = resetLogger();

	for( int x = 0; x < howmany; x++ ){
		LOG4CXX_INFO_FMT( logger, "This is a static string to see what happens");
	}
}

void log4cxxbenchmarker::logDisabledDebug( int howmany ){
	log4cxx::LoggerPtr logger = resetLogger();

	for( int x = 0; x < howmany; x++ ){
		LOG4CXX_DEBUG( logger, "This is a static string to see what happens");
	}
}

void log4cxxbenchmarker::logDisabledTrace( int howmany ){
	log4cxx::LoggerPtr logger = resetLogger();

	for( int x = 0; x < howmany; x++ ){
		LOG4CXX_TRACE( logger, "This is a static string to see what happens");
	}
}

void log4cxxbenchmarker::logEnabledDebug( int howmany ){
	log4cxx::LoggerPtr logger = resetLogger();
	logger->setLevel( log4cxx::Level::getDebug() );

	for( int x = 0; x < howmany; x++ ){
		LOG4CXX_DEBUG( logger, "This is a static string to see what happens");
	}
}

void log4cxxbenchmarker::logEnabledTrace( int howmany ){
	log4cxx::LoggerPtr logger = resetLogger();
	logger->setLevel( log4cxx::Level::getTrace() );

	for( int x = 0; x < howmany; x++ ){
		LOG4CXX_DEBUG( logger, "This is a static string to see what happens");
	}
}
