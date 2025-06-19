#ifndef MYHEADER_H   // If MYHEADER_H is not defined
#define MYHEADER_H   // Define MYHEADER_H

// Your declarations go here
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>

#define MAX_WORD_SIZE 500
#define ADDER 100
#define MAX_LINE_SIZE 1024

#define MOD_SUBTRACTION(a, b) ((a - b) < 0 ? -(a - b) : (a - b))

typedef struct lines{
    int numb;
    char *line;
}Lines;

typedef struct files{
    FILE *fp_input;
    FILE *fp_output;
    FILE *fp_dict;
}Files;

/**
 * @brief 
 * 
 * @param word 
 * @param candidate 
 * @param dictionary 
 * @param dict_size 
 * @param output 
 * @param n_alternativas 
 * @param first_cor 
 * @param case_buffer 
 */
void search_complement_m2(char *word, char *candidate, char **dictionary, int dict_size, FILE *output, int *n_alternativas, bool *first_cor, char *case_buffer);

/**
 * @brief 
 * 
 * @param word 
 * @param candidate 
 * @param dictionary 
 * @param dict_size 
 * @param output 
 * @param n_alternativas 
 * @param first_cor 
 * @param diff 
 * @param alt_n 
 * @param case_buffer 
 * @param CASE_dictionary 
 * @return int 
 */
int error_ctr_m2(char *word, char *candidate, char **dictionary, int dict_size, FILE *output, int *n_alternativas, bool *first_cor, int diff, int alt_n, char *case_buffer);

/**
 * @brief Emite uma mensagem de ajuda no stdout
 * 
 */
void print_help();

/**
 * @brief Abre os ficheiros de qualquer modo e fecha o programa em caso de erro
 * 
 * @param filename String a conter o nome do ficheiro
 * @param mode Inserir o modo de operação do f_open
 * @return FILE* 
 */
FILE *file_open(char *filename,char *mode);

/**
 * @brief Modo de trabalho 1: Deteta os erros presentes no texto, segundo cada frase.
 * 
 * @param f apontador para estrutura a conter os ficheiros necessários para o normal funcionamento
 */
void modo(Files *f, int alt_num, int diff_num, int mode);

/**
 * @brief Fecha todos os ficheiros abertos inicialmente
 * 
 * @param f apontador para os ficheiros
 */
void close_files(Files *f);

/**
 * @brief Contador de palavras do dicionário
 * 
 * @param f apontador para um ficheiro que contém do dicionário
 * @return int - Número de palavras do dicionário
 */
int dictionary_word_counter(Files *f);

/**
 * @brief Retorna a comparação entre as duas string
 * 
 * @param a 
 * @param b 
 * @return int - Valor que provém do retorno
 */
int compareWords(const void *a, const void *b);

/**
 * @brief Seleciona, e analisa cada palavra presente na string
 * 
 * @param str 
 * @param str_n número da string
 * @param dictionary 
 * @param dict_size tamanho do dicionário
 * @param f apontador para os ficheiros necessários
 */
void getWord(char *str,int str_n, char **dictionary, int dict_size, Files *f, int alt_num, int diff_num, int mode, char *newString);
/**
 * @brief Analisa cada palavra provida pela função getWord
 * 
 * @param dictionary 
 * @param dict_size tamanho do dicionário
 * @param word 
 * @param f apontador para os ficheiros necessários
 * @param str 
 * @param str_n número da string
 * @param is_printed Verifica se já foi escrita a string onde se encontra o erro
 */
void analyse_word(char **dictionary, int dict_size, char *word, Files *f, char *str, int str_n, bool *is_printed, int alt_num, int diff_num, int mode, char *newString);
/**
 * @brief Verifica se o caracter é especial
 * 
 * @param c caracter
 * @return true : O caracter é especial
 * @return false 
 */
bool is_special(char c);

/**
 * @brief Insere sugestões de palavras do dicionário parecidas com a palavra incorreta
 * 
 * @param word 
 * @param dictionary 
 * @param dict_size tamanho do dicionário
 * @param alt_num númeto total de alternativas a imprimir
 * @param diff_num número total de alternativas possíveis
 */
void insert_sugestions_m2(char *word, char **dictionary, int dict_size, int alt_num, int diff_num, FILE *output);

/**
 * @brief 
 * 
 * @param word 
 * @param dictionary 
 * @param dict_size 
 * @param alt_num 
 * @param diff_num 
 * @param output 
 * @param CASE_dictionary 
 * @param str 
 * @param newString 
 */
void insert_sugestions_m3(char *word, char **dictionary, int dict_size, int alt_num, int diff_num, FILE *output, char *str, char *newString);

/**
 * @brief 
 * 
 * @param word 
 * @param candidate 
 * @param dictionary 
 * @param dict_size 
 * @param output 
 * @param n_alternativas 
 * @param first_cor 
 * @param diff 
 * @param alt_n 
 * @param case_buffer 
 * @param CASE_dictionary 
 * @param firstSugestion 
 * @return int 
 */
int error_ctr_m3(char *word, char *candidate, char **dictionary, int dict_size, FILE *output, int *n_alternativas, bool *first_cor, int diff, int alt_n, char *case_buffer, char *firstSugestion);

/**
 * @brief 
 * 
 * @param word 
 * @param candidate 
 * @param dictionary 
 * @param dict_size 
 * @param output 
 * @param n_alternativas 
 * @param first_cor 
 * @param case_buffer 
 * @param firstSugestion 
 */
void search_complement_m3(char *word, char *candidate, char **dictionary, int dict_size, FILE *output, int *n_alternativas, bool *first_cor, char *case_buffer, char *firstSugestion);

#endif  // End of the ifndef block
