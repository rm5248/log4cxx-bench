#include <log4cxx/logger.h>
#include <log4cxx/xml/domconfigurator.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/consoleappender.h>

#include <string>
#include <thread>

#include <fmt/core.h>
#include <fmt/chrono.h>
#include <fmt/ostream.h>

#include "log4cxxbenchmarker.h"

static log4cxx::LoggerPtr console = log4cxx::Logger::getLogger( "console" );
static std::vector<uint64_t> results;

static void benchmark_function( std::string name, void (*fn)(int), int howmany ){
	using std::chrono::duration;
	using std::chrono::duration_cast;
	using std::chrono::high_resolution_clock;

	auto start = high_resolution_clock::now();
	fn(howmany);
	auto delta = high_resolution_clock::now() - start;
	auto delta_d = duration_cast<duration<double>>(delta).count();

	LOG4CXX_INFO_FMT( console, "Log4cxx {} Elapsed: {:.4} secs {:L}/sec",
					  name,
					  delta_d,
					  int(howmany / delta_d));

	results.push_back( howmany / delta_d );
}

static void benchmark_conversion_pattern( std::string name,
										  std::string conversion_pattern,
										  void(*fn)(std::string,int),
										  int howmany){
	using std::chrono::duration;
	using std::chrono::duration_cast;
	using std::chrono::high_resolution_clock;

	auto start = high_resolution_clock::now();
	fn(conversion_pattern, howmany);
	auto delta = high_resolution_clock::now() - start;
	auto delta_d = duration_cast<duration<double>>(delta).count();

	LOG4CXX_INFO_FMT( console, "Log4cxx {} pattern: {} Elapsed: {:.4} secs {:L}/sec",
				  name,
				  conversion_pattern,
				  delta_d,
				  int(howmany / delta_d) );

	results.push_back( howmany / delta_d );
}

static void bench_log4cxx_single_threaded(int iters)
{
    LOG4CXX_INFO(console, "**************************************************************");
	LOG4CXX_INFO_FMT(console, "Benchmarking Single threaded: {} messages", iters );
    LOG4CXX_INFO(console, "**************************************************************");

	benchmark_conversion_pattern( "NoFormat", "%m%n", &log4cxxbenchmarker::logWithConversionPattern, iters );
	benchmark_conversion_pattern( "DateOnly", "[%d] %m%n", &log4cxxbenchmarker::logWithConversionPattern, iters );
	benchmark_conversion_pattern( "DateClassLevel", "[%d] [%c] [%p] %m%n", &log4cxxbenchmarker::logWithConversionPattern, iters );

	benchmark_function( "Logging with FMT", &log4cxxbenchmarker::logWithFMT, iters );
	benchmark_function( "Logging static string", &log4cxxbenchmarker::logString, iters );
	benchmark_function( "Logging static string with FMT", &log4cxxbenchmarker::logStringFMT, iters );
	benchmark_function( "Logging disabled debug", &log4cxxbenchmarker::logDisabledDebug, iters );
	benchmark_function( "Logging disabled trace", &log4cxxbenchmarker::logDisabledTrace, iters );
	benchmark_function( "Logging enabled debug", &log4cxxbenchmarker::logEnabledDebug, iters );
	benchmark_function( "Logging enabled trace", &log4cxxbenchmarker::logEnabledTrace, iters );
}

static void bench_log4cxx_multi_threaded(size_t threads, int iters)
{
	LOG4CXX_INFO(console, "**************************************************************");
	LOG4CXX_INFO_FMT(console, "Benchmarking multithreaded threaded: {} messages/thread, {} threads", iters, threads );
	LOG4CXX_INFO(console, "**************************************************************");

	std::vector<std::thread> runningThreads;

	log4cxxbenchmarker::log_MT_setup();
	for( size_t x = 0; x < threads; x++ ){
		runningThreads.push_back( std::thread( [iters](){
			benchmark_function( "Logging with FMT", &log4cxxbenchmarker::logWithFMT_MT, iters );
		}) );
	}

	for( std::thread& th : runningThreads ){
		th.join();
	}
}

int main(int argc, char *argv[])
{
	int iters = 1000000;
	size_t threads = 4;
	size_t max_threads = 32;

	std::setlocale( LC_ALL, "" ); /* Set locale for C functions */
	std::locale::global(std::locale("")); /* set locale for C++ functions */

	console->setAdditivity( false );
	log4cxx::PatternLayoutPtr pattern( new log4cxx::PatternLayout() );
	pattern->setConversionPattern( "%m%n" );

	log4cxx::ConsoleAppenderPtr consoleWriter( new log4cxx::ConsoleAppender );
	consoleWriter->setLayout( pattern );
	consoleWriter->setTarget( "System.out" );
	log4cxx::helpers::Pool p;
	consoleWriter->activateOptions(p);

	console->addAppender( consoleWriter );

	if (argc > 1)
	{
		iters = std::stoi(argv[1]);
	}
	if (argc > 2)
	{
		threads = std::stoul(argv[2]);
	}

	if (threads > max_threads)
	{
		throw std::runtime_error("Number of threads exceeds maximum({}})");
	}

	LOG4CXX_INFO_FMT(console, "Benchmarking library only(no writing out):");
	bench_log4cxx_single_threaded(iters);
	bench_log4cxx_multi_threaded(threads, iters);

	LOG4CXX_INFO_FMT(console, "Results for use in spreadsheet:");
	for( uint64_t result : results ){
		LOG4CXX_INFO_FMT(console, "{}", result );
	}

    return EXIT_SUCCESS;
}
