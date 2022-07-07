#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-interfaces-global-init"
#ifndef GUI_LOG_H
#define GUI_LOG_H

#include "GL/glew.h"
#include <windows.h>
#include <fstream>
#include <iostream>

using std::ostream, std::ofstream, std::string;

class Log {
private:
    inline static int MAX_LOG_SIZE = 512;
    inline static auto out = &std::cout;
public:
    static void APIENTRY GLDebugMessageCallback(GLenum
                                                source,
                                                GLenum type, GLuint
                                                id,
                                                GLenum severity, GLsizei
                                                length,
                                                const GLchar *msg,
                                                const void *data
    );

    static void file(const string &filename);

    template<class ... Args>
    static void logf(const string &format, Args ... args) {
      char buf[MAX_LOG_SIZE];
      string newFmt = "[%s] " + format + "\n";

      // Get timestamp
      time_t ltime;
      ltime = time(nullptr);
      string timestamp(asctime(localtime(&ltime)));
      timestamp.pop_back(); /* Remove  */

      snprintf(buf, MAX_LOG_SIZE, newFmt.c_str(), timestamp.c_str(), args...);
      *out << buf;
      out->flush();
    }

    template<class ... Args>
    static void debugf(const string &format, Args ... args) {
#ifdef DEBUG
      logf(format, args...);
#endif
    }

    static void close();

};

#endif //GUI_LOG_H

#pragma clang diagnostic pop