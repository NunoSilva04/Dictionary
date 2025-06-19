#include "ortografia.h"

void insert_sugestions_m2(char *word, char **dictionary, int dict_size, int alt_num, int diff_num, FILE *output){
    int n_alternativas = 0, diff = 1;
    int errors = 0;
    bool first_cor = true;
    bool its_separated = false;

    char buffer_comp[MAX_LINE_SIZE]; //Buffer para palavras com ou sem case nos complementos
    char buffer_corr[MAX_LINE_SIZE]; //Buffer para palavras com ou sem case nas correções
    strcpy(buffer_comp,"");
    strcpy(buffer_corr,"");

    for(;diff <= diff_num; diff++){

        for(register int i  = 0;i < dict_size && n_alternativas < alt_num;i++){

            //Emite o número de erros da palavra atual
            errors = error_ctr_m2(word,dictionary[i],dictionary,dict_size,output,&n_alternativas,&first_cor,diff,alt_num,buffer_comp);

            //  Caso o número de diferenças seja o procurado, o número de alternativas ainda não tenha sido atingido e a palavra 
            //não seja a anterior com capitulação EX:(Centered e centred) irá imprimir a sugestão no ficheiro de saída:
            if(errors == diff && !its_separated && n_alternativas < alt_num && strcasecmp(buffer_corr,dictionary[i]) != 0 ){
                strcpy(buffer_corr,dictionary[i]);
                if(first_cor){
                    if(output == NULL) printf("%s", dictionary[i]);
                    else fprintf(output,"%s",dictionary[i]);
                    first_cor = false;
                }else{
                    if(output == NULL) printf(", %s", dictionary[i]);
                    else fprintf(output,", %s",dictionary[i]);
                }
                n_alternativas++;
                
            }
        }
    }

    if(output == NULL) printf("\n");
    else fprintf(output,"\n");
}

void search_complement_m2(char *word, char *candidate, char **dictionary, int dict_size, FILE *output, int *n_alternativas, bool *first_cor, char *case_buffer){
    int cand_size = strlen(candidate); 
    int word_size = strlen(word);
    char buffer_word[MAX_LINE_SIZE]; //string auxiliar para armazenar o candidato anterior
    strcpy(buffer_word,"");
    

    if(cand_size < word_size){
        int j;
        //A palavra correta está desde as posições de 0 a j-1
        for(j = 0;j < cand_size;j++){
            if(tolower(word[j]) != tolower(candidate[j])) {
                break;
            }
        }

        //Caso a palavra tenha um prefixo igual e não tenha sido vista anteriormente
        if(j == cand_size && strcasecmp(case_buffer,candidate) != 0){

            strcpy(case_buffer,candidate);
            char *ptr = NULL;
            int k = 0; //Para ajustar para a procura do caracter errado
            for(ptr = word;candidate[k] != '\0' && *ptr != '\0';k++,ptr++);

            //ptr agora aponta para o sufixo
            for(register int l = 0;l < dict_size;l++){
                //Assegura-se que ptr é o complemento do resto da palavra e que este não foi repetido
                if(strcasecmp(dictionary[l],ptr) == 0 && strcasecmp(dictionary[l],buffer_word) != 0){
                    strcpy(buffer_word,dictionary[l]);
                    char *aux = strdup(dictionary[l]);
                    for(int i = 0;aux[i] != '\0';i++) aux[i] = tolower(aux[i]);
                    if(*first_cor){
                        if(output == NULL) printf("%s %s", candidate, aux);
                        else fprintf(output,"%s %s", candidate, aux);
                        *first_cor = false;
                    }else{
                        if(output == NULL) printf(", %s %s", candidate, aux);
                        else fprintf(output,", %s %s", candidate, aux);
                    }
                    *n_alternativas += 1;
                    free(aux);
                }
            }
        }
    }
}

