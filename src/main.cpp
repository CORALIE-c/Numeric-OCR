/**
 * @file main.cpp
 *
 * @todo Add description
 *
 * @author Matthew Rodusek (matthew.rodusek@gmail.com)
 * @date   Sep 21, 2015
 *
 */

// Purely syntactical stuff
#define COMPILED_MONTH \
  (\
    __DATE__ [2] == 'n' ? (__DATE__ [1] == 'a' ? 1 : 6) \
  : __DATE__ [2] == 'b' ? 2 \
  : __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M' ? 3 : 4) \
  : __DATE__ [2] == 'y' ? 5 \
  : __DATE__ [2] == 'l' ? 7 \
  : __DATE__ [2] == 'g' ? 8 \
  : __DATE__ [2] == 'p' ? 9 \
  : __DATE__ [2] == 't' ? 10 \
  : __DATE__ [2] == 'v' ? 11 \
  : 12\
  )



#define VERSION(maj,min,rev) #maj "." #min "." #rev

#define OCR_VERSION VERSION(1,0,0)

// RapidJSON for loading/storing JSON elements
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

// For traversing directories
#include <dirent.h>

// Data structures
#include <list>
#include <vector>
#include <string>

// Standard Libraries (writing/math)
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <iomanip>

#ifndef M_PI
#  define M_PI 3.14159265359
#endif

// Image Processing Libraries
#include "ocr/base_types.hpp"
#include "ocr/BMP_Loader.hpp"
#include "ocr/Image.hpp"
#include "ocr/Kernel_Image_Operator.hpp"
#include "ocr/input.hpp"
#include "ocr/Feature_Loader.hpp"
#include "ocr/Feature_Database.hpp"
//----------------------------------------------------------------------------
// Types
//----------------------------------------------------------------------------

//
enum menu_type{
  menu_main,
  menu_filters_and_thinning,
  menu_vector_recognition,
  menu_utilities,
  menu_exit
};
typedef void (*menu_ptr)();
typedef std::pair<std::string, menu_ptr> menu_op;
typedef std::pair<menu_ptr, menu_op>     menu_tx;

typedef std::vector<std::pair<std::string,ocr::Kernel_Image_Operator*>> kernel_collection;

//----------------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------------

const int X_DIVS = 5;
const int Y_DIVS = 5;

//----------------------------------------------------------------------------
// Globals
//----------------------------------------------------------------------------

kernel_collection             g_kernels;
ocr::feature_collection  g_scanned_image_features;
ocr::boundary_collection g_scanned_image_boundaries;
ocr::Image*              g_scanned_image;
ocr::Feature_Database    g_feature_db;

//----------------------------------------------------------------------------
// Prototypes
//----------------------------------------------------------------------------

// Basic Menu
void display_menu( int );

// Phase I
void display_filters( void );
void load_filters( void );
void view_filters( void );
void generate_gaussian_filter( void );

// Phase II
void generate_feature_database( void );
void load_feature_database_from_file( const char* );
void load_feature_database( void );
void scan_for_features( void );

// Phase III
void analyze_features( void );

// Utilities
void convert_color_to_grayscale( void );
void convert_grayscale_to_binary_static( void );
void convert_grayscale_to_binary_adaptive( void );

//----------------------------------------------------------------------------
// Definitions
//----------------------------------------------------------------------------

void display_menu( int menu ){

  std::ostringstream version;
  version << OCR_VERSION << "-"
          << std::string(__DATE__,4,2)  // Day
          << std::dec << std::setfill('0') << std::setw(2) << COMPILED_MONTH // Month
          << std::string(__DATE__,7,4); // Year


  std::cout <<
    " __  __       _   _     _           _    \n"
    "|  \\/  |     | | | |   | |         | |    \n"
    "|      | __ _| |_| |_  | |     __ _| |__  \n"
    "| |\\/| |/ _` | __| __| | |    / _` | '_ \\ \n"
    "| |  | | (_| | |_| |_  | |___| (_| | |_) |\n"
    "|_|  |_|\\__,_|\\__|\\__| |______\\__,_|_.__/ \n";

  std::cout << "CP467: Image Processing\n"
               "Image Recognition Software\n"
               "Version " << version.str() << "\n"
               "Compiled " __DATE__ " " __TIME__ "\n";
  switch(menu){
  case menu_main: // title
    std::cout << "==[ Main Menu ]====================================\n"
                 "1 - Phase 1: Filters & Thinning\n"
                 "2 - Phase 2 & 3: Vectors and Recognition\n"
                 "3 - Utilities\n"
                 "0 - exit\n"
                 "---------------------------------------------------\n";
    break;
  case menu_filters_and_thinning:
    std::cout << "==[ Phase 1 | Filters & Thinning ]=================\n"
                 "1 - Load filter\n"
                 "2 - View filters\n"
                 "3 - Apply Filter\n"
                 "4 - Construct Gaussian Filter\n"
                 "5 - ZS Thinning Algorithm\n"
                 "0 - Return to main menu\n"
                 "---------------------------------------------------\n";
    break;
  case menu_vector_recognition:
    std::cout << "==[ Phase 2 & 3 | Feature Vector ]=================\n"
              << "1 - Generate feature database\n"
              << "2 - Load feature database\n"
              << "3 - Scan image for features\n"
              << "4 - Analyze features\n"
              << "0 - Return to main menu\n"
              << "---------------------------------------------------\n";
    break;
  case menu_utilities:
    std::cout << "==[ Utilities ]====================================\n"
              << "1 - Convert color image to grayscale\n"
              << "2 - Convert Grayscale->Binary (Static Threshold)\n"
              << "3 - Convert Grayscale->Binary (Adaptive Threshold)\n"
              << "0 - Return to main menu\n"
              << "---------------------------------------------------\n";
    break;

  default:
    break;
  };
}

bool string_ends_with( const std::string& str, const std::string& sub ){
  if (str.length() >= sub.length()) {
    return (0 == str.compare(str.length() - sub.length(), sub.length(), sub));
  }else {
    return false;
  }
}

namespace ocr{

  void zs_thinning(){

    std::string infile  = ocr::get_string_input("Enter input file: ", "Error, invalid input");

    ocr::Image* image;
    int status = ocr::load_bmp_image_binary( infile.c_str(), &image );

    if( status != ocr::IS_SUCCESS ){
      std::cout << "Error loading input file\n";
      ocr::get_any_input("Press enter to continue...\n");
      return;
    }

    std::string outfile = ocr::get_string_input("Enter output file: ", "Error, invalid input");

    //-------------------=-----------------------------------------------------

    Image active_buffer = Image(*image);

    ocr::destroy_image( &image );

    typedef std::vector<std::pair<int,int>> marked_collection;

    marked_collection marked;

    //-------------------=-----------------------------------------------------

    bool deletion_made = false;
    bool prime_pass = false;
    do{
      deletion_made = false;
      for( size_t x = 0; x < active_buffer.width(); ++x ){
        for( size_t y = 0; y < active_buffer.height(); ++y ){

          // Only interested if the pixel is already set
          if( active_buffer.at_binary(x,y) ){
            ubyte p2 = active_buffer.at_binary( x  , y-1 );
            ubyte p3 = active_buffer.at_binary( x+1, y-1 );
            ubyte p4 = active_buffer.at_binary( x+1, y   );
            ubyte p5 = active_buffer.at_binary( x+1, y+1 );
            ubyte p6 = active_buffer.at_binary( x  , y+1 );
            ubyte p7 = active_buffer.at_binary( x-1, y+1 );
            ubyte p8 = active_buffer.at_binary( x-1, y   );
            ubyte p9 = active_buffer.at_binary( x-1, y-1 );

            ubyte a = (int) (p2 < p3) + (int) (p3 < p4) + (int) (p4 < p5) +
                      (int) (p5 < p6) + (int) (p6 < p7) + (int) (p7 < p8) +
                      (int) (p8 < p9) + (int) (p9 < p2);
            ubyte b = p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;

            // Conditions 1 and 2:
            // (a) 2 <= B(p1) <= 6
            // (b) A(p1) = 1
            if((2 <= b && b <= 6) && (a == 1)){
              // The changes are made to the opposite image
              // (c) P2*P4*P6 = 0
              // (d) P4*P6*P8 = 0
              if(!prime_pass){
                if( (p2*p4*p6 == 0) && (p4*p6*p8 == 0) ){
                  marked.push_back(std::make_pair(x,y));
                  deletion_made = true;
                }
              // (c') P2*P4*P8 = 0
              // (d') P2*P6*P8 = 0
              }else{
                if( (p2*p4*p8 == 0) && (p2*p6*p8 == 0) ){
                  marked.push_back(std::make_pair(x,y));
                  deletion_made = true;
                }
              }
            }
          }
        } // end for
      } // end for

      // Update the image
      marked_collection::iterator iter = marked.begin();
      for( ;iter!=marked.end(); ++iter ){
        active_buffer.set_binary(iter->first, iter->second,0);
      }
      marked.clear();

      // invert the current pass type
      prime_pass = !prime_pass;
    }while(deletion_made);

    if( !ocr::save_bmp_image( outfile.c_str(), &active_buffer ) ){
      std::cout << "Error saving output file\n";
      ocr::get_any_input("Press enter to continue...\n");
      return;
    }

    std::cout << "File successfully output to '" << outfile << "'.\n";
    ocr::get_any_input("Press enter to continue...\n");
  }


  //-----------------------------------------------------------------------------
  // Kernels: Load from File/Directory
  //-----------------------------------------------------------------------------

  bool load_kernel_from_json( rapidjson::Value& kernel_def, kernel_collection& collection ){
    static int i = 0;

    std::string name;
    std::vector<std::vector<float>> kern;

    // Handle the object
    if(!kernel_def.IsObject()){
      std::cout << "   [ ] unable to load filter\n";
      std::cout << "       ...filter does not contain root object.\n";
      return false;
    }

    // Handle the name of the kernel
    if(!kernel_def.HasMember("name")){
      name = "untitled filter " + std::to_string(++i);
    }else{
      rapidjson::Value& name_attribute = kernel_def["name"];
      if(!name_attribute.IsString()){
        std::cout << "   [ ] unable to load filter\n";
        std::cout << "       ...attribute 'name' is not a string.\n";
        return false;
      }else{
        name = name_attribute.GetString();
      }
    }

    // Handle the 2D Kernel
    if(!kernel_def.HasMember("kernel")){
      std::cout << "   [ ] unable to load filter '" << name << "'.\n";
      std::cout << "       ...filter does not contain 'kernel' attribute.\n";
      return false;
    }
    rapidjson::Value& kernel_attribute = kernel_def["kernel"];

    if(!kernel_attribute.IsArray()){
      std::cout << "   [ ] unable to load filter '" << name << "'.\n";
      std::cout << "       ...attribute 'kernel' is not an array.\n";
      return false;
    }

    const int rows = (int) kernel_attribute.Size();
    int prev_cols = -1;

    for( int i = 0; i < rows; ++i ){

      if(!kernel_attribute[i].IsArray()){
        std::cout << "   [ ] unable to load filter '" << name << "'.\n";
        std::cout << "       ...kernel is not 2D array.\n";
        return false;
      }

      int cols = (int) kernel_attribute[i].Size();

      if(prev_cols != -1 && prev_cols != cols ){
        std::cout << "   [ ] unable to load filter '" << name << "'.\n";
        std::cout << "       ...2D kernel is not consistent.\n";
        return false;
      }

      kern.push_back(std::vector<float>());
      for( int j = 0; j < cols; ++j ){
        if(!kernel_attribute[i][j].IsDouble() && !kernel_attribute[i][j].IsInt()){
          std::cout << "   [ ] unable to load filter '" << name << "'.\n";
          std::cout << "       ...kernel values are invalid.\n";
          return false;
        }
        if(kernel_attribute[i][j].IsDouble()){
          kern[i].push_back( (float) kernel_attribute[i][j].GetDouble() );
        }else{
          kern[i].push_back( (float) kernel_attribute[i][j].GetInt() );
        }
      }
    }

    for( size_t i=1; i < kern.size(); ++i ){
      if(kern[i-1].size() != kern[i].size()){
        std::cout << "   [ ] unable to load filter '" << name << "'.\n";
        std::cout << "       ...kernel matrix is not consistent.\n";
        return false;
      }
    }

    // Push the new kernel image operator down
    collection.push_back(
      std::make_pair( name, new ocr::Kernel_Image_Operator(kern) )
    );

    std::cout << "   [X] loaded filter '" << name << "'.\n";
    return true;
  }

  //-----------------------------------------------------------------------------

  int load_kernel_from_file( const std::string& filename, kernel_collection& collection ){
    std::ifstream file;
    file.open( filename, std::ios::in );

    // Make sure file is opened and good
    if(!(file && file.good())) return false;

    // Get the size of the file
    file.seekg( 0, file.end );
    size_t length = file.tellg();
    file.seekg( 0, file.beg );

    char* file_buffer = new char[length+1];
    file.read( file_buffer, length );
    file_buffer[ file.gcount() ] = 0;

    file.close();

    //---------------------------------------------------------------------------

    int  loaded = 0;
    int  total  = 0;

    rapidjson::Document doc;
    doc.Parse(file_buffer);

    delete [] file_buffer;

    if(!doc.IsObject()){
      std::cout << "   ...Error: Missing main document object.\n";
      return false;
    }

    // Check if it is an array of kernels, or a single kernel
    if(!doc.HasMember("kernels")){
      if(doc.HasMember("name") && doc.HasMember("kernel")){
        rapidjson::Value& value = doc;

        load_kernel_from_json( value, collection );

        return true;
      }else{
        std::cout << "   ...error: missing 'kernels' attribute.\n";
        return false;
      }
    }

    rapidjson::Value& kernels_attribute = doc["kernels"];
    if(!kernels_attribute.IsArray()){
      std::cout << "   ...error: 'kernels' attribute is not an array.\n";
      return false;
    }

    // Iterate through kernels members
    total = (int) kernels_attribute.Size();

    for( int i = 0; i < total; ++i ){
      rapidjson::Value& kernel_def = kernels_attribute[i];

      if( load_kernel_from_json( kernel_def, collection )){
        ++loaded;
      }
    }

    //---------------------------------------------------------------------------

    return loaded;
  }

  //-----------------------------------------------------------------------------

  void load_kernels_from_dir( const std::string& directory, kernel_collection& kernels ){
    std::cout << "Loading filters from \"" << directory << "\".\n";
    DIR *dir;
    struct dirent *entry;
    if((dir = opendir( directory.c_str() )) != NULL) {
      while ((entry = readdir(dir)) != NULL) {
        // Only open files, not previous directory or current one
        if( !(strcmp( entry->d_name, "." ) == 0 || strcmp( entry->d_name, ".." ) == 0) &&
            string_ends_with( entry->d_name, ".kernel" ) ){
          // Load the file
          std::cout << " * Opening " << entry->d_name << ":\n";
          std::string filename = directory + '/' + entry->d_name;

          int loaded = load_kernel_from_file( filename, kernels );

          if(loaded){
            std::cout << "   ...successfully loaded " << loaded << (loaded == 1 ? " kernel.\n" : " kernels.\n");
          }else{
            std::cout << "   ...no filters loaded.\n";
          }
        }
      }
      closedir(dir);
    }else{
      std::cout << "Directory " << directory << "' not found.\n";
    }
  }
}

//-----------------------------------------------------------------------------
// Phase I: Filters
//-----------------------------------------------------------------------------

void load_filters(){
  std::cout << "Please enter either a *.filter file, or a directory containing\n"
            << "*.kernel files to load.\n";
  std::string path = ocr::get_string_input( "Filter path: ", "Error, invalid input." );

  if(string_ends_with(path,".kernel")){
    std::cout << " * Opening " << path << ":\n";
    ocr::load_kernel_from_file( path, g_kernels );
  }else{
    ocr::load_kernels_from_dir( path, g_kernels );
  }
  std::cout << std::flush;
  ocr::get_any_input("Press enter to continue...\n");
}

//-----------------------------------------------------------------------------

void display_filters(){
  if(g_kernels.empty()){
    std::cout << "No filters currently loaded\n";
  }else{
    kernel_collection::iterator iter = g_kernels.begin();
    int i = 0;
    for( ; iter != g_kernels.end(); ++iter ){
      std::cout << std::setw(3) << i++ << std::setw(0) << "  "  << (*iter).first << '\n';
    }
  }
  std::cout << std::flush;
}

//-----------------------------------------------------------------------------

void view_filters(){
  display_filters();
  ocr::get_any_input("Press enter to continue...\n");
}

//-----------------------------------------------------------------------------

void apply_filters(){

  display_filters();
  if(!g_kernels.size()){
    ocr::get_any_input("Press enter to continue...\n");
    return;
  }

  std::cout << "Please enter the kernel number to load, the path to the image to modify,\n"
               "and the path to the output file.\n";
  int index = ocr::get_int_input("Kernel number: ","Error, invalid input");
  while( index < 0 || index > (int) (g_kernels.size()-1) ){
    std::cout << "Error, input out of range.\n";
    index = ocr::get_int_input("Kernel number: ","Error, invalid input");
  }
  std::string infile  = ocr::get_string_input("Enter input file: ", "Error, invalid input");
  std::string outfile = ocr::get_string_input("Enter output file: ", "Error, invalid input");

  ocr::Image* image;
  int status = ocr::load_bmp_image_color( infile.c_str(), &image );

  if( status != ocr::IS_SUCCESS ){
    std::cout << "Error loading input file\n";
    ocr::get_any_input("Press enter to continue...\n");
    return;
  }

  ocr::Kernel_Image_Operator* op = g_kernels[index].second;

  ocr::Image out = op->operate(*image);

  if( !ocr::save_bmp_image( outfile.c_str(), &out ) ){
    ocr::destroy_image( &image );
    std::cout << "Error saving output file\n";
    ocr::get_any_input("Press enter to continue...\n");
    return;
  }

  ocr::destroy_image( &image );
  std::cout << "File successfully output to '" << outfile << "'.\n";
  ocr::get_any_input("Press enter to continue...\n");
}

//----------------------------------------------------------------------------

void generate_gaussian_filter(){
  std::cout << "Please enter the sigma value for the gaussian filter, the \n"
               "spread of the filter (width from center; width/2), the name \n"
               "of the filter, and a file path to store it to.\n";

  //---------------------------------------------------------------------------

  float sigma = ocr::get_float_input("Sigma (>0.0): ", "Invalid value entered.\n");
  // Sanitize input
  while(sigma<0.0f){
    std::cout << "Sigma must be positive!\n";
    sigma = ocr::get_float_input("Sigma (>0.0): ", "Invalid value entered.\n");
  }

  //---------------------------------------------------------------------------

  int width = ocr::get_int_input("Width from center: ", "Invalid value\n");
  // Sanitize input
  while( width < 0){
    std::cout << "Width must be positive!\n";
    width = ocr::get_int_input("Width from center: ", "Invalid value\n");
  }

  //---------------------------------------------------------------------------

  std::string name = ocr::get_string_input("Name of Filter: ", "Invalid value\n");
  std::string filename = ocr::get_string_input("Filename: ", "Invalid value\n");

  if(!string_ends_with(filename,".kernel")){
    filename += ".kernel";
  }
  std::ofstream file(filename);
  if(!file.good()){
    std::cout << "Error: Unable to open file " << filename << "\n";
    ocr::get_any_input("Press enter to continue...\n");
    return;
  }

  std::vector<std::vector<float>> kernel;

  const float SIG2SQUARE = 2*sigma*sigma;
  for(int x = -width; x <= width; ++x ){
    kernel.push_back(std::vector<float>());
    for(int y = -width; y <= width; ++y ){
      float g = (1.0/(SIG2SQUARE * M_PI)) * std::exp(-(x*x + y*y)/SIG2SQUARE);
      kernel[x+width].push_back(g);
    }
  }

  //---------------------------------------------------------------------------

  // Create the DOM

  rapidjson::Document doc;
  doc.SetObject();

  rapidjson::Value name_attribute;
  name_attribute.SetString(name.c_str(), name.size(), doc.GetAllocator());

  rapidjson::Value kernel_attribute;
  kernel_attribute.SetArray();


  for( size_t i = 0; i < kernel.size(); ++i ){
    rapidjson::Value array;
    array.SetArray();
    for( size_t j = 0; j < kernel.size(); ++j ){
      array.PushBack( kernel[i][j], doc.GetAllocator() );
    }
    kernel_attribute.PushBack( array, doc.GetAllocator() );
  }

  doc.AddMember("name", name_attribute, doc.GetAllocator() );
  doc.AddMember("kernel", kernel_attribute, doc.GetAllocator() );

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);
  const char* output = buffer.GetString();

  //---------------------------------------------------------------------------

  file << output;
  file.close();

  g_kernels.push_back(std::make_pair( name, new ocr::Kernel_Image_Operator(kernel) ));

  std::cout << "Successfully written to file\n";
  ocr::get_any_input("Press enter to continue...\n");
}

