#define JYRO_ADD_W (0xD4)
#define JYRO_ADD_R (0xD5)
I2C i2c(PB_9,PB_8);
int j_memo;
char 
     receive[1] = {0},
     write[1] = {0x20},
     write2[1] = {0x0F};
float degree = 0,
      memo_deg = 0,
      before = 0,
      j_deg = 0,
      J_deg = 0,
      add = 0,
      a = 0,
      memo_d = 0,
      time = 0;

while(j_deg != 0xD4){
    i2c.write(JYRO_ADD_W, write2, 1, 0);
    i2c.read(JYRO_ADD_R, receive, 1, 0);
    j_deg = receive[0];
}
i2c.write(JYRO_ADD_W, write, 1, 1);
i2c.write(JYRO_ADD_W, write2, 1, 0);
while(i < 1001){
    write[0] = 0x2D;
    i2c.write(JYRO_ADD_W, write, 1, 1);
    i2c.read(JYRO_ADD_R, receive, 1, 0);
    j_memo = receive[0];
    j_memo <<= 8;
    write[0] = 0x2C;
    i2c.write(JYRO_ADD_W, write, 1, 1);
 
    i2c.read(JYRO_ADD_R, receive, 1, 0);
    j_memo |= receive[0];
    timer.stop();
    time = timer.read();
    j_deg = j_memo;
    if(j_deg > 0x8000){
        j_deg = -(0xFFFF - j_deg);
    }
    j_deg *= 0.00875;
    J_deg += (j_deg + before) * time / 2;
    before = j_deg;
    add += time;
    i++;
    timer.reset();
    timer.start();
}
a = J_deg / add;
    
while(1){
    write[0] = 0x2D;
    i2c.write(JYRO_ADD_W, write, 1, 1);
    i2c.read(JYRO_ADD_R, receive, 1, 0);
    j_memo = receive[0];
    j_memo <<= 8;
    write[0] = 0x2C;
    i2c.write(JYRO_ADD_W, write, 1, 1);
    i2c.read(JYRO_ADD_R, receive, 1, 0);
    j_memo |= receive[0];
    timer.stop();
    time = timer.read();
    j_deg = j_memo;
    if(j_deg > 0x8000){
        j_deg = -(0xFFFF - j_deg);
    }
    j_deg = j_deg * 0.00875;
    before = j_deg;
    J_deg += ((j_deg + before) * time / 2 - time * a) * 90 / 80;
    if(J_deg > 360 || J_deg < -360){
        memo_d = (int)J_deg / 360;
        J_deg -= (360 * memo_d);
    }
    if(J_deg - memo_deg < 0.13 && J_deg - memo_deg > -0.13){
        J_deg = memo_deg;
    }
    memo_deg = J_deg;
    timer.reset();
    timer.start();
}
