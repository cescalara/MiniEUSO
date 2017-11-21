#ifndef _SYNCHRONISED_FILE_H
#define _SYNCHRONISED_FILE_H

#include <mutex>

#include "log.h"

/* handles asynchronous writing to file from multiple threads */
class SynchronisedFile {
public:
  SynchronisedFile(std::string path); 
  ~SynchronisedFile();
  template <class GenericType>
  size_t Write(GenericType payload);  
  
private:
  std::string _path;
  std::mutex _writerMutex;
  FILE * _ptr_to_file;
};


class Writer {
public:
  Writer(std::shared_ptr<SynchronisedFile> sf);
  
  template <class GenericType>
  void WriteToSynchFile(GenericType payload); 
  
private:
  std::shared_ptr<SynchronisedFile> _sf;
};

#endif
/* _SYNCHRONISED_FILE_H */
