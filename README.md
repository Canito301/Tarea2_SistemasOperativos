# Barrera Reutilizable

## Compilación
```bash
make parte1
```
## Uso
```bash
./parte1 <numero de hebras> <numero de etapas>
```
# Simulador de Memoria Virtual
Simulador de memoria virtual con paginación que implementa el algoritmo de reemplazo de páginas **Clock (reloj)**.

## Compilación
```bash
make parte2
```

## Uso
```bash
./parte2 Nmarcos tamaño_marco [--verbose] archivo_traza.txt
```

### Parámetros
- `Nmarcos`: Número de marcos de página disponibles en memoria física
- `tamaño_marco`: Tamaño del marco/página en bytes (debe ser potencia de 2)
- `--verbose`: (Opcional) Muestra detalles de cada traducción de dirección
- `archivo_traza.txt`: Archivo con direcciones virtuales (una por línea, formato decimal o hexadecimal con prefijo 0x)

### Formato del archivo de traza
Cada línea debe contener una dirección virtual en formato decimal o hexadecimal:
```
0x1000
0x2000
8192
0x3000
```

## Ejemplos
```bash
# Sin verbose
./parte2 8 4096 trazas/archivo_traza.txt

# Con verbose
./parte2 4 4096 --verbose trazas/archivo_traza.txt

# Variando el número de marcos
./parte2 4 4096 trazas/archivo_traza.txt
./parte2 8 4096 trazas/archivo_traza.txt
./parte2 16 4096 trazas/archivo_traza.txt

# Con diferentes tamaños de página
./parte2 8 1024 trazas/archivo_traza.txt
./parte2 8 2048 trazas/archivo_traza.txt
./parte2 8 4096 trazas/archivo_traza.txt
```

## Salida
El simulador muestra:
- **Referencias**: Número total de accesos a memoria
- **Fallos de página**: Número de page faults
- **Tasa de fallos**: Porcentaje de fallos de página

En modo `--verbose`, muestra para cada referencia:
- DV (Dirección Virtual), NPV (Número de Página Virtual), Offset
- HIT o FALLO
- Marco asignado
- DF (Dirección Física) resultante

