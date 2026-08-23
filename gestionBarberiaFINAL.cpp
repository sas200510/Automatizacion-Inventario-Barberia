//Isaac Espinosa 00342611 - Sebastian Alcoser 00337279
//FINAL PROJECT - BARBERSHOP SYSTEM
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <iomanip>
using namespace std;

//-----Class and CRUD Function Definitions-----

// ==================== CUSTOM EXCEPTIONS ====================
class NotFoundException : public runtime_error {
public:
    NotFoundException(const string& message) : runtime_error(message) {}
};

class ValidationException : public runtime_error {
public:
    ValidationException(const string& message) : runtime_error(message) {}
};

class OverlapException : public runtime_error {
public:
    OverlapException(const string& message) : runtime_error(message) {}
};

// ==================== AUTHENTICABLE INTERFACE ====================
class Autenticable {
public: 
    virtual bool autenticar(const string& username, const string& password) const = 0;
    virtual ~Autenticable() = default;
};

// ==================== ABSTRACT PERSON CLASS ====================
class Persona {
protected: 
    int id;
    string name;
    string phone;
public: 
    Persona(int id, const string& name, const string& phone)
    : id(id), name(name)
{
    setTelefono(phone);
}

    virtual ~Persona() = default;
    virtual string rol() const = 0;
    
    int getId() const { return id; }
    const string& getNombre() const { return name; }
    const string& getTelefono() const { return phone; }
    void setNombre(const string& newName) {
        if (newName.empty()) throw ValidationException("El nombre no puede estar vacio");
        name = newName;
    }
    bool esNumero(const string& value) {
    if (value.empty()) return false;
    for (char character : value) {
        if (character < '0' || character > '9') return false;
    }
    return true;
}

    void setTelefono(const string& newPhone) {
    if (newPhone.empty())
        throw ValidationException("El telefono no puede estar vacio");

    if (!esNumero(newPhone))
        throw ValidationException("El telefono debe contener solo numeros");

    phone = newPhone;
}

};

// ==================== CLIENT CLASS ====================
class Cliente : public Persona {
private:
    string email;
    string notes;
    bool active;
public:
    Cliente(int id, const string& name, const string& phone, const string& email, const string& notes)
        : Persona(id, name, phone), email(email), notes(notes), active(true) {
        validarEmail(email);
    }
    
    string rol() const override { return "Cliente"; }
    
    void validarEmail(const string& emailAddress) {
        bool containsAt = false;
        for (char character : emailAddress) {
            if (character == '@') {
                containsAt = true;
                break;
            }
        }
        if (!containsAt) throw ValidationException("Email invalido: debe contener @");
    }
    
    const string& getEmail() const { return email; }
    const string& getNotas() const { return notes; }
    bool isActivo() const { return active; }
    
    void setEmail(const string& newEmail) {
        validarEmail(newEmail);
        email = newEmail;
    }
    void setNotas(const string& newNotes) { notes = newNotes; }
    void setActivo(bool isActive) { active = isActive; }
    
    string toString() const {
         return "Cliente ID: " + to_string(id) + ", Nombre: " + name +
             ", Email: " + email + ", Telefono: " + phone +
             ", Activo: " + (active ? "Si" : "No");
    }
};

// ==================== EMPLOYEE CLASS ====================
class Empleado : public Persona, public Autenticable {
private: 
    string username;
    string password;
    string position;
public: 
        Empleado(int id, const string& name, const string& phone,
                         const string& username, const string& password, const string& position)
                : Persona(id, name, phone), username(username),
                    password(password), position(position) {}
    
    bool autenticar(const string& user, const string& pass) const override {
        return user == username && pass == password;
    }
    
    string rol() const override { return "Empleado"; }
    const string& getCargo() const { return position; }
    const string& getUsuario() const { return username; }
};

