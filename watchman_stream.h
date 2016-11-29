/* Copyright 2014-present Facebook, Inc.
 * Licensed under the Apache License, Version 2.0 */
#ifndef WATCHMAN_STREAM_H
#define WATCHMAN_STREAM_H

// Very limited stream abstraction to make it easier to
// deal with portability between Windows and POSIX.

class watchman_event {
 public:
  virtual ~watchman_event() = default;
  virtual void notify() = 0;
  virtual bool testAndClear() = 0;
};

using w_evt_t = watchman_event*;

class watchman_stream {
 public:
  virtual ~watchman_stream() = default;
  virtual int read(void* buf, int size) = 0;
  virtual int write(const void* buf, int size) = 0;
  virtual w_evt_t getEvents() = 0;
  virtual void setNonBlock(bool nonBlock) = 0;
  virtual bool rewind() = 0;
  virtual bool shutdown() = 0;
  virtual bool peerIsOwner() = 0;
};
using w_stm_t = watchman_stream*;

struct watchman_event_poll {
  watchman_event* evt;
  bool ready;
};

// Make a event that can be manually signalled
std::unique_ptr<watchman_event> w_event_make(void);

// Go to sleep for up to timeoutms.
// Returns sooner if any of the watchman_event objects referenced
// in the array P are signalled
int w_poll_events(struct watchman_event_poll *p, int n, int timeoutms);

// Create a connected unix socket or a named pipe client stream
std::unique_ptr<watchman_stream> w_stm_connect(const char* path, int timeoutms);

int w_stm_read(w_stm_t stm, void *buf, int size);
int w_stm_write(w_stm_t stm, const void *buf, int size);
void w_stm_get_events(w_stm_t stm, w_evt_t *readable);
void w_stm_set_nonblock(w_stm_t stm, bool nonb);
bool w_stm_rewind(w_stm_t stm);
bool w_stm_shutdown(w_stm_t stm);
bool w_stm_peer_is_owner(w_stm_t stm);

w_stm_t w_stm_stdout(void);
w_stm_t w_stm_stdin(void);
#ifndef _WIN32
std::unique_ptr<watchman_stream> w_stm_connect_unix(
    const char* path,
    int timeoutms);
std::unique_ptr<watchman_stream> w_stm_fdopen(int fd);
std::unique_ptr<watchman_stream> w_stm_open(const char* path, int flags, ...);
int w_stm_fileno(w_stm_t stm);
#else
std::unique_ptr<watchman_stream> w_stm_connect_named_pipe(
    const char* path,
    int timeoutms);
std::unique_ptr<watchman_stream> w_stm_handleopen(HANDLE h);
std::unique_ptr<watchman_stream> w_stm_open(const char* path, int flags, ...);
HANDLE w_stm_handle(w_stm_t stm);
HANDLE w_handle_open(const char *path, int flags);
#endif

// Make a temporary file name and open it.
// Marks the file as CLOEXEC
std::unique_ptr<watchman_stream> w_mkstemp(char* templ);

#endif
