#ifndef GRIDGENERATOR_LOG_H
#define GRIDGENERATOR_LOG_H
#include <fstream>
#include <memory>
#include <mpi.h>
#include <sstream>
#include <vector>
#include "common/compiler_config.h"
#include "common/types.h"

inline std::ostream cerr0(nullptr); // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

class Log_buffer : public std::stringbuf {
  friend class Log;

 protected:
  static const GInt m_fileFormatVersion = 1; //!< File format version (increase this by one every time you make
  //!< changes that could affect postprocessing tools)
  GBool              m_rootOnly;      //!< Stores whether only the root domain writes a log file
  int                m_domainId;      //!< Contains the MPI rank (= domain id) of this process
  int                m_noDomains;     //!< Contains the MPI rank count (= number of domains)
  GInt               m_minFlushSize;  //!< Minimum length of the internal buffer before flushing
  GString            m_prefixMessage; //!< Stores the prefix that is prepended to each output
  GString            m_suffixMessage; //!< Stores the suffix that is appended to each output
  std::ostringstream m_tmpBuffer;     //!< Temporary buffer to hold string until flushing
  GInt               m_argc{};
  GChar**            m_argv{};

  std::vector<std::pair<GString, GString>> m_prefixAttributes;

  virtual auto encodeXml(const GString& str) -> GString;
  virtual auto getXmlHeader() -> GString;
  virtual auto getXmlFooter() -> GString;
  virtual void createPrefixMessage();
  virtual void createSuffixMessage();
  virtual void flushBuffer() = 0;

 public:
  Log_buffer() : m_rootOnly(false), m_domainId(0), m_noDomains(1), m_minFlushSize(0), m_prefixMessage(), m_suffixMessage(), m_tmpBuffer() {}

  Log_buffer(const GInt argc, GChar** argv)
    : m_rootOnly(false),
      m_domainId(0),
      m_noDomains(1),
      m_minFlushSize(0),
      m_prefixMessage(),
      m_suffixMessage(),
      m_tmpBuffer(),
      m_argc(argc),
      m_argv(argv) {}

  virtual auto setRootOnly(GBool rootOnly = true) -> GBool;
  virtual auto setMinFlushSize(GInt minFlushSize) -> GInt;
  virtual void close(GBool forceClose = false) = 0;
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
  GBool         m_isOpen{false};            //!< Stores whether the file(s) were already opened
  GBool         m_rootOnlyHardwired{false}; //!< If true, only domain 0 opens and uses a file
  GString       m_filename;                 //!< Filename on disk
  std::ofstream m_file;                     //!< File stream tied to physical file on disk
  MPI_Comm      m_mpiComm{};                //!< MPI communicator group

 protected:
  auto sync() -> int override;
  void flushBuffer() override;

 public:
  Log_simpleFileBuffer() : m_filename(), m_file(){};
  Log_simpleFileBuffer(const GString& filename, const GInt m_argc, GChar** m_argv, MPI_Comm mpiComm = MPI_COMM_WORLD,
                       GBool rootOnlyHardwired = false);
  ~Log_simpleFileBuffer() override { close(); };

  Log_simpleFileBuffer(const Log_simpleFileBuffer&) = delete;
  Log_simpleFileBuffer(Log_simpleFileBuffer&&)      = delete;
  auto operator=(const Log_simpleFileBuffer&) -> Log_simpleFileBuffer& = delete;
  auto operator=(Log_simpleFileBuffer&&) -> Log_simpleFileBuffer& = delete;

  void open(const GString& filename, MPI_Comm mpiComm = MPI_COMM_WORLD, GBool rootOnlyHardwired = false);
  void close(GBool forceClose = false) override;
};

/**
 * \brief Base class for all Log<xyz> classes.
 * \author Michael Schlottke, Sven Berger
 * \date June 2012
 * \details This class is used to hold stream/buffer-independent methods. All Log<xyz>
 * subclasses inherit from this class. The auxiliary classes Log_<xyz> (especially the
 * buffers), however, should NOT have this class as their baseclass.
 */
class Log : public std::ostream {
  friend class Log_buffer;

 public:
#ifdef CLANG_COMPILER
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wuninitialized"
#endif
  Log() : std::ostream(m_buffer.get()){};
#ifdef CLANG_COMPILER
#pragma clang diagnostic pop
#endif
  virtual auto setRootOnly(GBool rootOnly = true) -> GBool = 0;

  /** \brief Adds an attribute to the prefix of the XML string.
   * \author Andreas Lintermann, Sven Berger
   * \date 13.08.2012
   *
   * \param[in] att The attribute to add, consists of a pair of MStrings.
   * \return The location of the attribute in the vector of pairs.
   */
  auto addAttribute(const std::pair<GString, const GString>& att) -> GInt {
    m_buffer->m_prefixAttributes.emplace_back(att);
    m_buffer->createPrefixMessage();
    return static_cast<GInt>(m_buffer->m_prefixAttributes.size() - 1);
  }

  /** \brief Erases an attribute from the prefix of the XML string.
   * \author Andreas Lintermann, Sven Berger
   * \date 13.08.2012
   *
   *  \param[in] attId The ID of the attribute to delete.
   */
  void eraseAttribute(GInt attId) {
    m_buffer->m_prefixAttributes.erase(m_buffer->m_prefixAttributes.begin() + attId);
    m_buffer->createPrefixMessage();
  }

  /** \brief Modifies an attribute of the prefix of the XML string.
   * \author Andreas Lintermann, Sven Berger
   * \date 13.08.2012
   *
   *  \param[in] attId The ID of the attribute to modify.
   *  \param[in] att The new attribute to replace the old one, given by a pair of MStrings.
   */
  void modifyAttribute(GInt attId, const std::pair<GString, GString>& att) {
    m_buffer->m_prefixAttributes[attId] = att;
    m_buffer->createPrefixMessage();
  }

  inline auto buffer() -> std::unique_ptr<Log_buffer>& { return m_buffer; }
  inline auto buffer() const -> const std::unique_ptr<Log_buffer>& { return m_buffer; }

 private:
  std::unique_ptr<Log_buffer> m_buffer;
};

/**
 * \brief Class to create an output stream for a writable file, using a physical file.
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
  /**
   * \brief Constructor creates LogFile and calls ostream constructor with reference to it.
   * \author Sven Berger
   * \details When this constructor is used, the stream is immediately ready to use. For information about the
   * parameters, please have a look at Log*Buffer::open.
   *
   * \param[in] filename Name of the file to open.
   * \param[in] mpiComm MPI communicator for which to open the file.
   */
  LogFile(const GString& filename, MPI_Comm mpiComm = MPI_COMM_WORLD, GBool rootOnlyHardwired = false) {
    open(filename, rootOnlyHardwired, 0, nullptr, mpiComm);
  }
  ~LogFile() override { close(); };

  LogFile(const LogFile&) = delete;
  LogFile(LogFile&&)      = delete;
  auto operator=(const LogFile&) -> LogFile& = delete;
  auto operator=(LogFile&&) -> LogFile& = delete;

  void open(const GString& filename, const GBool rootOnlyHardwired, const GInt argc, GChar** argv, MPI_Comm mpiComm = MPI_COMM_WORLD);
  void close(GBool forceClose = false);
  auto setRootOnly(GBool rootOnly = true) -> GBool override;
  auto setMinFlushSize(GInt minFlushSize) -> GInt;
};
inline LogFile gridgen_log; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)


#endif // GRIDGENERATOR_LOG_H
