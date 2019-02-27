# granular_synth
granular_synth in JUCE
Plug-in para DAW Sintesis granular.
Hecho en juce y c++.

- Open file.

    Abre la ventana de busqueda para que el usuario elija el archivo de audio.
    
- Reverse file.

    Invierte el sentido de reproducción de los samples.
    
- Volume.

    A cada grano se le asigna un valor de gain (volúmen). Este valor es aleatorio en un rango [min,max]
      
        Rango total [0, 1].
    
- Frame.

    Solo se consideran samples del archivo dentro del intrvalo min (Sample inicio del frame) y max (Sample final del frame).

        Rango total [1, número de samples del archivo].
      
- Grain size.

    Tamaño del grano o número de samples. Este valor es aleatorio en un rango [min, max].

        Rango total [1, medio segundo ó el número de samples del archivo].

- Density.

    Proporción de granos y silencios. Este valor es aleatorio en un rango [min, max].
      
        Rango total [0, 1].
        
- Repetitions.

    Número de veces que se repite cada grano.
    
        Rango total [0,20].
