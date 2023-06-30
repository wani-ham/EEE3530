###########################################################
# Kite: Architecture Simulator for RISC-V Instruction Set #
# Developed by William J. Song                            #
# Intelligent Computing Systems Lab, Yonsei University    #
# Version: 1.10                                           #
###########################################################

# Kite program code
#   1. The first instruction starts at PC = 4. PC = 0 is reserved as invalid.
#   2. To terminate the program, let the next PC naturally go out of range.
#   3. All the instructions and labels are case-insensitive.
#   4. The program code supports only the following list of instructions
#      (sorted by alphabetical order in each type).
#      R-type: add, and, div, divu, mul, or, rem, remu, sll, sra, srl, sub, xor
#      I-type: addi, andi, jalr, ld, slli, srai, srli, ori, xori
#      S-type: sd
#      SB-type: beq, bge, blt, bne
#      U-type: lui
#      UJ-type: jal
#      No-type: nop

insertion_sort:
	# initialize i = 1
	addi x5, x0, 1  

OUTER_LOOP:
	# if i>= size END program
	bge x5, x11, END  
	# element(x23) = array[i]
	slli x7, x5, 3  
	add x22, x7, x10
	ld x23, 0(x22)  
	add x6, x5, x0  
INNER_LOOP:
	# x24 = array[j-1]
	addi x28, x6, -1
	slli x29, x28, 3
	add x30, x29, x10
	ld x24, 0(x30)
	# if j!=0 goto FLAG, if-not goto L1
	bne x6, x0, FLAG  
	jal x1, L1  
PASS:
	# array[j] = array[j-1]
	slli x29, x6, 3  
	add x30, x29, x10
	sd x24, 0(x30)  
	# j--
	addi x6, x6, -1
	jal x1, INNER_LOOP	
L1:
	# array[i] = element(x23)
	slli x29, x6, 3
	add x30, x29, x10
	sd x23, 0(x30)  
	# i++
	addi x5, x5, 1
	jal x1, OUTER_LOOP
FLAG:
	# if array[j-1] <= element goto L1, if-not goto PASS
	bge x23, x24, L1
	jal x1, PASS
END:
