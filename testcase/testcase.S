# initial pc =100
    lw $8, 4($0)   # reg[8] = 0x8000 0000
    addi $8, $8, -1 # reg[8] = 0x7FFF FFFF
    lh $9, 50($0) # reg[9] = 0xFFFF FFFF
    lb $10, 8($0) # reg[10] = 11
    add $9, $9, $8 # reg[9] = 0x7FFF FFFE
    sll $0, $0, $0
    lhu $9, 50($0) # reg[9] = 0x0000 FFFF
    lui $10, 65535 # reg[10] = 0xFFFF 0000
    srl $10, $10, 16 
    beq $10, $9, GO # reg[10] == reg[9]
    j TJ
    add $9, $9, $8
GO: jal GS
TJ: halt
    halt
    halt
    halt
TG: halt
GS: sll $13, $10, 16 # reg[13] = 0xFFFF 0000
    xor $13, $13, $9 # reg[13] = 0xFFFF FFFF
    sra $13, $13, 5
    slt $12, $13, $9 # reg[13] < reg[9] is true
    beq $12, $0, TG
    sh $12, 14($0) # d_addr 12~16 = 0xFFFF 0001
    lh $14, 14($0) # reg[14] = 0x0000 0001
    lbu $12, 16($0) # reg[12] = 0x0000 00FC
    lw $14, 12($0) # reg[14] = 0xFFFF 0001
    sub $14, $14, $12 
    addi $8, $0, -1
    addi $9, $0, -2
    sub $9, $9, $8
    nor $15, $9, $0 # reg[15] = 0x0000 0000
    slti $10, $15, -1
    ori $10, $10, -1 # reg[10] = 0x0000 FFFF
    lw $11, 12($0) # reg[11] = 0xFFFF 0001
    lui $10, 65534 # reg[10] = 0xFFFE 0000
    sra $11, $10, 16
    srl $13, $10, 16
    addi $8, $0, -1
    sub $13, $13, $8 # reg[13] = 0x0000 FFFF
    lb $14, 21($0) # reg[14] = 0x0000 0034
    nand $13, $11, $14 #reg[14] = 0x0000 FFCB
    addi $8, $0, -1 
    addi $9, $0, -2
    sub $9, $9, $8
    sw $13, 1($9) #d_addr 0~3 = 0x0000 FFCB
    add $0, $0, $0
    sub $0, $0, $0
    addi $0, $0, $0
    lw $0, 4($0)
    lh $0, 0($0)
    lb $0, 0($0)
    and $0, $8, $9
    or $0, $9, $8
    xor $0, $8, $8
    nor $0, $10, $11
    nand $0, $12, $13
    slt $0, $15, 15
    srl $0, $0, 0
    lbu $8, 16($0)
    nori $8, $9, 65535
    add $9, $8, $11
    nand $10, $11, $13
    srl $13, $14, 10
    addi $8, $0, 0
    addi $9, $0, 65534
    addi $12, $0, 65534
    andi $9, $9, 65534
    sll $12, $12, 2
    lw $7, 4($0)
    addi $7, $7, -1
    srl $7, $7, 18
HE: addi $10, $0, -1
    sub $9, $9, $10
    slt $12, $9, $7
    bne $12, $0, HE
    sll $0, $0, $0
    lw $14, 0($0)
    jal Q1
    jal Q2
    jal Q3
    jal Q4
    j Q5
Q1: jr $ra
Q2: jr $ra
Q3: jr $ra
Q4: jr $ra
Q5: lw $8, 0($8)
    addi $8, $0, $0
    addi $9, $0, $0
    jr $8 #jump to pc = 0
    lw $8, 76($0) #reg[t] = 0000 0000
    beq $12, $12, N2
N2: bne $12, $7, N3
N3: sll $0, $0, 0
    sra $12, $31, 16
    addi $13, $12, 4
    lw $0, -2($14) # 4 error at same time
    jr $ra
    done