// ==================== ABSTRACT ITEM CLASS ====================
class Item {
protected:
    int id;
    string name;
    double price;
public:
    Item(int id, const string& name, double price)
        : id(id), name(name), price(price) {
        if (price < 0) throw ValidationException("El precio no puede ser negativo");
    }
    virtual ~Item() = default;
    
    virtual double precioFinal() const = 0;
    virtual string toString() const = 0;
    
    int getId() const { return id; }
    const string& getNombre() const { return name; }
    double getPrecio() const { return price; }
    
    void setNombre(const string& newName) {
        if (newName.empty()) throw ValidationException("El nombre no puede estar vacio");
        name = newName;
    }
    void setPrecio(double newPrice) {
        if (newPrice < 0) throw ValidationException("El precio no puede ser negativo");
        price = newPrice;
    }
};

// ==================== PRODUCT CLASS ====================
class Producto : public Item {
private:
    int stock;
public:
    Producto(int id, const string& name, double price, int stock)
        : Item(id, name, price), stock(stock) {
        if (stock < 0) throw ValidationException("El stock no puede ser negativo");
    }
    
    double precioFinal() const override { return price; }
    
    string toString() const override {
        return "Producto ID: " + to_string(id) + ", Nombre: " + name +
             ", Precio: $" + to_string(price) + ", Stock: " + to_string(stock);
    }
    
    int getStock() const { return stock; }
    void setStock(int newStock) {
        if (newStock < 0) throw ValidationException("El stock no puede ser negativo");
        stock = newStock;
    }
};

// ==================== SERVICE CLASS ====================
class Servicio : public Item {
private:
    int durationMinutes;
public:
    Servicio(int id, const string& name, double price, int durationMinutes)
        : Item(id, name, price), durationMinutes(durationMinutes) {
        if (durationMinutes <= 0) throw ValidationException("La duracion debe ser positiva");
    }
    
    double precioFinal() const override { return price; }
    
    string toString() const override {
        return "Servicio ID: " + to_string(id) + ", Nombre: " + name +
             ", Precio: $" + to_string(price) + ", Duracion: " + to_string(durationMinutes) + " min";
    }
    
    int getDuracionMin() const { return durationMinutes; }
    void setDuracionMin(int newDuration) {
        if (newDuration <= 0) throw ValidationException("La duracion debe ser positiva");
        durationMinutes = newDuration;
    }
};

// ==================== APPOINTMENT CLASS ====================
class Cita {
private:
    int id;
    Cliente* client;
    Servicio* service;
    Empleado* employee;
    string dateTime;
    bool confirmed;
public:
        Cita(int id, Cliente* client, Servicio* service, Empleado* employee, const string& dateTime)
                : id(id), client(client), service(service), employee(employee),
                    dateTime(dateTime), confirmed(true) {}
    
    int getId() const { return id; }
    Cliente* getCliente() const { return client; }
    Servicio* getServicio() const { return service; }
    Empleado* getEmpleado() const { return employee; }
    const string& getFechaHora() const { return dateTime; }
    bool isConfirmada() const { return confirmed; }
    
    void setFechaHora(const string& newDateTime) { dateTime = newDateTime; }
    void setConfirmada(bool isConfirmed) { confirmed = isConfirmed; }
    
    string toString() const {
        return "Cita ID: " + to_string(id) + 
               ", Cliente: " + client->getNombre() +
               ", Servicio: " + service->getNombre() +
               ", Empleado: " + employee->getNombre() +
               ", Fecha/Hora: " + dateTime +
               ", Estado: " + (confirmed ? "Confirmada" : "Cancelada");
    }
};

// ==================== MANAGER TEMPLATE CLASS ====================
template <typename T>
class Gestor {
protected:
    vector<T*> collection;
public:
    virtual ~Gestor() {
        for (auto item : collection) {
            delete item;
        }
    }
    
    void agregar(T* item) {
        collection.push_back(item);
    }
    
