
#include "mbed.h"
#include "math.h"

#define PI (3.1415926)
#define M_ADD_1 (0x10)
#define M_ADD_2 (0x12)
#define M_ADD_3 (0x14)
#define AIR_ADDR (0x16)
#define LCD_ADDR (0x7C)
#define ULTRA1_ADD_W (0xE0)
#define ULTRA1_ADD_R (0xE1)
#define ULTRA2_ADD_W (0xE2)
#define ULTRA2_ADD_R (0xE3)
#define ULTRA3_ADD_W (0xE4)
#define ULTRA3_ADD_R (0xE5)
#define ULTRA4_ADD_W (0xE6)
#define ULTRA4_ADD_R (0xE7)

DigitalIn LeftPi(PA_6);
DigitalIn RightPi(PA_7);
I2C i2c(PB_9,PB_8);
Serial pc(USBTX,USBRX);
Serial Pic(PA_11,PA_12);
Serial xbee(PA_9,PA_10);
DigitalOut led1(LED1);
int CorrectionJyro = 0;

///////////////////////////////////mojacon//////////////////////////////////////
char work[4];
short value=0;
int many=0,
    cmd = 0,
    type=0,
    firstvalue=0;
volatile int gDegree=0,
             gDuty=0,
             gTurnSpeed=0,
             Up = 0,
             Down = 0,
             Left = 0,
             Right = 0,
             Triangle = 0,
             Cross = 0,
             Square = 0,
             Circle = 0;
void data(){
    if(many<4) {
        work[many]=xbee.getc();
        many++;
    } else {
        if(work[0]==0xff && work[1]==0xff && work[2]==0 && work[3]==0) {
            if(type==0) {
                type=xbee.getc();
            } else {
                if(firstvalue==0) {
                    value=xbee.getc();
                    value=value<<8;
                    firstvalue++;
                } else {
                    value=value|xbee.getc();
                    switch(type) {
                        case 1 :
                            gDuty=value;
                            if(gDuty<10){
                                gDuty=0;
                            }
                            if(gDuty > 127){
                                gDuty = 127;    
                            }
                            break;
                        case 2 :
                            gDegree=value;
                            break;
                        case 3 :
                            gTurnSpeed=value;
                            if(gTurnSpeed<=1&&gTurnSpeed>=-1){
                                gTurnSpeed=0;
                            }
                            if(gTurnSpeed > 60){
                                gTurnSpeed = 60;
                            }
                            if(gTurnSpeed < -60){
                                gTurnSpeed = -60;
                            }
                            break;
                        case 4 : Up = value; cmd = type; break;
                        case 5 : Down = value; cmd = type; break;
                        case 6 : Left = value; cmd = type; break;
                        case 7 : Right = value; cmd = type; break;
                        case 8 : Triangle = value; cmd = type; break;
                        case 9 : Cross = value; cmd = type; break;
                        case 10 : Square = value; cmd = type; break;
                        case 11 : Circle = value; cmd = type; break;
                        }
                    if(gDuty < 10){
                        gDegree = 0;
                    }
                    many=0;
                    type=0;
                    firstvalue=0;
                }
            }
        } else {
            work[0]=work[1];
            work[1]=work[2];
            work[2]=work[3];
            work[3]=xbee.getc();
        }
    }
}
////////////////////////////////////////////////////////////////////////////////

class UME_PID{
    private:
        double Kp,   //Proportion
              Ki,   //Integral
              Kd,   //differential
              En,   //now
              En_1, //before
              En_2, //more before
              b;
    public:
        UME_PID(double, double, double);
        double get_pid(double, double);
};
UME_PID::UME_PID(double p, double i, double d){
    Kp = p;
    Ki = i;
    Kd = d;
}
double UME_PID::get_pid(double target, double now){
    En = target - now;
    b = Kp * (En - En_1) + Ki * En + Kd * ((En - En_1) - (En_1 - En_2));
    En_1 = En;
    En_1 = En_2;
    return b;
}
UME_PID spin(0.3, 0.2, 0.01);
UME_PID set(0.4 ,0.3 ,0.4);
UME_PID side(0.4 ,0.3 ,0.4);
////////////////////////////////////////////////////////////////////////////////

class PIC{
    private:
        char i[2],
             l;
        int jyroc;
    public:
        PIC();
        int getj();
};
PIC::PIC(){}
int PIC::getj(){
    if(l < 2){
        i[l] = Pic.getc();
        l++;
    }
    while(1){
            if(i[0] == 0xAA && i[1] == 0xBB){
            jyroc = Pic.getc();
            jyroc <<= 8;
            jyroc |= Pic.getc();
            if(jyroc > 0x8000){
                jyroc = -(0xFFFF - jyroc);
            }
            if(jyroc > 360 || jyroc < -360){
                jyroc = jyroc % 360;
            }
            l = 0;
            i[0] = 0;
            i[1] = 0;
            return jyroc;
        }
        else{
            i[0] = i[1];
            i[1] = Pic.getc();
        }
    }
}
PIC pic;
////////////////////////////////////////////////////////////////////////////////

class MD{
    private:
        signed char m[3],
                    z,
                    max,
                    pid,
                    memo[3];
        char m1[1],
             m2[1],
             m3[1],
             memo1,
             memo2,
             memo3;
    public:
        MD();
        void drive(int, int, int, int, int);
};
MD::MD(){}
void MD::drive(int deg, int power, int turn, int jyro, int keep){
    jyro -= CorrectionJyro;
    if(turn < 0){
        z = -turn;
    }
    else z = turn;
    m[0] = sin((deg - 240 + keep) * PI / 180) * power;
    m[1] = sin((deg - 120 + keep) * PI / 180) * power;
    m[2] = sin((deg - 0 + keep) * PI / 180) * power;
    if(m[0] < 0)memo[0] = -m[0];
    else memo[0] = m[0];
    if(m[1] < 0)memo[1] = -m[1];
    else memo[1] = m[1];
    if(m[2] < 0)memo[2] = -m[2];
    else memo[2] = m[2];
    memo[0] > memo[1] ? max = memo[0] : max = memo[1];
    max > memo[2] ? max = max : max = memo[2];
    m[0] = (m[0] * power / max) * power / (power + z) + turn * z / (power + z);
    m[1] = (m[1] * power / max) * power / (power + z) + turn * z / (power + z);
    m[2] = (m[2] * power / max) * power / (power + z) + turn * z / (power + z);
    pid = spin.get_pid(keep, jyro);
    if(pid > 127) pid = 127;
    if(pid < -127) pid = -127;
    if(pid < 20 && pid > 3) pid = 20;
    if(pid < -3 && pid > -20) pid = -20;
    if(pid <= 3 && pid >= -3) pid = 0;
    if(m[0] < 0)memo[0] = -m[0];
    else memo[0] = m[0];
    if(m[1] < 0)memo[1] = -m[1];
    else memo[1] = m[1];
    if(m[2] < 0)memo[2] = -m[2];
    else memo[2] = m[2];
    memo[0] > memo[1] ? max = memo[0] : max = memo[1];
    max > memo[2] ? max = max : max = memo[2];
    if(pid < 0) z = -pid;
    else z = pid;
    m[0] = m[0] * max / (max + z) + pid * z / (max + z);
    m[1] = m[1] * max / (max + z) + pid * z / (max + z);
    m[2] = m[2] * max / (max + z) + pid * z / (max + z);
    //pc.printf("%d\t%d\t%d\t%d\n",m[0],m[1],m[2],pid);
    m1[0] = m[0];
    m2[0] = m[1];
    m3[0] = m[2];
    //m1[0] = (m[0] * 8 + memo1 * 2) / 10;
    //m2[0] = (m[1] * 8 + memo2 * 2) / 10;
    //m3[0] = (m[2] * 8 + memo3 * 2) / 10;
    memo1 = m1[0];
    memo2 = m2[0];
    memo3 = m3[0];
    i2c.write(M_ADD_1, m1, 1, 0);
    i2c.write(M_ADD_2, m2, 1, 0);
    i2c.write(M_ADD_3, m3, 1, 0);
}
MD omni;
////////////////////////////////////////////////////////////////////////////////

class JIDOU{
    private:
        float mae;
        char cut;
        int power,
            length,
            deg,
            before1,
            before2,
            before3,
            before4;
    public:
        JIDOU();
        bool Start(int, int, int, int, int);
        void Side(int, int, int, int, int);
        void Back(int, int, int, int, int);
        void aim(int);
};
JIDOU::JIDOU(){}
bool JIDOU::Start(int jyro, int ultra1, int ultra2, int ultra3, int ultra4){
    unsigned int target = 190,
                 ultra0 = (ultra1 + ultra2) / 2,
                 targetl = target + 130;
    cut = 0;
    length = (ultra3 + ultra4) / 2;
    if(cmd == 8) return (false);
    while(ultra1 < target){
        omni.drive(-93, 40, 0, jyro, 0);
        if(cmd == 8) return (false);
    }
    for(int i = 0; i < 8; i++){
        omni.drive(-30, 60, 0, jyro, 0);
        if(cmd == 8) return (false);
    }
    while(length > target){
        omni.drive(-30, 30, 0, jyro, 0);
        if(cmd == 8) return (false);
    }
    while(cut < 5){
        deg = (int)atan2f((targetl - length), (ultra2 - 450));
        power = (int)sqrt(pow(targetl - length, 2.0) + pow(ultra2 - 450, 2.0));
        power = set.get_pid(0, power);
        if(power > 127) power = 127;
        if(power < 7 && power > 1) power = 7;
        if(power <= 1) power = 0;
        omni.drive(deg, power, 0, jyro, 0);
        if(length <= targetl + 3 && length >= targetl - 3 && ultra1 <= ultra0 + 3 && ultra1 >= ultra0 - 3 && ultra2 <= ultra0 + 3 && ultra2 >= ultra0 - 3){
            cut++;
        }
        if(cmd == 8) return (false);
    }
    return (true);
}
void JIDOU::Side(int jyro, int ultra1, int ultra2, int ultra3, int ultra4){
    //1:right
    //2:left
    //3:left back
    //4:right back
    //int ultra0 = (ultra1 + ultra2) / 2;
    long int ultra0 = ultra1;
    float pid = 0;
    pid = set.get_pid((double)300, (double)ultra0);
    deg = -90;
    if(pid < 0){
        pid = -pid;
        deg = 90;
    }
    if(pid > 127) pid = 127;
    if(pid < 6 && pid > 2) pid = 6;
    if(pid <= 2) pid = 0;
    if(ultra0 <=  300 + 5 && ultra0 >= 300 - 5){
        pid = 0;
    }
    omni.drive(deg, pid, 0, jyro, 0);
}
void JIDOU::Back(int jyro, int ultra1, int ultra2, int ultra3, int ultra4){
    int ultra0 = ultra3;
    float pid = 0;
    pid = set.get_pid(30, (float)ultra0);
    deg = 0;
    if(pid < 0){
        pid = -pid;
        deg = 180;
    }
    if(pid > 127) pid = 127;
    if(pid < 6 && pid > 2) pid = 6;
    if(pid <= 2) pid = 0;
    if(ultra0 <=  30 + 3 && ultra0 >= 30 - 3){
        pid = 0;
    }
    //pc.printf("%d\tTd\n",ultra3,ultra4);
    omni.drive(deg, pid, 0, jyro, 0);
}
void JIDOU::aim(int jyro){/*
    if(LeftPi){
        if(RightPi){
            CorrectionJyro = jyro;
            while(jyro < 30){
                omni.drive(0, 0, -30, jyro, 0);
            }
            while(jyro > 30){
                omni.drive(0, 0, 30, jyro, 0);
            }
        }
    }*/
}
JIDOU Auto;//3946
////////////////////////////////////////////////////////////////////////////////

