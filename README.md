# 006-albero-magico
Albero usato durante la diretta del Muro di Hyde

Il dispositivo è composto dai seguenti elementi:

* https://amzn.to/3tW567Z : Arduino Nano
* https://amzn.to/3SxNTfJ : Arduino UNO
* https://amzn.to/3rZgEXL : MAX4466 Microfono
* https://amzn.to/3MgNdqW : Cavo mini USB
* https://amzn.to/3FyPwlm : Caricatore USB

## Software
Il software si suddivide in due parti:

### Collaudo e calibrazione
La prima parte del software è il test_microfono.ino che serve a valutare il segnale di ingresso e calibrare il volume del preamplificatore.

### Funzionalità completa
La seconda parte albero_magico.ino esegue la decifratura delle armoniche per mezzo di FFT (Fast Fourier Transform) e attiva l'uscita con un gioco di luci.
