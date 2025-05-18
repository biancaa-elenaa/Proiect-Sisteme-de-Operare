# 💎 Treasure Hunt Game System – Sisteme de Operare

> **Universitatea Politehnica Timisoara  – Facultatea de Automatica si Calculatoare**  
> Proiect pentru disciplina **Sisteme de Operare**  
> 📅 Anul universitar: 2024-2025

---

## 🧠 Descriere Generală

Acest proiect implementează un **sistem digital de vânătoare de comori** în C, pentru mediul **UNIX/Linux**, folosind funcții și concepte specifice sistemului de operare: lucrul cu fișiere binare, directoare, symlink-uri, logare și apeluri de sistem.

Proiectul este structurat în **faze**, fiecare adăugând noi funcționalități și consolidând conceptele învățate.

---

## 🚀 Faza 1: Sisteme de Fișiere

### 🎯 Obiectiv
Construirea unui manager (`treasure_manager`) care permite crearea, afișarea, modificarea și ștergerea de comori digitale, fiecare asociată unui „hunt” – o sesiune de joc reprezentată de un director.

---

## 🧰 Funcționalități implementate

| Comandă                                   | Descriere                                                                 |
|------------------------------------------|--------------------------------------------------------------------------|
| `--add <hunt_id>`                        | Adaugă o nouă comoară în sesiunea de joc specificată                     |
| `--list <hunt_id>`                       | Listează toate comorile și afișează detalii despre fișierul treasure     |
| `--view <hunt_id> <id>`                  | Afișează detaliile unei comori după ID-ul acesteia                       |
| `--remove_treasure <hunt_id> <id>`       | Șterge comoara cu ID-ul specificat                                       |
| `--remove_hunt <hunt_id>`                | Șterge întreaga sesiune (directorul și toate fișierele aferente)         |

---

### 💻 Exemplu comandă:

```bash
./treasure_manager --remove game7 2
```

## 🧱 Structura fișierelor și directoarelor

- Comorile sunt salvate într-un fișier binar `treasures.b` în interiorul directorului fiecărui hunt.
- Toate operațiile sunt logate în fișierul text `logged_hunt` din același director.
- Pentru fiecare `logged_hunt`, se creează un symlink la nivelul directorului principal: `logged_hunt-<hunt_id>`
- Se folosesc apeluri de sistem: `open()`, `read()`, `write()`, `close()`, `lseek()`
- Se folosesc: `stat()` și `mkdir()` pentru informații și directoare

---

## 📦 Structura unei comori (`Treasure_t`)

| Câmp             | Tip         | Descriere                           |
|------------------|-------------|-------------------------------------|
| `id`             | `int`       | ID unic pentru comoară              |
| `username`       | `char[]`    | Numele utilizatorului               |
| `GPS_latitude`   | `float`     | Coordonată GPS – latitudine         |
| `GPS_longitude`  | `float`     | Coordonată GPS – longitudine        |
| `clue`           | `char[]`    | Textul indiciului                   |
| `value`          | `int`       | Valoarea numerică a comorii         |

---

## 🧩 Faza 2: Procese și Semnale (Săptămânile 8–9)

### 🎯 Obiectiv:
Crearea unui program interactiv `treasure_hub` care controlează un proces „monitor”.

### 🧠 Comenzi acceptate în interfața `treasure_hub`:

| Comandă           | Acțiune                                                                 |
|-------------------|------------------------------------------------------------------------|
| `start_monitor`   | Pornește un proces `monitor` în background                             |
| `list_hunts`      | Cere monitorului să afișeze numele și nr. de comori per hunt           |
| `list_treasures`  | Cere monitorului toate comorile dintr-un hunt                          |
| `view_treasure`   | Cere monitorului o comoară specifică                                   |
| `stop_monitor`    | Trimite semnal să oprească monitorul și așteaptă terminarea lui        |
| `exit`            | Iese doar dacă monitorul este deja oprit, altfel dă eroare             |

### 📡 Comunicare:

- Se folosesc **semnale** (`SIGUSR1`, `SIGCHLD`, etc.)
- Pentru semnale se folosește `sigaction()` (nu `signal()`!)
- Informațiile despre comenzi pot fi transmise prin fișiere

---

## 🔗 Faza 3: Pipe-uri, Redirecționări și Procese Externe (Săptămânile 10–11)

### 🎯 Obiectiv:
Adăugarea de **comunicare prin pipe-uri** și integrarea unui program extern `score_calculator`.

### 🧠 Funcționalități noi:

| Comandă               | Descriere                                                                 |
|------------------------|--------------------------------------------------------------------------|
| `calculate_score`      | Creează un proces pentru fiecare hunt și calculează scorurile per utilizator |

### 🛠️ Detalii tehnice:

- `monitor` **trimite outputul** către `treasure_hub` printr-un **pipe**, nu direct pe ecran
- `score_calculator` poate fi un program C extern sau script
- Se folosește `dup2()` pentru redirecționarea `stdout`
- `treasure_hub` citește outputul fiecărui proces `score_calculator` printr-un pipe
- Outputul conține: scorul total per utilizator (suma valorilor comorilor)

---

## 📦 Structură Finală a Directorului

```bash
.
├── treasure_manager.c
├── treasure_hub.c
├── monitor.c
├── score_calculator.c
├── treasure.c
├── treasure.h
├── Makefile
├── README.md
├── Hunt1/
│   ├── treasures.b
│   └── logged_hunt
├── logged_hunt-Hunt1 -> Hunt1/logged_hunt