class CONTROL{
    private:
        char air[1],
             i;
    public:
        CONTROL();
        void drive(int, int, int, int, int);
};
CONTROL::CONTROL(){}
void CONTROL::drive(int jyro, int ultra1, int ultra2, int ultra3, int ultra4){
    switch(cmd){
        case 4 : air[0] = 0x04; i2c.write(AIR_ADDR, air, 1, 0); break;      //up
        case 5 : Auto.Back(jyro, ultra1, ultra2, ultra3, ultra4); break;    //down
        case 6 : air[0] = 0x01; i2c.write(AIR_ADDR, air, 1, 0); break;      //left
        case 7 : air[0] = 0x02; i2c.write(AIR_ADDR, air, 1, 0); break;      //right
        case 8 : omni.drive(gDegree, gDuty, gTurnSpeed, jyro, 0); break;    //triangle
        //case 9 : CorrectionJyro = jyro; break;                            //cross
        case 10 : Auto.aim(jyro); break;                              //square
        case 11 : NVIC_SystemReset(); break;   //circle
        default : omni.drive(gDegree, gDuty, gTurnSpeed, jyro, 0); break;
    }
}
CONTROL mojacon;
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
////////////////////////////////////////////////////////////////////////////////

void lcd_init(){
    char init[9] = {0x38, 0x39, 0x14, 0x70, 0x56, 0x6C, 0x38, 0x0C, 0x01};
    i2c.write(LCD_ADDR, init, 9, 0);
}

void send_cmd(char a){
    char send[2] = {0, a};
    i2c.write(LCD_ADDR, send, 2, 0);
}

void send_char(char a){
    char send[2] = {0x40, a};
    i2c.write(LCD_ADDR, send, 2, 0);
}

void send_mess(char a){
    switch(a){
        case 0 : send_char(0x30); break;
        case 1 : send_char(0x31); break;
        case 2 : send_char(0x32); break;
        case 3 : send_char(0x33); break;
        case 4 : send_char(0x34); break;
        case 5 : send_char(0x35); break;
        case 6 : send_char(0x36); break;
        case 7 : send_char(0x37); break;
        case 8 : send_char(0x38); break;
        case 9 : send_char(0x39); break;
        case 10 : send_char(0x2E); break; //.
        case 11 : send_char(0x2C); break; //-
        case 12 : send_char(0x45); send_char(0x72); send_char(0x72); send_char(0x6F); send_char(0x72); break; //Error
        case 13 : send_char(0x41); send_char(0x75); send_char(0x74); send_char(0x6F); break; //Auto
        case 14 : send_char(0x4D); send_char(0x61); send_char(0x6E); send_char(0x75); send_char(0x61); send_char(0x6C); break; //Manual
        case 15 : send_char(0x3C); send_char(0x28); send_char(0x20); send_char(0x8E); send_char(0x20); send_char(0x29); send_char(0x2F); break; //L(A)>
        case 16 : send_char(0xCD); send_char(0x28); send_char(0x20); send_char(0x8E); send_char(0x20); send_char(0x29); send_char(0x3E); break; //L(A)>
    }
}

