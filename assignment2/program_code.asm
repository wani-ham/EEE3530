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


# void quick_sort(int64_t* array, size_t size);
	# store stored register
	addi x2, x2, -96
	sd x8, 88(x2)
	sd x9, 80(x2)
	sd x18, 72(x2)
	sd x19, 64(x2)
	sd x20, 56(x2)
	sd x21, 48(x2)
	sd x22, 40(x2)
	sd x23, 32(x2)
	sd x24, 24(x2)
	sd x25, 16(x2)
	sd x26, 8(x2)
	sd x27, 0(x2) 

	# initialize
	add x18, x0, x0
	add x19, x0, x11
	addi x19, x19, -1

#x5 = i
#x6 = j
#x7 = 
#x11 = size
#x18 = low
#x19 = high
#x20 = arr[low]
#x21 = arr[high]
#x22 = arr[n/2]
#x23
#x24
#x25
#x26
#x27 = size
#x28 = temp value
#x29 = memory addr of arr[pivot]
#x30 = memory addr of arr[i]
#x31 = memory addr of arr[j]
	
quick_sort:
	addi x2, x2, -32  # make room for 4 items
	sd x1, 0(x2)  # store return address in stack
	sd x18, 8(x2)  # x18 = low	
	sd x19, 16(x2)  # x19 = high
	sd x5, 24(x2)  # x7 = i

	bge x18, x19, clear_stack  # if(high <= low) end function
	addi x27, x19, 1
	sub x27, x27, x18  # x27 = size of array
	addi x28, x0, 8  # temp_value = 8 (for check insertion sort condition)
	blt x27, x28, insertion_sort  # if size < 8 execute insertion sort
partition:	
	jal x1, find_pivot  # execute find_pivot function
	addi x5, x18, 1  # i = low+1
	addi x6, x19, -2  # j = high-2
	addi x7, x19, -1  # pivot = high-1	
	slli x28, x7, 3  # x28 = x7 * 8
	add x28, x10, x28  # x28 = address of arr[pivot]
	ld x29, 0(x28)  #x29 = arr[pivot]

outer_while:
	blt x5, x6, inner_first # if i < j continue while	
	jal x0, break_while  # if not, break while loop
inner_first:
	slli x28, x5, 3  # x28 = x5 * 8(size of int_64)
	add x28, x10, x28  # x28 = address of arr[i]	
	ld x30, 0(x28)  # x30 = arr[i]

	bge x30, x29, inner_second  # if arr[pivot] <= arr[i] break loop 
	addi x5, x5, 1  # i++
	jal x0, inner_first  # while loop
	
inner_second:
	slli x28, x6, 3  # x28 = x6 * 8
	add x28, x10, x28  # x28 = address of arr[j]
	ld x30, 0(x28)  # x30 = arr[j]

	bge x29, x30, inner_else  # if arr[j] <= arr[pivot] break loop
	addi x6, x6, -1  # j--
	jal x0, inner_second  # while loop

inner_else:
	slli x28, x5, 3
	add x28, x10, x28
	add x30, x0, x28  # copy &arr[i] to x30
	slli x28, x6, 3
	add x28, x10, x28
	add x31, x0, x28  # copy &arr[j] to x31
	bge x5, x6, outer_while # if j <= i repeat while loop
	jal x1, swap  # swap(i, j)
#else:
	jal x0, outer_while  # while loop
break_while:
	slli x28, x5, 3
	add x28, x10, x28
	add x30, x0, x28  # copy &arr[i] to x30
	addi x28, x19, -1
	slli x28, x28, 3
	add x28, x10, x28
	add x31, x28, x0  # copy &arr[high-1] to x31
	jal x1, swap  # swap(i, high-1)
	
	
	#quick_sort(low, i-1)
	addi x18, x18, 0 # low
	addi x19, x5, -1 # i-1
	jal x1, quick_sort
	#quick_sort(i+1, high)
	addi x18, x5, 1  # i+1
	addi x19, x19, 0  # high
	jal x1, quick_sort

clear_stack:
#	ld x1, 0(x2)  # get return address
#	addi x2, x2, 24  # clear stack
#
	ld x1, 0(x2)
	ld x18, 8(x2)
	ld x19, 16(x2)
	ld x5, 24(x2) 
	addi x2, x2, 32	
	beq x1, x0, end
	jalr x0, 0(x1)

