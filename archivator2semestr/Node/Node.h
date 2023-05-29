
#ifndef ARCHIVATOR2SEMESTR_NODE_H
#define ARCHIVATOR2SEMESTR_NODE_H


#include "../Pair/Pair.h"

struct Node{
public:

    //частота элемента или всех дочерних узлов
    unsigned int frequency;
    //сам код двоичный. Для связующих узлов index = 0
    unsigned long long int index;
    //указатоль на левую ветку. для конечного элемента = NULL
    struct node* left;
    struct node* right;


    static void swap(Node *ind1, Node *ind2) {
        Node temp = *ind1;
        *ind1 = *ind2;
        *ind2 = temp;
    }


    // Сортировка
    static void sortData(Node *arr, unsigned int start, unsigned int end) {
        unsigned int i, j, m;

        for (i = start; i < end+1; i++) {
            m = i;
            for (j = i + 1; j < end+1; j++)
                if (arr[j].frequency < arr[m].frequency)
                    m = j;
            if (m != i)
                swap(&arr[i], &arr[m]);
        }
    }
    //рекурсивная функция создания кодового дерева по алгоритму Хаффмана; k- максимальная длина слова-кода по Хаффману

    static void code_tree(Node *node, unsigned int k, Pair *dict, char code_block[], unsigned int current_block_size){
        //если узел не имеет дочерних ответвлений(левая и правая ветвь), то он является последним
        //для этого элемента назначается итоговый код по Хаффману и его итоговая длина
        if(node->left==NULL && node->right==NULL) {
            for (int i = 0; i < k; i++) {
                if(dict[i].element==node->index){
                    for(int j=0;j<current_block_size;j++) dict[i].code[j]=code_block[j];
                    dict[i].size_code=current_block_size; //размер слова-кода по Хаффману
                }
            }
        }
        else{
            if(node->right!=NULL){
                code_block[current_block_size]='1';
                code_tree((Node*)node->right,k, dict, code_block,current_block_size+1);
            }
            if(node->left!=NULL){
                code_block[current_block_size]='0';
                code_tree((Node*)node->left,k, dict, code_block,current_block_size+1);
            }
        }
    }
};



#endif //ARCHIVATOR2SEMESTR_NODE_H