//----------------------------------------------------------------------------
// Phase II
//----------------------------------------------------------------------------

void generate_feature_database(){

  std::cout << "Please enter the path to a directory of images to generate a feature database for\n"
               "followed by a bitmap image for the glyph used to represent that database.\n";

  std::string path  = ocr::get_string_input("Enter feature path: ", "Error, invalid input");

  DIR *dir;
  struct dirent *entry;
  if((dir = opendir( path.c_str() )) == NULL) {
    std::cout << "Error opening directory '" << path << "'\n";
    ocr::get_any_input("Press enter to continue...\n");
    return;
  }

  //--------------------------------------------------------------------------

  // Assume glyph is in the same folder, otherwise ask the user
  std::string glyph_path = path;

  if(string_ends_with( path, "/") || string_ends_with( path, "\\")){
    glyph_path += "glyph.bmp";
  }else{
    glyph_path += "/glyph.bmp";
  }

  ocr::Image* glyph_image;
  int status = ocr::load_bmp_image_binary( glyph_path.c_str(), &glyph_image );

  if( status != ocr::IS_SUCCESS ){
    std::string glyph = ocr::get_string_input("Enter glyph path: ", "Error, invalid input");

    int status = ocr::load_bmp_image_binary( glyph.c_str(), &glyph_image );

    if( status != ocr::IS_SUCCESS ){
      closedir(dir);
      std::cout << "Error loading glyph file\n";
      ocr::get_any_input("Press enter to continue...\n");
      return;
    }
  }

  //--------------------------------------------------------------------------

  std::string outfile = ocr::get_string_input("Enter output file (*.fdb): ", "Error, invalid input");
  if(!string_ends_with(outfile,".fdb")){
    outfile += ".fdb";
  }

  std::ofstream file(outfile);
  if(!file.good()){
    closedir(dir);
    ocr::destroy_image( &glyph_image );
    std::cout << "Error: Unable to open file " << outfile << "\n";
    ocr::get_any_input("Press enter to continue...\n");
    return;
  }

  //--------------------------------------------------------------------------

  ocr::feature_collection  all_vectors;

  while ((entry = readdir(dir)) != NULL) {
    ocr::feature_collection  current_vectors;
    ocr::boundary_collection unused;
    // Only open files, not previous directory or current one
    if( !(strcmp( entry->d_name, "." ) == 0 || strcmp( entry->d_name, ".." ) == 0) &&
        string_ends_with( entry->d_name, ".bmp") && strcmp( entry->d_name, "glyph.bmp") != 0 ){

      // Generate path to the file
      std::string filepath = path;
      if(string_ends_with( path, "/") || string_ends_with( path, "\\")){
        filepath += entry->d_name;
      }else{
        filepath += "/";
        filepath += entry->d_name;
      }

      std::cout << "Scanning " << entry->d_name << "\n";
      ocr::Image* image_entry;

      int status = ocr::load_bmp_image_binary( filepath.c_str(), &image_entry );

      if( status == ocr::IS_SUCCESS ){
        std::cout << " o Loading feature vector from " << entry->d_name << ".\n";
        ocr::load_features( *image_entry, current_vectors, unused, X_DIVS, Y_DIVS );
        ocr::destroy_image( &image_entry );
      }
    }
    all_vectors.insert( all_vectors.end(), current_vectors.begin(), current_vectors.end() );
  }

  closedir(dir);

  //--------------------------------------------------------------------------

  if(!all_vectors.size()){
    std::cout << "No feature vectors found! Unable to create database.\n";
    file.close();
    ocr::destroy_image( &glyph_image );

    ocr::get_any_input("Press enter to continue...\n");
  }

  // Write to database file

  // [width] [height] [length of vector] [# of vectors]
  file << glyph_image->width()  << " "
       << glyph_image->height() << " "
       << all_vectors[0].size()   << " "
       << all_vectors.size()      << "\n";

  for( size_t y = 0; y < glyph_image->height(); ++y ){
    for( size_t x = 0; x < glyph_image->width(); ++x ){
      file << (int) glyph_image->at_binary(x,y);
    }
    file << "\n";
  }
  file << "\n";

  for( ocr::Feature_Vector& vector : all_vectors ){
    for( ocr::Feature_Vector::iterator iter = vector.begin(); iter != vector.end(); ++iter ){
      file << *iter << " ";
    }
    file << "\n";
  }

  file.close();

  //--------------------------------------------------------------------------

  ocr::destroy_image( &glyph_image );
  ocr::get_any_input("Press enter to continue...\n");
}