void send_mess_KAI(char words[], char cnt){
    for(int i = 0; i < cnt; i++){
        switch(words[i]){
            
            case 'a': send_char(0x61); break;
            case 'b': send_char(0x62); break;
            case 'c': send_char(0x63); break;
            case 'd': send_char(0x64); break;
            case 'e': send_char(0x65); break;
            case 'f': send_char(0x66); break;
            case 'g': send_char(0x67); break;
            case 'h': send_char(0x68); break;
            case 'i': send_char(0x69); break;
            case 'j': send_char(0x6A); break;
            case 'k': send_char(0x6B); break;
            case 'l': send_char(0x6C); break;
            case 'm': send_char(0x6D); break;
            case 'n': send_char(0x6E); break;
            case 'o': send_char(0x6F); break;
            case 'p': send_char(0x70); break;
            case 'q': send_char(0x71); break;
            case 'r': send_char(0x72); break;
            case 's': send_char(0x73); break;
            case 't': send_char(0x74); break;
            case 'u': send_char(0x75); break;
            case 'v': send_char(0x76); break;
            case 'w': send_char(0x77); break;
            case 'x': send_char(0x78); break;
            case 'y': send_char(0x79); break;
            case 'z': send_char(0x7A); break;
            
            case 'A': send_char(0x41); break;
            case 'B': send_char(0x42); break;
            case 'C': send_char(0x43); break;
            case 'D': send_char(0x44); break;
            case 'E': send_char(0x45); break;
            case 'F': send_char(0x46); break;
            case 'G': send_char(0x47); break;
            case 'H': send_char(0x48); break;
            case 'I': send_char(0x49); break;
            case 'J': send_char(0x4A); break;
            case 'K': send_char(0x4B); break;
            case 'L': send_char(0x4C); break;
            case 'M': send_char(0x4D); break;
            case 'N': send_char(0x4E); break;
            case 'O': send_char(0x4F); break;
            case 'P': send_char(0x50); break;
            case 'Q': send_char(0x51); break;
            case 'R': send_char(0x52); break;
            case 'S': send_char(0x53); break;
            case 'T': send_char(0x54); break;
            case 'U': send_char(0x55); break;
            case 'V': send_char(0x56); break;
            case 'W': send_char(0x57); break;
            case 'X': send_char(0x58); break;
            case 'Y': send_char(0x59); break;
            case 'Z': send_char(0x5A); break;
            default : send_char(0x45); send_char(0x72); send_char(0x72); send_char(0x6F); send_char(0x72); break; //Error
        }
    }
}
////////////////////////////////////////////////////////////////////////////////

int main(){
    pc.baud(57600);
    Pic.baud(9600);
    xbee.baud(57600);
    xbee.format(8,Serial::None,1);
    xbee.attach(data,Serial::RxIrq);
    i2c.frequency(400000);
    //lcd_init();
    char zero[1] = {0};
    i2c.write(AIR_ADDR, zero, 1, 0);
    int ultra1 = 0,
        ultra2 = 0,
        ultra3 = 0,
        ultra4 = 0,
        before1 = 0,
        before2 = 0,
        before3 = 0,
        before4 = 0,
        jyro = 0,
        flag  = 0;
    signed char cnt_u = 0;
         //word[] = "Sampling",
    while(1){
        jyro = -pic.getj();
        if(cmd == 9){
            CorrectionJyro = jyro;
        }
        else if(cmd == 10){
            CorrectionJyro = 0;
        }
        led1 = LeftPi;
        ultra(&ultra1, &ultra2, &ultra3, &ultra4, 3);
        ultra(&ultra1, &ultra2, &ultra3, &ultra4, 4);
        //ultra(&ultra1, &ultra2, &ultra3, &ultra4, 5);
        if(ultra3 < 10) ultra3 = 800;
        //if(ultra4 < 10) ultra4 = 800;
        ultra3 = (ultra3 * 8 + before3 * 2) / 10;
        //ultra4 = (ultra4 * 8 + before4 * 2) / 10;
        //if((before1 - ultra1) > 100 || (ultra1 - before1) > 100) ultra1 = before1;
        //if((before2 - ultra2) > 100 || (ultra2 - before2) > 100) ultra2 = before2;
        //if((before3 - ultra3) > 100 || (ultra3 - before3) > 100) ultra3 = before3;
        //if((before4 - ultra4) > 100 || (ultra4 - before4) > 100) ultra4 = before4;
        before3 = ultra3;
        //before4 = ultra4;
        mojacon.drive(jyro, ultra1, ultra2, ultra3, ultra4);
        //Auto.Start(jyro, ultra2);
        pc.printf("%d",cmd);
    }
}
