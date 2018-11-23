#include <muduo/base/Config.h>
#include <muduo/base/LogFile.h>
#include <muduo/base/Logging.h> // strerror_tl
//#include <muduo/base/ProcessInfo.h>

#include <assert.h>
#include <locale>
#include <sstream>
#include <stdio.h>
#include <time.h>

#include <boost/filesystem.hpp>

using namespace std;
using namespace muduo;


// not thread safe
class LogFile::File : boost::noncopyable
{
 public:
  explicit File(const char * filename, const char * mode)
	  : fp_(::fopen(filename, mode)),
      writtenBytes_(0)
  {
    //assert(fp_);
    //::setbuffer(fp_, buffer_, sizeof buffer_);
    // posix_fadvise POSIX_FADV_DONTNEED ?
  }

  ~File()
  {
	  if (fp_ != NULL)
	  {
		  ::fclose(fp_);
	  }
  }

  bool isOk()
  {
	  return (fp_ != NULL);
  }

  void append(const char* logline, const size_t len)
  {
    size_t n = write(logline, len);
    size_t remain = len - n;
    while (remain > 0)
    {
      size_t x = write(logline + n, remain);
      if (x == 0)
      {
        int err = ferror(fp_);
        if (err)
        {
          fprintf(stderr, "LogFile::File::append() failed %s\n", strerror_tl(err));
        }
        break;
      }
      n += x;
      remain = len - n; // remain -= x
    }

    writtenBytes_ += len;
  }

  void flush()
  {
    ::fflush(fp_);
  }

  size_t writtenBytes() const { return writtenBytes_; }

 private:

  size_t write(const char* logline, size_t len)
  {
//#undef fwrite_unlocked
    return ::fwrite_unlocked(logline, 1, len, fp_);
  }

  FILE* fp_;
  char buffer_[64*1024];
  size_t writtenBytes_;
};

LogFile::LogFile(const string& basedir, 
			     const string& basename,
                 size_t rollSize,
                 bool threadSafe,
                 int flushInterval)
  : basedir_(basedir),
	basename_(basename),
    rollSize_(rollSize),
    flushInterval_(flushInterval),
    count_(0),
    mutex_(threadSafe ? new MutexLock : NULL),
    startOfPeriod_(0),
    lastRoll_(0),
    lastFlush_(0)
{
  assert(basename_.find('/') == string::npos);

  rollFile();
}

LogFile::~LogFile()
{
}

void LogFile::append(const char* logline, int len)
{
  if (mutex_)
  {
    MutexLockGuard lock(*mutex_);
    append_unlocked(logline, len);
  }
  else
  {
    append_unlocked(logline, len);
  }
}

void LogFile::flush()
{
  if (mutex_)
  {
    MutexLockGuard lock(*mutex_);
    file_->flush();
  }
  else
  {
    file_->flush();
  }
}

void LogFile::append_unlocked(const char* logline, int len)
{
  file_->append(logline, len);

  string fileID = getFileID(logline);
  if (fileID_ != fileID || file_->writtenBytes() > rollSize_)
  {
	  rollFile(fileID);
  }
}

void LogFile::rollFile(string newfileID)
{
	if (newfileID.empty())
	{
		ptime pt = second_clock::local_time();

		ostringstream oss;
		time_facet* output_facet = new time_facet();
		output_facet->format("%Y%m%d %H");
		oss.imbue(locale(locale::classic(), output_facet));

		oss << pt;

		newfileID = oss.str();
	}

	if (fileID_ != newfileID)
	{
		fileID_ = newfileID;
		count_ = 0;
	}

	istringstream iss(newfileID);
	time_input_facet* input_facet = new time_input_facet();
	input_facet->format("%Y%m%d %H");
	iss.imbue(locale(locale::classic(), input_facet));

	ptime t;
	iss >> t;

	ostringstream filename;
	filename << getLogFileName(basedir_, basename_, t) << ".log";
	file_.reset(new File(filename.str().c_str(), "a+"));
	lastFlush_ = time(NULL);
	//while (true)
	//{
	//	ostringstream fname;

	//	fname << filename.str();

	//	if (count_ != 0)
	//	{
	//		fname << "." << count_;
	//	}

	//	fname << ".log";

	//	file_.reset(new File(fname.str().c_str(), "r"));
	//	if (!file_->isOk())
	//	{
	//		file_.reset(new File(fname.str().c_str(), "w"));
	//		if (file_->isOk())
	//		{
	//			break;
	//		}
	//	}

	//	count_++;
	//	if (count_>10000)
	//	{
	//		throw "create log file faild";
	//	}
	//}
}

string LogFile::getLogFileName(const string& basedir, const string& basename, ptime thetime)
{
	ostringstream oss;
	time_facet* output_facet = new time_facet();
	output_facet->format("%Y-%m-%d");
	oss.imbue(locale(locale::classic(), output_facet));

	oss << basedir << "/" << thetime;
	boost::filesystem::create_directories(oss.str());

	output_facet->format("%Y%m%d-%H");
	oss << "/" << basename << "_" << thetime;

	return oss.str();
}

string LogFile::getFileID(string log)
{
	return log.substr(0, 11);
}

