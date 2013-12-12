
#include <iostream>
#include "assembler.h"
using namespace std;

// Simple constructor.
Assembler::Assembler(istream &in, ostream &out)
{
   currentAddress = 0;
   for (int i = 0; i < MAXLABEL; i++)
      labelTable[i] = 0;
   insource = &in;
   outsource = &out;
}

// Default destructor.
Assembler::~Assembler()
{ }

// The first pass of the assmebler.  This just builds the labelTable.
// Don't translate just yet.
void Assembler::firstPass()
{
   // Start at address 0 (not 1 as I had previously thought)
   currentAddress = 0;
   string nextop;
   (*insource) >> nextop;
   // Loop until we find the ENDPROG operation.
   for (;;) {
      // Record the current address in the label table.
      if (nextop == "DEFADDR") {
	 int index;
	 (*insource) >> index;
	 labelTable[index] = currentAddress;
	 (*insource) >> nextop;
      }
      // Record the associated value in the label table.
      else if (nextop == "DEFARG") {
	 int index, value;
	 (*insource) >> index >> value;
	 labelTable[index] = value;
	 (*insource) >> nextop;
      }
      // Stop when we find ENDPROG.
      else if (nextop == "ENDPROG")
	 return;
      else if (nextop == "ARROW" || nextop == "ASSIGN" || nextop == "BAR" ||
	       nextop == "CONSTANT" || nextop == "FI" || nextop == "READ" ||
	       nextop == "WRITE") {
	 // Skip one machine words.
	 (*insource) >> nextop;
	 (*insource) >> nextop;
	 currentAddress += 2;
      }
      else if (nextop == "CALL" || nextop == "INDEX" || nextop == "PROC" ||
	       nextop == "PROG" || nextop == "VARIABLE") {
	 // Skip two machine words.
	 (*insource) >> nextop;
	 (*insource) >> nextop;
	 (*insource) >> nextop;
	 currentAddress += 3;
      }
      else {
	 // just get the next machine word.
	 (*insource) >> nextop;
	 currentAddress++;
      }
   }
}

// The second pass of the assembler.  Here we actually translate operations
// to their opcodes.
void Assembler::secondPass()
{
   string nextop;
   (*insource) >> nextop;
   // Loop until ENDPROG.
   for (;;) {
     if (nextop == "ADD") {
	 (*outsource) << 0 << endl;;
	 currentAddress++;
      }
      else if (nextop == "AND") {
	 (*outsource) << 1 << endl;;
	 currentAddress++;
      }
      else if (nextop == "ARROW") {
	 (*outsource) << 2 << endl;;
	 int temp;
	 (*insource) >> temp;
	 // Output the absolute jump address.
	 (*outsource) << labelTable[temp] << endl;
	 currentAddress += 2;
      }
      else if (nextop == "ASSIGN") {
	 (*outsource) << 3 << endl;
	 int temp;
	 (*insource) >> temp;
	 (*outsource) << temp << endl;
	 currentAddress += 2;
      }
      else if (nextop == "BAR") {
	 (*outsource) << 4 << endl;
	 int temp;
	 (*insource) >> temp;
	 (*outsource) << labelTable[temp] << endl;
	 currentAddress += 2;
      }
      else if (nextop == "CALL") {
	 (*outsource) << 5 << endl;
	 int temp;
	 (*insource) >> temp;
	 (*outsource) << temp << endl;
	 (*insource) >> temp;
	 (*outsource) << labelTable[temp] << endl;
	 currentAddress += 3;
      }
      else if (nextop == "CONSTANT") {
	 (*outsource) << 6 << endl;
	 int temp;
	 (*insource) >> temp;
	 (*outsource) << temp << endl;
	 currentAddress += 2;
      }
      else if (nextop == "DIVIDE") {
	 (*outsource) << 7 << endl;
	 currentAddress++;
      }
      else if (nextop == "ENDPROC") {
	 (*outsource) << 8 << endl;
	 currentAddress++;
      }
      else if (nextop == "ENDPROG") {
	 (*outsource) << 9 << endl;
	 break;
      }
      else if (nextop == "EQUAL") {
	 (*outsource) << 10 << endl;
	 currentAddress++;
      }
      else if (nextop == "FI") {
	 (*outsource) << 11 << endl;
	 int temp;
	 (*insource) >> temp;
	 (*outsource) << temp << endl;
	 currentAddress += 2;
      }
      else if (nextop == "GREATER") {
	 (*outsource) << 12 << endl;
	 currentAddress++;
      }
      else if (nextop == "INDEX") {
	 (*outsource) << 13 << endl;
	 int temp;
	 (*insource) >> temp;
	 (*outsource) << temp << endl;
	 (*insource) >> temp;
	 (*outsource) << temp << endl;
	 currentAddress += 3;
      }
      else if (nextop == "LESS") {
	 (*outsource) << 14 << endl;
	 currentAddress++;
      }
      else if (nextop == "MINUS") {
	 (*outsource) << 15 << endl;
	 currentAddress++;
      }
      else if (nextop == "MODULO") {
	 (*outsource) << 16 << endl;
	 currentAddress++;
      }
      else if (nextop == "MULTIPLY") {
	 (*outsource) << 17 << endl;
	 currentAddress++;
      }
      else if (nextop == "NOT") {
	 (*outsource) << 18 << endl;
	 currentAddress++;
      }
      else if (nextop == "OR") {
	 (*outsource) << 19 << endl;
	 currentAddress++;
      }
      else if (nextop == "PROC") {
	 (*outsource) << 20 << endl;
	 int temp;
	 (*insource) >> temp;
	 (*outsource) << labelTable[temp] << endl;
	 (*insource) >> temp;
	 (*outsource) << labelTable[temp] << endl;
	 currentAddress += 3;
      }
      else if (nextop == "PROG") {
	 (*outsource) << 21 << endl;
	 int temp;
	 (*insource) >> temp;
	 (*outsource) << labelTable[temp] << endl;
	 (*insource) >> temp;
	 (*outsource) << labelTable[temp] << endl;
	 currentAddress += 3;
      }
      else if (nextop == "READ") {
	 (*outsource) << 22 << endl;
	 int temp;
	 (*insource) >> temp;
	 (*outsource) << temp << endl;
	 currentAddress += 2;
      }
      else if (nextop == "SUBTRACT") {
	 (*outsource) << 23 << endl;
	 currentAddress++;
      }
      else if (nextop == "VALUE") {
	 (*outsource) << 24 << endl;
	 currentAddress++;
      }
      else if (nextop == "VARIABLE") {
	 (*outsource) << 25 << endl;
	 int temp;
	 (*insource) >> temp;
	 (*outsource) << temp << endl;
	 (*insource) >> temp;
	 (*outsource) << temp << endl;
	 currentAddress += 3;
      }
      else if (nextop == "WRITE") {
	 (*outsource) << 26 << endl;
	 int temp;
	 (*insource) >> temp;
	 (*outsource) << temp << endl;
	 currentAddress += 2;
      }
      else if (nextop == "DEFADDR") {
	 int temp;
	 (*insource) >> temp;
      }
      else if (nextop == "DEFARG") {
	 int temp1, temp2;
	 (*insource) >> temp1 >> temp2;
      }
      else {
	 // We should never see this message.
	cerr << "Assembler encountered an unknown operator: `" 
	     << nextop << "'.  Bailing...\n";
	exit(2);
      }
      (*insource) >> nextop;
   }
}
