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
