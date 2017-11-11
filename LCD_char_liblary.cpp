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
