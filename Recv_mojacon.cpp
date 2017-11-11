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
