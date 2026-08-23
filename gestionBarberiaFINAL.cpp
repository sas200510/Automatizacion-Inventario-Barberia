//Isaac Espinosa 00342611 - Sebastian Alcoser 00337279
//PROYECTO FINAL - SISTEMA DE BARBERIA
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <iomanip>
using namespace std;

//-----Definiciones de Clases y Funciones CRUD-----

// ==================== EXCEPCIONES PERSONALIZADAS ====================
class NotFoundException : public runtime_error {
public:
    NotFoundException(const string& msg) : runtime_error(msg) {}
};

class ValidationException : public runtime_error {
public:
    ValidationException(const string& msg) : runtime_error(msg) {}
};

class OverlapException : public runtime_error {
public:
    OverlapException(const string& msg) : runtime_error(msg) {}
};

// ==================== INTERFAZ AUTENTICABLE ====================
class Autenticable {
public: 
    virtual bool autenticar(const string& user, const string& pass) const = 0;
    virtual ~Autenticable() = default;
};

// ==================== CLASE ABSTRACTA PERSONA ====================
class Persona {
protected: 
    int id;
    string nombre;
    string telefono;
public: 
   Persona(int id, const string& nombre, const string& telefono)
    : id(id), nombre(nombre)
{
    setTelefono(telefono);  
}

    virtual ~Persona() = default;
    virtual string rol() const = 0;
    
    int getId() const { return id; }
    const string& getNombre() const { return nombre; }
    const string& getTelefono() const { return telefono; }
    void setNombre(const string& n) { 
        if (n.empty()) throw ValidationException("El nombre no puede estar vacio");
        nombre = n; 
    }
    bool esNumero(const string& s) {
    if (s.empty()) return false;
    for (char c : s) {
        if (c < '0' || c > '9') return false;
    }
    return true;
}

    void setTelefono(const string& t) {
    if (t.empty())
        throw ValidationException("El telefono no puede estar vacio");

    if (!esNumero(t))
        throw ValidationException("El telefono debe contener solo numeros");

    telefono = t;
}

};

// ==================== CLASE CLIENTE ====================
class Cliente : public Persona {
private:
    string email;
    string notas;
    bool activo;
public:
    Cliente(int id, const string& nombre, const string& telefono, const string& email, const string& notas)
        : Persona(id, nombre, telefono), email(email), notas(notas), activo(true) {
        validarEmail(email);
    }
    
    string rol() const override { return "Cliente"; }
    
    void validarEmail(const string& e) {
        bool contieneArroba = false;
        for (char c : e) {
            if (c == '@') {
                contieneArroba = true;
                break;
            }
        }
        if (!contieneArroba) throw ValidationException("Email invalido: debe contener @");
    }
    
    const string& getEmail() const { return email; }
    const string& getNotas() const { return notas; }
    bool isActivo() const { return activo; }
    
    void setEmail(const string& e) { 
        validarEmail(e);
        email = e; 
    }
    void setNotas(const string& n) { notas = n; }
    void setActivo(bool a) { activo = a; }
    
    string toString() const {
        return "Cliente ID: " + to_string(id) + ", Nombre: " + nombre + 
               ", Email: " + email + ", Telefono: " + telefono + 
               ", Activo: " + (activo ? "Si" : "No");
    }
};

// ==================== CLASE EMPLEADO ====================
class Empleado : public Persona, public Autenticable {
private: 
    string usuario;
    string contrasena;
    string cargo;
public: 
    Empleado(int id, const string& nombre, const string& telefono, 
             const string& usuario, const string& contrasena, const string& cargo)
        : Persona(id, nombre, telefono), usuario(usuario), 
          contrasena(contrasena), cargo(cargo) {}
    
    bool autenticar(const string& user, const string& pass) const override {
        return user == usuario && pass == contrasena;
    }
    
    string rol() const override { return "Empleado"; }
    const string& getCargo() const { return cargo; }
    const string& getUsuario() const { return usuario; }
};

