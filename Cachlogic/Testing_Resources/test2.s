main:
  addi $8, $0, 0xa1
  addi $9, $0, 0xa2
  addi $10, $0, 0xa3
  addi $11, $0, 0xa4
  addi $12, $0, 0xa5
  addi $13, $0, 0xa6
  addi $14, $0, 0xa7
  addi $15, $0, 0xa8
  addi $16, $0, 0xa9
  addi $17, $0, 0xaa

  addi $25, $0, 0x10010000
  sw   $8, 0($25)
  sw   $9, 4($25)
  sw   $10, 8($25)
  sw   $11, 12($25)
  sw   $12, 16($25)
  sw   $13, 20($25)
  sw   $14, 24($25)
  sw   $15, 28($25)
  sw   $16, 32($25)
  sw   $17, 36($25)

  lw    $12, 0($25)
  lw    $13, 4($25)
  lw    $14, 8($25)
  lw    $15, 12($25)
  lw    $16, 16($25)
  lw    $17, 20($25)
  lw    $18, 24($25)
  lw    $19, 28($25)
  lw    $20, 32($25)
  lw    $21, 36($25)
