#include <log4cxx/logger.h>
#include <log4cxx/xml/domconfigurator.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/consoleappender.h>

#include "utils.h"
#include <atomic>
#include <cstdlib> // EXIT_FAILURE
#include <memory>
#include <string>
#include <thread>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>

#include <fmt/core.h>
#include <fmt/chrono.h>
#include <fmt/ostream.h>

#include <boost/log/trivial.hpp>

#include "log4cxxbenchmarker.h"

static log4cxx::LoggerPtr console = log4cxx::Logger::getLogger( "console" );

static void benchmark_function( std::string name, void (*fn)(int), int howmany ){
	using std::chrono::duration;
	using std::chrono::duration_cast;
	using std::chrono::high_resolution_clock;

	auto start = high_resolution_clock::now();
	fn(howmany);
	auto delta = high_resolution_clock::now() - start;
	auto delta_d = duration_cast<duration<double>>(delta).count();

	LOG4CXX_INFO( console, "Log4cxx " << name << " Elapsed: " << delta_d << " secs " << int(howmany / delta_d) << "/sec" );
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

	LOG4CXX_INFO( console, "Log4cxx " << name
				  << " pattern: " << conversion_pattern
				  << " Elapsed: " << delta_d << " secs " << int(howmany / delta_d) << "/sec" );
}

