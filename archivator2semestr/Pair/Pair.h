
#ifndef ARCHIVATOR2SEMESTR_PAIR_H
#define ARCHIVATOR2SEMESTR_PAIR_H


#include <fstream>
#include <cmath>


class Pair{
    friend class Node;
public:
    //двоичный код элемента
    unsigned int element;
    //размер кода элемента(важно для учета незначащих нулей)
    unsigned int size_code;
    //указатель на массив, дробыщие код по одному символу {'0','0','1'}
    char *code;

    static void encode(char* bytes_to_write, int bit_interval, int len, char block[], Pair *dict, unsigned int size_dict, int *total_current, std::ofstream &out_newtext){
        unsigned int interval=0;
        int len_miniblock=64;
        unsigned int current_miniblock=0;
        //массив, который хранит (элементы) битовые интервалы
        unsigned int miniblock[len_miniblock];
        //из считанного блока текста block[len] в miniblock[len_miniblock] выделяются элементы
        //например, при интервале 2 бита: block[len] - 11100101, miniblock[len_miniblock] - 11,10,01,01
        for(int i=0;i<len_miniblock*bit_interval;i++){
            interval<<=1;
            interval+=(block[i/8] & (char)pow((double)2,(double)(7-i%8)))>>(7-i%8);
            if((i+1)%bit_interval==0){
                miniblock[current_miniblock]=interval;
                interval=0;
                current_miniblock++;
            }
        }
        //в code_elem хранится кодовое слово для элемента по Хаффману
        char code_elem[100];
        unsigned int size_code_elem=0;
        for (int q = 0; q < len_miniblock; q++){
            for (int j = 0; j < size_dict; j++) {
                //сопоставляем miniblock[len_miniblock] и словарь, чтобы записать их словаря кодовое слово, соответствующее элементу
                if(dict[j].element==miniblock[q]){
                    for (int i = 0; i < dict[j].size_code; i++) code_elem[i]=dict[j].code[i];
                    size_code_elem=dict[j].size_code;
                    break;
                }
            }
            //bytes_to_write - массив, состоящий из одного символа
            // в него побитово записывается code_elem - код по Хаффману - и выводится в файл
            for(int i=0;i<size_code_elem;i++){
                bytes_to_write[0] += (int)(code_elem[i]=='1'? 1 : 0 )*(int)pow((double)2,
                                                                               (double)(7-total_current[1]));
                total_current[1]++;
                if(total_current[1]==8){
                    total_current[1]=0;
                    total_current[0]++;
                    out_newtext << bytes_to_write[0];
                    bytes_to_write[0]=0;
                }
            }
        }
    }
};



#endif //ARCHIVATOR2SEMESTR_PAIR_H