void load_feature_database_from_file( const char* filename ){
  std::ifstream file(filename);
  if(!file.good()){
    std::cout << " o Unable to open file " << filename << "\n";
    return;
  }

  size_t width, height, vector_length, no_of_vectors;

  file >> width;
  file >> height;
  file >> vector_length;
  file >> no_of_vectors;
  file.get(); // get endline

  //--------------------------------------------------------------------------

  // Extract Image

  ocr::Image image(width,height);

  char* line = new char[width+1];

  for( size_t y = 0; y < height; ++y ){
    file.getline( line, width + 1 );
    if( (size_t) file.gcount() != width + 1 ){
      std::cout << " o Invalid Glyph (inconsistent width). Unable to read database.\n";
      return;
    }

    // Generate the image row

    for( size_t x = 0; x < width; ++x ){
      image.set_binary( x, y, line[x] == '1' );
    }
  }

  //--------------------------------------------------------------------------

  // Extract Vectors

  file.get(); // ignore the \n in the file

  ocr::feature_collection features;

  std::cout << " o " << no_of_vectors << " features loaded\n";
  for( size_t i = 0; i < no_of_vectors; ++i ){
    ocr::Feature_Vector::feature_collection feat_values;
    for( size_t j = 0; j < vector_length; ++j ){
      double value;
      file >> value;
      feat_values.push_back(value);
    }
    ocr::Feature_Vector feature(feat_values);
    //std::cout << "  - " << (i+1) << ": " << feature << std::endl;
    features.push_back(feature);
  }

  // Add them to the global features database
  g_feature_db.insert( image, features );

  //--------------------------------------------------------------------------

  file.close();
}

