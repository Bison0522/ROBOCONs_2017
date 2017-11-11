////////////////////////////////////////////////////////////////////////////////

void ultra(int *ultra1, int *ultra2, int *ultra3, int *ultra4, char num){
    char write[2] = {0x00, 0x51};
    char read1[2] = {0},
         read2[2] = {0},
         read3[2] = {0},
         read4[3] = {0};
    if(num > 0 && num < 7){
        if(num == 1 || num == 5) i2c.write(ULTRA1_ADD_W, write, 2, 0);
        if(num == 2 || num == 5) i2c.write(ULTRA2_ADD_W, write, 2, 0);
        if(num == 3 || num == 5 || num == 6) i2c.write(ULTRA3_ADD_W, write, 2, 0);
        if(num == 4 || num == 5 || num == 6) i2c.write(ULTRA4_ADD_W, write, 2, 0);
        wait_ms(70);
        write[0] = 0x02;
        if(num == 1 || num == 5) i2c.write(ULTRA1_ADD_W, write, 1, 0);
        if(num == 2 || num == 5) i2c.write(ULTRA2_ADD_W, write, 1, 0);
        if(num == 3 || num == 5 || num == 6) i2c.write(ULTRA3_ADD_W, write, 1, 0);
        if(num == 4 || num == 5 || num == 6) i2c.write(ULTRA4_ADD_W, write, 1, 0);
        if(num == 1 || num == 5) i2c.read(ULTRA1_ADD_R, read1, 2, 0);
        if(num == 2 || num == 5) i2c.read(ULTRA2_ADD_R, read2, 2, 0);
        if(num == 3 || num == 5 || num == 6) i2c.read(ULTRA3_ADD_R, read3, 2, 0);
        if(num == 4 || num == 5 || num == 6) i2c.read(ULTRA4_ADD_R, read4, 2, 0);
        if(num == 1 || num == 5){
            *ultra1 = read1[0];
            *ultra1 <<= 8;
            *ultra1 |= read1[1];
        }
        if(num == 2 || num == 5){
            *ultra2 = read2[0];
            *ultra2 <<= 8;
            *ultra2 |= read2[1];
        }
        if(num == 3 || num == 5 || num == 6){
            *ultra3 = read3[0];
            *ultra3 <<= 8;
            *ultra3 |= read3[1];
        }
        if(num == 4 || num == 5 || num == 6){
            *ultra4 = read4[0];
            *ultra4 <<= 8;
            *ultra4 |= read4[1];
        }
    }
}
