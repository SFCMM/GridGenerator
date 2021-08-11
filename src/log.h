#ifndef GRIDGENERATOR_LOG_H
#define GRIDGENERATOR_LOG_H
#include <fstream>
#include <memory>
#include <mpi.h>
#include <sfcmm_common.h>
#include <sstream>
#include <vector>
#include "config.h"
#include "macros.h"


// todo: fix documentation
// todo: add tests

class Log_buffer : public std::stringbuf {
  friend class Log;

 public:
  Log_buffer() = default;

  Log_buffer(const GInt argc, GChar** argv) : m_argc(argc), m_argv(argv) {}

  /**
   * \brief Sets interal state of whether only the root domain (rank 0) should write to file.
   * \params[in] rootOnly If true, only rank 0 of the specified MPI communicator writes to file.
   * \return The previous internal state (may be stored to return to the previous behavior).
   */
  virtual auto setRootOnly(GBool rootOnly = true) -> GBool {
    GBool previousValue = m_rootOnly;
    m_rootOnly          = rootOnly;
    return previousValue;
  }

  /**
   * \brief Sets the minimum buffer length that has to be reached before the buffer is flushed.
   * \params[in] minFlushSize Minimum buffer length.
   * \return The previous value of the minimum flush size.
   */
  virtual auto setMinFlushSize(GInt minFlushSize) -> GInt {
    GInt previousValue = m_minFlushSize;
    m_minFlushSize     = minFlushSize;
    return previousValue;
  }

  virtual void close(GBool forceClose = false) = 0;


 protected:
  /**
   * \brief Parses the string input and returns the string with XML entities escaped
   * \details This method iterates over each character of the given input string str and replaces relevant XML
   *          entities with their escaped counterparts.
   *          This code is adapted from http://www.mdawson.net/misc/xmlescape.php (Matson Dawson, 2009).
   *
   * \param[in] str Input string that has characters which need escaping.
   * \return Modified string with XML entities escaped.
   */
  virtual auto encodeXml(const GString& inputStr) -> GString {
    std::ostringstream tmpEncodeBuffer; // Used as a temporary string buffer

    // Create a for loop that uses an iterator to traverse the complete string
    for(GString::const_iterator iter = inputStr.begin(); iter < inputStr.end(); iter++) {
      // Get current character
      auto c = static_cast<GChar>(*iter);

      // Use a switch/case statement for the five XML entities
      switch(c) {
        case '"':
          tmpEncodeBuffer << "&quot;";
          break; // Replace double quotes
        case '&':
          tmpEncodeBuffer << "&amp;";
          break; // Replace ampersand
        case '\'':
          tmpEncodeBuffer << "&apos;";
          break; // Replace single quote
        case '<':
          tmpEncodeBuffer << "&lt;";
          break; // Replace less-than sign
        case '>':
          tmpEncodeBuffer << "&gt;";
          break; // Replace greater-than sign
        default:
          tmpEncodeBuffer << c; // By default just append current character
      }
    }

    // Return encoded stream as a string
    return tmpEncodeBuffer.str();
  }

  /**
   * \brief Creates an XML prefix using the domain id that is prepended to each message.
   * Makes use of an array attribute filled by the user to generate the XML string.
   *
   */
  virtual void createPrefixMessage() {
    // Create temporary stream
    std::ostringstream tmpStream;

    // Fill stream with formatted domain id
    tmpStream << "<m d=\"" << m_domainId << "\" ";

    for(auto& m_prefixAttribute : m_prefixAttributes) {
      tmpStream << m_prefixAttribute.first << "=\"" << m_prefixAttribute.second << "\" ";
    }

    tmpStream << ">";

    // Set prefix message to tmpBuffer string
    m_prefixMessage = tmpStream.str();

    // Reset buffer
    tmpStream.str("");
  }

  // todo: remove...
  /**
   * \brief Creates an XML suffix that is appended to each message.
   */
  virtual void createSuffixMessage() { m_suffixMessage = "</m>\n"; }