void load_feature_database(){

  std::cout << "Please enter either the feature database file (*.fdb), or a directory containing them to load.\n";

  std::string inpath = ocr::get_string_input("Enter input path: ", "Error, invalid input");

  // Open feature database file (*.fdb) if specified, or directory otherwise
  if(string_ends_with(inpath,".fdb")){
    load_feature_database_from_file( inpath.c_str() );
  }else{
    DIR *dir;
    struct dirent *entry;
    if((dir = opendir( inpath.c_str() )) == NULL) {
      std::cout << "Error opening directory '" << inpath << "'\n";
      ocr::get_any_input("Press enter to continue...\n");
      return;
    }

    while ((entry = readdir(dir)) != NULL) {
      // Only open files, not previous directory or current one
      if( !(strcmp( entry->d_name, "." ) == 0 || strcmp( entry->d_name, ".." ) == 0) &&
          string_ends_with( entry->d_name, ".fdb" ) ){

        std::string filepath = inpath;
        if(string_ends_with( inpath, "/" ) || string_ends_with( inpath, "\\")){
          filepath += entry->d_name;
        }else{
          filepath += "/";
          filepath += entry->d_name;
        }

        std::cout << "Loading " << entry->d_name << ".\n";
        load_feature_database_from_file( filepath.c_str() );
      }
    }

    closedir(dir);
  }

  ocr::get_any_input("Press enter to continue...\n");
}

