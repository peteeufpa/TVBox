# 🖥️ Gateway  

## Instalação do Mosquitto Broker na TvBox  

Com a TvBox já preparada, execute os seguintes comandos:

---

### 1. Atualize o sistema operacional da TvBox (recomendado)  
```bash
sudo apt update
```

---

### 2. Instale o Mosquitto  
```bash
sudo apt install mosquitto mosquitto-clients
```

---

### 3. Verifique o status do serviço  
```bash
sudo systemctl status mosquitto
```

---

### 4. Configure o usuário e senha para o Mosquitto  
```bash
sudo mosquitto_passwd -c /etc/mosquitto/passwd seu_usuario
```

> **OBS:** Substitua `seu_usuario` pelo nome desejado. Em seguida, digite e confirme a senha para esse usuário.  

---

### 5. Edite o arquivo de configuração do Mosquitto  
```bash
sudo nano /etc/mosquitto/mosquitto.conf
```

Adicione as seguintes linhas no final do arquivo:  
```conf
listener 1883
allow_anonymous false
password_file /etc/mosquitto/passwd
```

Feche o arquivo e salve as modificações: pressione `CTRL+X`, depois `Y` e por fim `Enter`.  

---

### 6. Reinicie o serviço Mosquitto  
```bash
sudo systemctl restart mosquitto
```

> **OBS:** Caso apareça erro ao reiniciar o Mosquitto, use:  
```bash
sudo chown mosquitto:mosquitto /etc/mosquitto/passwd
sudo chmod 600 /etc/mosquitto/passwd
```
Esses comandos autorizam o usuário `mosquitto` a ser dono do arquivo de senhas.  

---

### 7. Faça o teste local  
Abra **dois terminais** no Linux.  

No **primeiro terminal** (ouvinte), digite:  
```bash
mosquitto_sub -h localhost -t "teste/local" -u "seu_usuario" -P "sua_senha" -v
```

No **segundo terminal** (publicador), digite:  
```bash
mosquitto_pub -h localhost -t "teste/local" -u "seu_usuario" -P "sua_senha" -m "Olá, Le Potato!"
```

➡️ No primeiro terminal deve aparecer a frase que você digitou.  

---

### 8. Dados do gateway  

- **Broker:** será o IP da TvBox. Para visualizar, digite:  
  ```bash
  hostname -I
  ```
- **Subscrever:** `teste/local` (definido no `mosquitto_sub`).  
- **Publicar:** `teste/local` (definido no `mosquitto_pub`).  
- **Login:** usuário e senha configurados no passo 4.  
