# Proiect - Sisteme de Operare (SO)

## Treasure Hunt Game System

Acest proiect este un program scris în C, care simulează un sistem de vânătoare de comori digital, rulând în mediu UNIX. Proiectul este structurat în mai multe faze, fiecare adăugând funcționalități suplimentare.

---

## 🔍 Faza 1: Sisteme de Fișiere (Săptămânile 6-7)

### 🎯 Scop
Crearea unei baze de date locale pentru jocul de tip "treasure hunt", folosind operații pe fișiere.

---

### 📁 Structură și Funcționalități

Programul `treasure_manager` permite:

- **Crearea și gestionarea de comori într-un fișier binar structurat**
- **Fiecare vânătoare de comori este stocată într-un director separat**
- **Operații disponibile prin linia de comandă:**
  - `--add <hunt_id>` – adaugă o comoară într-o vânătoare
  - `--list <hunt_id>` – listează toate comorile dintr-o vânătoare
  - `--view <hunt_id> <id>` – afișează o comoară specifică
  - `--remove_treasure <hunt_id> <id>` – șterge o comoară specifică
  - `--remove_hunt <hunt_id>` – șterge întreaga vânătoare (directorul)

---

### 📦 Formatul Comorii
- `Treasure ID`: int
- `Username`: text (unic)
- `Coordonate GPS`: float (latitudine și longitudine)
- `Clue`: string
- `Value`: int

---

### 🛠 Operații pe fișiere
- Fișierele sunt stocate în format binar cu structuri de dimensiuni fixe
- Se folosesc apeluri de sistem: `open()`, `read()`, `write()`, `close()`, `lseek()`
- Se folosesc: `stat()` și `mkdir()` pentru informații și directoare

---

### 📑 Log-uri
- Fiecare operație este logată într-un fișier `logged_hunt` din directorul vânătorii
- Se creează un symlink în directorul principal: `logged_hunt-<ID>`

---

### 💻 Exemplu comandă:
```bash
./treasure_manager --remove game7 2