void scan_for_features(){
  std::cout << "Please enter the path to the binary image.\n";
  std::string infile  = ocr::get_string_input("Enter input file: ", "Error, invalid input");

  ocr::Image* image;

  int status = ocr::load_bmp_image_binary( infile.c_str(), &image );

  if( status != ocr::IS_SUCCESS ){
    std::cout << "Error loading input file\n";
    ocr::get_any_input("Press enter to continue...\n");
    return;
  }

  // Error checking: Only allow 1 globally stored scanned image.
  if(g_scanned_image_boundaries.size() || g_scanned_image_features.size() || g_scanned_image){
    std::cout << "Clearing previously scanned image data." << std::endl;
    g_scanned_image_boundaries.clear();
    g_scanned_image_features.clear();

    if(g_scanned_image){
      ocr::destroy_image(&g_scanned_image);
    }
  }

  g_scanned_image = image;

  ocr::feature_collection&  vec    = g_scanned_image_features;
  ocr::boundary_collection& bounds = g_scanned_image_boundaries;
  ocr::load_features( *g_scanned_image, vec, bounds, X_DIVS, Y_DIVS );

  // Print newly discovered glyph
  int current = 1;
  for( ocr::boundary& bound : bounds ){
    std::cout << "Glyph " << current++ << " found at " << bound << "\n";
  }

  current = 1;
  for( ocr::Feature_Vector& vector : vec ){
    std::cout << "Vector " << current++ << ": " << vector << "\n";
  }

  // Print calculated vectors
  ocr::get_any_input("Press enter to continue...\n");
}

