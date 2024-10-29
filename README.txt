/* DRAGU Nicolae - 314CC */

    Am ales sa construiesc cate o structura diferita pentru arborele in sine 
si nodurile acestuia. Prima oara citesc inceputul fisierului de intrare, .ppm,
unde se stie mereu cu ce va incepe. Variabila end citeste "\n" de dupa valoarea
maxima pe care poate sa o ia o culoare. Astfel se trece pe urmatorul rand,
de unde se incepe citirea matricei de pixeli. 
    Pentru prima cerinta am ales sa lucrez cu nodurile initial, pe care le 
modific pe parcursul functiei patrat_mare. Dupa ce trec prin toata matricea,
deci am parcurs toate nodurile, alcatuiesc arborele prin functia creare_arbore.
Apelez recursiv functia patrat_mic, ce imparte patratul mare in 4 patrate cu
aria un sfert din aria imaginii, pana cand se ajunge la o culoare medie mai
mica sau egala cu pragul dat. Cand observ ca nu se mai imparte patratul, 
calculez latura acestuia impartind aria la 2, din formula ariei patratului.
Totodata, numar cate patrate au culoarea medie mai mica sau egala decat prag.
Acest numar este numarul de blocuri din imagine pentru care scorul 
similaritatii pixelilor este mai mic sau egal decât factorul furnizat.
    Pentru a doua cerinta alcatuiesc arborele analog cerintei 1, doar ca 
acum afisez continutul nodurilor arborelui, conform cerintei.