# Barbershop Management System

C++ console application developed to manage barbershop clients, products, services, employees, and appointments.

## Features

- Client registration, search, editing, and deactivation.
- Product inventory management and low-stock alerts.
- Service catalog.
- Employee registration and authentication.
- Appointment scheduling, rescheduling, and cancellation.
- General reports and queries.
- Custom validations and exceptions for error handling.

## Requirements

- Windows.
- C++17-compatible compiler, such as MinGW-w64/TDM-GCC.

## Compilation

The main source code is located in the parent folder, alongside this directory:

```powershell
cd ".."
g++ -std=c++17 -Wall -Wextra gestionBarberiaFINAL.cpp -o output\gestionBarberia.exe
```

## Execution

From the project folder:

```powershell
.\output\gestionBarberia.exe
```

When starting, the system asks you to register at least one employee. You must then log in using the credentials created during that registration.

## Structure

```text
.
|-- gestionBarberiaFINAL.cpp   # Main source code
|-- gestionBarberia.cpp        # Previous version
|-- output/
    |-- README.md
    |-- .gitignore
    |-- gestionBarberia.exe     # Generated executable
```