int error_ctr_m2(char *word, char *candidate, char **dictionary, int dict_size, FILE *output, int *n_alternativas, bool *first_cor, int diff, int alt_n, char *case_buffer){
    register int i=0,j=0;
    int err = 0, err_aux_1 = 0, err_aux_2 = 0, err_aux_3 = 0, err_aux_4 = 0;
    int w_len = strlen(word), c_len= strlen(candidate);
    int char_diff = MOD_SUBTRACTION(w_len,c_len);

    //Sem offset:
    for(i = 0, j = 0;i < w_len && j < c_len;i++,j++){
        if(tolower(word[i]) != tolower(candidate[j]))err_aux_1++;
    }
    if(candidate[j] == '\0' && diff == 1 && *n_alternativas < alt_n && (strcasecmp(case_buffer,candidate) != 0)){
        search_complement_m2(word,candidate,dictionary,dict_size,output,n_alternativas,first_cor, case_buffer);
        
    }
    err_aux_1 += char_diff;
    err = err_aux_1;


    //Procura por offset:
    if(tolower(word[0]) == tolower(candidate[0]) && tolower(word[w_len-1]) == tolower(candidate[c_len-1])){
        if(w_len > c_len){
            
            
            char *c_ptr = candidate, *w_ptr = word;

            int prefix_size = 0, sufix_size = 0;

            //Procura até onde o candidato e a palavra errada estão coerentes de frente para o final e anota-se o sufixo que identificou como igual
            for(;tolower(*w_ptr) == tolower(*c_ptr);w_ptr++,c_ptr++,prefix_size++);

            sufix_size = c_len - prefix_size;

            char *to_search = strdup(w_ptr); //Onde procurar
            char *to_find = strdup(c_ptr); //O que procurar
            int to_s_len = strlen(to_search);
            int to_f_len = strlen(to_find);

            //Iremos procurar por c_ptr em word...
            bool doesnt_apply = false;

            int a,b;
            for(a = to_f_len - 1, b = to_s_len - 1, err_aux_3 = 0; a > -1 && b > -1 ;a--,b--){
                if (tolower(to_find[a]) != tolower(to_search[b])) {
                    doesnt_apply = true;    //Ou seja não se pode tomar em conta o método do offset
                    break;
                }
            }            

            //Caso exista um sufixo e o prefixo corretos na palavra errada, podemos assim considerar que os erros do candidato são os caracteres que tem em falta
            if(doesnt_apply) err_aux_3 = w_len - prefix_size;
            else err_aux_3 = w_len - prefix_size - sufix_size;
            free(to_find);
            free(to_search);
            if(err > err_aux_3) err = err_aux_3;
            
        }else if(w_len < c_len){
            char *c_ptr = candidate, *w_ptr = word;
            int prefix_size = 0, sufix_size = 0;

            //Procura até onde o candidato e a palavra errada estão coerentes de frente para o final e anota-se o sufixo que identificou como igual
            for(;tolower(*w_ptr) == tolower(*c_ptr);w_ptr++,c_ptr++,prefix_size++);
            sufix_size = w_len - prefix_size;

            char *to_search = strdup(w_ptr);
            int to_search_len = strlen(to_search);

            //Iremos procurar por c_ptr em word...
            bool doesnt_apply = false;
            int end_search = w_len - (c_len - prefix_size) - 1;
            for(i = c_len - 1, j = to_search_len - 1, err_aux_2 = 0; i >= end_search && j > -1;i--,j--){
                if (tolower(candidate[i]) != tolower(to_search[j])) {
                    doesnt_apply = true;    //Ou seja não se pode tomar em conta o método do offset
                    break;
                }
            }
            
            //Caso exista um sufixo e o prefixo corretos no candidato, podemos assim considerar que os erros do candidato são os caracteres que tem em falta
            if(doesnt_apply) err_aux_2 = c_len - prefix_size;
            else err_aux_2 = c_len - prefix_size - sufix_size;

            free(to_search);
            if(err > err_aux_2) err = err_aux_2;
    
        }
        
    }

    //Ver inverso:

    for(i = w_len - 1, j = c_len - 1;i > -1 && j > -1;i--,j--){
        if(tolower(word[i]) != tolower(candidate[j]))err_aux_4++;
    }
    err_aux_4 += char_diff;
    if(err > err_aux_4)err = err_aux_4;

    //Vai retornar o menor dos erros encontrados na palavra
    return err;
}