static void bench_log4cxx_single_threaded(int iters)
{
    LOG4CXX_INFO(console, "**************************************************************");
    LOG4CXX_INFO(console, "Single threaded: " << iters << " messages" );
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

static void bench_fputs(int howmany){
	using std::chrono::duration;
	using std::chrono::duration_cast;
	using std::chrono::high_resolution_clock;

	FILE* f = fopen( "log/fputs", "w+" );
	auto start = high_resolution_clock::now();
	for (auto i = 0; i < howmany; ++i)
	{
		std::string msg =  "Hello logger: msg number ";
		msg.append( std::to_string( i ) );
		msg.append( "\n" );
		fputs( msg.c_str(), f );
	}
	fclose(f);

	auto delta = high_resolution_clock::now() - start;
	auto delta_d = duration_cast<duration<double>>(delta).count();

	LOG4CXX_INFO( console, "fputs: Elapsed: " << delta_d << " secs " << int(howmany / delta_d) << "/sec" );
}

static void bench_fputs_libfmt(int howmany){
	using std::chrono::duration;
	using std::chrono::duration_cast;
	using std::chrono::high_resolution_clock;

	FILE* f = fopen( "log/fputs", "w+" );
	auto start = high_resolution_clock::now();
	for (auto i = 0; i < howmany; ++i)
	{
//		std::string msg = fmt::format("Hello logger: msg number {}", i);
		std::time_t now = std::time(nullptr);
		std::string msg = fmt::format( "date: {}\n",
//					fmt::arg("localdate",fmt::localtime(now)),
//					fmt::arg("gmtdate", fmt::gmtime(now)),
					fmt::arg("logger", "foo.bar.baz"),
					fmt::arg("class", "ClassName"),
					fmt::arg("filename","foo.cpp"),
					fmt::arg("line", 67),
					fmt::arg("message", "this is the message for the logger"),
					fmt::arg("method", "example_method"),
					fmt::arg("newline", "\n"),
					fmt::arg("level", "INFO"),
					fmt::arg("runtime", 892342),
					fmt::arg("thread", "threadname"),
					fmt::arg("ndc", "" ),
					fmt::arg("mdc", "") );
		fputs( msg.c_str(), f );
	}
	fclose(f);

	auto delta = high_resolution_clock::now() - start;
	auto delta_d = duration_cast<duration<double>>(delta).count();

	LOG4CXX_INFO( console, "fputs_libfmt: Elapsed: " << delta_d << " secs " << int(howmany / delta_d) << "/sec" );
}

static void bench_write(int howmany){
	using std::chrono::duration;
	using std::chrono::duration_cast;
	using std::chrono::high_resolution_clock;

	int fd = open( "log/write", O_RDWR | O_CREAT | O_NONBLOCK, S_IFREG | S_IRUSR | S_IWUSR );
	auto start = high_resolution_clock::now();
	for (auto i = 0; i < howmany; ++i)
	{
		std::string msg =  "Hello logger: msg number ";
		msg.append( std::to_string( i ) );
		msg.append( "\n" );
		write( fd, msg.c_str(), msg.length() );
	}
	close(fd);

	auto delta = high_resolution_clock::now() - start;
	auto delta_d = duration_cast<duration<double>>(delta).count();

	LOG4CXX_INFO( console, "write: Elapsed: " << delta_d << " secs " << int(howmany / delta_d) << "/sec" );
}

static void bench_fstream(int howmany){
	using std::chrono::duration;
	using std::chrono::duration_cast;
	using std::chrono::high_resolution_clock;

	std::fstream f( "log/fstream" );
	auto start = high_resolution_clock::now();
	for (auto i = 0; i < howmany; ++i)
	{
		std::string msg =  "Hello logger: msg number ";
		msg.append( std::to_string( i ) );
		msg.append( "\n" );
		f << msg;
	}
	f.close();

	auto delta = high_resolution_clock::now() - start;
	auto delta_d = duration_cast<duration<double>>(delta).count();

	LOG4CXX_INFO( console, "fstream: Elapsed: " << delta_d << " secs " << int(howmany / delta_d) << "/sec" );
}

static void bench_fwrite(int howmany){
	using std::chrono::duration;
	using std::chrono::duration_cast;
	using std::chrono::high_resolution_clock;

	FILE* f = fopen( "log/fputs", "w+" );
	auto start = high_resolution_clock::now();
	for (auto i = 0; i < howmany; ++i)
	{
		std::string msg =  "Hello logger: msg number ";
		msg.append( std::to_string( i ) );
		msg.append( "\n" );
		fwrite( msg.c_str(), 1, msg.length(), f );
	}
	fclose(f);

	auto delta = high_resolution_clock::now() - start;
	auto delta_d = duration_cast<duration<double>>(delta).count();

	LOG4CXX_INFO( console, "fwrite: Elapsed: " << delta_d << " secs " << int(howmany / delta_d) << "/sec" );
}

static void bench_libfmt(int howmany, log4cxx::LoggerPtr log)
{
	using std::chrono::duration;
	using std::chrono::duration_cast;
	using std::chrono::high_resolution_clock;

	auto start = high_resolution_clock::now();
	for (auto i = 0; i < howmany; ++i)
	{
		LOG4CXX_INFO( log, fmt::format("Hello logger: msg number {}", i));
	}

	auto delta = high_resolution_clock::now() - start;
	auto delta_d = duration_cast<duration<double>>(delta).count();

	LOG4CXX_INFO( console, "(libfmt)" << log->getName() << " Elapsed: " << delta_d << " secs " << int(howmany / delta_d) << "/sec" );
}

int main(int argc, char *argv[])
{
	int iters = 500000;
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

    try
    {

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

		bench_log4cxx_single_threaded(iters);


//		LOG4CXX_INFO(console, "Benchmarking raw fputs:" );
//		bench_fputs(iters);

//		LOG4CXX_INFO(console, "Benchmarking raw write:" );
//		bench_write(iters);

//		LOG4CXX_INFO(console, "Benchmarking fstream:" );
//		bench_fstream(iters);

//		LOG4CXX_INFO(console, "Benchmarking fwrite:" );
//		bench_fwrite(iters);

//		LOG4CXX_INFO(console, "Benchmarking fputs with libfmt:" );
//		bench_fputs_libfmt(iters);

//		LOG4CXX_INFO(console, "Benchmarking libfmt:" );
//		bench_libfmt(iters, logger1);
        //bench_threaded_logging(1, iters);
        //bench_threaded_logging(threads, iters);

    }
    catch (std::exception &ex)
    {
		LOG4CXX_ERROR( console, ex.what() );
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
