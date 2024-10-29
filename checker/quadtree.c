/* DRAGU Nicolae - 314CC */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Structura pentru un element din arbore
typedef struct Nod
{
    // 0 pentru nod INTERN
    // 1 pentru nod FRUNZA
    unsigned char is_leaf;
    unsigned char red, green, blue;
    unsigned long long size;
    // struct Nod *stangaSus, *dreaptaSus, *dreaptaJos, *stangaJos;
    // 0=st sus | 1 = dr sus | 2 = dr jos | 3 = st jos
    struct Nod *children[4];
} Nod;

// Structura pentru arbore
typedef struct QuadTree
{
    unsigned char red, green, blue;
    unsigned char is_leaf;
    unsigned long long size;
    int index, nivel;
} *QuadTree;

// structura pentru matricea de pixeli
typedef struct color
{
    unsigned char red, green, blue;
} color;

// functie de alocare memorie pentru radacina
void initializare_radacina(Nod **temp)
{
    *temp = (Nod *)malloc(sizeof(Nod));
    (*temp)->is_leaf = 0;
    int i;
    for (i = 0; i < 4; i++)
        (*temp)->children[i] = NULL;
    (*temp)->size = 0;
}

// functie de creare a matricei de pixeli
color **citire_image(int width, int height, FILE *fp)
{
    int i;
    color **image;
    // alocare memorie pentru linii
    image = (color **)malloc(height * sizeof(color *));
    for (i = 0; i < height; i++)
        // alocare memorie pentru coloane
        image[i] = (color *)malloc(width * sizeof(color));
    for (i = 0; i < height; i++)
        fread(image[i], sizeof(color), width, fp);
    return image;
}

void patrat_mic(Nod *patrat, color **image, int *numar_noduri, int *numar_blocuriOK, int prag, unsigned long long size, int x, int xsf, int y, int ysf, int *latura)
{
    int i, j;
    unsigned long long red = 0, green = 0, blue = 0, mean = 0;
    // x, xsf si y, ysf sunt coordonatele patratului
    // formulele de calculare din cerinta
    for (i = x; i < xsf; i++)
        for (j = y; j < ysf; j++)
        {
            red += image[i][j].red;
            green += image[i][j].green;
            blue += image[i][j].blue;
        }
    // in cerinta, formulele apar cu size^2, cu size = latura subatricei
    // patratice, in timp ce eu folosesc in functie variabila size precum
    // aria sa, deci nu mai este nevoie de ridicarea sa la putere
    red /= size;
    green /= size;
    blue /= size;
    patrat->red = (unsigned char)red;
    patrat->green = (unsigned char)green;
    patrat->blue = (unsigned char)blue;
    for (i = x; i < xsf; i++)
        for (j = y; j < ysf; j++)
            // formula principala de calculare a mediei
            mean += (red - image[i][j].red) * (red - image[i][j].red) + (green - image[i][j].green) * (green - image[i][j].green) + (blue - image[i][j].blue) * (blue - image[i][j].blue);
    mean /= (3 * size);
    if (mean > prag) // trebuie divizat
    {
        *numar_noduri = *numar_noduri + 1;
        // e nod intern
        patrat->is_leaf = 0;
        for (i = 0; i < 4; i++)
            patrat->children[i] = (Nod *)malloc(sizeof(Nod));
        patrat_mic(patrat->children[0], image, numar_noduri, numar_blocuriOK, prag, size / 4, x, (x + xsf) / 2, y, (y + ysf) / 2, latura);
        patrat_mic(patrat->children[1], image, numar_noduri, numar_blocuriOK, prag, size / 4, x, (x + xsf) / 2, (y + ysf) / 2, ysf, latura);
        patrat_mic(patrat->children[2], image, numar_noduri, numar_blocuriOK, prag, size / 4, (x + xsf) / 2, xsf, (y + ysf) / 2, ysf, latura);
        patrat_mic(patrat->children[3], image, numar_noduri, numar_blocuriOK, prag, size / 4, (x + xsf) / 2, xsf, y, (y + ysf) / 2, latura);
    }
    else // ramane nedivizat
    {
        *numar_noduri = *numar_noduri + 1;
        *numar_blocuriOK = *numar_blocuriOK + 1;
        // e frunza
        patrat->is_leaf = 1;
        patrat->size = size;
        if ((xsf - x) > *latura)
            *latura = (xsf - x);
    }
}