void insert_sugestions_m3(char *word, char **dictionary, int dict_size, int alt_num, int diff_num, FILE *output, char *str, char *newString){
    int n_alternativas = 0, diff = 1;
    int errors = 0;
    bool first_cor = true;
    bool its_separated = false;

    char buffer_comp[MAX_LINE_SIZE]; //Buffer para palavras com ou sem case nos complementos
    char buffer_corr[MAX_LINE_SIZE]; //Buffer para palavras com ou sem case nas correções
    strcpy(buffer_comp,"");
    strcpy(buffer_corr,"");
    char *firstSuggestion = (char *) calloc(MAX_WORD_SIZE, sizeof(char));

    //Look for words separated...
    for(;diff <= diff_num; diff++){

        for(int i  = 0;i < dict_size && n_alternativas < alt_num;i++){

            errors = error_ctr_m3(word,dictionary[i],dictionary,dict_size,output,&n_alternativas,&first_cor,diff,alt_num,buffer_comp, firstSuggestion);

            if(errors == diff && !its_separated && n_alternativas < alt_num && strcasecmp(buffer_corr,dictionary[i]) != 0 ){
                strcpy(buffer_corr,dictionary[i]);
                strcpy(firstSuggestion, dictionary[i]);
                if(first_cor){
                    first_cor = false;
                }
                n_alternativas++;
                
            }
        }
    }

    if(strcmp("", firstSuggestion) == 0){
        strcpy(firstSuggestion, word);
    }

    char *pos = strstr(newString, word);
    int index = 0;
    index = pos - newString;

    char *leftSideString, *rightSideString;
    leftSideString = (char *) calloc(MAX_LINE_SIZE, sizeof(char));
    rightSideString = (char *) calloc(MAX_LINE_SIZE, sizeof(char));

    strncpy(leftSideString, newString, index);
    leftSideString[index] = '\0';
    strcpy(rightSideString, newString + index + strlen(word));

    char *tempString = (char *) calloc(MAX_LINE_SIZE, sizeof(char));
    strcat(tempString, leftSideString);
    strcat(tempString, firstSuggestion);
    strcat(tempString, rightSideString);
    strcpy(newString, tempString);

    free(leftSideString);
    free(rightSideString);
    free(tempString);
    free(firstSuggestion);
}

void search_complement_m3(char *word, char *candidate, char **dictionary, int dict_size, FILE *output, int *n_alternativas, bool *first_cor, char *case_buffer, char *firstSugestion){
    int cand_size = strlen(candidate); 
    int word_size = strlen(word);
    char buffer_word[MAX_LINE_SIZE];
    strcpy(buffer_word,"");
    

    if(cand_size < word_size){
        int j;
        for(j = 0;j < cand_size;j++){
            if(tolower(word[j]) != tolower(candidate[j])) {
                break;
            }
        }
        if(j == cand_size && strcasecmp(case_buffer,candidate) != 0){
            strcpy(case_buffer,candidate);
            char *ptr = NULL;
            int k = 0; //Para ajustar para a procura do caracter errado
            for(ptr = word;candidate[k] != '\0' && *ptr != '\0';k++,ptr++);
            for(int l = 0;l < dict_size;l++){
                if(strcasecmp(dictionary[l],ptr) == 0 && strcasecmp(dictionary[l],buffer_word) != 0){
                    strcpy(buffer_word,dictionary[l]);
                    char *aux = strdup(dictionary[l]);
                    for(int i = 0;aux[i] != '\0';i++) aux[i] = tolower(aux[i]);
                    if(*first_cor){
                        if(output == NULL){
                            char *tempStr = (char *) calloc(MAX_WORD_SIZE,  sizeof(char));
                            strcat(tempStr, candidate);
                            strcat(tempStr, " ");
                            strcat(tempStr, aux);
                            strcpy(firstSugestion, tempStr);
                            free(tempStr);
                        }
                        *first_cor = false;
                    }else{
                        if(output == NULL){
                            char *tempStr = (char *) calloc(MAX_WORD_SIZE,  sizeof(char));
                            strcat(tempStr, candidate);
                            strcat(tempStr, " ");
                            strcat(tempStr, aux);
                            strcpy(firstSugestion, tempStr);
                            free(tempStr);
                        }
                    }
                    *n_alternativas += 1;
                    free(aux);
                }
            }
        }
    }
}

