# JAVA-0-Compiler
A simple compiler that I made for my compiler class. I will take a simple JAVA-0 and translates it to Linux NASM 32-bit machine code.
I used Icon for the lexical analyzer and C for the syntax and semantic analyzer.

Lexical Analyzer:
  1. To run the lexical analyzer, you need to have Icon install https://www2.cs.arizona.edu/icon/index.htm.
  2. Simply open the terminal at the location of the JAVA-0 file, then run these commands:
      + icont lexical.icn
      + iconx lexical
          - Here you will need to input the name of your JAVA-0 file. It will gives you the token_list.txt and symbol_table.txt
          
Syntax Analyzer:
  1. Compile and run the syntax.c. It will gives you some basic syntax error if your program has any problems.
  2. After you had successfully run the syntax analyzer, it will give you the quad.txt file
  

Semantic Analyzer:
  1.  Compile and run the semantic.c. It will use the quad.txt and symbol_table.txt to generate the machine code for you.
  
 
Then use Virtual Box (or other VM system) to run the .asm file we just generated. Make sure you have a Linux 32-bit machine on your VM.
And run these commands:
  1. assemble:	nasm -f elf -l [program_name].lst  [program_name].asm
  2. link:  	  gcc -o [program_name]  [program_name].o
  3. run:       ./[program_name]
  
  
The I/O routine of my compiler was written by Dr. David Burris from the Department of Computer Science at Sam Houston State University.
