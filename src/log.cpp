#include "log.h"
#include <sfcmm_common.h>
#include "config.h"
#include "macros.h"

#include <array>
#include <pwd.h>
#include <unistd.h>

using namespace std;

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
Log_simpleFileBuffer::Log_simpleFileBuffer(const GString& filename, const GInt argc, GChar** argv, MPI_Comm mpiComm,
                                           GBool rootOnlyHardwired)
  : Log_buffer(argc, argv) {
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
void Log_simpleFileBuffer::open(const GString& filename, MPI_Comm mpiComm, const GBool rootOnlyHardwired) {
  // Open file only if it was not yet done
  if(!m_isOpen) {
    // Set MPI communicator group
    m_mpiComm = mpiComm;

    // Get domain id and number of domains
    MPI_Comm_rank(m_mpiComm, &domainId());
    MPI_Comm_size(m_mpiComm, &noDomains());

    // Set whether only domain 0 should do any writing (including the creation of a file)
    m_rootOnlyHardwired = rootOnlyHardwired;

    // Only open the file if m_rootOnlyHardwired was not set as true. Otherwise the file state remains closed.
    if(!(m_rootOnlyHardwired && domainId() != 0)) {
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
void Log_simpleFileBuffer::close(const GBool forceClose) {
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
    m_tmpBuffer << prefixMessage() << encodeXml(str()) << suffixMessage();

    // Only write to file if current buffer length exceeds the minimum size for flushing
    if(m_tmpBuffer.str().length() >= static_cast<unsigned>(minFlushSize())) {
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
void LogFile::open(const GString& filename, const GBool rootOnlyHardwired, const GInt argc, GChar** argv, MPI_Comm mpiComm) {
  // Only open file if it was not yet opened
  if(!m_isOpen) {
    // Open a simple file
    buffer() = make_unique<Log_simpleFileBuffer>(filename, argc, argv, mpiComm, rootOnlyHardwired);

    // Associate the stream with the newly created buffer
    rdbuf(buffer().get());

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
void LogFile::close(const GBool forceClose) {
  // Only close file if was already opened
  if(m_isOpen) {
    buffer()->close(forceClose); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)

    // Delete internal buffer to prevent memory leaks
    buffer().reset();

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
auto LogFile::setRootOnly(const GBool rootOnly) -> GBool { return buffer()->setRootOnly(rootOnly); }

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
auto LogFile::setMinFlushSize(const GInt minFlushSize) -> GInt {
  if(m_isOpen) {
    return buffer()->setMinFlushSize(minFlushSize);
  }

  return 0;
}