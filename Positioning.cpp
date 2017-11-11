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
    int ultra0 = (ultra3 + ultra4) / 2;
    float pid = 0;
    pid = set.get_pid(230, (float)ultra0);
    deg = 0;
    if(pid < 0){
        pid = -pid;
        deg = 180;
    }
    if(pid > 127) pid = 127;
    if(pid < 6 && pid > 2) pid = 6;
    if(pid <= 2) pid = 0;
    if(ultra0 <=  230 + 5 && ultra0 >= 230 - 5){
        pid = 0;
    }
    omni.drive(deg, pid, 0, jyro, 0);
}