// functie de creare a arborelui din nodurile ce contin datele
void creare_arbore(int *index, int nivel, QuadTree radacina, Nod *element)
{
    radacina[*index].index = *index;
    radacina[*index].nivel = nivel;
    radacina[*index].size = element->size;
    if (element->is_leaf == 0)
    {
        radacina[*index].is_leaf = 0;
        *index = *index + 1;
        int i, copie_nivel;
        // creste un nivel deoarece se aplica functia pe copiii nodului
        copie_nivel = nivel + 1;
        for (i = 0; i < 4; i++)
            creare_arbore(index, copie_nivel, radacina, element->children[i]);
    }
    else
    {
        // e frunza, nu se mai aplica functia
        radacina[*index].red = element->red;
        radacina[*index].green = element->green;
        radacina[*index].blue = element->blue;
        radacina[*index].is_leaf = 1;
        *index = *index + 1;
    }
}

void print_quadtree(color **image, FILE *f, QuadTree quadtree, int numar_noduri, int *indice, int nivel_maxim, unsigned long long size)
{
    int i;
    // se afiseaza dimensiunea imaginii, in format unsigned int
    fwrite((unsigned int *)&size, sizeof(unsigned int), 1, f);
    if (nivel_maxim == 1)
    {
        // poza este o culoare solida
        quadtree[*indice].is_leaf = 1;
        fwrite(&quadtree[*indice].is_leaf, sizeof(unsigned char), 1, f);
        fwrite(&image[0]->red, sizeof(unsigned char), 1, f);
        fwrite(&image[0]->green, sizeof(unsigned char), 1, f);
        fwrite(&image[0]->blue, sizeof(unsigned char), 1, f);
        return;
    }
    for (i = 1; i <= nivel_maxim; i++)
        for (*indice = 0; *indice < numar_noduri; *indice = *indice + 1)
        {
            if (quadtree[*indice].nivel == i)
            {
                if (quadtree[*indice].is_leaf == 1)
                {
                    // e frunza, se afiseaza tipul si valorile pentru red, green, blue
                    fwrite(&quadtree[*indice].is_leaf, sizeof(unsigned char), 1, f);
                    fwrite(&quadtree[*indice].red, sizeof(unsigned char), 1, f);
                    fwrite(&quadtree[*indice].green, sizeof(unsigned char), 1, f);
                    fwrite(&quadtree[*indice].blue, sizeof(unsigned char), 1, f);
                }
                else
                {
                    // nu e frunza, se afiseaza daor tipul nodului
                    fwrite(&quadtree[*indice].is_leaf, sizeof(unsigned char), 1, f);
                }
            }
        }
}

void gasire_nivel(QuadTree quadtree, int *nivel, int numar_noduri)
{
    int i;
    // se cauta nivelul maxim, cautand in fiecare nod nivelul acestuia
    for (i = 0; i < numar_noduri; i++)
    {
        if (*nivel < quadtree[i].nivel)
            *nivel = quadtree[i].nivel;
    }
}

void patrat_mare(color **image, int width, int height, int prag, char *out)
{
    int indice, i, numar_noduri = 1, numar_blocuriOK = 0;
    Nod *radacina;
    initializare_radacina(&radacina);
    // quadtree o sa fie arborele cuaternar
    QuadTree quadtree;
    FILE *g = fopen(out, "w");
    // e radacina, trebuie sa ii alocam memorie pentru copii
    for (i = 0; i < 4; i++)
        radacina->children[i] = (Nod *)malloc(sizeof(Nod));
    unsigned long long size;
    size = width * height;
    int latura = 0, nivel = 1;
    // se verifica daca e imaginea e un singur bloc
    patrat_mic(radacina, image, &numar_noduri, &numar_blocuriOK, prag, size, 0, height, 0, width, &latura);
    if (radacina->is_leaf == 1)
        fprintf(g, "%d\n%d\n%d\n", nivel, numar_blocuriOK, latura);
    else
    {
        numar_blocuriOK = 0;
        numar_noduri = 1;
        // se descompune blocul initial in cele 4 blocuri
        patrat_mic(radacina->children[0], image, &numar_noduri, &numar_blocuriOK, prag, size / 4, 0, height / 2, 0, width / 2, &latura);
        patrat_mic(radacina->children[1], image, &numar_noduri, &numar_blocuriOK, prag, size / 4, 0, height / 2, width / 2, width, &latura);
        patrat_mic(radacina->children[2], image, &numar_noduri, &numar_blocuriOK, prag, size / 4, height / 2, height, width / 2, width, &latura);
        patrat_mic(radacina->children[3], image, &numar_noduri, &numar_blocuriOK, prag, size / 4, height / 2, height, 0, width / 2, &latura);
        // alocam memorie pentru arbore, va fi un vector
        quadtree = (QuadTree)malloc((numar_noduri * 3) * sizeof(QuadTree));
        quadtree[0].nivel = 1;
        indice = 0;
        creare_arbore(&indice, nivel, quadtree, radacina);
        // se afla nivelul
        gasire_nivel(quadtree, &nivel, numar_noduri);
        fprintf(g, "%d\n", nivel);
        fprintf(g, "%d\n", numar_blocuriOK);
        fprintf(g, "%d\n", latura);
    }
    fclose(g);
}