// ==================== CLASE ABSTRACTA ITEM ====================
class Item {
protected:
    int id;
    string nombre;
    double precio;
public:
    Item(int id, const string& nombre, double precio) 
        : id(id), nombre(nombre), precio(precio) {
        if (precio < 0) throw ValidationException("El precio no puede ser negativo");
    }
    virtual ~Item() = default;
    
    virtual double precioFinal() const = 0;
    virtual string toString() const = 0;
    
    int getId() const { return id; }
    const string& getNombre() const { return nombre; }
    double getPrecio() const { return precio; }
    
    void setNombre(const string& n) { 
        if (n.empty()) throw ValidationException("El nombre no puede estar vacio");
        nombre = n; 
    }
    void setPrecio(double p) { 
        if (p < 0) throw ValidationException("El precio no puede ser negativo");
        precio = p; 
    }
};

// ==================== CLASE PRODUCTO ====================
class Producto : public Item {
private:
    int stock;
public:
    Producto(int id, const string& nombre, double precio, int stock)
        : Item(id, nombre, precio), stock(stock) {
        if (stock < 0) throw ValidationException("El stock no puede ser negativo");
    }
    
    double precioFinal() const override { return precio; }
    
    string toString() const override {
        return "Producto ID: " + to_string(id) + ", Nombre: " + nombre + 
               ", Precio: $" + to_string(precio) + ", Stock: " + to_string(stock);
    }
    
    int getStock() const { return stock; }
    void setStock(int s) { 
        if (s < 0) throw ValidationException("El stock no puede ser negativo");
        stock = s; 
    }
};

// ==================== CLASE SERVICIO ====================
class Servicio : public Item {
private:
    int duracionMin;
public:
    Servicio(int id, const string& nombre, double precio, int duracionMin)
        : Item(id, nombre, precio), duracionMin(duracionMin) {
        if (duracionMin <= 0) throw ValidationException("La duracion debe ser positiva");
    }
    
    double precioFinal() const override { return precio; }
    
    string toString() const override {
        return "Servicio ID: " + to_string(id) + ", Nombre: " + nombre + 
               ", Precio: $" + to_string(precio) + ", Duracion: " + to_string(duracionMin) + " min";
    }
    
    int getDuracionMin() const { return duracionMin; }
    void setDuracionMin(int d) { 
        if (d <= 0) throw ValidationException("La duracion debe ser positiva");
        duracionMin = d; 
    }
};

// ==================== CLASE CITA ====================
class Cita {
private:
    int id;
    Cliente* cliente;
    Servicio* servicio;
    Empleado* empleado;
    string fechaHora;
    bool confirmada;
public:
    Cita(int id, Cliente* cliente, Servicio* servicio, Empleado* empleado, const string& fechaHora)
        : id(id), cliente(cliente), servicio(servicio), empleado(empleado), 
          fechaHora(fechaHora), confirmada(true) {}
    
    int getId() const { return id; }
    Cliente* getCliente() const { return cliente; }
    Servicio* getServicio() const { return servicio; }
    Empleado* getEmpleado() const { return empleado; }
    const string& getFechaHora() const { return fechaHora; }
    bool isConfirmada() const { return confirmada; }
    
    void setFechaHora(const string& fh) { fechaHora = fh; }
    void setConfirmada(bool c) { confirmada = c; }
    
    string toString() const {
        return "Cita ID: " + to_string(id) + 
               ", Cliente: " + cliente->getNombre() + 
               ", Servicio: " + servicio->getNombre() + 
               ", Empleado: " + empleado->getNombre() + 
               ", Fecha/Hora: " + fechaHora +
               ", Estado: " + (confirmada ? "Confirmada" : "Cancelada");
    }
};

// ==================== CLASE TEMPLATE GESTOR ====================
template <typename T>
class Gestor {
protected:
    vector<T*> coleccion;
public:
    virtual ~Gestor() {
        for (auto item : coleccion) {
            delete item;
        }
    }
    
    void agregar(T* item) {
        coleccion.push_back(item);
    }
    
