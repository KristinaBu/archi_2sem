
#ifndef ARCHIVATOR2SEMESTR_READ_DICT_H
#define ARCHIVATOR2SEMESTR_READ_DICT_H

#include <fstream>
#include <cmath>


class Read_dict{
public:
    unsigned long long int read_elem;
    unsigned long long int read_code_int;
    char *read_code_char;
    int read_code_size;


    static int decode(std::ofstream &decfile, std::ifstream &readfile, int size_readDict, Read_dict readDict[], int bit_interval, unsigned int len_text){
        char symbol='0';
        //переменная, в которую будет записываться побитово считанный из закодированного текста symbol
        unsigned long long int Read=0;
        //число битов, записанных в Read
        int str_read_counter=0;
        //число расшифрованных символов. Переменная в конце работы функции будет иметь такое же значение, как и длина исходного текста
        int l=0;
        //эта переменная будет заполняться дешифрованными битами и печататься в декодированный файл
        char byte_print;
        int current_byte_print=0;
        while(!readfile.eof()){
            //в Read побитово записывается symbol
            symbol = readfile.get();
            for(int j=0; j < 8; j++){
                //Read сопоставляется со словарем кодируемых элементов
                Read<<=1;
                str_read_counter++;
                Read += ((symbol& 0b10000000)>>(7));
                symbol<<=1;
                for (int i = 0; i < size_readDict; i++) {
                    if(str_read_counter==readDict[i].read_code_size && Read==readDict[i].read_code_int){
                        Read = readDict[i].read_elem;
                        //в byte_print записывается исходный декодируемый битовый интервал
                        for (int k = 0; k < bit_interval; k++) {
                            byte_print<<=1;
                            byte_print+=(Read & (unsigned long long)pow((double)2,(double)(bit_interval-1-k)))>>(bit_interval-1-k);
                            current_byte_print++;
                            if(current_byte_print%8==0){
                                decfile << byte_print;
                                //при выводе символа byte_print длина дешифрованного текста увеличивается на 1
                                byte_print=0;
                                l++;
                                //если длина декодируемого текста равна длине исходного текста, функция прекращает работу
                                if(l==len_text){
                                    printf("%d",l);
                                    return 0;
                                }
                            }
                        }
                        str_read_counter=0;
                        Read=0;
                    }
                }
            }
        }
        printf("%d",l);
    }

};


#endif //ARCHIVATOR2SEMESTR_READ_DICT_H