//-----------------------------------------------------------------------------
// Phase III: Recognition
//-----------------------------------------------------------------------------

void analyze_features(){
  if(!(g_scanned_image_boundaries.size() && g_scanned_image_features.size() && g_scanned_image)){
    std::cout << "Error: No image scanned. Please scan image first.\n";
    ocr::get_any_input("Press enter to continue...\n");
    return;
  }
  if(!g_feature_db.size()){
    std::cout << "Error: No features in database. Please scan database first.\n";
    ocr::get_any_input("Press enter to continue...\n");
    return;
  }

  std::string path  = ocr::get_string_input("Output filename: ", "Error, invalid input");

  if( !string_ends_with(path,".bmp") ){
    path += ".bmp";
  }

  ocr::Image result( g_scanned_image->width(), g_scanned_image->height() );
  result.fill_binary(0);

  g_feature_db.analyze( result, g_scanned_image_boundaries, g_scanned_image_features );

  ocr::save_bmp_image( path.c_str(), &result );

  ocr::get_any_input("Press enter to continue...\n");
}

//-----------------------------------------------------------------------------
// Image Conversion
//-----------------------------------------------------------------------------

void convert_color_to_grayscale(){
  std::cout << "Please enter the path to the color image, and the \n"
               "path to store the grayscale image.\n";
  std::string infile  = ocr::get_string_input("Enter input file: ", "Error, invalid input");

  ocr::Image* image;
  int status = ocr::load_bmp_image_grayscale( infile.c_str(), &image );

  if( status != ocr::IS_SUCCESS ){
    std::cout << "Error loading input file\n";
    ocr::get_any_input("Press enter to continue...\n");
    return;
  }

  std::string outfile = ocr::get_string_input("Enter output file: ", "Error, invalid input");

  if( !ocr::save_bmp_image( outfile.c_str(), image ) ){
    std::cout << "Error saving output file\n";
  }else{
    std::cout << "File successfully output to '" << outfile << "'.\n";
  }

  ocr::destroy_image( &image );
  ocr::get_any_input("Press enter to continue...\n");
}

