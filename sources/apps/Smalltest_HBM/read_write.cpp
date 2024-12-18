#include "instruction.h"
#include "prog.h"
#include "platform.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <list>

// A 4 GB per stack device has 4 Gb per channel, and each channel has
// two 2 Gb or 256 MB pseudo channels.

// THIS LOOKS LIKE THE CORRECT ORGANIZATION:
// 32 byte per column x 32 (cols) x 16K (rows) x 16 (banks) x 2 PC x 8 channels

using namespace std;

#define USE_SMC_FILE 0

#define PC_UNDER_TEST 1

#define NUM_CH 8
#define NUM_PC 2
#define NUM_BANKS 16
#define NUM_ROWS 1024*16
#define NUM_COLS 32
#define BYTES_PER_READ 32
#define ROW_SIZE (NUM_COLS*BYTES_PER_READ)

#define NUM_ROWS_REG  8
#define NUM_BANKS_REG 11
// Stride register ids are fixed and should not be changed
// CASR should always be reg 0
// BASR should always be reg 1
// RASR should always be reg 2
#define CASR 0
#define BASR 1
#define RASR 2

#define PATTERN_REG 12

#define BAR 7
#define RAR 6
#define CAR 4

#define NUM_COLS_REG 14
#define LOOP_COLS 13

#define NUM_CH_REG 3
#define LOOP_CH 5

#define TEMP_PATTERN_REG 15

/**
 * @return an instruction formed by NOPs
 */
Inst all_nops()
{
  return  __pack_mininsts(SMC_NOP(), SMC_NOP(), SMC_NOP(), SMC_NOP());
}

