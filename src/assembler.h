
#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <iostream>

#define MAXLABEL 1000

class Assembler
{
  public:
  Assembler(std::istream &in, std::ostream &out);
  ~Assembler();
  // The two passes of the assembler.
  void firstPass(); 
  void secondPass();
  
 private:
  int labelTable[MAXLABEL]; 
  int currentAddress; 
  std::istream *insource;  // Input file
  std::ostream *outsource; // Output file 
};
#endif
