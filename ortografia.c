#include "ortografia.h"

int main(int argc, char **argv){

        int opt;
        bool help = false;
        Files f;
        f.fp_input = NULL, f.fp_output = NULL, f.fp_dict = NULL; 
    
        char input[MAX_WORD_SIZE];
        char output[MAX_WORD_SIZE];
        char dictName[MAX_WORD_SIZE];
    
        int alt_num = 10, diff_num  = 2,mode = 1;
       
        while ((opt = getopt(argc, argv, "hi:o:d:a:n:m:")) != -1) {
            switch (opt) {
                case 'h':
                    help = true;
                    break;
                case 'i':
                    if(optarg != NULL){
                        strcpy(input,optarg);
                        f.fp_input = file_open(input,"r");
                        if(f.fp_input == NULL){
                            close_files(&f);
                            return EXIT_FAILURE;
                        }
                    }
                    break;
                case 'o':
                    if(optarg != NULL){
                        strcpy(output,optarg);
                        f.fp_output = file_open(output,"w");
                        if(f.fp_output == NULL){
                            close_files(&f);
                            return EXIT_FAILURE;
                        }
                    }
                    break;
                case 'd':
                    if(optarg != NULL){
                        strcpy(dictName,optarg);
                        f.fp_dict = file_open(dictName,"r");
                        if(f.fp_dict == NULL){
                            close_files(&f);
                            return EXIT_FAILURE;
                        } 
                    }
                    //printf("I arrived\n");
                    break;
                case 'a':
                    //print_usage();
                    //return 1;
                    for(int i = 0; i < strlen(optarg); i++){
                        if(optarg[i] < '0' || optarg[i] > '9'){
                            close_files(&f);
                            return EXIT_FAILURE;
                        }
                    }
                    alt_num = atoi(optarg);
                    break;
                case 'n':
                    //print_usage();
                    //return 1;
                    for(int i = 0; i < strlen(optarg); i++){
                        if(optarg[i] < '0' || optarg[i] > '9'){
                            close_files(&f);
                            return EXIT_FAILURE;
                        }
                    }
                    diff_num = atoi(optarg);
                    break;
                case 'm':
                    for(int i = 0; i < strlen(optarg); i++){
                        if(optarg[i] < '0' || optarg[i] > '9'){
                            close_files(&f);
                            return EXIT_FAILURE;
                        }
                    }
                    mode = atoi(optarg);
                    if(!(mode == 1 || mode == 2 || mode == 3)){
                        close_files(&f);
                        print_help();
                        return EXIT_FAILURE;
                    }
                    break;
                default:
                    //print_usage();
                    close_files(&f);
                    print_help();
                    return EXIT_FAILURE;
            }
        }

    if(f.fp_dict == NULL){
        f.fp_dict = file_open("words","r");
        if(f.fp_dict == NULL) {
            close_files(&f);
            return EXIT_FAILURE;
        }
    }

    // Print the results
    if (help) {
        print_help();
        close_files(&f);
        return 0;
    }
    if(mode == 3){
        alt_num = 1;
    }
    
    modo(&f,alt_num,diff_num,mode);

    close_files(&f);
    return 0;
}


void modo(Files *f, int alt_num, int diff_num, int mode){
    //Leitura do dicionario
    char *dynamic_str = (char *)malloc(sizeof(char) * MAX_LINE_SIZE);
    if(dynamic_str == NULL){
        close_files(f);
        exit(1);
    }

    int dict_size = dictionary_word_counter(f);
    
    char **dictionary = (char **)malloc(dict_size * sizeof(char *));
    if(dictionary == NULL){
        close_files(f);
        exit(1);
    }
    for(int i = 0;i < dict_size;i++){
        int val = fscanf(f->fp_dict,"%s",dynamic_str);
        
        if(val == 0){
            close_files(f);
            exit(1);
        } 

        dictionary[i] = strdup(dynamic_str);
        if(dictionary[i] == NULL){
            close_files(f);
            exit(1);
        }
    }
    fseek(f->fp_dict,0,SEEK_SET);

    qsort(dictionary,dict_size,sizeof(char *),compareWords);

    int line_number = 0;

    //printf("I arrived\n");
    char *newString = (char *) malloc(MAX_LINE_SIZE * sizeof(char));
    
    memset(newString, '\0', sizeof(*newString));
    if(f->fp_input == NULL){
        while(fgets(dynamic_str,MAX_LINE_SIZE,stdin) != 0){
            //Now it searches for errors in the phrase
            strcpy(newString, dynamic_str);
            getWord(dynamic_str,line_number,dictionary,dict_size,f,alt_num,diff_num,mode, newString);
            if(mode == 3){
                if(f->fp_output == NULL){
                    printf("%s", newString);
                }else{
                    fprintf(f->fp_output, "%s", newString);
                }
            }
            line_number++;
        }
        
    }else{
        while(fgets(dynamic_str,MAX_LINE_SIZE,f->fp_input) != 0){
            //Now it searches for errors in the phrase
            strcpy(newString, dynamic_str);
            getWord(dynamic_str,line_number,dictionary,dict_size,f,alt_num,diff_num,mode, newString);
            if(mode == 3){
                if(f->fp_output == NULL){
                    printf("%s", newString);
                }else{
                    fprintf(f->fp_output, "%s", newString);
                }
            }
            line_number++;
        }   
    }

    //Libertar memória
    free(newString);
    free(dynamic_str);

    for(int i = 0;i < dict_size;i++){
        free(dictionary[i]);
    }
    free(dictionary);
}