    T* buscar(int id) {
        for (auto item : coleccion) {
            if (item->getId() == id) {
                return item;
            }
        }
        throw NotFoundException("No se encontro el elemento con ID: " + to_string(id));
    }
    
    void listar() const {
        if (coleccion.empty()) {
            cout << "No hay elementos registrados.\n";
            return;
        }
        for (const auto& item : coleccion) {
            cout << item->toString() << endl;
        }
    }
    
    const vector<T*>& getColeccion() const { return coleccion; }
};

// ==================== CLASE INVENTARIO ====================
class Inventario : public Gestor<Item> {
public:
    void editar(int id) {
        try {
            Item* item = buscar(id);
            double nuevoPrecio;
            cout << "Nuevo precio: ";
            cin >> nuevoPrecio;
            item->setPrecio(nuevoPrecio);
            
            Producto* producto = dynamic_cast<Producto*>(item);
            if (producto) {
                int nuevoStock;
                cout << "Nuevo stock: ";
                cin >> nuevoStock;
                producto->setStock(nuevoStock);
            }
            
            cout << "Item actualizado exitosamente.\n";
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
        }
    }
    
    void eliminar(int id) {
        try {
            Item* item = buscar(id);
            for (size_t i = 0; i < coleccion.size(); ++i) {
                if (coleccion[i]->getId() == id) {
                    delete coleccion[i];
                    coleccion.erase(coleccion.begin() + i);
                    cout << "Item eliminado exitosamente.\n";
                    return;
                }
            }
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
        }
    }
    
    void buscarPorNombre(const string& nombre) const {
        bool encontrado = false;
        for (const auto& item : coleccion) {
            if (item->getNombre().find(nombre) != string::npos) {
                cout << item->toString() << endl;
                encontrado = true;
            }
        }
        if (!encontrado) {
            cout << "No se encontraron items con ese nombre.\n";
        }
    }
    
    void verificarStockBajo() const {
        cout << "\n=== PRODUCTOS CON STOCK BAJO (<5) ===\n";
        bool hayBajoStock = false;
        for (const auto& item : coleccion) {
            Producto* producto = dynamic_cast<Producto*>(item);
            if (producto && producto->getStock() < 5) {
                cout << "ALERTA: " << producto->toString() << endl;
                hayBajoStock = true;
            }
        }
        if (!hayBajoStock) {
            cout << "Todos los productos tienen stock suficiente.\n";
        }
    }
};

// ==================== CLASE AGENDA ====================
class Agenda : public Gestor<Cita> {
public:
    void agendar(Cita* cita) {
        try {
            confirmaDisponibilidad(cita);
            agregar(cita);
            cout << "Cita agendada exitosamente con ID: " << cita->getId() << endl;
        } catch (const exception& e) {
            cout << "Error al agendar: " << e.what() << endl;
            delete cita;
        }
    }
    
    void confirmaDisponibilidad(Cita* nuevaCita) {
        for (const auto& cita : coleccion) {
            if (cita->getEmpleado()->getId() == nuevaCita->getEmpleado()->getId() &&
                cita->getFechaHora() == nuevaCita->getFechaHora() &&
                cita->isConfirmada()) {
                throw OverlapException("El empleado ya tiene una cita en ese horario");
            }
        }
    }
    
    void reprogramar(int id) {
        try {
            Cita* cita = buscar(id);
            string nuevaFechaHora;
            cout << "Nueva fecha/hora (formato: DD/MM/YYYY HH:MM): ";
            cin.ignore();
            getline(cin, nuevaFechaHora);
            cita->setFechaHora(nuevaFechaHora);
            confirmaDisponibilidad(cita);
            cout << "Cita reprogramada exitosamente.\n";
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
        }
    }
    
    void cancelar(int id) {
        try {
            Cita* cita = buscar(id);
            cita->setConfirmada(false);
            cout << "Cita cancelada exitosamente.\n";
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
        }
    }
    
    void listarPorEmpleado(int empId) const {
        bool encontrado = false;
        for (const auto& cita : coleccion) {
            if (cita->getEmpleado()->getId() == empId && cita->isConfirmada()) {
                cout << cita->toString() << endl;
                encontrado = true;
            }
        }
        if (!encontrado) {
            cout << "No hay citas para ese empleado.\n";
        }
    }
};