int main()
{
  SoftMCPlatform platform(false);
  int err;
  uint32_t wr_pattern;
  // buffer allocated for reading data from the board
  uint8_t buf[ROW_SIZE * 2]; // * 2 because of PCs

  // Initialize the platform, opens file descriptors for the board PCI-E interface.
  if((err = platform.init()) != SOFTMC_SUCCESS){
      cerr << "Could not initialize SoftMC Platform: " << err << endl;
  }
  // reset the board to hopefully restore the board's state
  platform.reset_fpga();

  Program program;
  /* SoftMC programs are formed sequentially by adding
    * instructions to the program one by one. Instructions
    * can be added to the program using add_inst().
    */

  program.add_inst(SMC_LI(NUM_ROWS, NUM_ROWS_REG)); // load NUM_ROWS into NUM_ROWS_REG
  program.add_inst(SMC_LI(NUM_BANKS, NUM_BANKS_REG)); // load NUM_BANKS into NUM_BANKS_REG
  program.add_inst(SMC_LI(NUM_CH, NUM_CH_REG)); // load NUM_CH into NUM_CH_REG
  
  // Stride registers are used to increment the row,bank,column registers without
  // using regular (non-DDR) instructions. This way the DRAM array can be traversed much faster

  program.add_inst(SMC_LI(1, CASR)); // Load 1 into CASR since each READ reads 1 column but also skip one because it is another PC
  program.add_inst(SMC_LI(1, BASR)); // Load 1 into BASR
  program.add_inst(SMC_LI(1, RASR)); // Load 1 into RASR

  // Initialize PRNG, this test will randomly generate
  // the first data to write into DRAM, it will modify
  // the data by cyclic shifting and multiplying it with itself
  // during runtime (this will be further explained down below).
  srand(time(NULL)+getpid());
  uint16_t rand1 = (uint16_t) rand();
  wr_pattern = rand1 << 16;
  wr_pattern |= ((uint16_t)rand());
  printf("wr_pattern: %x\n", wr_pattern);

  program.add_inst(SMC_LI(0, LOOP_CH)); // load NUM_CH into NUM_CH_REG

  program.add_label("CH_BEGIN");

  // Load WR data into pattern register whose content
  // will be replicated among all words in the wide register
  // Wide register is a 512-bit register, its content will
  // be written to DRAM with SMC_WR commands.
  program.add_inst(SMC_LI(wr_pattern, PATTERN_REG)); 
  for(int i = 0 ; i < 16 ; i++)
    program.add_inst(SMC_LDWD(PATTERN_REG,i));

  program.add_inst(SMC_LI(0, BAR)); // Initialize BAR (bank address register) with 0

  // add_label is used to add a label that can be used as a branch target to the program
  program.add_label("BANK_BEGIN");

  program.add_inst(SMC_LI(0, RAR)); // Initialize RAR with 0
  
  program.add_label("ROW_BEGIN");

  program.add_inst(SMC_LI(0, CAR)); // Initialize CAR with 0

  // Four DRAM commands are issued at the same time
  // to DRAM by SoftMC.
  
  // PREcharge bank BAR and wait for tRP (11 NOPs, 11 SoftMC cycles) 
  program.add_inst(
    SMC_PRE(BAR, 0, 0, PC_UNDER_TEST),
    SMC_NOP(), SMC_NOP(), SMC_NOP()
  );
  program.add_inst(all_nops());
  program.add_inst(all_nops());
  program.add_inst(all_nops());
  program.add_inst(all_nops());
  
  // ACT & wait for tRCD
  program.add_inst(
    SMC_ACT(BAR, 0, RAR, 0, PC_UNDER_TEST),
    SMC_NOP(), SMC_NOP(), SMC_NOP()
  );
  program.add_inst(all_nops());
  program.add_inst(all_nops());
  program.add_inst(all_nops());
  program.add_inst(all_nops());

  // We are now going to loop over all 512-bits in the row
  // Since each row is 2^13 bytes we need to WR 128 times
  program.add_inst(SMC_LI(NUM_COLS,NUM_COLS_REG));  // Load COL_SIZE register
  program.add_inst(SMC_LI(0,LOOP_COLS));   // Load loop variable

  /*
    * Try to write a different pattern to each cache line
    * in DRAM.
    */
  program.add_label("WR_BEGIN");

  // Write to a whole row, increment CAR per WR
  program.add_inst(
    SMC_WRITE(BAR, 0, CAR, 1, PC_UNDER_TEST, 0),
    SMC_NOP(), SMC_NOP(), SMC_NOP()
  );    
  
  // Reload wide reg with the new data pattern
  program.add_inst(SMC_SRC(PATTERN_REG,PATTERN_REG));
  for(int i = 0 ; i < 16 ; i++)
    program.add_inst(SMC_LDWD(PATTERN_REG,i));
  
  program.add_inst(SMC_ADDI(LOOP_COLS,1,LOOP_COLS));
  // add a branch lower instruction which will jump to WR_BEGIN
  // as long as LOOP_COLS<NUM_COLS, to iterate over the whole row
  program.add_branch(program.BR_TYPE::BL,LOOP_COLS,NUM_COLS_REG, "WR_BEGIN");
  // END "WR_BEGIN" LOOP

  // Add more randomness to the PATTERN_REG by multiplying it by 3
  // This happens when a row is written to
  program.add_inst(SMC_MV(PATTERN_REG,TEMP_PATTERN_REG));
  program.add_inst(SMC_ADD(PATTERN_REG,TEMP_PATTERN_REG,PATTERN_REG));
  program.add_inst(SMC_ADD(PATTERN_REG,TEMP_PATTERN_REG,PATTERN_REG));
  for(int i = 0 ; i < 16 ; i++)
    program.add_inst(SMC_LDWD(PATTERN_REG,i));
  
  // Wait for t(write-precharge)
  // & precharge the open bank
  program.add_inst(all_nops());
  program.add_inst(all_nops());
  program.add_inst(
    SMC_PRE(BAR, 0, 0, PC_UNDER_TEST),
    SMC_NOP(), SMC_NOP(), SMC_NOP()
  );

  program.add_inst(SMC_LI(0, CAR)); // reload CAR we are now going to read the whole row
  
  program.add_inst(all_nops());
  program.add_inst(all_nops());
  program.add_inst(all_nops());
  program.add_inst(all_nops());
  program.add_inst(all_nops());

  // ACT & wait for tRCD
  program.add_inst(
    SMC_ACT(BAR, 0, RAR, 0, PC_UNDER_TEST),
    SMC_NOP(), SMC_NOP(), SMC_NOP()
  );
  program.add_inst(all_nops());
  program.add_inst(all_nops());

  // Read from a whole row
  // Essentially this can be imagined as an unrolled read whole row loop
  for(int i = 0 ; i < 32 ; i++){
    program.add_inst(
      SMC_READ(BAR, 0, CAR, 1, PC_UNDER_TEST, 0),
      SMC_NOP(), SMC_NOP(), SMC_NOP()
    );
    program.add_inst(all_nops());
  }
  
  // Wait for t(read-precharge)
  // & precharge the open bank
  program.add_inst(all_nops());
  program.add_inst(all_nops());
  program.add_inst(
    SMC_PRE(BAR, 0, 0, PC_UNDER_TEST),
    SMC_NOP(), SMC_NOP(), SMC_NOP()
  );

  // Increment row address by one since we are finished with this row
  program.add_inst(SMC_ADDI(RAR,1,RAR));
  program.add_branch(program.BR_TYPE::BL,RAR,NUM_ROWS_REG, "ROW_BEGIN");
  program.add_inst(SMC_ADDI(BAR,1,BAR));
  program.add_branch(program.BR_TYPE::BL,BAR,NUM_BANKS_REG,"BANK_BEGIN");
  
  program.add_inst(all_nops());
  program.add_inst(all_nops());
  program.add_inst(all_nops());
  program.add_inst(all_nops());

  program.add_inst(SMC_ADDI(LOOP_CH, 1, LOOP_CH));
  program.add_branch(program.BR_TYPE::BL, LOOP_CH, NUM_CH_REG,"CH_BEGIN");

  // program.dump_registers();

  // Tells SoftMC that the user program has ended
  program.add_inst(SMC_END());

  // Transfer the program to the FPGA board
  platform.execute(program);
  printf("Program Ready\n");
  program.pretty_print();
  printf("Sent instructions\n");
  printf("Testing PC: %d\n", PC_UNDER_TEST);

  // Read count
  int rc = 0;

  uint64_t err_count = 0;  
  uint32_t pattern = wr_pattern;  

  for (int ch = 0 ; ch < NUM_CH ; ch++)
  {  
    pattern = wr_pattern;  
    // Below loop reads continuously from the FPGA board
    // since we know in what order we are issuing our reads
    // we know what physical address they are reading
    while(1)
    {
      // Read the data from the FPGA board
      platform.receiveData(buf, ROW_SIZE * 2);

      for(int j = 0 ; j < 32 ; j++){
        // printf("Whole pattern for cacheline %d: %x\n", j, pattern);
        for (int k = 0 ; k < 32 ; k++){ // each byte in cache line
          uint8_t mini_patt = pattern >> ((uint32_t)(k%4)*8);
          if(mini_patt != (uint8_t)buf[j*64 + (PC_UNDER_TEST*32) + k]){ // Skip every other 32 bytes because they come from a different PC
            err_count++;
            fprintf(stderr,"t1: Pattern mismatch! CH:%d Bank: %d Row: %d CacheLine: %d Byte: %d Expect: %x Read: %x\n", 
                  ch, rc/(1024*32), rc%(1024*32), j, k, mini_patt, (uint8_t)buf[j*64 + k]);
            //fprintf(stderr,"%d %d %d %x\n", 
              //    rc/(1024*32), rc%(1024*32), j*64 + k, mini_patt^(uint8_t)buf[j*64 + k]);
          }
        }
        //shift pattern at each row iteration
        uint32_t imd  = pattern >> (uint32_t)1;
        uint32_t imd2 = pattern << (uint32_t)31;
        pattern = imd | imd2; 
      }
      pattern = pattern*(uint32_t)3;
      rc++;
      if(rc % (NUM_ROWS) == 0)
        printf("Bank %d finished!\n", (rc/(NUM_ROWS)));
      if(rc % (NUM_ROWS * NUM_BANKS) == 0)
        break;
    }
    printf("Channel %d finished!\n", ch + 1);
  }
  printf("%ld out of %ld bytes have errors, last pattern: 0x%x\n",
    err_count, (uint64_t)NUM_CH*NUM_BANKS*NUM_ROWS*1024, pattern);
  //platform.readRegisterDump();
}
