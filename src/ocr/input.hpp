/**
 * @file input.hpp
 *
 * @todo Add description
 *
 * @author Matthew Rodusek (matthew.rodusek@gmail.com)
 * @date   Oct 29, 2015
 *
 */

/*
 * Change Log:
 *
 * Oct 29, 2015: 
 * - input.hpp created
 */
#ifndef OCR_INPUT_HPP_
#define OCR_INPUT_HPP_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif

#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <stdlib.h>
namespace ocr{

  ///
  /// @brief Clear the system screen
  ///
  /// I hate having to do it using system() calls, but it's the simplest way
  /// for a software project on short notice, instead of using curses or
  /// windows api.
  ///
  inline void clear(){
    if(system("cls")) system("clear");
  }

  template<typename T>
  struct input{
    static T get( const char* prompt, const char* error_message ){
      std::string line;
      T result;
      std::cout << prompt;
      while(std::getline(std::cin, line)){
        std::stringstream ss(line);
        if (ss >> result){
          if (ss.eof()) break;
        }
        std::cout << error_message << '\n';
        std::cout << prompt;
      }
      return result;
    }
  };

  template<>
  struct input<std::string>{
    static std::string get( const char* prompt, const char* error_message ){
      std::string line;

      std::cout << prompt;
      while(std::getline(std::cin,line)){
        if(!line.empty()) break;
        std::cout << error_message << '\n';
        std::cout << prompt;
      }
      return line;
    }
  };

  ///
  /// @brief Get integer input from standard in
  ///
  /// @param prompt        the prompt to print to the user
  /// @param error_message the error message to display on error
  /// @return the integer entered
  ///
  inline int get_int_input( const char* prompt, const char* error_message ){
    return input<int>::get(prompt,error_message);
  }

  ///
  /// @brief Get float input from standard in
  ///
  /// @param prompt        the prompt to print to the user
  /// @param error_message the error message to display on error
  /// @return the float entered
  ///
  inline float get_float_input( const char* prompt, const char* error_message ){
    return input<float>::get(prompt,error_message);
  }

  ///
  /// @brief Get string input from standard in
  ///
  /// @param prompt        the prompt to print to the user
  /// @param error_message the error message to display on error
  /// @return the string entered
  ///
  inline std::string get_string_input( const char* prompt, const char* error_message ){
    return input<std::string>::get(prompt,error_message);
  }

  ///
  /// @brief Get any input from standard in, up until enter is pressed
  ///
  /// @param prompt        the prompt to print to the user
  ///
  inline void get_any_input( const char* prompt ){
    std::string garbage;
    std::cout << prompt;
    std::getline(std::cin,garbage);
  }

}
#endif /* OCR_INPUT_HPP_ */
