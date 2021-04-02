/*
  ESPFlash.h

  Copyright (c) 2020 Dale Giancono. All rights reserved..
  This file defines the ESPFlash class. It is an abstraction layer that 
  simplifies the storing of vectorised data in the filesystem on the 
  ESP8266 and ESP32. 

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef ESPFLASH_H_
#define ESPFLASH_H_

#include <SPIFFS.h>

template<class T>
class ESPFlash
{  
  public:
    ESPFlash(bool formatOnFail);
    /* Creates the ESPFlash instance. In practice it starts SPIFFS and gets the file name. */
    ESPFlash(const char* fileName, bool formatOnFail);
    /* Gets the number of elements of type T stored in the ESPFlash instance. 
     *  In practice it gets the ESPFlash file length in bytes using SPIFFS and divides the result 
     *  by the size of type T.
     */
    uint32_t length(void);
    
    /* Truncates the associated ESPFlash SPIFFS file and creates the first element containing type T. */
    /* Returns true if successful */
    bool set(const T data);
    /* Sets the element specified by the index parameter if it exists. */
    /* Returns true if successful */
    bool setElementAt(const T data, uint32_t index);
    /* Truncates the associated ESPFlash SPIFFS file and creates elements specified by Type T. */
    /* Returns true if successful */
    bool setElements(const T* data, uint32_t size);
    /* Truncates the associated ESPFlash SPIFFS file and creates elements specified by Type T. 
    that are stored using PROGMEM. */
    /* Returns true if successful */
    bool setElements_P(const T* data, uint32_t size);
    
    /* Opens the associated ESPFlash SPIFFS file and appends element containing type T. */
    /* Returns true if successful */
    bool append(const T data);
    /* Opens the associated ESPFlash SPIFFS file and appends elements specified by Type T. */
    /* Returns true if successful */
    bool appendElements(const T* data, uint32_t size);
    /* Opens the associated ESPFlash SPIFFS file and appends elements specified by Type T
    that are stored using PROGMEM. */
    /* Returns true if successful */
    bool appendElements_P(const T* data, uint32_t size);
    
    /* Gets the first element of type T stored in the associated ESPFlash SPIFFS file if it exists */
    T get(void);
    /* Gets the element of type T as specified by the index parameter if it exists. */
    T getElementAt(uint32_t index);
    /* Gets elements of Type T from the start of the file stored in the associated ESPFlash SPIFFS file if it exists */
    /* Returns the number of elements "got" */
    bool getFrontElements (T* data, uint32_t size);
    /* Gets elements of Type T from the end of the file stored in the associated ESPFlash SPIFFS file if it exists */
    /* Returns the number of elements "got" */
    bool getBackElements(T* data, uint32_t size);
    /* Deletes the associated ESPFlash SPIFFS file. */
    /* Returns true if successful */
    void clear(void);
    void setFileName(const char*);
    const char* getFileName(void);

  private:
    bool isInitialised = false;
    char fileName[32];  
    enum WRITE_MODE
    {
      OVERWRITE = 0,
      APPEND,
    };
    
    bool writeElement(const T data, enum WRITE_MODE mode);
    bool writeElements(const T* data, uint32_t size, enum WRITE_MODE mode);
    bool writeElements_P(const T* data, uint32_t size, enum WRITE_MODE mode);

};

template<class T> ESPFlash<T>::ESPFlash(bool formatOnFail)
{
  isInitialised = SPIFFS.begin(formatOnFail);
  return;
};

template<class T> ESPFlash<T>::ESPFlash(const char* fileName, bool formatOnFail)
{
  setFileName(fileName);
  isInitialised = SPIFFS.begin(formatOnFail);
  return;
};

template<class T> uint32_t ESPFlash<T>::length(void)
{
  File file;
  uint32_t sizeInBytes;
  uint32_t numberOfElements;

  numberOfElements = 0;
  if(isInitialised == true)
  {
    sizeInBytes = 0;
    /* Open the file specified by the filename with read privileges*/
    file = SPIFFS.open(this->fileName, "r");
    
    if(file)
    {
      /* Get the size of the file in bytes */
      sizeInBytes = file.size();
      
      /* Calculate number of elements by dividing by size of type T */ 
      numberOfElements = sizeInBytes/sizeof(T);
    }
    file.close();
  }
  return numberOfElements;
}

