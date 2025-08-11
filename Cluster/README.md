# 🖥️Cluster
## Sumário
1. [Instalar o virtual box e criar as VMs (máquinas virtuais)](#1-instalar-o-virtual-box-e-criar-as-vms-maquinas-virtuais)
2. [Configurando IPS](#2-configurando-ips)
3. [Anotando os IPs](#3anotando-os-ips)
4. [Atualize o Ubunto das VMs](#4atualize-o-ubunto-das-vms)
5. [Configurando a comunicação via SSH](#5configurando-a-comunicação-via-ssh)
6. [Configurando a pasta compartilhada](#6configurando-a-pasta-compartilhada)
7. [Instalando Pacotes](#7instalando-pacotes)
8. [Instalando o PBS no HeadNode](#8instalando-o-pbs-no-headnote)
9. [Instalando o PBS nos ComputeNodes](#9instalando-o-pbs-no-computenodes)
10. [Criando Filas](#10criando-filas)

## 1. Instalar o virtual box e criar as VMs (máquinas virtuais)
Para baixar o virtual box acesse o site official pelo [link](https://www.virtualbox.org/). Para as máquinas virtuais foi usado o `ubunto-server-24.04.02 LTS`, baixe a imagem de instalação no site official [clicando aqui](https://ubuntu.com/download/server) e crie as máquinas virtuais. Aqui foram usadas 3 máquinas virtuais, uma para o `Head Node` e outras duas para os `Compute Nodes`. 
Um passo muito importante quando se cria uma VM no virtual Box é a escolha da sua configuração de rede. Há várias configurações disponíveis, mas a escolhida e a  “Rede Brigde”.

Para configurar a rede clique na máquina virtual e acesse:
```
Configurações  >  Redes  >  Adaptador 1  >  conectado a : Rede Brigde
```
## 2. Configurando IPs
A comunicação entre os nós acontece a partir do protocolo SSH, que utiliza IPs para comunicação. Normalmente o IP muda a cada login em uma rede devido ao protocolo DHCP, mas para criar um cluster esses IPs não podem mudar. Para deixar o IP estático, desligando o DHCP e configurando o IP manualmente, siga os passos abaixo. 
```
sudo nano /etc/netplan/50-installer-config.yaml
```
**OBS: _Use a interface de rede da sua distribuição Linux (a do ubuntu e a netplan)_**
  
Edite o arquivo replicando a configuração abaixo usando o IP desejado.
```
network:
  version: 2
  renderer: networkd 
  ethernets:
    enp3s0: 
      dhcp4: no  #Desligando o DHCP
      addresses: [192.168.1.100/24]  #IP desejado
      routes:
        - to: default
          via: 192.168.1.1   #IP do seu roteador
      nameservers:
        addresses: [8.8.8.8, 8.8.4.4] 	
```
Depois aplique a configuração e verifique o IP com os comandos abaixo:
```
sudo netplan apply
ip a
```
## 3.	Anotando os IPs 
Para os passos posteriores será necessário conhecer os endereços IPs de todas as máquinas virtuais. Para descobrir qual o IP da sua máquina atual use o comando abaixo:
```
ip a
```
Escolha o seu `Head Node`, caso não tenha escolhido, e anote os `IPs` e os `hostnames` das 3 VMs no arquivo hosts na pasta “etc” usando o comando abaixo. Esse é um passo importante para a configuração do PBS futuramente, vale ressaltar pelos menos o IP e o hostname do Head Node devem estar presentes no mesmo arquivo nos Compute Nodes.
```
sudo nano /etc/hosts
```
Exemplo: 
```
192.168.1.150	HeadNode
192.168.1.151	CompN1
192.168.1.152	CompN2
```
Para salvar aperte `ctrl + O  e enter`.  
Para sair aperte `ctrl + X`.

**OBS: Os hostnames no arquivo do Head Node devem ser os mesmos dos Compute Nodes. Verifique com `hostname` no terminal e caso necessário use `sudo hostnamectl set-hostname novo-hostname` para definir um hostname permanente.**

## 4.	Atualize o Ubunto das VMs
Mesmo baixando a imagem de instalação mais atualizada, ela não possui todas as atualizações de segurança. Então é recomendado sempre atualizar a máquina após a sua criação.
Use os comandos:
```
sudo apt update
sudo apt upgrade
```  

E depois reinicie as VMs com:
```
sudo reboot
```
## 5.	Configurando a comunicação via SSH
`SSH` é um protocolo de rede que permite acesso remoto seguro a computadores e servidores e é por ele que os nós vão se comunicar. Mas antes precisa baixá-lo, baixe em todos os nós.
```
sudo apt install -y openssh-server
```
Para acessar um computador remotamente com o protocolo SSH precisa-se do usuário, endereço de IP e senha. Como os nós precisam se comunicar livremente, gere chaves criptografadas para eles se comunicarem sem a necessidade de senha e de forma segura. No `Head Node` execute os comandos.
```
ssh-keygen -t rsa
```
Com a chave criada compartilhe com os `Compute Nodes`.
```
ssh-copy-id usuarioDoNo@ipDoNoDeComputação
```
## 6.	Configurando a pasta compartilhada
O NFS é um protocolo que permite o compartilhamento de arquivos e diretórios em uma rede. Crie uma pasta compartilhada no Head Node e dê acesso aos Compute Node. É por meio dessa pasta que vamos configurar o PBS e executar os Jobs(trabalhos).
Instale no Head Node o NFS server:
```
sudo apt install -y nfs-kernel-server 
```
Crie a pasta e de permissão aos usuários do Head Node.
```
sudo mkdir /mnt/nfs
sudo chmod -R 777 /mnt/nfs
```
Edite o arquivo exports para configurar o compartilhamento da pasta.
```
sudo nano /etc/exports
```
Replique o exemplo abaixo utilizando o diretório que será compartilhado.
```
 /mnt/nfs	*(rw,sync,no_subtree_check,no_root_squash)
```
Depois de configurar recarregue as exportações e reinicie o servidor NFS.
```
sudo exportfs
sudo systemctl restart nfs-kernel-server
```
Crie um arquivo qualquer, apenas para usar como teste.
```
sudo touch /mnt/nfs/blankfile
```
Agora configure os Compute Nodes. Em cada nó de computação instale o NFS para cliente.
```
sudo apt install -y nfs-common 
```
Crie a pasta nos nós e de a permissão ao usuário principal.
```
sudo mkdir -p /mnt/nfs
sudo chown -R usuario:usuario /mnt/nfs 
```
Monte um diretório compartilhado.
```
sudo mount   x.x.x.x:/mnt/nfs	 /mnt/nfs 
```
Verifique se o arquivo criada no Head Node aparece no diretório compartilhado.
```
 ls /mnt/nfs/
```
Na reinicialização das máquinas a montagem do diretório vai ser perdida. Então faça o Linux realizar a montagem sempre que iniciar.
```
sudo nano /etc/fstab
```

Adicione a linha abaixo no arquivo usando o IP do Head Node.
```
x.x.x.x:/mnt/nfs	/mnt/nfs	nfs	rw	0	0 
```
## 7.	Instalando Pacotes
Antes de instalar o PBS, é necessário instalar vários pacotes como compiladores, bibliotecas e softwares que o PBS precisa para compilar e rodar. Instale esses pacotes no Head Node e em todos os Compute Nodes.
```
sudo apt install -y gcc make libtool libhwloc-dev libx11-dev \
libxt-dev libedit-dev libical-dev ncurses-dev perl \
postgresql-server-dev-all postgresql-contrib python3-dev tcl-dev tk-dev swig \
libexpat-dev libssl-dev libxext-dev libxft-dev autoconf \
automake g++ libcjson-dev
```
Instale também:
```
sudo apt install -y expat libedit2 postgresql python3 postgresql-contrib sendmail-bin \
sudo tcl tk libical3t64 postgresql-server-dev-all
```
**OBS: É comum que o nome de alguns pacotes mude, o que pode fazer os comandos acima mostrarem erro. Caso ocorra algum erro acesse o GITHUB do PBS [clicando aqui](https://github.com/openpbs/openpbs/blob/master/INSTALL).**  

## 8.	Instalando o PBS no HeadNode
O PBS (Portable Batch System) é um sistema de gerenciamento de tarefas que permite aos usuários submeterem e gerenciar trabalhos em um ambiente de computação distribuída. No Head Node siga os passos abaixo:  
Baixe o git.
 ```
sudo apt install git
```
Entre na pasta compartilhada e baixe os diretórios do PBS.
```
cd /mnt/nfs
git clone https://github.com/openpbs/openpbs
```
Agora começa o processo de instalação do PBS. Entre na pasta do PBS, gere o script de instalação e defina o local de instalação.
```
cd openpbs
./autogen.sh
./configure --prefix=/opt/pbs
```
Compile e instale o PBS.
```
make
sudo make install
```
Finalize a instalação.
```
sudo /opt/pbs/libexec/pbs_postinstall
```
Com o PBS instalado agora só falta configurar o arquivo de configuração principal. Entre no arquivo `pbs.conf` e modifique-o como no exemplo abaixo.
`sudo nano /etc/pbs.conf`

Exemplo:
```
PBS_SERVER=admin (Troque o ‘admin’ pelo hostname do Head Node no /etc/hosts)  
PBS_START_SERVER=1 
PBS_START_SCHED=1 
PBS_START_COMM=1 
PBS_START_MOM=0 
PBS_EXEC=/opt/pbs 
PBS_HOME=/var/spool/pbs 
PBS_CORE_LIMIT=unlimited 
PBS_SCP=/usr/bin/scp
```
Execute o chmod abaixo para configurar permissões e só então inicie o PBS.
```
sudo chmod 4755 /opt/pbs/sbin/pbs_iff /opt/pbs/sbin/pbs_rcp
sudo /etc/init.d/pbs start
```
## 9.	Instalando o PBS nos ComputeNodes
Com o PBS já instalado no Head Node usando o código fonte presente na pasta compartilhada, o processo de instalação nos outros nós será menor.
Rode os comandos abaixo para instalar o PBS.
```
cd /mnt/nfs/openpbs
sudo make install 
sudo /opt/pbs/libexec/pbs_postinstall
```
**OBS: Talvez seja necessário criar a pasta pbs (`sudo mkdir /opt/pbs`) e dar permissão para modificada-la (`sudo chmod -R 777 /opt/pbs`).**

Entre no arquivo de configuração do PBS e reproduza a configuração do exemplo abaixo.
`sudo nano /etc/pbs.conf`



Exemplo:
```
PBS_SERVER=admin (Troque o ‘admin’ pelo nome do Head Node no /etc/hosts) PBS_START_SERVER=0 
PBS_START_SCHED=0 
PBS_START_COMM=0 
PBS_START_MOM=1 
PBS_EXEC=/opt/pbs 
PBS_HOME=/var/spool/pbs 
PBS_CORE_LIMIT=unlimited 
PBS_SCP=/usr/bin/scp 
```
Execute o `chmod` abaixo para configurar permissões e depois inicie o PBS.
```
sudo chmod 4755 /opt/pbs/sbin/pbs_iff /opt/pbs/sbin/pbs_rcp 
sudo /etc/init.d/pbs start
```
## 10.	Criando Filas
O PBS é um gerenciador de filas, com o PBS instalado em todos os nós crie as filas para execução dos trabalhos.
Crie uma fila e a defina como padrão.
```
sudo /opt/pbs/bin/qmgr -c "create queue dev queue_type=e, started=t, enabled=t "
sudo /opt/pbs/bin/qmgr -c "set server default_queue=dev "
```

Habilite historico de trabalho e habilite o modo flta UID.
```
sudo /opt/pbs/bin/qmgr -c "set server job_history_enable=true " 
sudo /opt/pbs/bin/qmgr -c "set server flatuid=true"
```
Com as filas criadas adicione os Compute Nodes usando os seus respectivos hostnames presentes no arquivo `/etc/hosts do Head Node`.
```
sudo /opt/pbs/bin/qmgr -c "create node nomeDoNos"
```
Verifique se os nós estão disponíveis com: 
`pbsnodes -a`