void afisare_arbore(color **image, char *out, int prag, int width, int height)
{
    int i, numar_noduri = 1, numar_blocuriOK = 0, indice;
    Nod *radacina;
    initializare_radacina(&radacina);
    // alocare memorie pentru nodurile arborelui
    QuadTree quadtree;
    // pentru cerinta 2, fisierul in care scrii este binar
    FILE *g = fopen(out, "wb");
    // e radacina, trebuie sa ii alocam memorie pentru copii
    for (i = 0; i < 4; i++)
        radacina->children[i] = (Nod *)malloc(sizeof(Nod));
    unsigned long long size;
    size = width * height;
    int latura = 0, nivel = 1;
    patrat_mic(radacina, image, &numar_noduri, &numar_blocuriOK, prag, size, 0, height, 0, width, &latura);
    if (radacina->is_leaf != 1)
    {
        numar_blocuriOK = 0;
        numar_noduri = 1;
        patrat_mic(radacina->children[0], image, &numar_noduri, &numar_blocuriOK, prag, size / 4, 0, height / 2, 0, width / 2, &latura);
        patrat_mic(radacina->children[1], image, &numar_noduri, &numar_blocuriOK, prag, size / 4, 0, height / 2, width / 2, width, &latura);
        patrat_mic(radacina->children[2], image, &numar_noduri, &numar_blocuriOK, prag, size / 4, height / 2, height, width / 2, width, &latura);
        patrat_mic(radacina->children[3], image, &numar_noduri, &numar_blocuriOK, prag, size / 4, height / 2, height, 0, width / 2, &latura);
        quadtree = (QuadTree)malloc((numar_noduri * 3) * sizeof(QuadTree));
        quadtree[0].nivel = 1;
        indice = 0;
    }
    else
    {
        numar_noduri--;
        quadtree = (QuadTree)malloc((numar_noduri * 3) * sizeof(QuadTree));
        quadtree[0].nivel = 1;
        indice = 0;
    }
    creare_arbore(&indice, nivel, quadtree, radacina);
    gasire_nivel(quadtree, &nivel, numar_noduri);
    // se apeleaza functia ce afiseaza datele
    print_quadtree(image, g, quadtree, numar_noduri, &indice, nivel, width);
    fclose(g);
}

int main(int argc, char *argv[])
{
    // fisierul de intrare
    FILE *fp = fopen(argv[3], "rb");
    // se citeste antetul fisierului .ppm
    char tipPPM[2];
    int width, height;
    int val_max;
    char end[1], cerinta[3];
    fread(tipPPM, sizeof(char), 3, fp);
    fscanf(fp, "%d %d\n", &width, &height);
    fscanf(fp, "%d%c", &val_max, end);
    // image este matricea de pixeli, citita printr-o functie
    color **image = citire_image(width, height, fp);
    // pragul este al doilea argument din linia de comanda
    int prag = atoi(argv[2]);
    // cerinta este primul argument din linia de comanda
    strcpy(cerinta, argv[1]);

    if (strcmp(cerinta, "-c1") == 0) // prima cerinta
        patrat_mare(image, width, height, prag, argv[4]);
    if (strcmp(cerinta, "-c2") == 0) // a doua cerinta
        afisare_arbore(image, argv[4], prag, width, height);

    fclose(fp);
    return 0;
}