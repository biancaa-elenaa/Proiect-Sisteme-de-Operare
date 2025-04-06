# 💎 Treasure Hunt Game System – Sisteme de Operare

> **Universitatea Politehnica Timisoara  – Facultatea de Automatica si Calculatoare**  
> Proiect pentru disciplina **Sisteme de Operare**  
> 📅 Anul universitar: 2024-2025

---

## 🧠 Descriere Generală

Acest proiect implementează un **sistem digital de vânătoare de comori** în C, pentru mediul **UNIX/Linux**, folosind funcții și concepte specifice sistemului de operare: lucrul cu fișiere binare, directoare, symlink-uri, logare și apeluri de sistem (`syscalls`).

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

## 🧱 Structura fișierelor și directoarelor

- Comorile sunt salvate într-un fișier binar `treasures.b` în interiorul directorului fiecărui hunt.
- Toate operațiile sunt logate în fișierul text `logged_hunt` din același director.
- Pentru fiecare `logged_hunt`, se creează un symlink la nivelul directorului principal: `logged_hunt-<hunt_id>`

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

## 📁 Exemplu de structură de directoare

```bash
.
├── treasure_manager.c
├── treasure.c
├── treasure.h
├── README.md
├── Hunt1/
│   ├── treasures.b
│   └── logged_hunt
├── logged_hunt-Hunt1 -> Hunt1/logged_hunt