void convert_grayscale_to_binary_static(){
  std::cout << "Please enter the path to the grayscale image, the \n"
               "path to store the binary image, and the threshold \n"
               "for producing a binary image.\n";
  std::string infile  = ocr::get_string_input("Enter input file: ", "Error, invalid input");

  ocr::Image* image;
  int status = ocr::load_bmp_image_grayscale( infile.c_str(), &image );

  if( status != ocr::IS_SUCCESS ){
    std::cout << "Error loading input file\n";
    ocr::get_any_input("Press enter to continue...\n");
    return;
  }

  std::string outfile = ocr::get_string_input("Enter output file: ", "Error, invalid input");

  int threshold = ocr::get_int_input("Threshold (0-255): ","Error, invalid input");
  while( threshold < 0 || threshold > 255){
    std::cout << "Error, input out of range.\n";
    threshold = ocr::get_int_input("Threshold (0-255): ","Error, invalid input");
  }

  for( size_t i = 0; i < image->width(); ++i ){
    for( size_t j = 0; j < image->height(); ++j ){
      ocr::Image::pixel_type out_pixel;
      ocr::Image::pixel_type pixel = image->at(i,j);
      ocr::ubyte out = ((((pixel.r + pixel.g + pixel.b) / 3) > threshold) ? 255 : 0);

      out_pixel.r = out;
      out_pixel.g = out;
      out_pixel.b = out;

      image->set(i,j,out_pixel);
    }
  }

  if( !ocr::save_bmp_image( outfile.c_str(), image ) ){
    std::cout << "Error saving output file\n";
  }else{
    std::cout << "File successfully output to '" << outfile << "'.\n";
  }

  ocr::destroy_image( &image );
  ocr::get_any_input("Press enter to continue...\n");
}