    T* buscar(int id) {
        for (auto item : collection) {
            if (item->getId() == id) {
                return item;
            }
        }
        throw NotFoundException("No se encontro el elemento con ID: " + to_string(id));
    }
    
    void listar() const {
        if (collection.empty()) {
            cout << "No hay elementos registrados.\n";
            return;
        }
        for (const auto& item : collection) {
            cout << item->toString() << endl;
        }
    }
    
    const vector<T*>& getColeccion() const { return collection; }
};

// ==================== INVENTORY CLASS ====================
class Inventario : public Gestor<Item> {
public:
    void editar(int id) {
        try {
            Item* item = buscar(id);
            double newPrice;
            cout << "Nuevo precio: ";
            cin >> newPrice;
            item->setPrecio(newPrice);
            
            Producto* producto = dynamic_cast<Producto*>(item);
            if (producto) {
                int newStock;
                cout << "Nuevo stock: ";
                cin >> newStock;
                producto->setStock(newStock);
            }
            
            cout << "Item actualizado exitosamente.\n";
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
        }
    }
    
    void eliminar(int id) {
        try {
            buscar(id);
            for (size_t i = 0; i < collection.size(); ++i) {
                if (collection[i]->getId() == id) {
                    delete collection[i];
                    collection.erase(collection.begin() + i);
                    cout << "Item eliminado exitosamente.\n";
                    return;
                }
            }
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
        }
    }
    
    void buscarPorNombre(const string& name) const {
        bool found = false;
        for (const auto& item : collection) {
            if (item->getNombre().find(name) != string::npos) {
                cout << item->toString() << endl;
                found = true;
            }
        }
        if (!found) {
            cout << "No se encontraron items con ese nombre.\n";
        }
    }
    
    void verificarStockBajo() const {
        cout << "\n=== PRODUCTOS CON STOCK BAJO (<5) ===\n";
        bool hasLowStock = false;
        for (const auto& item : collection) {
            Producto* product = dynamic_cast<Producto*>(item);
            if (product && product->getStock() < 5) {
                cout << "ALERTA: " << product->toString() << endl;
                hasLowStock = true;
            }
        }
        if (!hasLowStock) {
            cout << "Todos los productos tienen stock suficiente.\n";
        }
    }
};

// ==================== SCHEDULE CLASS ====================
class Agenda : public Gestor<Cita> {
public:
    void agendar(Cita* appointment) {
        try {
            confirmaDisponibilidad(appointment);
            agregar(appointment);
            cout << "Cita agendada exitosamente con ID: " << appointment->getId() << endl;
        } catch (const exception& e) {
            cout << "Error al agendar: " << e.what() << endl;
            delete appointment;
        }
    }
    
    void confirmaDisponibilidad(Cita* newAppointment) {
        for (const auto& appointment : collection) {
            if (appointment->getEmpleado()->getId() == newAppointment->getEmpleado()->getId() &&
                appointment->getFechaHora() == newAppointment->getFechaHora() &&
                appointment->isConfirmada()) {
                throw OverlapException("El empleado ya tiene una cita en ese horario");
            }
        }
    }
    