template<class T> bool ESPFlash<T>::set(const T data)
{
  return writeElement(data, WRITE_MODE::OVERWRITE); 
};

template<class T> bool ESPFlash<T>::setElements(const T* data, uint32_t size)
{
  return writeElements(data, size, WRITE_MODE::OVERWRITE);
};

template<class T> bool ESPFlash<T>::setElements_P(const T* data, uint32_t size)
{
  return writeElements_P(data, size, WRITE_MODE::OVERWRITE);
};

template<class T> bool ESPFlash<T>::append(const T data)
{
  return writeElement(data, WRITE_MODE::APPEND); 
};

template<class T> bool ESPFlash<T>::appendElements(const T* data, uint32_t size)
{
  return writeElements(data, size, WRITE_MODE::APPEND);
};

template<class T> bool ESPFlash<T>::appendElements_P(const T* data, uint32_t size)
{
  return writeElements_P(data, size, WRITE_MODE::APPEND);
};

template<class T> T ESPFlash<T>::get(void)
{
  T value;
  if(isInitialised == true)
  {
    File file = SPIFFS.open(this->fileName, "r");
    file.read((uint8_t*)&value, sizeof(T));
    file.close();
  }
  return value;
};

template<class T> T ESPFlash<T>::getElementAt(uint32_t index)
{
  T value;
  uint32_t bytesRead;
  File file;
  
  if(isInitialised == true)
  {  
    bytesRead = 0;
    value = (T)0; 
    if(index < length())
    {
      file = SPIFFS.open(this->fileName, "r");
      if(file)
      {
        file.seek(index*sizeof(T), SeekSet);
        bytesRead = file.read((uint8_t*)&value, sizeof(T));
        /* Check if successful by the number of bytes read */
        if(bytesRead != sizeof(T))
        {
          /* An error has occured */
          value = (T)0; 
        }
      }
      file.close();
    }
  }  
  return value;
};

template<class T> bool ESPFlash<T>::getFrontElements(T* data, uint32_t size)
{
  File file;
  uint32_t numberOfBytes;
  uint32_t bytesRead;
  bool success;
  
  success = false;
  if(isInitialised == true)
  {
    numberOfBytes = sizeof(T)*size;;
    bytesRead = 0;
    /* Open the file specified by the filename with read privileges*/
    file = SPIFFS.open(this->fileName, "r");
    if(size <= length())
    {
      if(file)
      {
        bytesRead = file.read((uint8_t*)data, numberOfBytes);
        /* Check if successful by the number of bytes read */
        if(bytesRead == numberOfBytes)
        {
          success = true;
        }
      }
    }
    file.close();
  }
  return success;
};

template<class T> bool ESPFlash<T>::getBackElements(T* data, uint32_t size)
{
  File file;
  uint32_t numberOfBytes;
  uint32_t firstElementIndex;
  uint32_t bytesRead;
  bool success;
  
  success = false;
  if(isInitialised == true)
  {
    bytesRead = 0;
    numberOfBytes = sizeof(T)*size;
    firstElementIndex = 0;
    /* Open the file specified by the filename with read privileges*/
    file = SPIFFS.open(this->fileName, "r");
    if(size < length())
    {
      firstElementIndex = file.size() - numberOfBytes;
      if(file)
      {
        file.seek(firstElementIndex, SeekSet);
        bytesRead = file.read((uint8_t*)data, numberOfBytes);
        file.close();
    
        /* Check if successful by the number of bytes read */
        if(bytesRead == numberOfBytes)
        {
          success = true;
        }
      }
    }
    file.close();
  }
  return success;
};

template<class T> void ESPFlash<T>::clear(void)
{  
  if(isInitialised == true)
  {
    SPIFFS.remove(this->fileName);
  }
  return;
};