  /**
   * \brief Return an XML header that should written at the beginning of each log file.
   * \return The XML header.
   */
  virtual auto getXmlHeader() -> GString {
    using namespace std;

    static constexpr GInt maxNoChars = 1024;

    // Gets the current hostname
    std::array<GChar, maxNoChars> host{};
    gethostname(&host[0], maxNoChars - 1);
    host[maxNoChars - 1] = '\0';

    // Gets the current username
    GString user;

    passwd* p = getpwuid(getuid());
    if(p != nullptr) {
      user = GString(p->pw_name);
    } else {
      user = "n/a";
    }

    // Gets the current executionCommand
    stringstream executionCommand;
    executionCommand.str("");
    executionCommand << m_argv[0];
    for(GInt n = 1; n < m_argc; n++) {
      executionCommand << " " << m_argv[n];
    }

    // Create temporary buffer
    ostringstream tmpBuffer;

    // Write XML header information to buffer
    tmpBuffer << R"(<?xml version="1.0" standalone="yes" ?>\n)";
    tmpBuffer << R"(<root>\n)";
    tmpBuffer << R"(<meta name="noDomains" content=")" << m_noDomains << "\" />\n";
    tmpBuffer << R"(<meta name="dateCreation" content=")" << dateString() << "\" />\n";
    tmpBuffer << R"(<meta name="fileFormatVersion" content=")" << m_fileFormatVersion << "\" />\n";
    tmpBuffer << R"(<meta name="user" content=")" << user << "\" />\n";
    tmpBuffer << R"(<meta name="host" content=")" << host.data() << "\" />\n";
    tmpBuffer << R"(<meta name="dir" content=")" << getCWD() << "\" />\n";
    tmpBuffer << R"(<meta name="executionCommand" content=")" << executionCommand.str() << "\" />\n";
    tmpBuffer << R"(<meta name="revision" content=")" << XSTRINGIFY(PROJECT_VER) << "\" />\n";
    tmpBuffer << R"(<meta name="build" content=")" << XSTRINGIFY(COMPILER_NAME) << " " << XSTRINGIFY(BUILD_TYPE) << " ("
              << GString(XSTRINGIFY(COMPILER_VER)) << ")"
              << "\" />\n";


    // Return XML header
    return tmpBuffer.str();
  }

  /**
   * \brief Return an XML footer that should written at the end of each log file.
   * \return The XML footer.
   */
  virtual auto getXmlFooter() -> GString {
    // Create temporary buffer
    std::ostringstream tmpBuffer;

    // Write XML footer to buffer
    tmpBuffer << R"(<meta name="dateClosing" content=")" << dateString() << "\" />\n";
    tmpBuffer << "</root>\n";

    // Return XML footer
    return tmpBuffer.str();
  }

  virtual void flushBuffer() = 0;

  [[nodiscard]] auto inline domainId() const -> int { return m_domainId; }
  auto inline domainId() -> int& { return m_domainId; }
  [[nodiscard]] auto inline noDomains() const -> int { return m_noDomains; }
  auto inline noDomains() -> int& { return m_noDomains; }

  [[nodiscard]] auto inline prefixMessage() const -> const GString& { return m_prefixMessage; }
  [[nodiscard]] auto inline suffixMessage() const -> const GString& { return m_suffixMessage; }
  [[nodiscard]] auto inline minFlushSize() const -> GInt { return m_minFlushSize; }

 private:
  static constexpr GInt m_fileFormatVersion = 1; //!< File format version (increase this by one every time you make
  //!< changes that could affect postprocessing tools)
  GBool   m_rootOnly{false}; //!< Stores whether only the root domain writes a log file
  int     m_domainId{0};     //!< Contains the MPI rank (= domain id) of this process
  int     m_noDomains{1};    //!< Contains the MPI rank count (= number of domains)
  GInt    m_minFlushSize{0}; //!< Minimum length of the internal buffer before flushing
  GString m_prefixMessage;   //!< Stores the prefix that is prepended to each output
  GString m_suffixMessage;   //!< Stores the suffix that is appended to each output
  GInt    m_argc{};
  GChar** m_argv{};

  std::vector<std::pair<GString, GString>> m_prefixAttributes;
};


/**
 * \brief Customized buffer to facilitate of a regular physical file for each processor within an MPI communicator.
 * \details This class can be used as a regular string buffer, as it inherits from stringbuf. On flushing the
 *          buffer, the contents of the buffer are written to a file using an ofstream. This is mainly for cases
 *          where logging speed is crucial, as the implementation is very lightweight and since for each process
 *          an individual file is maintained. There is an option to use this buffer but to create only one file
 *          for the MPI root domain (see #m_rootOnlyHardwired).
 */
class Log_simpleFileBuffer : public Log_buffer {
 private:
  GBool              m_isOpen{false};            //!< Stores whether the file(s) were already opened
  GBool              m_rootOnlyHardwired{false}; //!< If true, only domain 0 opens and uses a file
  GString            m_filename;                 //!< Filename on disk
  std::ofstream      m_file;                     //!< File stream tied to physical file on disk
  MPI_Comm           m_mpiComm{};                //!< MPI communicator group
  std::ostringstream m_tmpBuffer;                //!< Temporary buffer to hold string until flushing

 protected:
  auto sync() -> int override;
  void flushBuffer() override;

 public:
  Log_simpleFileBuffer() = default;
  Log_simpleFileBuffer(const GString& filename, const GInt m_argc, GChar** m_argv, MPI_Comm mpiComm = MPI_COMM_WORLD,
                       GBool rootOnlyHardwired = false);
  ~Log_simpleFileBuffer() override { Log_simpleFileBuffer::close(); };

  Log_simpleFileBuffer(const Log_simpleFileBuffer&) = delete;
  Log_simpleFileBuffer(Log_simpleFileBuffer&&)      = delete;
  auto operator=(const Log_simpleFileBuffer&) -> Log_simpleFileBuffer& = delete;
  auto operator=(Log_simpleFileBuffer&&) -> Log_simpleFileBuffer& = delete;

  void open(const GString& filename, MPI_Comm mpiComm = MPI_COMM_WORLD, GBool rootOnlyHardwired = false);
  void close(GBool forceClose = false) override;
};

/**
 * \brief Base class for all Log<xyz> classes.
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
inline LogFile logger; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)


#endif // GRIDGENERATOR_LOG_H
