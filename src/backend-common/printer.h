#ifndef BACKEND_COMMON_PRINTER_H_
#define BACKEND_COMMON_PRINTER_H_

//------------------------------------------------------------------------------
// Physical World functions to connect/disconnect the Printer interface
//------------------------------------------------------------------------------

// Connect the printer to the given file.
// Notes:
// - The printer will first be disconnected if needed.
// - The data that will be send by ilo Konpu will be appended at the end of
//   the file. If the file doesn't exist, il will created.
// - Return true if success or false if the connection fails (ie the given file
//   cannot be opened or created), in which case the printer will remain
//   disconnected.
int KonpuConnectPrinterToFile(const char *filename);

// Connect the printer to the standard output stream
// Note: the printer will first be disconnected if needed.
void KonpuConnectPrinterToStdout(void);

// Connect it to the standard error stream.
// Note: the printer will first be disconnected if needed.
void KonpuConnectPrinterToStderr(void);

// Disconnect the printer
int KonpuDisconnectPrinter(void);

#endif //include guard