find_pivot:
	addi x2, x2, -8
	sd x1, 0(x2)  # store return address of caller function
	slli x23, x18, 3 # x23 = x18 * 3
	add x23, x10, x23  # x23 = mem address of arr[low]
	ld x24, 0(x23)  # x24 = arr[low]
	slli x25, x19, 3  # x25 = x19 * 3
	add x25, x10, x25  # x25 = mem address of arr[high]
	ld x26, 0(x25)  # x26 = arr[high]
	add x27, x18, x19
	srli x27, x27, 1  # x27 = (low+high)/2
	slli x27, x27, 3  # x27 = x27 * 8
	add x27, x10, x27  # x27 = mem address of arr[mid]
	ld x28, 0(x27)  # x28 = arr[mid]
	bge x24, x28, else # if arr[mid] <= arr[low] goto else
	blt x28, x26, if_one  # if arr[mid] < arr[high] goto if_one
	blt x26, x24, else_one  # if arr[high] < arr[low] goto else_one
	add x30, x27, x0
	add x31, x25, x0
	jal x1, swap  # swap(mid, high)
	add x30, x27, x0
	addi x31, x25, -8
	jal x1, swap  # swap(mid, high-1)
if_one:
	add x30, x27, x0  
	addi x31, x25, -8
	jal x1, swap  # swap(mid, high-1)
else_one:
	add x30, x27, x0
	add x31, x25, x0
	jal x1, swap  # swap(mid, high)
	add x30, x23, x0
	add x31, x27, x0
	jal x1, swap  # swap(low, mid)
	add x30, x27, x0
	addi x31, x25, -8
	jal x1, swap  # swap(mid, high-1)

else:
	blt x24, x26, if_two  # if arr[low] < arr[high] goto if_two
	blt x26, x28, else_two  # if arr[high] < arr[mid] goto else_two
	add x30, x23, x0
	add x31, x27, x0
	jal x1, swap  # swap(low, mid)
	add x30, x27, x0
	add x31, x25, x0
	jal x1, swap  # swap(mid, high)
	add x30, x27, x0
	addi x31, x25, -8
	jal x1, swap  # swap(mid, high-1)
if_two:
	add x30, x23, x0
	add x31, x27, x0
	jal x1, swap  # swap(low, mid)
	add x30, x27, x0  
	addi x31, x25, -8
	jal x1, swap  # swap(mid, high-1)
else_two:
	add x30, x23, x0
	add x31, x25, x0
	jal x1, swap  # swap(low, high)
	add x30, x27, x0
	addi x31, x25, -8
	jal x1, swap  # swap(mid, high-1)
	ld x1, 0(x2)  # load return address of caller function
	addi x2, x2, 8  # clear stack
	jalr x0, 0(x1)  # return to caller function

swap:
	# x = x30, y = x31, tmp = x7
	#add x30, x10, x30  # x30 = address of arr[x]
	ld x28, 0(x30)  # x28 = arr[x]
	#add x31, x10, x31  # x31 = address of arr[y]
	ld x29, 0(x31)  # x29 = arr[y]
	add x22, x28, x0  # copy x28 to x30 (tmp = arr[x])
	sd x29, 0(x30)  # arr[x] = arr[y]
	sd x22, 0(x31)  # arr[y] = tmp
	add x30, x0, x0
	add x31, x0, x0

	jalr x0, 0(x1)  # return to caller
	

# void insertion_sort(int64_t* array, size_t size);
insertion_sort:
	addi x2, x2, -16  # make room for two element
	sd x10, 0(x2)  # store x10
	sd x11, 8(x2)  # store x11
	
	slli x28, x18, 3 
	add x28, x10, x28  # x28 = mem address of arr[low]
	add x10, x28, x0  # x10 = array
	add x11, x27, x0  # x11 = size
	# initialize i = 1
	addi x5, x0, 1  

OUTER_LOOP:
	# if i>= size END program
	bge x5, x11, END_ins_sort  
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
	jal x0, L1  
PASS:
	# array[j] = array[j-1]
	slli x29, x6, 3  
	add x30, x29, x10
	sd x24, 0(x30)  
	# j--
	addi x6, x6, -1
	jal x0, INNER_LOOP	
L1:
	# array[i] = element(x23)
	slli x29, x6, 3
	add x30, x29, x10
	sd x23, 0(x30)  
	# i++
	addi x5, x5, 1
	jal x0, OUTER_LOOP
FLAG:
	# if array[j-1] <= element goto L1, if-not goto PASS
	bge x23, x24, L1
	jal x0, PASS
	
END_ins_sort:
	ld x10, 0(x2)
	ld x11, 8(x2)
	addi x2, x2, 16  # clear stack
	jal x0, partition  # return to caller function
end:
	#addi x2, x2, -96
	ld x8, 88(x2)
	ld x9, 80(x2)
	ld x18, 72(x2)
	ld x19, 64(x2)
	ld x20, 56(x2)
	ld x21, 48(x2)
	ld x22, 40(x2)
	ld x23, 32(x2)
	ld x24, 24(x2)
	ld x25, 16(x2)
	ld x26, 8(x2)
	ld x27, 0(x2) 
	addi x2, x2, 96
