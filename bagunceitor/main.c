/*

Para compilar de dentro do VS Code:

Windows/Linux: CTRL+SHIFT+B (Terminal -> Run Build Task)
macOS: COMMAND+SHIFT+B

Para executar de dentro do VS Code:

Executar normalmente: CTRL+F5 (Run -> Run Without Debugging)
Debugar: F5 (Run -> Start Debugging)

Pelo terminal:

Windows: mingw32-make
Linux/macOS: make

Para executar:

./bagunceitor [arquivo com a imagem de entrada]

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // Para usar strings
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <stb_image.h>
#include <stb_image_write.h>

// Um pixel Pixel (24 bits)
typedef struct {
  unsigned char r, g, b;
} Pixel;

// Uma imagem Pixel
typedef struct {
  int width, height;  // largura, altura
  int channels;       // qtd de canais (geralmente 3, RGB)
  Pixel* pixels;
} Img;

// As 2 imagens
Img in, out;

// Protótipos
void load(char* name, Img* pic);

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("bagunceitor [origem]\n");
    exit(1);
  }

  // Carrega a imagem original
  load(argv[1], &in);

  // Exibe as dimensões na tela, para conferência
  printf("Origem   : %s %d x %d\n", argv[1], in.width, in.height);

  printf("Processando...\n");

  // Cria imagem de saída e "zera" ela
  int tam = in.width * in.height;
  out = in;
  out.pixels = malloc(tam * sizeof(Pixel));
  memset(out.pixels, 0, tam * sizeof(Pixel));

  // Converte para interpretar como matrizes
  Pixel(*pin)[in.width] = (Pixel(*)[in.height])in.pixels;
  Pixel(*pout)[in.width] = (Pixel(*)[in.height])out.pixels;

  //
  // Neste ponto, voce deve implementar os seus algoritmos!
  // (ou chamar funcoes para fazer isso)
  //
  // Aplica o algoritmo em pin e gera a saida em pout


  // CONFUSÃO: Altera os valores de cor
  for (int i = 0; i < in.height; i++) {
    for (int j = 0; j < in.width; j++) {
      pout[i][j].r = (255 - pin[i][j].r) + i - 37;
      pout[i][j].g = (255 - pin[i][j].g) + i - 37;
      pout[i][j].b = (255 - pin[i][j].b) + i - 37;
    }
  }

//DIFUSÃO: divide a imagem em duas metades, pega o pixel oposto como referencia e embaralha

  // Percorre apenas a metade superior da imagem para não destrocar o que já foi trocado
  for (int i = 0; i < in.height / 2; i++) {
    for (int j = 0; j < in.width; j++) {
      
      // Cada linha se desloca horizontalmemte (multiplicando por 37)
      int salto_j = (j + i * 37) % in.width;  // % serve para o pixel voltar pela esquerda se passar do limite

      // Encontra a linha correspondente espelhada na metade de baixo
      int linha_oposta = in.height - 1 - i;

      //troca o pixel de cima com o pixel bagunçado de baixo
      Pixel temp = pout[i][j];
      pout[i][j] = pout[linha_oposta][salto_j];
      pout[linha_oposta][salto_j] = temp;
    }
  }

  // Guarda a imagem bagunçada como saida
  stbi_write_png("saida.png", out.width, out.height, 3, pout, 0);
  printf("Imagem 'saida.png' gerada com sucesso!\n");
  // Grava a imagem como PNG para registro
  stbi_write_png("saida.png", out.width, out.height, 3, pout, 0);

  // ETAPA 2: Reversibilidade
  
  // INVERSA DA DIFUSÃO: apenas reaplicar o mesmo código
  for (int i = 0; i < in.height / 2; i++) {
    for (int j = 0; j < in.width; j++) {
      int salto_j = (j + i * 37) % in.width;
      int linha_oposta = in.height - 1 - i;

      Pixel temp = pout[i][j];
      pout[i][j] = pout[linha_oposta][salto_j];
      pout[linha_oposta][salto_j] = temp;
    }
  }

  // INVERSA DA CONFUSÃO: apenas reaplica o codigo invertendo os operadores
  for (int i = 0; i < in.height; i++) {
    for (int j = 0; j < in.width; j++) {
      pin[i][j].r = (255 - pout[i][j].r) + i - 37;
      pin[i][j].g = (255 - pout[i][j].g) + i - 37;
      pin[i][j].b = (255 - pout[i][j].b) + i - 37;
    }
  }

  // Guarda a imagem restaurada
  stbi_write_png("recuperada.png", in.width, in.height, 3, pin, 0);
  printf("Imagem 'recuperada.png' gerada com sucesso!\n");
  
  free(in.pixels);
  free(out.pixels);
}

void load(char* name, Img* pic) {
  pic->pixels =
      (Pixel*)stbi_load(name, &pic->width, &pic->height, &pic->channels, 0);
  if (!pic->pixels) {
    printf("Erro de leitura: %s\n", stbi_failure_reason());
    exit(1);
  }
  printf("Load: %d x %d x %d\n", pic->width, pic->height, pic->channels);
  // Exibe um bloco de 8 x 8 pixels em hexadecimal (teste)
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      printf("[%02X %02X %02X] ", pic->pixels[i].r, pic->pixels[i].g,
             pic->pixels[i].b);
    }
    printf("\n");
  }
}
