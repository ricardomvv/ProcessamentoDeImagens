#include <iostream>
#define MAX 512
#define strMax 200
#include <cstring>
#include <fstream>
#include <cstdlib>

using namespace std;

struct Img
{
    int largura;
    int altura;
    int pixel[MAX][MAX][3];
};

void lerImagem(char nome [], Img& img);
void salvarImagem(char nome[], Img img);
void equaliza_histograma(Img img_in, Img& img_out);
void computa_negativo(Img img_in, Img& img_out);
void converte_para_cinza(Img img_in, Img& img_out);
void corta_rgb(Img img_in, Img& img_out, int li, int ci, int lf, int cf);
void filtrar_dilatacao(Img img_in, Img& img_out);
void somar_histograma(int h[][256], int hSaida[][256]);
void binariza(Img img_in, Img& img_out, int limiar);
bool verificar_binario (Img img_in);
void converte_para_cinza_ponderado(Img img_in, Img& img_out);

int main()
{
    char nome[strMax], nomeSalvar[strMax];
    Img img, img_out;
    int n;
    do
    {
        cout << "Escolha a opcao de edicao que deseja realizar:" << endl;
        cout << "1 - Cortar Imagem\n2 - Negativo de imagem\n3 - Conversao para niveis de cinza\n4 - Filtragem por dilatacao\n5 - Equalizacao de histograma\n6 - Conversao para niveis de cinza (media ponderada)\n0 - Sair" << endl;
        cin >> n;

        while (n < 0 || n > 6)
        {
            cout << "Digite uma opcao valida:" << endl;
            cin >> n;
        }
        cin.ignore();

        if(n != 0)
        {
            cout << "Digite o nome da imagem que deseja editar:" << endl;
            cin.getline(nome, strMax);
            lerImagem(nome, img);
        }

        switch(n)
        {
        case 5:
            equaliza_histograma(img, img_out);
            break;
        case 2:
            computa_negativo(img, img_out);
            break;
        case 3:
            converte_para_cinza(img,img_out);
            break;
        case 1:
        {
            int li, ci, lf, cf;
            cout << "Digite a linha inicial" << endl;
            cin >> li;
            while (li < 0 || li > (img.altura - 1))
            {
                cout << "Digite uma opcao entre 0 e " << img.altura - 1 << " :" << endl;
                cin >> li;
            }
            cout << "Digite a coluna inicial" << endl;
            cin >> ci;
            while (ci < 0 || ci > (img.largura - 1))
            {
                cout << "Digite uma opcao entre 0 e " << img.largura - 1 << " :" << endl;
                cin >> ci;
            }
            cout << "Digite a linha final" << endl;
            cin >> lf;
            while (lf < li || lf > (img.altura - 1))
            {
                cout << "Digite uma opcao entre " << li << " e " << img.altura - 1 << " :" << endl;
                cin >> lf;
            }
            cout << "Digite a coluna final" << endl;
            cin >> cf;
            while (cf < ci || cf > (img.largura - 1))
            {
                cout << "Digite uma opcao entre " << ci << " e " << img.largura - 1 << " :" << endl;
                cin >> cf;
            }
            corta_rgb(img, img_out,  li, ci, lf, cf);
            cin.ignore();
        };
        break;
        case 4:
        {
            filtrar_dilatacao(img, img_out);
        }
        break;
        case 6:
           converte_para_cinza_ponderado(img,img_out);
            break;
        }
        if(n != 0)
        {
            cout << "Digite o nome que voce deseja salvar a imagem:" << endl;
            cin.getline(nomeSalvar, strMax);
            salvarImagem(nomeSalvar, img_out);
            cout << endl;
        }
    }
    while(n!=0);

    return 0;
}

void lerImagem(char nome [], Img& img)
{
    char caminho[strMax] = "Imagens/";
    strcat(caminho, nome);
    fstream arq;
    arq.open(caminho);

    if(!arq.is_open())
    {
        cout << "Erro: imagem nao encontrada!" << endl;
        exit(0);
    }
    else
    {
        char tipoArquivo[MAX];
        arq.getline(tipoArquivo, MAX);
        if(strcasecmp(tipoArquivo, "P3") != 0)
        {
            cout << "Erro: formato de imagem invalido!" << endl;
            exit(0);
        }
        else
        {
            arq >> img.largura >> img.altura;
            int n;
            arq >> n;
            for(int i = 0; i < img.altura; i++)
            {
                for(int j = 0; j < img.largura; j++)
                {
                    for(int k = 0; k < 3; k++)
                    {
                        arq >> img.pixel[i][j][k];
                    }
                }
            }
        }
    }
    arq.close();
}

void salvarImagem(char nome[], Img img)
{
    ofstream arq;
    char caminho[strMax] = "Imagens/";
    char formato[strMax] = ".ppm";
    strcat(caminho, nome);
    strcat(caminho, formato);
    arq.open(caminho);
    if(!arq.is_open())
    {
        cout << "Erro: nao foi possivel salvar a imagem." << endl;
        exit(0);
    }
    else
    {
        arq << "P3" << endl;
        arq << img.largura << " " << img.altura << endl;
        arq << "255" << endl;
        for(int i = 0; i < img.altura; i++)
        {
            for(int j = 0; j < img.largura; j++)
            {
                for(int k = 0; k < 3; k++)
                {
                    arq  << img.pixel[i][j][k] << " ";
                }
            }
            arq << endl;
        }
         cout << "Imagem salva com sucesso." << endl;
    }
    arq.close();
}