template<class T> bool ESPFlash<T>::writeElement(const T data, WRITE_MODE mode)
{
    File file;
  uint32_t bytesWritten;
  bool success; 
  success = false;

  if(isInitialised == true)
  {
    bytesWritten = 0;
    /*open file specified by fileName with write privileges*/
    if(WRITE_MODE::OVERWRITE == mode)
    {
      file = SPIFFS.open(this->fileName, "w");
    }
    else if(ESPFlash::APPEND == mode)
    {
      file = SPIFFS.open(this->fileName, "a");
    }
    else
    {
      /* error */
      return success;
    }
    
    if(file)
    {
      /* Write type T to SPIFFS */
      bytesWritten = file.write((uint8_t*)&data, sizeof(T));
      /* Check if successful by the number of bytes written */
      /* If not successful, delete the file that was possibly 
       *  created to make sure data does not end up being
       *  corrupted */
      if(bytesWritten == sizeof(T))
      {
        success = true;
      }
      else
      {
        SPIFFS.remove(this->fileName);
      }
    }
    file.close();
  }
  return success;
};

template<class T> bool ESPFlash<T>::writeElements(const T* data, uint32_t size, enum WRITE_MODE mode)
{
  File file;
  uint32_t bytesWritten;
  uint32_t elementsSizeInBytes;
  bool success; 
  success = false;

  if(isInitialised == true)
  {
    bytesWritten = 0;
  
    /*open file specified by fileName with write privileges*/
    if(WRITE_MODE::OVERWRITE == mode)
    {
      file = SPIFFS.open(this->fileName, "w");
    }
    else if(WRITE_MODE::APPEND == mode)
    {
      file = SPIFFS.open(this->fileName, "a");
    }
    else
    {
      /* error */
      return success;
    }
  
    if(file)
    {
      elementsSizeInBytes = sizeof(T)*size;
      /* Write type T to SPIFFS */
      bytesWritten = file.write((uint8_t*)data, elementsSizeInBytes);
      /* Check if successful by the number of bytes written */
      /* If not successful, delete the file that was possibly 
       *  created to make sure data does not end up being
       *  corrupted */
      if(bytesWritten == elementsSizeInBytes)
      {
        success = true;
      }
      else
      {
        SPIFFS.remove(this->fileName);
      }
    }
    file.close();
  }
  return success;
};

template<class T> bool ESPFlash<T>::writeElements_P(const T* data, uint32_t size, enum WRITE_MODE mode)
{
  File file;
  uint32_t bytesWritten;
  uint32_t elementsSizeInBytes;
  bool success; 
  success = false;

  if(isInitialised == true)
  {
    elementsSizeInBytes = sizeof(T)*size;
    uint8_t buffer[elementsSizeInBytes];
    bytesWritten = 0;
    memcpy_P(buffer, data, elementsSizeInBytes);
  
  
    /*open file specified by fileName with write privileges*/
    if(WRITE_MODE::OVERWRITE == mode)
    {
      file = SPIFFS.open(this->fileName, "w");
    }
    else if(WRITE_MODE::APPEND == mode)
    {
      file = SPIFFS.open(this->fileName, "a");
    }
    else
    {
      /* error */
      return success;
    }
  
    if(file)
    {
      /* Write type T to SPIFFS */
      bytesWritten = file.write((uint8_t*)buffer, elementsSizeInBytes);
      /* Check if successful by the number of bytes written */
      /* If not successful, delete the file that was possibly 
       *  created to make sure data does not end up being
       *  corrupted */
      if(bytesWritten == elementsSizeInBytes)
      {
        success = true;
      }
      else
      {
        SPIFFS.remove(this->fileName);
      }
    }
    file.close();
  }
  return success;
};

template<class T> void ESPFlash<T>::setFileName(const char* fileName)
{
  if(strlen(fileName) < 31)
  {
    strcpy(this->fileName, fileName);
  }
  else
  {
    strncpy(this->fileName, fileName, 27);
    char* pch = strrchr(fileName, '.');
    strcpy(this->fileName+27, pch);
  }
  isInitialised = true;
  return;
}

template<class T> const char* ESPFlash<T>::getFileName(void)
{
  return this->fileName;
}

#endif /*ESPFLASHDATA_H_*/
