# Sistema de Gestion de Barberia

Aplicacion de consola desarrollada en C++ para administrar clientes, productos, servicios, empleados y citas de una barberia.

## Funcionalidades

- Registro, busqueda, edicion y baja de clientes.
- Gestion de inventario de productos y alertas de stock bajo.
- Catalogo de servicios.
- Registro y autenticacion de empleados.
- Agendamiento, reprogramacion y cancelacion de citas.
- Reportes y consultas generales.
- Validaciones y excepciones personalizadas para controlar errores.

## Requisitos

- Windows.
- Compilador compatible con C++17, por ejemplo MinGW-w64/TDM-GCC.

## Compilacion

El codigo fuente principal se encuentra en la carpeta superior, junto a este directorio:

```powershell
cd ".."
g++ -std=c++17 -Wall -Wextra gestionBarberiaFINAL.cpp -o output\gestionBarberia.exe
```

## Ejecucion

Desde la carpeta del proyecto:

```powershell
.\output\gestionBarberia.exe
```

Al iniciar, el sistema solicita registrar al menos un empleado. Luego se debe iniciar sesion con las credenciales creadas durante ese registro.

## Estructura

```text
.
|-- gestionBarberiaFINAL.cpp   # Codigo fuente principal
|-- gestionBarberia.cpp        # Version anterior
|-- output/
    |-- README.md
    |-- .gitignore
    |-- gestionBarberia.exe     # Ejecutable generado
```