void equaliza_histograma(Img img_in, Img& img_out)
{
    int h[3][256] = {0};

    for(int i = 0; i < img_in.altura; i++)
    {
        for(int j = 0; j < img_in.largura; j++)
        {
            for(int k = 0; k < 3; k++)
                h[k][img_in.pixel[i][j][k]]++;
        }
    }

    int hSaida[3][256];
    somar_histograma(h,hSaida);

    for(int i = 0; i < img_in.altura; i++)
    {
        for(int j = 0; j < img_in.largura; j++)
        {
            for(int k = 0; k < 3; k++)
            {
                img_out.pixel[i][j][k] = (hSaida[k][img_in.pixel[i][j][k]] * 255) / (img_in.altura * img_in.largura);
            }
        }
    }

    img_out.largura = img_in.largura;
    img_out.altura = img_in.altura;
}

void somar_histograma(int h[][256], int hSaida[][256])
{
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 256; j++)
        {
            if(j == 0)
            {
                hSaida[i][j] = h[i][j];
            }
            else
            {
                hSaida[i][j] = h[i][j] + hSaida[i][j-1];
            }
        }
    }
}

void computa_negativo(Img img_in, Img& img_out)
{
    for(int i = 0; i < img_in.altura; i++)
    {
        for(int j = 0; j < img_in.largura; j++)
        {
            for(int k = 0; k < 3; k++)
            {
                img_out.pixel[i][j][k] = 255 - img_in.pixel[i][j][k];
            }
        }
    }
    img_out.largura = img_in.largura;
    img_out.altura = img_in.altura;
}

void converte_para_cinza(Img img_in, Img& img_out)
{
    for(int i = 0; i < img_in.altura; i++)
    {
        for(int j = 0; j < img_in.largura; j++)
        {
            for(int k = 0; k < 3; k++)
            {
                img_out.pixel[i][j][k] = (img_in.pixel[i][j][0] + img_in.pixel[i][j][1] + img_in.pixel[i][j][2])/3;
            }
        }
    }
    img_out.largura = img_in.largura;
    img_out.altura = img_in.altura;
}

void converte_para_cinza_ponderado(Img img_in, Img& img_out)
{
    for(int i = 0; i < img_in.altura; i++)
    {
        for(int j = 0; j < img_in.largura; j++)
        {
            for(int k = 0; k < 3; k++)
            {
                img_out.pixel[i][j][k] = 0.21*(img_in.pixel[i][j][0]) + 0.72*(img_in.pixel[i][j][1]) + 0.07*(img_in.pixel[i][j][2]);
            }
        }
    }
    img_out.largura = img_in.largura;
    img_out.altura = img_in.altura;
}


void corta_rgb(Img img_in, Img& img_out, int li, int ci, int lf, int cf)
{
    int i, j, iS, jS;
    for(i = li, iS = 0; i <= lf; i++, iS++)
    {
        for(j = ci, jS = 0; j <= cf; j++, jS++)
        {
            for(int k = 0; k < 3; k++)
            {
                img_out.pixel[iS][jS][k] = img_in.pixel[i][j][k];
            }
        }
    }
    img_out.altura = iS;
    img_out.largura = jS;
}

void filtrar_dilatacao(Img img_in, Img& img_out)
{
    Img img_aux;
    if(verificar_binario(img_in) == true)
    {
        binariza(img_in,img_aux,127);
    }
    else
    {
        img_aux = img_in;
    }

    for(int i = 0; i < img_aux.altura-1; i++)
    {
        for(int j = 0; j < img_aux.largura-1; j++)
        {
            for(int k = 0; k < 3; k++)
            {
                if(img_aux.pixel[i][j][k] == 255 || img_aux.pixel[i+1][j][k] == 255 || img_aux.pixel[i][j+1][k] == 255 || img_aux.pixel[i+1][j+1][k] == 255)
                {
                    img_out.pixel[i][j][k] = 255;
                }
                else
                {
                    img_out.pixel[i][j][k] = 0;
                }
            }
        }
    }
    img_out.largura = img_aux.largura;
    img_out.altura = img_aux.altura;

}

bool verificar_binario(Img img_in)
{
    for(int i = 0; i < img_in.altura; i++)
    {
        for(int j = 0; j < img_in.largura; j++)
        {
            for(int k = 0; k < 3; k++)
            {
                if(img_in.pixel[i][j][k]!= 255 || img_in.pixel[i][j][k] != 0)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

void binariza(Img img_in, Img& img_out, int limiar)
{
    int aux = 0;

     for(int i = 0; i < img_in.altura; i++)
    {
        for(int j = 0; j < img_in.largura; j++)
        {
            aux = (img_in.pixel[i][j][0] + img_in.pixel[i][j][1] + img_in.pixel[i][j][2])/3;

            if(aux < limiar)
            {
                for(int k = 0; k < 3; k++)
                {
                    img_out.pixel[i][j][k]= 0;
                }
            }
            else
            {
                for(int k = 0; k < 3; k++)
                {
                    img_out.pixel[i][j][k] = 255;
                }
            }
        }
    }
    img_out.largura = img_in.largura;
    img_out.altura = img_in.altura;
}

















