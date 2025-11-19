# Red Team Projects

This repository contains tools and experiments for offensive security research in controlled environments. The material is intended for study, authorized testing, and professional training.

## Legal Notice

Usage is limited to **authorized** security testing and lab environments.  
Any unauthorized exploitation of systems or data is illegal and entirely the responsibility of the user.

No guarantees are provided regarding stability, safety, or fitness for any purpose.  
The author assumes no liability for misuse, damages, or legal consequences.

## Safe Use

- run only in isolated or virtualized environments;  
- ensure written authorization before testing external systems.

---

## Projects Structure
```
  .
  ├─ README.md
  ├── Advanced_Network/
  ├── Basic/
  │   ├── chat_server/
  │   │   ├── Multithread/
  │   │   │   ├── client.c
  │   │   │   └── server.c
  │   │   ├── TCP/
  │   │   │   ├── client.c
  │   │   │   └── server.c
  │   │   └── UDP/
  │   │       ├── client.c
  │   │       └── server.c
  │   ├── cryptography/
  │   │   ├── monoalphabetic.c
  │   │   └── xor.c     
  │   ├── webshell/
  │   │   └── webshell.c
  │   └── file_transfer/
  │       └── ft.c
  ├── Essential/
  │   └── cryptography/
  │       ├── rsa.c
  │       └── aes.c 
  ├── Payloads/
  ├── Post_exploitation/
  └─ .gitignore
```
## License
This project is licensed under the Apache 2.0 License.
