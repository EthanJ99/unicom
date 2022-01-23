#pragma once
#include "cpu.h"

/* Declare functions for different types of instruction */

/* ---------------------------- Load Operations ---------------------------- */
/* LDA - Loads a byte of memory into the accumulator setting the zero and
negative flags as appropriate. */
void LDA(CPU* cpu, uint16_t operand);

/* LDX - Loads a byte of memory into the X register setting the zero and
negative flags as appropriate. */
void LDX(CPU* cpu, uint16_t operand);

/* LDY - Loads a byte of memory into the Y register setting the zero and
negative flags as appropriate. */
void LDY(CPU* cpu, uint16_t operand);

/* ---------------------------- Store Operations ---------------------------- */
/* STA - Stores the contents of the accumulator into memory. */
void STA(CPU* cpu, uint16_t operand);

/* STX - Stores the contents of the X register into memory. */
void STX(CPU* cpu, uint16_t operand);

/* STX - Stores the contents of the Y register into memory. */
void STY(CPU* cpu, uint16_t operand);

/* ---------------------------- Register Transfers ---------------------------- */
/* TAX - Copies the current contents of the accumulator into the X register and sets the
zero and negative flags as appropriate. */
void TAX(CPU* cpu);

/* TAY - Copies the current contents of the accumulator into the Y register and sets the
zero and negative flags as appropriate. */
void TAY(CPU* cpu);

/* TXA - Copies the current contents of the X register into the accumulator and sets the
zero and negative flags as appropriate. */
void TXA(CPU* cpu);

/* TYA - Copies the current contents of the Y register into the accumulator and sets the
zero and negative flags as appropriate. */
void TYA(CPU* cpu);

/* ---------------------------- Stack Operations ---------------------------- */
/* TSX - Copies the current contents of the stack register into the X register and sets the
zero and negative flags as appropriate. */
void TSX(CPU* cpu);

/* TXS - Copies the current contents of the X register into the stack register. */
void TXS(CPU* cpu);

/* PHA - Pushes a copy of the accumulator on to the stack. */
void PHA(CPU* cpu);

/* PHP - Pushes a copy of the status flags on to the stack. */
void PHP(CPU* cpu);

/* PLA - Pulls an 8 bit value from the stack and into the accumulator.
The zero and negative flags are set as appropriate. */
void PLA(CPU* cpu);

/* PLP - Pulls an 8 bit value from the stack and into the processor flags.
The flags will take on new states as determined by the value pulled. */
void PLP(CPU* cpu);

/* ------------------------------------ Logical ------------------------------------ */
/* AND - A logical AND is performed, bit by bit, on the accumulator contents using the
contents of a byte of memory. */
void AND(CPU* cpu, uint16_t operand);

/* EOR - An exclusive OR is performed, bit by bit, on the accumulator contents using the
contents of a byte of memory. */
void EOR(CPU* cpu, uint16_t operand);

/* ORA - An inclusive OR is performed, bit by bit, on the accumulator contents using the
contents of a byte of memory. */
void ORA(CPU* cpu, uint16_t operand);

/* BIT - This instructions is used to test if one or more bits are set in a target memory location.
The mask pattern in A is ANDed with the value in memory to set or clear the zero flag, but the
result is not kept. Bits 7 and 6 of the value from memory are copied into the N and V flags. */
void BIT(CPU* cpu, uint16_t operand);

/* ------------------------------------ Arithmetic ------------------------------------ */
/* ADC - This instruction adds the contents of a memory location to the accumulator together with the carry bit.
If overflow occurs the carry bit is set, this enables multiple byte addition to be performed. */
void ADC(CPU* cpu, uint16_t operand);

/* SBC - This instruction subtracts the contents of a memory location to the accumulator together with the not of the carry bit.
If overflow occurs the carry bit is clear, this enables multiple byte subtraction to be performed. */
void SBC(CPU* cpu, uint16_t operand);

/* CMP - This instruction compares the contents of the accumulator with another memory held value and sets the zero and carry flags as appropriate. */
void CMP(CPU* cpu, uint16_t operand);

/* CPX - This instruction compares the contents of the X register with another memory held value and sets the zero and carry flags as appropriate. */
void CPX(CPU* cpu, uint16_t operand);

/* CPY - This instruction compares the contents of the X register with another memory held value and sets the zero and carry flags as appropriate. */
void CPY(CPU* cpu, uint16_t operand);

/* ------------------------------ Increments & Decrements ------------------------------ */
/* INC - Adds one to the value held at a specified memory location setting the zero and
negative flags as appropriate. */
void INC(CPU* cpu, uint16_t operand);

/* INX - Adds one to the X register setting the zero and negative flags as appropriate. */
void INX(CPU* cpu);

/* INY - Adds one to the Y register setting the zero and negative flags as appropriate. */
void INY(CPU* cpu);

/* DEC - Subtracts one from the value held at a specified memory location setting the zero and
negative flags as appropriate. */
void DEC(CPU* cpu, uint16_t operand);

/* DEX - Subtracts one from the X register setting the zero and negative flags as appropriate. */
void DEX(CPU* cpu);

/* DEY - Subtracts one from the Y register setting the zero and negative flags as appropriate. */
void DEY(CPU* cpu);

/* ------------------------------------ Shifts ------------------------------------ */
/* ASL - This operation shifts all the bits of the accumulator or memory contents one bit
left. Bit 0 is set to 0 and bit 7 is placed in the carry flag. The effect of this operation
is to multiply the memory contents by 2 (ignoring 2's complement considerations), setting the
carry if the result will not fit in 8 bits. */
void ASL(CPU* cpu, uint16_t operand, Mode addr_mode);
void LSR(CPU* cpu, uint16_t operand, Mode addr_mode);
void ROL(CPU* cpu, uint16_t operand, Mode addr_mode);
void ROR(CPU* cpu, uint16_t operand, Mode addr_mode);


/* ------------------------------------ System Functions ------------------------------------ */
/* BRK - Force Interrupt; The BRK instruction forces the generation of an interrupt request.
The program counter and processor status are pushed on the stack then the IRQ interrupt vector
at $FFFE/F is loaded into the PC and the break flag in the status set to one. */
void BRK(CPU* cpu);
void NOP();
void RTI(CPU* cpu);

/* ------------------------------------ Jumps and Calls ------------------------------------ */
void JMP(CPU* cpu, uint16_t operand);
void JSR(CPU* cpu, uint16_t operand);
void RTS(CPU* cpu);


/* ------------------------------------ Branches ------------------------------------ */
void BCC(CPU* cpu, uint16_t branch_addr);
void BCS(CPU* cpu, uint16_t branch_addr);
void BEQ(CPU* cpu, uint16_t branch_addr);
void BMI(CPU* cpu, uint16_t branch_addr);
void BNE(CPU* cpu, uint16_t branch_addr);
void BPL(CPU* cpu, uint16_t branch_addr);
void BVC(CPU* cpu, uint16_t branch_addr);
void BVS(CPU* cpu, uint16_t branch_addr);

/* ------------------------------------ Status Flag Changes ------------------------------------ */
void CLC(CPU* cpu);
void CLD(CPU* cpu);
void CLI(CPU* cpu);
void CLV(CPU* cpu);
void SEC(CPU* cpu);
void SED(CPU* cpu);
void SEI(CPU* cpu);







