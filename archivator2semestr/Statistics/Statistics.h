
#ifndef ARCHIVATOR2SEMESTR_STATISTICS_H
#define ARCHIVATOR2SEMESTR_STATISTICS_H

#include <cmath>
#include "map"


class Statistics{
public:
    std::map<unsigned long long int,unsigned int> dict_key_frequency;

    static void count_key(unsigned long long key, Statistics *dict){
        //если элемент есть, увеличиваем частоту
        if(dict->dict_key_frequency.count(key))     dict->dict_key_frequency[key] +=1;
            //иначе записываем новый элемент
        else dict->dict_key_frequency[key] = 1;

    }
    //функция, отвечающая за добавление в словарь символов после обработки считываемого блока
    static void new_dict(Statistics* dict, const int block_length, const char* block){
        //переменная, в которую записывается интервал битов
        unsigned long long read_block = 0;
        int current_bit = block_length;
        //массив, в котором содержатся считанные биты
        unsigned char bits[block_length*64];
        for (int i=0; i < block_length*8; i++) {
            for(int j=0; j < 8; j++)
                bits[i*8+j] = (block[i]&((int)pow(2, 7-j))) >> (7-j);
        }
        for (int i=0; i < block_length*64; i++){
            current_bit--;
            read_block += (bits[i] << current_bit);
            if (current_bit == 0){
                count_key(read_block, dict);
                read_block = 0;
                current_bit = block_length;
            }
        }
    }
};


#endif //ARCHIVATOR2SEMESTR_STATISTICS_H