int error_ctr_m3(char *word, char *candidate, char **dictionary, int dict_size, FILE *output, int *n_alternativas, bool *first_cor, int diff, int alt_n, char *case_buffer, char *firstSugestion){
    int i=0,j=0;
    int err = 0, err_aux_1 = 0, err_aux_2 = 0, err_aux_3 = 0, err_aux_4 = 0;
    int w_len = strlen(word), c_len= strlen(candidate);
    int char_diff = MOD_SUBTRACTION(w_len,c_len);

    //Sem offset:
    for(i = 0, j = 0;i < w_len && j < c_len;i++,j++){
        if(tolower(word[i]) != tolower(candidate[j]))err_aux_1++;
    }
    if(candidate[j] == '\0' && diff == 1 && *n_alternativas < alt_n && (strcasecmp(case_buffer,candidate) != 0)){
        search_complement_m3(word,candidate,dictionary,dict_size,output,n_alternativas,first_cor, case_buffer, firstSugestion);
        if(strcmp("", firstSugestion) != 0){
            return 1;
        }
    }
    err_aux_1 += char_diff;
    err = err_aux_1;
    
    if(tolower(word[0]) == tolower(candidate[0]) && tolower(word[w_len-1]) == tolower(candidate[c_len-1])){
        if(w_len > c_len){
            
            
            char *c_ptr = candidate, *w_ptr = word;
            int prefix_size = 0, sufix_size = 0;

            //Procura até onde o candidato e a palavra errada estão coerentes de frente para o final e anota-se o sufixo que identificou como igual
            for(;tolower(*w_ptr) == tolower(*c_ptr);w_ptr++,c_ptr++,prefix_size++);

            sufix_size = c_len - prefix_size;

            char *to_search = strdup(w_ptr); 
            char *to_find = strdup(c_ptr); 
            int to_s_len = strlen(to_search);
            int to_f_len = strlen(to_find);

            bool doesnt_apply = false; 

            int a,b;
            for(a = to_f_len - 1, b = to_s_len - 1, err_aux_3 = 0; a > -1 && b > -1 ;a--,b--){
                if (tolower(to_find[a]) != tolower(to_search[b])) {
                    doesnt_apply = true;    
                    break;
                }
            }            
            //Caso exista um sufixo e o prefixo corretos na palavra errada, podemos assim considerar que os erros do candidato são os caracteres que tem em falta
            if(doesnt_apply) err_aux_3 = w_len - prefix_size;
            else err_aux_3 = w_len - prefix_size - sufix_size;
            free(to_find);
            free(to_search);
            if(err > err_aux_3) err = err_aux_3;
            
        }else if(w_len < c_len){
            char *c_ptr = candidate, *w_ptr = word;
            int prefix_size = 0, sufix_size = 0;
            for(;tolower(*w_ptr) == tolower(*c_ptr);w_ptr++,c_ptr++,prefix_size++);
            sufix_size = w_len - prefix_size;

            char *to_search = strdup(w_ptr);
            int to_search_len = strlen(to_search);

            bool doesnt_apply = false;
            int end_search = w_len - (c_len - prefix_size) - 1;
            for(i = c_len - 1, j = to_search_len - 1, err_aux_2 = 0; i >= end_search && j > -1;i--,j--){
                if (tolower(candidate[i]) != tolower(to_search[j])) {
                    doesnt_apply = true;    
                    break;
                }
            }
            
            //Caso exista um sufixo e o prefixo corretos na palavra errada, podemos assim considerar que os erros do candidato são os caracteres que tem em falta
            if(doesnt_apply) err_aux_2 = c_len - prefix_size;
            else err_aux_2 = c_len - prefix_size - sufix_size;

            free(to_search);
            if(err > err_aux_2) err = err_aux_2;
    
        }
        
    }

    //Ver inverso:

    for(i = w_len - 1, j = c_len - 1;i > -1 && j > -1;i--,j--){
        if(tolower(word[i]) != tolower(candidate[j]))err_aux_4++;
    }
    err_aux_4 += char_diff;
    if(err > err_aux_4)err = err_aux_4;

    //Vai retornar o menor dos erros encontrados na palavra
    return err;
}

void print_help() {
    printf("Utilização: programa [-v] [-n name]\n");
    printf("-h \t mostra a ajuda para o utilizador e termina\n");
    printf("-i \t filename nome do ficheiro de entrada, em alternativa a stdin\n");
    printf("-o \t filename nome do ficheiro de saída, em alternativa a stdout\n");
    printf("-d \t filename nome do ficheiro de dicionário a usar\n");
    printf("-a nn \t o número máximo de alternativas a mostrar com cada erro ortográfico deve ser nn\n");
    printf("-n nn \t o número máximo de diferenças a considerar nos erros ortográficos deve ser nn\n");
    printf("-m n \t o modo de funcionamento do programa deve ser n = (1, 2, 3)\n\n");
}

FILE *file_open(char *filename,char *mode){
    FILE *fp = NULL;
    fp = fopen(filename,mode);
    if(fp == NULL){
        exit(EXIT_FAILURE);
    }
    return fp;
}

void close_files(Files *f){
    if(f->fp_dict != NULL) fclose(f->fp_dict);
    if(f->fp_output != NULL) fclose(f->fp_output);
    if(f->fp_input != NULL) fclose(f->fp_input);
}

bool is_special(char c){
    char *special = "àáéíóúãõâêôçÀÁÉÍÓÚÃÕÂÊÔÇ";
    for(int i = 0;special[i] != '\0';i++)
        if(c == special[i])     
            return true;

    return false;
}

