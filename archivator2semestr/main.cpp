#include <cstring>
#include "vector"
#include "string"
#include "Statistics/Statistics.h"

#include "Pair/Pair.h"
#include "Node/Node.h"
#include "Read_dict/Read_dict.h"
#include <cmath>
#include <clocale>
#include <iostream>
#include <fstream>


int bit_interval_=4;

int char_to_int(const char *str, int len){
    int Int=0;
    for (int i = 0; i < len; i++) {
        if(str[i]=='0') Int=Int*10;
        else if(str[i]=='1') Int=Int*10+1;
        else if(str[i]=='2') Int=Int*10+2;
        else if(str[i]=='3') Int=Int*10+3;
        else if(str[i]=='4') Int=Int*10+4;
        else if(str[i]=='5') Int=Int*10+5;
        else if(str[i]=='6') Int=Int*10+6;
        else if(str[i]=='7') Int=Int*10+7;
        else if(str[i]=='8') Int=Int*10+8;
        else if(str[i]=='9') Int=Int*10+9;
    }
    return Int;
}

int main() {
    setlocale(LC_ALL, "Russian");
    char bytes_to_write[1] = {0};
    int g_total_current[2] = {1, 0};
    Statistics new_stats;

    char symbol = '0';

    std::ifstream in_textorig;
    in_textorig.open("TextOrig.txt", std::ios::binary);
    if (!in_textorig.is_open()) { std::cerr << "Error open file1\n"; return 1; }

    unsigned int len_text = 0;
    char block[bit_interval_ * 8];
    while (!in_textorig.eof()) {
        memset(block, 0, bit_interval_ * 8);
        for (int i = 0; i < bit_interval_ * 8; i++) {

            symbol = in_textorig.get();

            len_text++;
            if (symbol == EOF) {
                break;
            }
            block[i] = symbol;
        }
        Statistics::new_dict(&new_stats, bit_interval_, block);
    }
    in_textorig.close();

    /*for (const auto& [key, frequency] : new_stats.dict_key_frequency)
        std::cout << key << "\t" << frequency << std::endl;*/

    //размер статистики
    unsigned int k = new_stats.dict_key_frequency.size();
    //размер статистика for code_tree
    unsigned int k_=k;
    Pair pair_list[k];
    //перенос статистики в pair_list
    //счетчик for для pair_list
    unsigned int i=0;

    for (const auto& [key, frequency] : new_stats.dict_key_frequency){
        pair_list[i].element = key;
        pair_list[i].code= static_cast<char *>(malloc((k_ + 1) * sizeof(char)));
        //k+1 чтобы перед мусором, который будет записываться в строку значения-кода элемента, была точно одна 2
        //например, "100112qqqjbwbqq....(мусор)"
        memset(pair_list[i].code,'2',k_+1);
        i++;
    }
    //массив узлов - для создания дерева по Хаффману
    Node *nodes= static_cast<Node *>(malloc(
            (int) ((((float) ((new_stats.dict_key_frequency.size())) * 1.0f + 1.0f) / 2.0f) *
                   (float) ((new_stats.dict_key_frequency.size())) * 1.0f) * sizeof(Node)));

    unsigned int last_node = 0;
    unsigned int current_size = new_stats.dict_key_frequency.size();

    i=0;
    //все элементы массива - узлы - не имеют еще дочерних узлов, поэтому значение NULL
    for (const auto& [key, frequency] : new_stats.dict_key_frequency){
        nodes[i] = (Node){ frequency,key, NULL, NULL};
        i++;
    }

    Node::sortData(nodes, 0, new_stats.dict_key_frequency.size()-1);
    //формирование самого дерева в последнем элементе массива nodes по алгоритму Хаффмана
    while (k != 1){
        //частоты у ветвей одного узла складываются в собственную частоту узла
        nodes[current_size] = (Node){nodes[last_node].frequency+nodes[last_node+1].frequency, 0,
                                     reinterpret_cast<struct node *>(&nodes[last_node]),
                                     reinterpret_cast<struct node *>(&nodes[last_node + 1])};
        last_node = current_size;
        current_size+=k-1;
        k--;
        //формирование узла как элемента массива nodes, напрмер, массив nodes[10]
        //рассматриваются группы 0-3, 4-6, 7-8, 9:
        // {0,1,2,3,4,5,6,7,8,9} ->
        // {0,1,2,3,4(узел из 0 и 1),5,6,7,8,9} -> {0,1,2,3,4(узел из 0 и 1),5,6,7(узел из 4 и 5),8,9}
        // ->{0,1,2,3,4(узел из 0 и 1),5,6,7(узел из 4 и 5),8,9(узел из 7 и 8)}, 9 - итоговое дерево
        for(i=last_node+1; i<current_size; i++){
            nodes[i] = (Node){ nodes[i-k].frequency,nodes[i-k].index,nodes[i-k].left, nodes[i-k].right};
        }
        Node::sortData(nodes, last_node, current_size-1);
    }
    //в переменную извлекается последний элемент - дерево
    Node node = *(&nodes[current_size-1]);
    char code_block[k_];
    Node::code_tree(&node,k_,pair_list,code_block,0);
    char *list_char= static_cast<char *>(malloc(0));
    ///запись словаря
    std::ofstream out_newtext;
    out_newtext.open("New_Text.txt", std::ios::binary);
    if (!out_newtext.is_open()) { std::cerr << "Error open file New_Text\n"; return 1; }
    in_textorig.open("TextOrig.txt", std::ios::binary);
    if (!in_textorig.is_open()) { std::cerr << "Error open file TextOrig 2\n"; return 1; }

    symbol='0';
    while(!in_textorig.eof()){
        memset(block,0,bit_interval_*8);
        for(i=0;i<bit_interval_*8;i++){
            symbol = in_textorig.get();
            if(symbol==EOF)  break;
            block[i]=symbol;
        }
        Pair::encode(bytes_to_write, bit_interval_, bit_interval_ * 8, block,
                     pair_list, k_, g_total_current, out_newtext);
    }
    if(bytes_to_write[0]!=0) out_newtext << bytes_to_write[0];

    in_textorig.close();
    out_newtext.close();
    printf("%d", g_total_current[0]);

///ФАЙЛ ЗАПИСЬ СЛОВАРЯ

    std::ofstream f_dict_write;
    f_dict_write.open("TextDict.txt");
    if (!f_dict_write.is_open()) { std::cerr << "Error open file TextDict.txt\n"; return 1; }

    f_dict_write << len_text;
    f_dict_write << ";";
    f_dict_write << bit_interval_;
    f_dict_write << ";";

    for (int i = 0; i < new_stats.dict_key_frequency.size(); i++) {
        f_dict_write << pair_list[i].element;
        f_dict_write << "-";
        for (int j = 0; j < pair_list[i].size_code; j++)  f_dict_write << pair_list[i].code[j];
        f_dict_write << ";";
    }
    f_dict_write.close();

    ///ЧТЕНИЕ СЛОВАРЯ

    //чтение данных из словаря
    std::ifstream f_dict_read;
    f_dict_read.open("TextDict.txt");
    if (!f_dict_read.is_open()) { std::cerr << "Error open file TextDict.txt\n"; return 1; }

    int c=0; //число символов ";" и "-"(этими символами разделяются данные в словаре
    symbol = '0'; //считываемый символ

    //данные про длину исходного текста
    char *len_text_char= static_cast<char *>(malloc(1));
    int len_text_counter=0;
    //данные про битовый интервал
    char *bin_interval_char= static_cast<char *>(malloc(1));
    int bin_interval_counter=0;

    while(!f_dict_read.eof()){
        //первой идет информация о длине исходного текста
        while (c==0){
            symbol = f_dict_read.get();
            if(symbol==';'){
                c++;
                break;
            }

            //запись в len_text_char данных о длине исходного текста
            len_text_char= static_cast<char *>(realloc(len_text_char,
                                                       (len_text_counter + 1) * sizeof(char)));
            len_text_char[len_text_counter]=symbol;
            len_text_counter++;
        }
        //Поле символа ";" идет информация о битовом интервале
        while (c==1){
            symbol = f_dict_read.get();
            if(symbol==';'){
                c++;
                break;
            }
            //запись в bin_interval_char данных о битовом интервале
            bin_interval_char= static_cast<char *>(realloc(bin_interval_char,
                                                           (bin_interval_counter + 1) * sizeof(char)));
            bin_interval_char[bin_interval_counter] = symbol;
            bin_interval_counter++;
        }
        break;
    }

    ///ЧТЕНИЕ ИЗ СЛОВАРЯ КОДИРОВКИ

    //перевод данных о битовом интервале и длине исходного текста из типа char в тип int
    int bin_interval_int= char_to_int(bin_interval_char,bin_interval_counter);
    int len_text_int = char_to_int(len_text_char,len_text_counter);
    printf("\nbin_interval_int %d len_text_int %d\n",bin_interval_int,len_text_int);

    //read_list - массив структур, который хранит данные из считанного словаря
    Read_dict read_list[(int)pow(2,bin_interval_int)];

    //значение кодировки битового интервала по Хаффману
    unsigned long long int code_int=0;
    //изначальное значение битового интервала
    char *elem_char= static_cast<char *>(malloc(1));
    int elem_char_counter=0;
    //значение кодировки битового интервала по Хаффману типа char
    char *code_char= static_cast<char *>(malloc(1));
    //длина кодировки(например, для 001 code_char_counter=3
    int code_char_counter=0;
    c=0;
    //число пар элемент-кодировка
    int new_pair=0;

    for (int i = 0; i < ((int)pow(2,bin_interval_int)); i++) {
        read_list[i].read_code_char= static_cast<char *>(malloc(1));
    }

    while(!f_dict_read.eof()){
        symbol = f_dict_read.get();
        //символ ";" разделяет пары элемент-кодировка
        if(symbol==';'){
            c++;
            //заполнение структур словаря считанными значениями кодировки
            read_list[new_pair].read_code_int= char_to_int(code_char,code_char_counter);
            read_list[new_pair].read_code_size= code_char_counter;
            read_list[new_pair].read_code_char= static_cast<char *>(realloc(read_list[new_pair].read_code_char,
                                                                            (code_char_counter) * sizeof(Read_dict)));
            //запись в словарь значения кодировки битового интервала по Хаффману типа char
            for (int i = 0; i < code_char_counter; i++) {
                code_int<<=1;
                code_int=(code_char[i]=='1' ? (code_int|0b000001) : (code_int));
                read_list[new_pair].read_code_char[i]=code_char[i];
            }
            read_list[new_pair].read_code_int=code_int;
            code_int=0;
            symbol = f_dict_read.get();
            //переход к новой паре элементов
            new_pair++;
            code_char= static_cast<char *>(realloc(code_char, (1) * sizeof(char)));
            code_char_counter=0;
        }
        //символ "-" разделяет элемент и кодировку
        if(symbol=='-'){
            c++;
            //заполнение структур словаря считанными значениями битового интервала
            read_list[new_pair].read_elem= char_to_int(elem_char,elem_char_counter);
            symbol = f_dict_read.get();
            elem_char= static_cast<char *>(realloc(elem_char, (1) * sizeof(char)));
            elem_char_counter=0;
        }
        //0 ;     1 -
        //символ ";" означает, что дальше по словарю идет битовая кодировка
        if(c%2==0){
            //!!
            elem_char= static_cast<char *>(realloc(elem_char, (elem_char_counter + 1) * sizeof(char)));
            elem_char[elem_char_counter]=symbol;
            elem_char_counter++;
        }
            //символ "-" означает, что дальше по словарю идет битовая кодировка
        else{
            //!!
            code_char= static_cast<char *>(realloc(code_char, (code_char_counter + 1) * sizeof(char)));
            code_char[code_char_counter]=symbol;
            code_char_counter++;
        }
    }
    f_dict_read.close();

    std::ofstream decode_write;
    decode_write.open("TextDecode.txt", std::ios::binary);
    if (!decode_write.is_open()) { std::cerr << "Error open file TextDecode.txt\n"; return 1; }
    std::ifstream newtext_read;
    newtext_read.open("New_Text.txt", std::ios::binary);
    if (!newtext_read.is_open()) { std::cerr << "Error open file TextDecode.txt\n"; return 1; }


    printf("\n");
    Read_dict::decode(decode_write, newtext_read, (int) new_stats.dict_key_frequency.size(), read_list, bit_interval_,
                      len_text);

    newtext_read.close();
    decode_write.close();

    return 0;
}
