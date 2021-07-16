#include "log.h"
#include "config.h"
#include "macros.h"
#include "sys.h"

#include <array>
#include <pwd.h>
#include <unistd.h>

using namespace std;

// todo: fix documentation
// todo: add tests

/** \brief Adds an attribute to the prefix of the XML string.
 * \author Andreas Lintermann, Sven Berger
 * \date 13.08.2012
 *
 * \param[in] att The attribute to add, consists of a pair of MStrings.
 * \return The location of the attribute in the vector of pairs.
 */
auto Log::addAttribute(const pair<GString, GString>& att) -> GInt {
  m_buffer->m_prefixAttributes.push_back(att);
  m_buffer->createPrefixMessage();
  return static_cast<GInt>(m_buffer->m_prefixAttributes.size() - 1);
}

/** \brief Erases an attribute from the prefix of the XML string.
 * \author Andreas Lintermann, Sven Berger
 * \date 13.08.2012
 *
 *  \param[in] attId The ID of the attribute to delete.
 */
void Log::eraseAttribute(GInt attId) {
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
void Log::modifyAttribute(GInt attId, const pair<GString, GString>& att) {
  m_buffer->m_prefixAttributes[attId] = att;
  m_buffer->createPrefixMessage();
}

/**
 * \brief Parses the string input and returns the string with XML entities escaped
 * \author Michael Schlottke, Sven Berger
 * \date April 2012
 * \details This method iterates over each character of the given input string str and replaces relevant XML
 *          entities with their escaped counterparts.
 *          This code is adapted from http://www.mdawson.net/misc/xmlescape.php (Matson Dawson, 2009).
 *
 * \param[in] str Input string that has characters which need escaping.
 * \return Modified string with XML entities escaped.
 */
auto Log_buffer::encodeXml(const GString& inputStr) -> GString {
  ostringstream tmpEncodeBuffer; // Used as a temporary string buffer

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
 * \author Michael Schlottke, Andreas Lintermann, Sven Berger
 * \date 13.08.2012
 *
 * Makes use of an array attribute filled by the user to gerenate the XML string.
 *
 */
inline void Log_buffer::createPrefixMessage() {
  // Create temporary stream
  ostringstream tmpStream;

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

/**
 * \brief Creates an XML suffix that is appended to each message.
 * \author Michael Schlottke, Sven Berger
 * \date April 2012
 */
inline void Log_buffer::createSuffixMessage() { m_suffixMessage = "</m>\n"; }


/**
 * \brief Sets interal state of whether only the root domain (rank 0) should write to file.
 * \author Michael Schlottke, Sven Berger
 * \date June 2012
 *
 * \params[in] rootOnly If true, only rank 0 of the specified MPI communicator writes to file.
 * \return The previous internal state (may be stored to return to the previous behavior).
 */
inline auto Log_buffer::setRootOnly(GBool rootOnly) -> GBool {
  GBool previousValue = m_rootOnly;
  m_rootOnly          = rootOnly;
  return previousValue;
}

/**
 * \brief Sets the minimum buffer length that has to be reached before the buffer is flushed.
 * \author Michael Schlottke, Sven Berger
 * \date June 2012
 *
 * \params[in] minFlushSize Minimum buffer length.
 * \return The previous value of the minimum flush size.
 */
inline auto Log_buffer::setMinFlushSize(GInt minFlushSize) -> GInt {
  GInt previousValue = m_minFlushSize;
  m_minFlushSize     = minFlushSize;
  return previousValue;
}

/**
 * \brief Return an XML header that should written at the beginning of each log file.
 * \author Michael Schlottke, Sven Berger
 * \date June 2012
 * \return The XML header.
 */
auto Log_buffer::getXmlHeader() -> GString {
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

  // Gets the current directory
  std::array<GChar, maxNoChars> dir{};
  if(getcwd(&dir[0], maxNoChars - 1) == nullptr) {
    TERMM(-1, "Invalid path!");
  }
  dir[maxNoChars - 1] = '\0';

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
  tmpBuffer << R"(<meta name="dir" content=")" << dir.data() << "\" />\n";
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
 * \author Sven Berger
 * \date July 2021
 *
 * \return The XML footer.
 */
auto Log_buffer::getXmlFooter() -> GString {
  // Create temporary buffer
  ostringstream tmpBuffer;

  // Write XML footer to buffer
  tmpBuffer << R"(<meta name="dateClosing" content=")" << dateString() << "\" />\n";
  tmpBuffer << "</root>\n";

  // Return XML footer
  return tmpBuffer.str();
}

/**
 * \brief This constructor yields a new instance that can immediately be used to write messages to a regular file.
 * \author Michael Schlottke, Sven Berger
 * \date June 2012
 * \details Internally, this constructor just passes the parameters to open() (see open() for more details).
 *
 * \param[in] filename          Filename that should be used for the file.
 * \param[in] mpiComm           MPI communicator which is used to determine rank/domain information.
 * \param[in] rootOnlyHardwired If true, only rank 0 creates a file and writes to it. On all other processors, no
 *                              file is opened and at each flushing of the buffer, the buffer content is discarded.
 */
Log_simpleFileBuffer::Log_simpleFileBuffer(const GString& filename, GInt argc, GChar** argv, MPI_Comm mpiComm,
                                           GBool rootOnlyHardwired)
  : Log_buffer(argc, argv), m_filename(), m_file() {
  open(filename, mpiComm, rootOnlyHardwired);
}

/**
 * \brief Initialization of the file I/O environment.
 * \author Michael Schlottke, Sven Berger
 * \date June 2012
 * \details After a successful call to this method the file stream is ready to use. Any previous information
 *          written to the buffer is lost when open is called. This function creates a new file as specified in
 *          filename (deleting any existing files with the same name), and creates the XML prefixes and suffixes
 *          used for each message. It also writes the necessary XML header information to the file.
 *
 * \param[in] filename          Filename that should be used for the file.
 * \param[in] mpiComm           MPI communicator which is used to determine rank/domain information.
 * \param[in] rootOnlyHardwired If true, only rank 0 creates a file and writes to it. On all other processors, no
 *                              file is opened and at each flushing of the buffer, the buffer content is discarded.
 */
void Log_simpleFileBuffer::open(const GString& filename, MPI_Comm mpiComm, GBool rootOnlyHardwired) {
  // Open file only if it was not yet done
  if(!m_isOpen) {
    // Set MPI communicator group
    m_mpiComm = mpiComm;

    // Get domain id and number of domains
    MPI_Comm_rank(m_mpiComm, &m_domainId);
    MPI_Comm_size(m_mpiComm, &m_noDomains);

    // Set whether only domain 0 should do any writing (including the creation of a file)
    m_rootOnlyHardwired = rootOnlyHardwired;

    // Only open the file if m_rootOnlyHardwired was not set as true. Otherwise the file state remains closed.
    if(!(m_rootOnlyHardwired && m_domainId != 0)) {
      // Set filename
      m_filename = filename;

      // Open file
      m_file.open(m_filename.c_str());

      // Clear internal buffer in order to dismiss any previous input
      str("");

      // Create prefix and suffix messages
      createPrefixMessage();
      createSuffixMessage();

      // Write root and meta information to file
      m_file << getXmlHeader() << flush;

      // Set state variable
      m_isOpen = true;
    }
  }
}

/**
 * \brief Closes the file.
 * \author Michael Schlottke, Sven Berger
 * \date June 2012
 * \details Any subsequent write statements to the file stream are discarded after this method is called. After
 *          close() is called, an XML footer is written to the file. Then the file is closed.
 */
void Log_simpleFileBuffer::close(GBool forceClose) {
  // forceClose is not needed here (only kept for interface consistency reasons)
  static_cast<void>(forceClose);

  // Only close file if was opened before
  if(m_isOpen) {
    // Force flushing of the internal buffer
    flushBuffer();

    // Write XML footer to file and flush stream
    m_file << getXmlFooter() << flush;

    // Close file stream
    m_file.close();

    // Set state variable
    m_isOpen = false;
  }
}

/**
 * \brief Flushes the buffer by writing the contents to the file.
 * \author Michael Schlottke, Sven Berger
 * \date June 2012
 * \details Sync is called automatically when an "endl" is sent to the stream. At first the buffer content is
 *          wrapped in the prefix and suffix messages, then the entire string is written to the file by calling
 *          flushBuffer(). Finally, the internal buffers are reset.
 *
 * \return Zero by default.
 */
auto Log_simpleFileBuffer::sync() -> int {
  // Only write if the file was already opened
  if(m_isOpen) {
    // Create formatted string, escape any XML entities in the message, and save to temporary buffer
    m_tmpBuffer << m_prefixMessage << encodeXml(str()) << m_suffixMessage;

    // Only write to file if current buffer length exceeds the minimum size for flushing
    if(m_tmpBuffer.str().length() >= static_cast<unsigned>(m_minFlushSize)) {
      // Write the string to the file and flush the stream
      m_file << m_tmpBuffer.str() << flush;

      // Reset temporary buffer
      m_tmpBuffer.str("");
    }
  }

  // Reset internal buffer
  str("");

  // Default return value for sync()
  return 0;
}

/**
 * \brief Flushes the buffer by writing the contents to the file.
 * \author Michael Schlottke, Sven Berger
 * \date June 2012
 * \details Sync is called automatically when an "endl" is sent to the stream. At first the buffer content is
 *          wrapped in the prefix and suffix messages, then the entire string is written to the file. Finally,
 *          the internal buffers are reset.
 *
 * \return Zero by default.
 */
inline void Log_simpleFileBuffer::flushBuffer() {
  // Only write if the file was already opened
  if(m_isOpen) {
    // Write the string to the file and flush the stream
    m_file << m_tmpBuffer.str() << flush;

    // Reset temporary buffer
    m_tmpBuffer.str("");
  }
}

/**
 * \brief Constructor creates LogFile and calls ostream constructor with reference to it.
 * \author Michael Schlottke, Sven Berger
 * \date April 2012
 * \details When this constructor is used, the stream is immediately ready to use. For information about the paramters,
 *          please have a look at Log*Buffer::open.
 *
 * \param[in] filename Name of the file to open.
 * \param[in] mpiComm MPI communicator for which to open the file.
 */
LogFile::LogFile(const GString& filename, MPI_Comm mpiComm, GBool rootOnlyHardwired) {
  open(filename, rootOnlyHardwired, 0, nullptr, mpiComm);
}

/**
 * \brief Opens a file by passing the parameters to Log_<xyz>FileBuffer::open(...).
 * \author Michael Schlottke, Sven Berger
 * \date April 2012
 * \details The method then creates a new internal buffer anbd passes along the parameters.
 *
 * \param[in] filename Name of the file to open.
 * \param[in] mpiComm MPI communicator for which to open the file.
 * \param[in] rootOnlyHardwired If true, only rank 0 creates a file and writes to it. On all other processors, no
 *                              file is opened and at each flushing of the buffer, the buffer content is discarded.
 */
void LogFile::open(const GString& filename, GBool rootOnlyHardwired, GInt argc, GChar** argv, MPI_Comm mpiComm) {
  // Only open file if it was not yet opened
  if(!m_isOpen) {
    // Open a simple file
    m_buffer = new Log_simpleFileBuffer(filename, argc, argv, mpiComm, rootOnlyHardwired);

    // Associate the stream with the newly created buffer
    rdbuf(m_buffer);

    // Set state variable
    m_isOpen = true;
  }
}

/**
 * \brief Pass the close call to the respective internal buffer.
 * \author Michael Schlottke, Sven Berger
 * \date June 2012
 * \details All attempts to write to the stream after closing it will be discarded.
 */
void LogFile::close(GBool forceClose) {
  // Only close file if was already opened
  if(m_isOpen) {
    static_cast<Log_simpleFileBuffer*>(m_buffer)->close(forceClose); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)

    // Delete internal buffer to prevent memory leaks
    delete m_buffer;

    // Set state variable
    m_isOpen = false;
  }
}

/**
 * \brief Sets interal state of whether only the root domain (rank 0) should write to file.
 * \author Michael Schlottke, Sven Berger
 * \date June 2012
 *
 * \params[in] rootOnly If true, only rank 0 of the specified MPI communicator writes to file.
 * \return The previous internal state (may be stored to return to the previous behavior).
 */
auto LogFile::setRootOnly(GBool rootOnly) -> GBool { return m_buffer->setRootOnly(rootOnly); }

/**
 * \brief Sets the minimum buffer length that has to be reached before the buffer is flushed.
 * \author Michael Schlottke, Sven Berger
 * \date June 2012
 * \details Flushing the buffer means that the contents of the buffer in memory is written to the file. If the
 *          file stream was not opened yet, this method just returns 0 and does nothing else.
 *
 * \params[in] minFlushSize Minimum buffer length.
 * \return The previous value of the minimum flush size.
 */
auto LogFile::setMinFlushSize(GInt minFlushSize) -> GInt {
  if(m_isOpen) {
    return m_buffer->setMinFlushSize(minFlushSize);
  }

  return 0;
}