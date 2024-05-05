# Desain Bahasa Pemrograman Atjeh
###### Ekstensi file .aceh

## Tipe data:
```python
    array   ==>   meusapat
    int     ==>   numboi
    float   ==>   bicah
    str     ==>   kalimat
    bool    ==>   kiban
    null    ==>   hana

```

## Keyword:
```js
    return    ==>   gisa
    if        ==>   meunyo
    else if   ==>   meunyolaen
    else      ==>   meungkon
    for       ==>   keu
    while     ==>   sira
    function  ==>   buet
    break     ==>   piyoh
    continue  ==>   sambong

```

## Assignment 
```
peubah nama_var :: nilai
```

## Logical Operator:
```lua
    true ==> beutoi
    false ==> salah
    
    and ==> ngon
    or ==> ato
    not ==> kon

```

## Special function:
```
    deuh(x) {hana} // output
    tamong(">>") {str} // input
```

## Desain Original:
Syntax:
```
buet utama() hana ->
    sira(beutoi) ->
        deuh("PROGRAM TES");
        deuh("PILIH\n1. A\n2. B\3. C\n");
        deuh(">> ")
        numboi pilih :: tamong(">> "); // User-Input akan diimplementasikan nanti
        meunyo (pilih == 1) ->
            deuh("OKE");
            piyoh;
        <-
        meunyolaen (pilih == 1) ->
            deuh("PILIH YANG BENAR");
            sambong;
        <-
        meungkon ->
            sambong;
        <-
    <-
<-
```
Penjelasan: Bahasa pemrograman statically-typed yang akan dikompilasi

## Desain yang akan diimplementasi:
Syntax:
```
buet utama() {
    sira(beutoi) {
        deuh("PROGRAM TES");
        deuh("PILIH\n1. A\n2. B\3. C\n");
        deuh(">> ")
        pilih :: tamong(">> "); // User-Input akan diimplementasikan nanti
        meunyo (pilih == 1) {
            deuh("OKE");
            piyoh;
        }
        meunyolaen (pilih == 1) {
            deuh("PILIH YANG BENAR");
            sambong;
        }
        meungkon {
            sambong;
        }
    }
}
```
Penjelasan: Bahasa pemrograman dynamically-typed yang akan diinterpretasi.  

Desain original mungkin akan diimplementasi dimasa depan tapi desain yang sekarang akan dipilih karena saya tidak punya waktu yang cukup untuk menyelesaikan bahasa ini sebelum ultah ke-16.
