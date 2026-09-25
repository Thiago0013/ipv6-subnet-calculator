#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void expand_ipv6(char ipv6_digited[100], char ip_completed[100]);
void hexToBin(char ipv6[100], int ip_binary[128]);
void binToHex(int rede_bin[128]);
void prefixCalc(int ipv6_bin[128], int prefixo_original, int prefixo_novo);

int main(){
  char input[100];
  char ipv6_digited[100];
  char result[100];
  int ip_binary[128];
  int prefix;
  int new_prefix;

  printf("Digite o ipv6 [EX: 2001:db8::/32)]: ");
  scanf("%s", input);

  printf("Digite qual prefixo você quer dividir [EX: 33]: ");
  scanf("%d", &new_prefix);


  if(sscanf(input, "%[^/]/%d", ipv6_digited, &prefix) == 2){
    if (new_prefix < prefix) {
    fprintf(stderr, "Erro: Você não pode colocar um prefixo menor que o prefixo original.\n");
    exit(EXIT_FAILURE);
    }

    expand_ipv6(ipv6_digited, result);
    printf("Resultado: %s\n", result);
    hexToBin(result, ip_binary);
    prefixCalc(ip_binary, prefix, new_prefix);
  }

  return 0;
}

void prefixCalc(int ipv6_bin[128], int original_prefix, int new_prefix){
  int rede_bin[128];
  int last_ip_bin[128];
  int borrowed_bits = new_prefix - original_prefix;
  
  int number_network = 1 << borrowed_bits; 

  printf("Bits emprestados: %d\n", borrowed_bits);
  printf("Gerando %d sub-redes...\n", number_network);

  for(int i=0; i<128; i++){
    if(i < new_prefix){
      rede_bin[i] = ipv6_bin[i];
    } else {
      rede_bin[i] = 0;
    }
  }

  for(int current_network = 1; current_network <= number_network; current_network++){
    for(int i=0; i<128; i++){
      if(i < new_prefix){
        last_ip_bin[i] = rede_bin[i];
      } else {
        last_ip_bin[i] = 1;
      }
    }

    printf("\n[ Sub-rede %d ]\n", current_network);
    printf("Primeiro IP (Rede)..:\n");
    printf("IPV6 binario: ");

    for(int i=0; i<128; i++){
      printf("%d", rede_bin[i]);
      if((i+1) % 4 == 0){
        printf(" ");
      }
    }
    printf("\n");
    printf("IPV6 Completo: ");
    binToHex(rede_bin);
    printf("\n");
    printf("Ultimo IP...........:\n");
    printf("IPV6 binario: ");

    for(int i=0; i<128; i++){
      printf("%d", last_ip_bin[i]);
      if((i+1) % 4 == 0){
        printf(" ");
      }
    }
    
    printf("\n");
    printf("IPV6 Completo: ");
    binToHex(last_ip_bin);

    int carry = 1;
    for(int i = 127; i >= 0; i--) {
        int soma = last_ip_bin[i] + carry;
        if (soma == 2) { 
            rede_bin[i] = 0; 
            carry = 1;
        } else if (soma == 1) { 
            rede_bin[i] = 1; 
            carry = 0; 
        } else { 
            rede_bin[i] = 0; 
            carry = 0;
        }
    }
  }
}

void binToHex(int rede_bin[128]) {
    for (int i = 0; i < 128; i += 4) {
        int valor = (rede_bin[i] * 8) + 
                    (rede_bin[i+1] * 4) + 
                    (rede_bin[i+2] * 2) + 
                    (rede_bin[i+3] * 1);

        printf("%x", valor);
        if ((i + 4) % 16 == 0 && (i + 4) != 128) {
            printf(":");
        }
    }
    printf("\n");
}

void hexToBin(char ipv6[100], int ip_binary[128]){
  int pos = 0;
  
  for(int i=0; i<strlen(ipv6); i++){
    char character = ipv6[i];

    if(character == ':') continue;

    int valor = 0;
    if (character >= '0' && character <= '9'){
      valor = character - '0';
    } else if(character >= 'a' && character <= 'f'){
      valor = (character - 'a') + 10;
    } else if (character >= 'A' && character <= 'F'){
      valor = (character - 'A') + 10;
    }

    int peso = 8;
    for (int j=0; j<4; j++){
      if(valor >= peso){
        ip_binary[pos] = 1;
        valor = valor - peso;
      } else {
        ip_binary[pos] = 0;
      }
      peso = peso / 2;
      pos++;
    }
  }
}

void expand_ipv6(char ipv6_digited[100], char ip_completed[100]){
  int right_count = 0;
  int left_count = 0;
  char *pos = strstr(ipv6_digited, "::");
  ip_completed[100] = '\0';
  char right_temp[100] = "";


  if(pos != NULL){
    *pos = '\0';

    char *left = ipv6_digited;
    char *right = pos + 2;

    char bloco_formatado[10];

    char *token = strtok(left, ":");
    while(token != NULL){
      sprintf(bloco_formatado, "%04x", (unsigned int)strtol(token, NULL, 16));
      
      strcat(ip_completed, bloco_formatado); 
      strcat(ip_completed, ":");

      token = strtok(NULL, ":");
      left_count++;
    }

    token = strtok(right, ":");
    while(token != NULL){
      sprintf(bloco_formatado, "%04x", (unsigned int)strtol(token, NULL, 16));
      
      strcat(ip_completed, bloco_formatado); 
      strcat(ip_completed, ":");

      token = strtok(NULL, ":");
      right_count++;
    }

    int zero_spaces = 8 - (right_count + left_count);
    for(int i=0; i<zero_spaces; i++){
      strcat(ip_completed, "0000:");
    }

    strcat(ip_completed, right_temp);

    ip_completed[strlen(ip_completed) - 1] = '\0';
  }
}