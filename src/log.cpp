#include "log.h"
#include <cstdio>
#include <string>
#include <ctime>

#include <windows.h>
#include <DbgHelp.h>

#include <cstdio>
#include <cstdlib>
#include <sstream>

string stack_trace() {

  HANDLE process = GetCurrentProcess();
  HANDLE thread = GetCurrentThread();

  CONTEXT context;
  memset(&context, 0, sizeof(CONTEXT));
  context.ContextFlags = CONTEXT_FULL;
  RtlCaptureContext(&context);

  SymInitialize(process, NULL, TRUE);

  DWORD image;
  STACKFRAME64 stackframe;
  ZeroMemory(&stackframe, sizeof(STACKFRAME64));

#ifdef _M_IX86
  image = IMAGE_FILE_MACHINE_I386;
  stackframe.AddrPC.Offset = context.Eip;
  stackframe.AddrPC.Mode = AddrModeFlat;
  stackframe.AddrFrame.Offset = context.Ebp;
  stackframe.AddrFrame.Mode = AddrModeFlat;
  stackframe.AddrStack.Offset = context.Esp;
  stackframe.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
  image = IMAGE_FILE_MACHINE_AMD64;
  stackframe.AddrPC.Offset = context.Rip;
  stackframe.AddrPC.Mode = AddrModeFlat;
  stackframe.AddrFrame.Offset = context.Rsp;
  stackframe.AddrFrame.Mode = AddrModeFlat;
  stackframe.AddrStack.Offset = context.Rsp;
  stackframe.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
  image = IMAGE_FILE_MACHINE_IA64;
  stackframe.AddrPC.Offset = context.StIIP;
  stackframe.AddrPC.Mode = AddrModeFlat;
  stackframe.AddrFrame.Offset = context.IntSp;
  stackframe.AddrFrame.Mode = AddrModeFlat;
  stackframe.AddrBStore.Offset = context.RsBSP;
  stackframe.AddrBStore.Mode = AddrModeFlat;
  stackframe.AddrStack.Offset = context.IntSp;
  stackframe.AddrStack.Mode = AddrModeFlat;
#endif

  std::stringstream trace;
  for (int i = 0; i < 25; i++) {

    BOOL result = StackWalk64(
            image, process, thread,
            &stackframe, &context, nullptr,
            SymFunctionTableAccess64, SymGetModuleBase64, nullptr);

    if (!result) { break; }

    char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
    auto symbol = (PSYMBOL_INFO) buffer;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = MAX_SYM_NAME;
    DWORD64 displacement = 0;
    if (SymFromAddr(process, stackframe.AddrPC.Offset, &displacement, symbol)) {
      trace << "[" << i << "] " << symbol->Name << "\n";
    } else {
      trace << "[" << i << "] ???\n";
    }

  }

  SymCleanup(process);

  return trace.str();

}

void APIENTRY Log::GLDebugMessageCallback(GLenum
                                          source,
                                          GLenum type, GLuint
                                          id,
                                          GLenum severity, GLsizei
                                          length,
                                          const GLchar *msg,
                                          const void *data
) {
  std::string _source;
  std::string _type;
  std::string _severity;

  // Suppress some useless warnings
  switch (id) {
    case 0x2: // NVIDIA: "shader will be recompiled due to GL state mismatches"
      return;
    default:
      break;
  }

  switch (source) {
    case GL_DEBUG_SOURCE_API:
      _source = "API";
      break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      _source = "WINDOW SYSTEM";
      break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      _source = "SHADER COMPILER";
      break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
      _source = "THIRD PARTY";
      break;

    case GL_DEBUG_SOURCE_APPLICATION:
      _source = "APPLICATION";
      break;

    case GL_DEBUG_SOURCE_OTHER:
    default:
      _source = "UNKNOWN";
      break;
  }

  switch (type) {
    case GL_DEBUG_TYPE_ERROR:
      _type = "ERROR";
      break;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      _type = "DEPRECATED BEHAVIOR";
      break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      _type = "UDEFINED BEHAVIOR";
      break;

    case GL_DEBUG_TYPE_PORTABILITY:
      _type = "PORTABILITY";
      break;

    case GL_DEBUG_TYPE_PERFORMANCE:
      _type = "PERFORMANCE";
      break;

    case GL_DEBUG_TYPE_OTHER:
      _type = "OTHER";
      break;

    case GL_DEBUG_TYPE_MARKER:
      _type = "MARKER";
      break;

    default:
      _type = "UNKNOWN";
      break;
  }

  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
      _severity = "HIGH";
      break;

    case GL_DEBUG_SEVERITY_MEDIUM:
      _severity = "MEDIUM";
      break;

    case GL_DEBUG_SEVERITY_LOW:
      _severity = "LOW";
      break;

    case GL_DEBUG_SEVERITY_NOTIFICATION:
      _severity = "NOTIFICATION";
      break;

    default:
      _severity = "UNKNOWN";
      break;
  }

  logf("%d: %s of %s severity, raised from %s: %s",
       id, _type.c_str(), _severity.c_str(), _source.c_str(), msg);
  logf("%s %d", data, length);

  string trace = stack_trace();
  logf("TRACE: %s", trace.c_str());
#ifdef DEBUG
  if(_severity == "HIGH")
    __debugbreak();
#endif
}

void Log::file(const string &filename) {
  if (out != &std::cout) {
    delete out;
  }
  out = new ofstream(filename, ofstream::out | ofstream::trunc);
}

void Log::close() {
  if (out != &std::cout) {
    auto outFile = (ofstream *) out;
    outFile->flush();
    outFile->close();
    delete out;
  }
}
