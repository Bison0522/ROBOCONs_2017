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