int dictionary_word_counter(Files *f){
    char *aux_str = (char *)malloc(MAX_WORD_SIZE*sizeof(char));
    char *aux_2 = NULL;
    if(aux_str == NULL) exit(1);
    int i = 0;
    

    for(;(aux_2 = fgets(aux_str,MAX_WORD_SIZE,f->fp_dict)) != NULL;i++){
        if(aux_2 == NULL){
            exit(1);
        }
    }
    fseek(f->fp_dict, 0, SEEK_SET);
    free(aux_str);
    return i;
    
}

int compareWords(const void *a, const void *b) {
    return strcasecmp(*(const char **)a,*(const char **)b);
}

void getWord(char *str,int str_n, char **dictionary, int dict_size, Files *f, int alt_num, int diff_num, int mode, char *newString) {

    int j = 0;
    int wordIndex = 0;
    char word[1024];
    bool is_printed = false;

    for(int i = 0; i <= strlen(str); i++){
        //Aceita apenas os caracteres ditos no enunciado do projeto.
        if (isalpha(str[i]) || (str[i] == '\'' && j != 0)) {
            word[j++] = str[i];
        
        }else if(is_special(str[i])){
            word[j++] = str[i];
        }else {
            if (j > 0) {
                if(word[j-1] == '\''){
                    word[j-1] = '\0';
                }
                word[j] = '\0';  
                analyse_word(dictionary,dict_size,word,f,str, str_n, &is_printed, alt_num,diff_num,mode, newString);
                wordIndex++;
                memset(word, '\0', sizeof(word));
                j = 0;  //Reinicialização para voltar a contar
            }
        }
    }
    if (j > 0) {
        word[j] = '\0';
    }
}


bool search_word_in_dictionary(const char *word, char *dictionary[], int dictionary_size) {
    if (bsearch(&word, dictionary, dictionary_size, sizeof(char *), compareWords) != NULL) {
        return true;
    } else {
        return false;
    }

}

void analyse_word(char **dictionary, int dict_size, char *word, Files *f, char *str, int str_n, bool *is_printed, int alt_num, int diff_num, int mode, char *newString){
    char *str_aux = NULL;
    bool found = search_word_in_dictionary(word,dictionary,dict_size);
    if(!found){
        if(mode == 1){
            if(!*is_printed){
                //Pode ter '\n' ou não, logo esperar o caso...
                str_aux = strtok(str,"\n");
                if(str_aux == NULL)exit(1);

                if(f->fp_output == NULL) printf("%d: %s\n",str_n+1,str_aux);
                else fprintf(f->fp_output,"%d: %s\n",str_n+1,str_aux);

                *is_printed = true;
            }
            if(f->fp_output == NULL) printf("Erro na palavra \"%s\"\n", word);
            else fprintf(f->fp_output,"Erro na palavra \"%s\"\n",word);
            
        }else{
            if(!*is_printed){
                str_aux = strtok(str,"\n");
                if(str_aux == NULL)exit(1);
                if(f->fp_output == NULL) {
                    if(mode == 2){
                        printf("%d: %s\n",str_n+1,str_aux);
                        printf("Erro na palavra \"%s\"\n", word);
                        insert_sugestions_m2(word, dictionary, dict_size, alt_num, diff_num, NULL);
                    }else{
                        insert_sugestions_m3(word, dictionary, dict_size, alt_num, diff_num, NULL, str, newString);
                    }
                }
                else {
                    if(mode == 2){
                        fprintf(f->fp_output,"%d: %s\n",str_n+1,str_aux); 
                        fprintf(f->fp_output,"Erro na palavra \"%s\"\n",word);
                        insert_sugestions_m2(word, dictionary, dict_size, alt_num, diff_num, f->fp_output);
                    }else{
                        insert_sugestions_m3(word, dictionary, dict_size, alt_num, diff_num, NULL, str, newString);
                    }
                }
                *is_printed = true;
            }else{
                if(f->fp_output == NULL){
                    if(mode == 2){
                        printf("Erro na palavra \"%s\"\n", word);
                        insert_sugestions_m2(word, dictionary, dict_size, alt_num, diff_num, NULL);
                    }else{
                        insert_sugestions_m3(word, dictionary, dict_size, alt_num, diff_num, NULL, str, newString);
                    }
                }
                
                else {
                    if(mode == 2){
                        fprintf(f->fp_output,"Erro na palavra \"%s\"\n",word);
                        insert_sugestions_m2(word, dictionary, dict_size, alt_num, diff_num, f->fp_output);
                    }else{
                        insert_sugestions_m3(word, dictionary, dict_size, alt_num, diff_num, NULL, str, newString);
                    }
                }
            }
            
        }
    }

}



