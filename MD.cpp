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
