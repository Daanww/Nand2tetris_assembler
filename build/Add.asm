// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/06/add/Add.asm

// Computes R0 = 2 + 3  (R0 refers to RAM[0])

//@2
//D=A
//@3
//D=D+A
//@0
//M=D

@i
D=A
A=M-D
MD=-1;JNE
AM=D|A;JEQ
AMD=-M;JLE
@20
D=A
@i
D=D+A