// ==================== VECTORES GLOBALES ====================
vector<Cliente*> clientes;
vector<Empleado*> empleados;
Inventario inventario;
Agenda agenda;
int siguienteIdCita = 1;

// ==================== FUNCIONES CRUD CLIENTES ====================
void registrarCliente() {
    try {
        int id;
        string nombre, telefono, email, notas;
        
        cout << "ID unico: ";
        cin >> id;
        
        for (auto c : clientes) {
            if (c->getId() == id) {
                throw ValidationException("El ID ya existe");
            }
        }
        
        cin.ignore();
        cout << "Nombre: ";
        getline(cin, nombre);
        if (nombre.empty()) throw ValidationException("El nombre no puede estar vacio");
        
        cout << "Telefono: ";
        getline(cin, telefono);
        if (telefono.empty()) throw ValidationException("El telefono no puede estar vacio");
        
        cout << "Email: ";
        getline(cin, email);
        
        cout << "Notas: ";
        getline(cin, notas);
        
        clientes.push_back(new Cliente(id, nombre, telefono, email, notas));
        cout << "Cliente registrado exitosamente.\n";
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}

void buscarCliente() {
    string busqueda;
    cout << "Buscar por nombre o ID: ";
    cin.ignore();
    getline(cin, busqueda);
    
    bool encontrado = false;
    for (auto c : clientes) {
        if (c->getNombre().find(busqueda) != string::npos || 
            to_string(c->getId()) == busqueda) {
            cout << c->toString() << endl;
            encontrado = true;
        }
    }
    if (!encontrado) {
        cout << "No se encontraron clientes.\n";
    }
}

void editarCliente() {
    try {
        int id;
        cout << "ID del cliente a editar: ";
        cin >> id;
        
        Cliente* cliente = nullptr;
        for (auto c : clientes) {
            if (c->getId() == id) {
                cliente = c;
                break;
            }
        }
        
        if (!cliente) throw NotFoundException("Cliente no encontrado");
        
        string nuevoTelefono, nuevasNotas, nuevoEmail;
        cin.ignore();
        cout << "Nuevo telefono (actual: " << cliente->getTelefono() << "): ";
        getline(cin, nuevoTelefono);
        cout << "Nuevo email (actual: " << cliente->getEmail() << "): ";
        getline(cin, nuevoEmail);
        cout << "Nuevas notas: ";
        getline(cin, nuevasNotas);
        
        cliente->setTelefono(nuevoTelefono);
        cliente->setEmail(nuevoEmail);
        cliente->setNotas(nuevasNotas);
        
        cout << "Cliente actualizado exitosamente.\n";
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}

void eliminarCliente() {
    try {
        int id;
        cout << "ID del cliente a dar de baja: ";
        cin >> id;
        
        Cliente* cliente = nullptr;
        for (auto c : clientes) {
            if (c->getId() == id) {
                cliente = c;
                break;
            }
        }
        
        if (!cliente) throw NotFoundException("Cliente no encontrado");
        
        cliente->setActivo(false);
        cout << "Cliente dado de baja (inactivo). Su historial se conserva.\n";
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}

void activarCliente() {
    try {
        int id;
        cout << "ID del cliente a activar: ";
        cin >> id;
        
        Cliente* cliente = nullptr;
        for (auto c : clientes) {
            if (c->getId() == id) {
                cliente = c;
                break;
            }
        }
        
        if (!cliente) throw NotFoundException("Cliente no encontrado");
        
        cliente->setActivo(true);
        cout << "Cliente nuevamente activo. Su historial se conserva.\n";
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}

void listarClientes() {
    if (clientes.empty()) {
        cout << "No hay clientes registrados.\n";
        return;
    }
    cout << "\n=== LISTA DE CLIENTES ===\n";
    for (auto c : clientes) {
        if (c->isActivo()) {
            cout << c->toString() << endl;
        }
    }
}

// ==================== FUNCIONES CRUD PRODUCTOS ====================
void registrarProducto() {
    try {
        int id, stock;
        string nombre;
        double precio;
        
        cout << "ID unico: ";
        cin >> id;
        
        try {
            inventario.buscar(id);
            throw ValidationException("El ID ya existe");
        } catch (const NotFoundException&) {
            // ID no existe, podemos continuar
        }
        
        cin.ignore();
        cout << "Nombre: ";
        getline(cin, nombre);
        cout << "Precio: ";
        cin >> precio;
        cout << "Stock: ";
        cin >> stock;
        
        inventario.agregar(new Producto(id, nombre, precio, stock));
        cout << "Producto registrado exitosamente.\n";
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}

void registrarServicio() {
    try {
        int id, duracion;
        string nombre;
        double precio;
        
        cout << "ID unico: ";
        cin >> id;
        
        try {
            inventario.buscar(id);
            throw ValidationException("El ID ya existe");
        } catch (const NotFoundException&) {
            // ID no existe, podemos continuar
        }
        
        cin.ignore();
        cout << "Nombre: ";
        getline(cin, nombre);
        cout << "Precio: ";
        cin >> precio;
        cout << "Duracion (minutos): ";
        cin >> duracion;
        
        inventario.agregar(new Servicio(id, nombre, precio, duracion));
        cout << "Servicio registrado exitosamente.\n";
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}

// ==================== FUNCIONES CRUD CITAS ====================
void agendarCita() {
    try {
        if (clientes.empty()) {
            cout << "No hay clientes registrados.\n";
            return;
        }
        if (empleados.empty()) {
            cout << "No hay empleados registrados.\n";
            return;
        }
        
        int clienteId, servicioId, empleadoId;
        string fechaHora;
        
        cout << "\n=== CLIENTES DISPONIBLES ===\n";
        listarClientes();
        cout << "ID del cliente: ";
        cin >> clienteId;
        
        Cliente* cliente = nullptr;
        for (auto c : clientes) {
            if (c->getId() == clienteId && c->isActivo()) {
                cliente = c;
                break;
            }
        }
        if (!cliente) throw NotFoundException("Cliente no encontrado o inactivo");
        
        cout << "\n=== SERVICIOS DISPONIBLES ===\n";
        for (const auto& item : inventario.getColeccion()) {
            Servicio* servicio = dynamic_cast<Servicio*>(item);
            if (servicio) {
                cout << servicio->toString() << endl;
            }
        }
        cout << "ID del servicio: ";
        cin >> servicioId;
        
        Item* item = inventario.buscar(servicioId);
        Servicio* servicio = dynamic_cast<Servicio*>(item);
        if (!servicio) throw ValidationException("El ID no corresponde a un servicio");
        
        cout << "\n=== EMPLEADOS DISPONIBLES ===\n";
        for (auto e : empleados) {
            cout << "ID: " << e->getId() << ", Nombre: " << e->getNombre() 
                 << ", Cargo: " << e->getCargo() << endl;
        }
        cout << "ID del empleado: ";
        cin >> empleadoId;
        
        Empleado* empleado = nullptr;
        for (auto e : empleados) {
            if (e->getId() == empleadoId) {
                empleado = e;
                break;
            }
        }
        if (!empleado) throw NotFoundException("Empleado no encontrado");
        
        cin.ignore();
        cout << "Fecha/hora (formato: DD/MM/YYYY HH:MM): ";
        getline(cin, fechaHora);
        
        Cita* nuevaCita = new Cita(siguienteIdCita++, cliente, servicio, empleado, fechaHora);
        agenda.agendar(nuevaCita);
        
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}

void registrarEmpleados() {
    int cantidad;
    cout << "Cuantos empleados desea registrar?: ";
    cin >> cantidad;

    for (int i = 0; i < cantidad; ++i) {
        try {
            int id;
            string nombre, telefono, usuario, contrasena, cargo;

            cout << "\n=== Empleado " << (i + 1) << " ===\n";
            cout << "ID: ";
            cin >> id;
            
            for (auto e : empleados) {
                if (e->getId() == id) {
                    throw ValidationException("El ID ya existe");
                }
            }
            
            cin.ignore();
            cout << "Nombre: ";
            getline(cin, nombre);
            cout << "Telefono: ";
            getline(cin, telefono);
            cout << "Usuario: ";
            getline(cin, usuario);
            cout << "Contrasena: ";
            getline(cin, contrasena);
            cout << "Cargo: ";
            getline(cin, cargo);

            empleados.push_back(new Empleado(id, nombre, telefono, usuario, contrasena, cargo));
            cout << "Empleado registrado exitosamente.\n";
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
            --i;
        }
    }
}

//-----MENUS Y MAIN-----

// ==================== MENUS DEL SISTEMA ====================

void menuClientes() {
    int opcion;
    do {
        cout << "\n========================================\n";
        cout << "        MODULO GESTION DE CLIENTES      \n";
        cout << "========================================\n";
        cout << "1. Registrar cliente\n";
        cout << "2. Buscar clientes\n";
        cout << "3. Listar todos los clientes activos\n";
        cout << "4. Editar cliente\n";
        cout << "5. Dar de baja cliente\n";
        cout << "6. Volver al menu principal\n";
        cout<<"7. Activar cliente\n";
        cout << "========================================\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
        case 1: registrarCliente(); break;
        case 2: buscarCliente(); break;
        case 3: listarClientes(); break;
        case 4: editarCliente(); break;
        case 5: eliminarCliente(); break;
        case 7: activarCliente(); break;
        case 6: 
            cout << "Volviendo al menu principal...\n"; 
            return;
        default: 
            cout << "Opcion invalida. Intente nuevamente.\n"; 
            break;
        }
    } while (true);
}

void menuProductos() {
    int opcion;
    do {
        cout << "\n========================================\n";
        cout << "       MODULO GESTION DE PRODUCTOS      \n";
        cout << "========================================\n";
        cout << "1. Registrar producto\n";
        cout << "2. Consultar inventario completo\n";
        cout << "3. Buscar producto por nombre\n";
        cout << "4. Verificar productos con stock bajo\n";
        cout << "5. Editar producto\n";
        cout << "6. Eliminar producto\n";
        cout << "7. Volver al menu principal\n";
        cout << "========================================\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
        case 1: registrarProducto(); break;
        case 2: 
            cout << "\n=== INVENTARIO COMPLETO ===\n";
            inventario.listar(); 
            break;
        case 3: {
            string nombre;
            cout << "Nombre del producto a buscar: ";
            cin.ignore();
            getline(cin, nombre);
            inventario.buscarPorNombre(nombre);
            break;
        }
        case 4: inventario.verificarStockBajo(); break;
        case 5: {
            int id;
            cout << "ID del producto a editar: ";
            cin >> id;
            inventario.editar(id);
            break;
        }
        case 6: {
            int id;
            cout << "ID del producto a eliminar: ";
            cin >> id;
            inventario.eliminar(id);
            break;
        }
        case 7: 
            cout << "Volviendo al menu principal...\n"; 
            return;
        default: 
            cout << "Opcion invalida. Intente nuevamente.\n"; 
            break;
        }
    } while (true);
}

void menuServicios() {
    int opcion;
    do {
        cout << "\n========================================\n";
        cout << "       MODULO GESTION DE SERVICIOS      \n";
        cout << "========================================\n";
        cout << "1. Registrar servicio\n";
        cout << "2. Consultar catalogo de servicios\n";
        cout << "3. Buscar servicio por nombre\n";
        cout << "4. Editar servicio\n";
        cout << "5. Eliminar servicio\n";
        cout << "6. Volver al menu principal\n";
        cout << "========================================\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
        case 1: registrarServicio(); break;
        case 2: {
            cout << "\n=== CATALOGO DE SERVICIOS ===\n";
            bool hayServicios = false;
            for (const auto& item : inventario.getColeccion()) {
                Servicio* servicio = dynamic_cast<Servicio*>(item);
                if (servicio) {
                    cout << servicio->toString() << endl;
                    hayServicios = true;
                }
            }
            if (!hayServicios) {
                cout << "No hay servicios registrados.\n";
            }
            break;
        }
        case 3: {
            string nombre;
            cout << "Nombre del servicio a buscar: ";
            cin.ignore();
            getline(cin, nombre);
            bool encontrado = false;
            for (const auto& item : inventario.getColeccion()) {
                Servicio* servicio = dynamic_cast<Servicio*>(item);
                if (servicio && servicio->getNombre().find(nombre) != string::npos) {
                    cout << servicio->toString() << endl;
                    encontrado = true;
                }
            }
            if (!encontrado) {
                cout << "No se encontraron servicios con ese nombre.\n";
            }
            break;
        }
        case 4: {
            int id;
            cout << "ID del servicio a editar: ";
            cin >> id;
            try {
                Item* item = inventario.buscar(id);
                Servicio* servicio = dynamic_cast<Servicio*>(item);
                if (!servicio) {
                    cout << "El ID no corresponde a un servicio.\n";
                } else {
                    double nuevoPrecio;
                    int nuevaDuracion;
                    cout << "Nuevo precio: ";
                    cin >> nuevoPrecio;
                    cout << "Nueva duracion (minutos): ";
                    cin >> nuevaDuracion;
                    servicio->setPrecio(nuevoPrecio);
                    servicio->setDuracionMin(nuevaDuracion);
                    cout << "Servicio actualizado exitosamente.\n";
                }
            } catch (const exception& e) {
                cout << "Error: " << e.what() << endl;
            }
            break;
        }
        case 5: {
            int id;
            cout << "ID del servicio a eliminar: ";
            cin >> id;
            inventario.eliminar(id);
            break;
        }
        case 6: 
            cout << "Volviendo al menu principal...\n"; 
            return;
        default: 
            cout << "Opcion invalida. Intente nuevamente.\n"; 
            break;
        }
    } while (true);
}

void menuCitas() {
    int opcion;
    do {
        cout << "\n========================================\n";
        cout << "         MODULO GESTION DE CITAS        \n";
        cout << "========================================\n";
        cout << "1. Agendar nueva cita\n";
        cout << "2. Consultar todas las citas\n";
        cout << "3. Consultar citas por empleado\n";
        cout << "4. Reprogramar cita\n";
        cout << "5. Cancelar cita\n";
        cout << "6. Volver al menu principal\n";
        cout << "========================================\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
        case 1: agendarCita(); break;
        case 2: 
            cout << "\n=== TODAS LAS CITAS ===\n";
            agenda.listar(); 
            break;
        case 3: {
            int empId;
            cout << "ID del empleado: ";
            cin >> empId;
            cout << "\n=== CITAS DEL EMPLEADO " << empId << " ===\n";
            agenda.listarPorEmpleado(empId);
            break;
        }
        case 4: {
            int id;
            cout << "ID de la cita a reprogramar: ";
            cin >> id;
            agenda.reprogramar(id);
            break;
        }
        case 5: {
            int id;
            cout << "ID de la cita a cancelar: ";
            cin >> id;
            agenda.cancelar(id);
            break;
        }
        case 6: 
            cout << "Volviendo al menu principal...\n"; 
            return;
        default: 
            cout << "Opcion invalida. Intente nuevamente.\n"; 
            break;
        }
    } while (true);
}

bool menuLogin(Empleado*& sesionActiva) {
    string user, pass;
    int intentos = 0;
    const int MAX_INTENTOS = 3;
    
    cout << "\n========================================\n";
    cout << "     SISTEMA DE GESTION - BARBERIA      \n";
    cout << "========================================\n";
    cout << "           INICIO DE SESION             \n";
    cout << "========================================\n";
    
    while (intentos < MAX_INTENTOS) {
        cout << "Usuario: ";
        cin >> user;
        cout << "Contrasena: ";
        cin >> pass;

        for (auto e : empleados) {
            if (e->autenticar(user, pass)) {
                sesionActiva = e;
                cout << "\n========================================\n";
                cout << "    BIENVENIDO AL SISTEMA, " << e->getNombre() << "\n";
                cout << "    Rol: " << e->rol() << " (" << e->getCargo() << ")\n";
                cout << "========================================\n";
                return true;
            }
        }
        
        intentos++;
        cout << "Credenciales incorrectas. ";
        if (intentos < MAX_INTENTOS) {
            cout << "Intentos restantes: " << (MAX_INTENTOS - intentos) << endl;
        }
    }
    
    cout << "\nNumero maximo de intentos alcanzado. Acceso denegado.\n";
    return false;
}

void menuPrincipal(Empleado* sesion) {
    int opcion;
    do {
        cout << "\n========================================\n";
        cout << "           MENU PRINCIPAL               \n";
        cout << "========================================\n";
        cout << " Usuario: " << sesion->getNombre() << " (" << sesion->getCargo() << ")\n";
        cout << "========================================\n";
        cout << "1. Gestion de Clientes\n";
        cout << "2. Gestion de Productos\n";
        cout << "3. Gestion de Servicios\n";
        cout << "4. Gestion de Citas\n";
        cout << "5. Reportes y Consultas\n";
        cout << "6. Cerrar sesion\n";
        cout << "========================================\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
        case 1: menuClientes(); break;
        case 2: menuProductos(); break;
        case 3: menuServicios(); break;
        case 4: menuCitas(); break;
        case 5: {
            cout << "\n=== REPORTES Y CONSULTAS ===\n";
            cout << "1. Total de clientes activos: ";
            int clientesActivos = 0;
            for (auto c : clientes) {
                if (c->isActivo()) clientesActivos++;
            }
            cout << clientesActivos << endl;
            
            cout << "2. Total de productos en inventario: ";
            int totalProductos = 0;
            for (const auto& item : inventario.getColeccion()) {
                if (dynamic_cast<Producto*>(item)) totalProductos++;
            }
            cout << totalProductos << endl;
            
            cout << "3. Total de servicios disponibles: ";
            int totalServicios = 0;
            for (const auto& item : inventario.getColeccion()) {
                if (dynamic_cast<Servicio*>(item)) totalServicios++;
            }
            cout << totalServicios << endl;
            
            cout << "4. Total de citas agendadas: ";
            int citasConfirmadas = 0;
            for (const auto& cita : agenda.getColeccion()) {
                if (cita->isConfirmada()) citasConfirmadas++;
            }
            cout << citasConfirmadas << endl;
            
            cout << "5. Total de empleados: " << empleados.size() << endl;
            
            inventario.verificarStockBajo();
            break;
        }
        case 6: 
            cout << "\n========================================\n";
            cout << "    Cerrando sesion. Hasta pronto!     \n";
            cout << "========================================\n";
            return;
        default: 
            cout << "Opcion invalida. Intente nuevamente.\n"; 
            break;
        }
    } while (true);
}

// ==================== FUNCION MAIN ====================
int main() {
    cout << fixed << setprecision(2);
    
    cout << "\n========================================\n";
    cout << "     SISTEMA DE GESTION - BARBERIA      \n";
    cout << "========================================\n";
    cout << "   Configuracion Inicial del Sistema    \n";
    cout << "========================================\n\n";
    
    // Registro inicial de empleados
    registrarEmpleados();
    
    if (empleados.empty()) {
        cout << "\nNo se registraron empleados. El sistema no puede iniciarse.\n";
        return 1;
    }
    
    // Login
    Empleado* sesion = nullptr;
    if (menuLogin(sesion)) {
        menuPrincipal(sesion);
    }
    
    // Liberar memoria
    cout << "\nLiberando recursos del sistema...\n";
    for (auto e : empleados) delete e;
    for (auto c : clientes) delete c;
    // inventario y agenda se liberan automaticamente con sus destructores
    
    cout << "Sistema cerrado exitosamente.\n";
    cout << "========================================\n";
    
    return 0;
}