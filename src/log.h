#ifndef GRIDGENERATOR_LOG_H
#define GRIDGENERATOR_LOG_H
#include <fstream>
#include <mpi.h>
#include <sstream>
#include <vector>

class Log_buffer : public std::stringbuf {
  friend class Log;

 protected:
  static const int m_fileFormatVersion = 1; //!< File format version (increase this by one every time you make
  //!< changes that could affect postprocessing tools)
  bool               m_rootOnly;      //!< Stores whether only the root domain writes a log file
  int                m_domainId;      //!< Contains the MPI rank (= domain id) of this process
  int                m_noDomains;     //!< Contains the MPI rank count (= number of domains)
  int                m_minFlushSize;  //!< Minimum length of the internal buffer before flushing
  std::string        m_prefixMessage; //!< Stores the prefix that is prepended to each output
  std::string        m_suffixMessage; //!< Stores the suffix that is apended to each output
  std::ostringstream m_tmpBuffer;     //!< Temporary buffer to hold string until flushing
  int                m_argc{};
  char**             m_argv{};

  std::vector<std::pair<std::string, std::string>> m_prefixAttributes;

  virtual auto encodeXml(const std::string& str) -> std::string;
  virtual auto getXmlHeader() -> std::string;
  virtual auto getXmlFooter() -> std::string;
  virtual void createPrefixMessage();
  virtual void createSuffixMessage();
  virtual void flushBuffer() = 0;

 public:
  Log_buffer()
    : m_rootOnly(false),
      m_domainId(0),
      m_noDomains(1),
      m_minFlushSize(0),
      m_prefixMessage(),
      m_suffixMessage(),
      m_tmpBuffer() {}

  Log_buffer(int argc, char** argv)
    : m_rootOnly(false),
      m_domainId(0),
      m_noDomains(1),
      m_minFlushSize(0),
      m_prefixMessage(),
      m_suffixMessage(),
      m_tmpBuffer(),
      m_argc(argc),
      m_argv(argv) {}

  virtual auto setRootOnly(bool rootOnly = true) -> bool;
  virtual auto setMinFlushSize(int minFlushSize) -> int;
};

/**
 * \brief Base class for all Log<xyz> classes.
 * \author Michael Schlottke, Sven Berger
 * \date June 2012
 * \details This class is used to hold stream/buffer-independent methods. All Log<xyz>
 * subclasses inherit from this class. The auxiliary classes Log_<xyz> (especially the
 * buffers), however, should NOT have this class as their parent.
 */
class Log : public std::ostream {
  friend class Log_buffer;

 protected:
  Log_buffer* m_buffer = nullptr;

 public:
  Log() : std::ostream(m_buffer){};
  virtual auto setRootOnly(bool rootOnly = true) -> bool = 0;
  auto         addAttribute(const std::pair<std::string, std::string>&) -> int;
  void         eraseAttribute(int);
  void         modifyAttribute(int, const std::pair<std::string, std::string>&);
};


/**
 * \brief Customized buffer to facilitate of a regular physical file for each processor within an MPI communicator.
 * \author Michael Schlottke, Sven Berger
 * \date June 2012
 * \details This class can be used as a regular string buffer, as it inherits from stringbuf. On flushing the
 *          buffer, the contents of the buffer are written to a file using an ofstream. This is mainly for cases
 *          where logging speed is crucial, as the implementation is very lightweight and since for each process
 *          an individual file is maintained. There is an option to use this buffer but to create only one file
 *          for the MPI root domain (see #m_rootOnlyHardwired).
 */
class Log_simpleFileBuffer : public Log_buffer {
 private:
  bool          m_isOpen;            //!< Stores whether the file(s) were already opened
  bool          m_rootOnlyHardwired; //!< If true, only domain 0 opens and uses a file
  std::string   m_filename;          //!< Filename on disk
  std::ofstream m_file;              //!< File stream tied to physical file on disk
  MPI_Comm      m_mpiComm;           //!< MPI communicator group

 protected:
  auto sync() -> int override;
  void flushBuffer() override;

 public:
  Log_simpleFileBuffer() : m_isOpen(false), m_rootOnlyHardwired(false), m_filename(), m_file(), m_mpiComm(){};
  Log_simpleFileBuffer(const std::string& filename, int m_argc, char** m_argv, MPI_Comm mpiComm = MPI_COMM_WORLD,
                       bool rootOnlyHardwired = false);
  ~Log_simpleFileBuffer() override;
  void open(const std::string& filename, MPI_Comm mpiComm = MPI_COMM_WORLD, bool rootOnlyHardwired = false);
  void close(bool forceClose = false);
};

/**
 * \brief Class to create a create an output stream for a writable file, using either MPI I/O or a physical file.
 * \author Michael Schlottke, Sven Berger
 * \date June 2012
 * \details This class can be used to open a file on all processors (alternatively: only on a specified MPI
 *          communicator) and write to it using the normal C++ stream syntax (i.e. just like cout or cerr).
 *
 *          A regular physical files for each processor, and to write
 *          directly to it using a regular ofstream. This mode also allows for the setting that only process 0
 *          within an MPI communicator opens a file to write to.
 */
class LogFile : public Log {
 private:
  bool m_isOpen = false; //!< Stores whether a file was already opened or not

 public:
  LogFile() = default;
  LogFile(const std::string& filename, MPI_Comm mpiComm = MPI_COMM_WORLD, bool rootOnlyHardwired = false);
  ~LogFile() override;
  void open(const std::string& filename, bool rootOnlyHardwired, int argc, char** argv,
            MPI_Comm mpiComm = MPI_COMM_WORLD);
  void close(bool forceClose = false);
  auto setRootOnly(bool rootOnly = true) -> bool override;
  auto setMinFlushSize(int minFlushSize) -> int;
};

#endif // GRIDGENERATOR_LOG_H
