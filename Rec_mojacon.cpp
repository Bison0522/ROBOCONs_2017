class CONTROL{
    private
        char air[1],
             i;
    public
        CONTROL();
        void drive(int, int, int, int, int);
};
CONTROLCONTROL(){}
void CONTROLdrive(int jyro, int ultra1, int ultra2, int ultra3, int ultra4){
    switch(cmd){
        case 4  air[0] = 0x04; i2c.write(AIR_ADDR, air, 1, 0); break;      up
        case 5  Auto.Back(jyro, ultra1, ultra2, ultra3, ultra4); break;    down
        case 6  air[0] = 0x01; i2c.write(AIR_ADDR, air, 1, 0); break;      left
        case 7  air[0] = 0x02; i2c.write(AIR_ADDR, air, 1, 0); break;      right
        case 8  omni.drive(gDegree, gDuty, gTurnSpeed, jyro, 0); break;    triangle
        case 9  CorrectionJyro = jyro; break;                            cross
        case 10  CorrectionJyro = 0; break;                              square
        case 11  NVIC_SystemReset(); break;   circle
        default  omni.drive(gDegree, gDuty, gTurnSpeed, jyro, 0); break;
    }
}