void convert_grayscale_to_binary_adaptive(){
  std::cout << "Please enter the path to the grayscale image, the \n"
               "path to store the binary image, and the neighborhood \n"
               "size for the adaptive threshold.\n"
               "Note: larger thresholds take longer to compute.\n";
  std::string infile  = ocr::get_string_input("Enter input file: ", "Error, invalid input");

  ocr::Image* image;
  int status = ocr::load_bmp_image_grayscale( infile.c_str(), &image );

  if( status != ocr::IS_SUCCESS ){
    std::cout << "Error loading input file\n";
    ocr::get_any_input("Press enter to continue...\n");
    return;
  }

  std::string outfile = ocr::get_string_input("Enter output file: ", "Error, invalid input");

  int neighborhood = ocr::get_int_input("Neighborhood (5-100): ","Error, invalid input");
  while( neighborhood < 5 || neighborhood > 100){
    std::cout << "Error, input out of range.\n";
    neighborhood = ocr::get_int_input("Neighborhood (5-100): ","Error, invalid input");
  }

  ocr::u32 x_neighborhood = neighborhood;
  ocr::u32 y_neighborhood = neighborhood;

  ocr::Image out_image( image->width(), image->height() );

  // Perform the calculation
  for( size_t x_seg = 0; x_seg < image->width(); ++x_seg ){

    if(x_seg + neighborhood >= image->width()){
      x_neighborhood = image->width() - x_seg;
    }else{
      x_neighborhood = neighborhood;
    }

    for( size_t y_seg = 0; y_seg < image->height(); ++y_seg ){

      if(y_seg + neighborhood >= image->height()){
        y_neighborhood = image->height() - y_seg;
      }else{
        y_neighborhood = neighborhood;
      }

      // Calculate the segment average
      ocr::u32 avg = 0;
      for( size_t i = 0; i < x_neighborhood; ++i ){
        for( size_t j = 0; j < y_neighborhood; ++j ){
          ocr::Image::pixel_type pixel = image->at( x_seg + i, y_seg + j );

          avg += pixel.r;
        }
      }
      avg /= (x_neighborhood*y_neighborhood);

      // Calculate the thresholded image
      for( size_t i = 0; i < x_neighborhood; ++i ){
        for( size_t j = 0; j < y_neighborhood; ++j ){
          ocr::Image::pixel_type pixel = image->at( x_seg + i, y_seg + j );
          ocr::ubyte out = (( pixel.r >= avg ) ? 255 : 0);

          ocr::Image::pixel_type out_pixel;
          out_pixel.r = out;
          out_pixel.g = out;
          out_pixel.b = out;

          out_image.set( x_seg + i, y_seg + j, out_pixel );
        }
      }
    }
  }

  if( !ocr::save_bmp_image( outfile.c_str(), &out_image ) ){
    std::cout << "Error saving output file\n";
  }else{
    std::cout << "File successfully output to '" << outfile << "'.\n";
  }

  ocr::destroy_image( &image );
  ocr::get_any_input("Press enter to continue...\n");

}

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

int main( int argc, char** argv ){

  g_scanned_image = nullptr;

  int menu_option = 0; // start on main menu

  menu_type current = menu_main;
  bool is_running = true;
  while(is_running){

    display_menu(current);
    menu_option = ocr::get_int_input("Enter option: ","Error, please try again.");

    switch(current){
    case menu_main:
      switch(menu_option){
      case 0:
        is_running = false;
        break;
      case 1:
        current = menu_filters_and_thinning;
        break;
      case 2:
        current = menu_vector_recognition;
        break;
      case 3:
        current = menu_utilities;
        break;
      default:
        break;
      }
      break;
    case menu_filters_and_thinning:
      switch(menu_option){
      case 0:
        current = menu_main;
        break;
      case 1:
        load_filters();
        break;
      case 2:
        view_filters();
        break;
      case 3:
        apply_filters();
        break;
      case 4:
        generate_gaussian_filter();
        break;
      case 5:
        ocr::zs_thinning();
        break;
      }

      break;
    case menu_vector_recognition:
      switch(menu_option){
      case 0:
        current = menu_main;
        break;
      case 1:
        generate_feature_database();
        break;
      case 2:
        load_feature_database();
        break;
      case 3:
        scan_for_features();
        break;
      case 4:
        analyze_features();
        break;
      }
      break;

    case menu_utilities:
      switch(menu_option){
      case 1:
        convert_color_to_grayscale();
        break;
      case 2:
        convert_grayscale_to_binary_static();
        break;
      case 3:
        convert_grayscale_to_binary_adaptive();
        break;
      case 0:
        current = menu_main;
        break;
      }
      break;
    case menu_exit:

      break;
    }
    ocr::clear();

  }

  return 0;
}