    void reprogramar(int id) {
        try {
            Cita* cita = buscar(id);
            string newDateTime;
            cout << "Nueva fecha/hora (formato: DD/MM/YYYY HH:MM): ";
            cin.ignore();
            getline(cin, newDateTime);
            cita->setFechaHora(newDateTime);
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
        bool found = false;
        for (const auto& appointment : collection) {
            if (appointment->getEmpleado()->getId() == empId && appointment->isConfirmada()) {
                cout << appointment->toString() << endl;
                found = true;
            }
        }
        if (!found) {
            cout << "No hay citas para ese empleado.\n";
        }
    }
};

// ==================== GLOBAL VECTORS ====================
vector<Cliente*> clients;
vector<Empleado*> employees;
Inventario inventory;
Agenda schedule;
int nextAppointmentId = 1;

// ==================== CLIENT CRUD FUNCTIONS ====================
void registrarCliente() {
    try {
        int id;
        string name, phone, email, notes;
        
        cout << "ID unico: ";
        cin >> id;
        
        for (auto client : clients) {
            if (client->getId() == id) {
                throw ValidationException("El ID ya existe");
            }
        }
        
        cin.ignore();
        cout << "Nombre: ";
        getline(cin, name);
        if (name.empty()) throw ValidationException("El nombre no puede estar vacio");
        
        cout << "Telefono: ";
        getline(cin, phone);
        if (phone.empty()) throw ValidationException("El telefono no puede estar vacio");
        
        cout << "Email: ";
        getline(cin, email);
        
        cout << "Notas: ";
        getline(cin, notes);
        
        clients.push_back(new Cliente(id, name, phone, email, notes));
        cout << "Cliente registrado exitosamente.\n";
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}

void buscarCliente() {
    string searchTerm;
    cout << "Buscar por nombre o ID: ";
    cin.ignore();
    getline(cin, searchTerm);
    
    bool found = false;
    for (auto client : clients) {
        if (client->getNombre().find(searchTerm) != string::npos ||
            to_string(client->getId()) == searchTerm) {
            cout << client->toString() << endl;
            found = true;
        }
    }
    if (!found) {
        cout << "No se encontraron clientes.\n";
    }
}

void editarCliente() {
    try {
        int id;
        cout << "ID del cliente a editar: ";
        cin >> id;
        
        Cliente* client = nullptr;
        for (auto storedClient : clients) {
            if (storedClient->getId() == id) {
                client = storedClient;
                break;
            }
        }
        
        if (!client) throw NotFoundException("Cliente no encontrado");
        
        string newPhone, newNotes, newEmail;
        cin.ignore();
        cout << "Nuevo telefono (actual: " << client->getTelefono() << "): ";
        getline(cin, newPhone);
        cout << "Nuevo email (actual: " << client->getEmail() << "): ";
        getline(cin, newEmail);
        cout << "Nuevas notas: ";
        getline(cin, newNotes);
        
        client->setTelefono(newPhone);
        client->setEmail(newEmail);
        client->setNotas(newNotes);
        
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
        
        Cliente* client = nullptr;
        for (auto storedClient : clients) {
            if (storedClient->getId() == id) {
                client = storedClient;
                break;
            }
        }
        
        if (!client) throw NotFoundException("Cliente no encontrado");
        
        client->setActivo(false);
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
        
        Cliente* client = nullptr;
        for (auto storedClient : clients) {
            if (storedClient->getId() == id) {
                client = storedClient;
                break;
            }
        }
        
        if (!client) throw NotFoundException("Cliente no encontrado");
        
        client->setActivo(true);
        cout << "Cliente nuevamente activo. Su historial se conserva.\n";
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}

void listarClientes() {
    if (clients.empty()) {
        cout << "No hay clientes registrados.\n";
        return;
    }
    cout << "\n=== LISTA DE CLIENTES ===\n";
    for (auto client : clients) {
        if (client->isActivo()) {
            cout << client->toString() << endl;
        }
    }
}

// ==================== PRODUCT CRUD FUNCTIONS ====================
void registrarProducto() {
    try {
        int id, stock;
        string name;
        double price;
        
        cout << "ID unico: ";
        cin >> id;
        
        try {
            inventory.buscar(id);
            throw ValidationException("El ID ya existe");
        } catch (const NotFoundException&) {
            // ID does not exist, so we can continue.
        }
        
        cin.ignore();
        cout << "Nombre: ";
        getline(cin, name);
        cout << "Precio: ";
        cin >> price;
        cout << "Stock: ";
        cin >> stock;
        
        inventory.agregar(new Producto(id, name, price, stock));
        cout << "Producto registrado exitosamente.\n";
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}

void registrarServicio() {
    try {
        int id, duration;
        string name;
        double price;
        
        cout << "ID unico: ";
        cin >> id;
        
        try {
            inventory.buscar(id);
            throw ValidationException("El ID ya existe");
        } catch (const NotFoundException&) {
            // ID does not exist, so we can continue.
        }
        
        cin.ignore();
        cout << "Nombre: ";
        getline(cin, name);
        cout << "Precio: ";
        cin >> price;
        cout << "Duracion (minutos): ";
        cin >> duration;
        
        inventory.agregar(new Servicio(id, name, price, duration));
        cout << "Servicio registrado exitosamente.\n";
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}

// ==================== APPOINTMENT CRUD FUNCTIONS ====================
void agendarCita() {
    try {
        if (clients.empty()) {
            cout << "No hay clientes registrados.\n";
            return;
        }
        if (employees.empty()) {
            cout << "No hay empleados registrados.\n";
            return;
        }
        
        int clientId, serviceId, employeeId;
        string dateTime;
        
        cout << "\n=== CLIENTES DISPONIBLES ===\n";
        listarClientes();
        cout << "ID del cliente: ";
        cin >> clientId;
        
        Cliente* client = nullptr;
        for (auto clientItem : clients) {
            if (clientItem->getId() == clientId && clientItem->isActivo()) {
            client = clientItem;
                break;
            }
        }
        if (!client) throw NotFoundException("Cliente no encontrado o inactivo");
        
        cout << "\n=== SERVICIOS DISPONIBLES ===\n";
        for (const auto& item : inventory.getColeccion()) {
            Servicio* service = dynamic_cast<Servicio*>(item);
            if (service) {
                cout << service->toString() << endl;
            }
        }
        cout << "ID del servicio: ";
        cin >> serviceId;
        
        Item* item = inventory.buscar(serviceId);
        Servicio* service = dynamic_cast<Servicio*>(item);
        if (!service) throw ValidationException("El ID no corresponde a un servicio");
        
        cout << "\n=== EMPLEADOS DISPONIBLES ===\n";
        for (auto employee : employees) {
            cout << "ID: " << employee->getId() << ", Nombre: " << employee->getNombre()
                 << ", Cargo: " << employee->getCargo() << endl;
        }
        cout << "ID del empleado: ";
        cin >> employeeId;
        
        Empleado* employee = nullptr;
        for (auto employeeItem : employees) {
            if (employeeItem->getId() == employeeId) {
            employee = employeeItem;
                break;
            }
        }
        if (!employee) throw NotFoundException("Empleado no encontrado");
        
        cin.ignore();
        cout << "Fecha/hora (formato: DD/MM/YYYY HH:MM): ";
        getline(cin, dateTime);
        
        Cita* newAppointment = new Cita(nextAppointmentId++, client, service, employee, dateTime);
        schedule.agendar(newAppointment);
        
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}

void registrarEmpleados() {
    int quantity;
    cout << "Cuantos empleados desea registrar?: ";
    cin >> quantity;

    for (int index = 0; index < quantity; ++index) {
        try {
            int id;
            string name, phone, username, password, position;

            cout << "\n=== Empleado " << (index + 1) << " ===\n";
            cout << "ID: ";
            cin >> id;
            
            for (auto employee : employees) {
                if (employee->getId() == id) {
                    throw ValidationException("El ID ya existe");
                }
            }
            
            cin.ignore();
            cout << "Nombre: ";
            getline(cin, name);
            cout << "Telefono: ";
            getline(cin, phone);
            cout << "Usuario: ";
            getline(cin, username);
            cout << "Contrasena: ";
            getline(cin, password);
            cout << "Cargo: ";
            getline(cin, position);

            employees.push_back(new Empleado(id, name, phone, username, password, position));
            cout << "Empleado registrado exitosamente.\n";
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
            --index;
        }
    }
}

//-----MENUS AND MAIN-----

// ==================== SYSTEM MENUS ====================

void menuClientes() {
    int option;
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
        cin >> option;

        switch (option) {
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
    int option;
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
        cin >> option;

        switch (option) {
        case 1: registrarProducto(); break;
        case 2: 
            cout << "\n=== INVENTARIO COMPLETO ===\n";
            inventory.listar();
            break;
        case 3: {
            string name;
            cout << "Nombre del producto a buscar: ";
            cin.ignore();
            getline(cin, name);
            inventory.buscarPorNombre(name);
            break;
        }
        case 4: inventory.verificarStockBajo(); break;
        case 5: {
            int id;
            cout << "ID del producto a editar: ";
            cin >> id;
            inventory.editar(id);
            break;
        }
        case 6: {
            int id;
            cout << "ID del producto a eliminar: ";
            cin >> id;
            inventory.eliminar(id);
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
    int option;
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
        cin >> option;

        switch (option) {
        case 1: registrarServicio(); break;
        case 2: {
            cout << "\n=== CATALOGO DE SERVICIOS ===\n";
            bool hasServices = false;
            for (const auto& item : inventory.getColeccion()) {
                Servicio* service = dynamic_cast<Servicio*>(item);
                if (service) {
                    cout << service->toString() << endl;
                    hasServices = true;
                }
            }
            if (!hasServices) {
                cout << "No hay servicios registrados.\n";
            }
            break;
        }
        case 3: {
            string name;
            cout << "Nombre del servicio a buscar: ";
            cin.ignore();
            getline(cin, name);
            bool found = false;
            for (const auto& item : inventory.getColeccion()) {
                Servicio* service = dynamic_cast<Servicio*>(item);
                if (service && service->getNombre().find(name) != string::npos) {
                    cout << service->toString() << endl;
                    found = true;
                }
            }
            if (!found) {
                cout << "No se encontraron servicios con ese nombre.\n";
            }
            break;
        }
        case 4: {
            int id;
            cout << "ID del servicio a editar: ";
            cin >> id;
            try {
                Item* item = inventory.buscar(id);
                Servicio* servicio = dynamic_cast<Servicio*>(item);
                if (!servicio) {
                    cout << "El ID no corresponde a un servicio.\n";
                } else {
                    double newPrice;
                    int newDuration;
                    cout << "Nuevo precio: ";
                    cin >> newPrice;
                    cout << "Nueva duracion (minutos): ";
                    cin >> newDuration;
                    servicio->setPrecio(newPrice);
                    servicio->setDuracionMin(newDuration);
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
            inventory.eliminar(id);
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
    int option;
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
        cin >> option;

        switch (option) {
        case 1: agendarCita(); break;
        case 2: 
            cout << "\n=== TODAS LAS CITAS ===\n";
            schedule.listar();
            break;
        case 3: {
            int employeeId;
            cout << "ID del empleado: ";
            cin >> employeeId;
            cout << "\n=== CITAS DEL EMPLEADO " << employeeId << " ===\n";
            schedule.listarPorEmpleado(employeeId);
            break;
        }
        case 4: {
            int appointmentId;
            cout << "ID de la cita a reprogramar: ";
            cin >> appointmentId;
            schedule.reprogramar(appointmentId);
            break;
        }
        case 5: {
            int appointmentId;
            cout << "ID de la cita a cancelar: ";
            cin >> appointmentId;
            schedule.cancelar(appointmentId);
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

bool menuLogin(Empleado*& activeSession) {
    string user, pass;
    int attempts = 0;
    const int MAX_ATTEMPTS = 3;
    
    cout << "\n========================================\n";
    cout << "     SISTEMA DE GESTION - BARBERIA      \n";
    cout << "========================================\n";
    cout << "           INICIO DE SESION             \n";
    cout << "========================================\n";
    
    while (attempts < MAX_ATTEMPTS) {
        cout << "Usuario: ";
        cin >> user;
        cout << "Contrasena: ";
        cin >> pass;

        for (auto employee : employees) {
            if (employee->autenticar(user, pass)) {
                activeSession = employee;
                cout << "\n========================================\n";
                cout << "    BIENVENIDO AL SISTEMA, " << employee->getNombre() << "\n";
                cout << "    Rol: " << employee->rol() << " (" << employee->getCargo() << ")\n";
                cout << "========================================\n";
                return true;
            }
        }
        
        attempts++;
        cout << "Credenciales incorrectas. ";
        if (attempts < MAX_ATTEMPTS) {
            cout << "Intentos restantes: " << (MAX_ATTEMPTS - attempts) << endl;
        }
    }
    
    cout << "\nNumero maximo de intentos alcanzado. Acceso denegado.\n";
    return false;
}

void menuPrincipal(Empleado* activeSession) {
    int option;
    do {
        cout << "\n========================================\n";
        cout << "           MENU PRINCIPAL               \n";
        cout << "========================================\n";
        cout << " Usuario: " << activeSession->getNombre() << " (" << activeSession->getCargo() << ")\n";
        cout << "========================================\n";
        cout << "1. Gestion de Clientes\n";
        cout << "2. Gestion de Productos\n";
        cout << "3. Gestion de Servicios\n";
        cout << "4. Gestion de Citas\n";
        cout << "5. Reportes y Consultas\n";
        cout << "6. Cerrar sesion\n";
        cout << "========================================\n";
        cout << "Seleccione una opcion: ";
        cin >> option;

        switch (option) {
        case 1: menuClientes(); break;
        case 2: menuProductos(); break;
        case 3: menuServicios(); break;
        case 4: menuCitas(); break;
        case 5: {
            cout << "\n=== REPORTES Y CONSULTAS ===\n";
            cout << "1. Total de clientes activos: ";
            int activeClients = 0;
            for (auto client : clients) {
                if (client->isActivo()) activeClients++;
            }
            cout << activeClients << endl;
            
            cout << "2. Total de productos en inventario: ";
            int totalProducts = 0;
            for (const auto& item : inventory.getColeccion()) {
                if (dynamic_cast<Producto*>(item)) totalProducts++;
            }
            cout << totalProducts << endl;
            
            cout << "3. Total de servicios disponibles: ";
            int totalServices = 0;
            for (const auto& item : inventory.getColeccion()) {
                if (dynamic_cast<Servicio*>(item)) totalServices++;
            }
            cout << totalServices << endl;
            
            cout << "4. Total de citas agendadas: ";
            int confirmedAppointments = 0;
            for (const auto& appointment : schedule.getColeccion()) {
                if (appointment->isConfirmada()) confirmedAppointments++;
            }
            cout << confirmedAppointments << endl;
            
            cout << "5. Total de empleados: " << employees.size() << endl;
            
            inventory.verificarStockBajo();
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

// ==================== MAIN FUNCTION ====================
int main() {
    cout << fixed << setprecision(2);
    
    cout << "\n========================================\n";
    cout << "     SISTEMA DE GESTION - BARBERIA      \n";
    cout << "========================================\n";
    cout << "   Configuracion Inicial del Sistema    \n";
    cout << "========================================\n\n";
    
    // Initial employee registration
    registrarEmpleados();
    
    if (employees.empty()) {
        cout << "\nNo se registraron empleados. El sistema no puede iniciarse.\n";
        return 1;
    }
    
    // Login
    Empleado* activeSession = nullptr;
    if (menuLogin(activeSession)) {
        menuPrincipal(activeSession);
    }
    
    // Release memory
    cout << "\nLiberando recursos del sistema...\n";
    for (auto employee : employees) delete employee;
    for (auto client : clients) delete client;
    // Inventory and schedule are released automatically by their destructors.
    
    cout << "Sistema cerrado exitosamente.\n";
    cout << "========================================\n";
    
    return 0;
}