# PASSO A PASSO DA DESCARACTERIZAÇÃO DA TVBox

#H6+

1° Passo - Baixar a imagem presente na pasta H6+ (https://ufpabr-my.sharepoint.com/:f:/r/personal/andre_carvalho_silva_itec_ufpa_br/Documents/Descaracteriza%C3%A7%C3%A3o%20da%20TV%20Box?csf=1&web=1&e=GPrZaO).

2° Passo - Utilizar o Balena Etcher para passar a imagem para um cartão micro SD.

3° Passo - Inserir o cartão micro SD na TV Box H6+. No entanto, para que a mesma inicialize lendo o cartão, é necessário pressionar o botão traseiro (Por cerca de 5s ou até aparecer a frase 'Bring You Home TV', solte o botão logo em seguida) enquanto liga a TV Box.

4° Passo - Efetuar as configurações iniciais do server (Só seguir o que é mostrado).

5° Passo - Para instalar a interface gráfica e configurar o teclado, utilize os seguintes comandos:

PARA CONFIGURAR O TECLADO

          sudo armbian-config

          Escolha a opção 'PERSONAL', em seguida 'KEYBOARD', procure pela opção 'Notebook ASUS' e selecione a linguagem 'Portuguẽs (Brasil).
          (PARA AS PROXIMAS OPÇÕES QUE APARECERÃO, APENAS CONFIRME SEM MUDAR NADA!).
           
PARA A INTERFACE GRÁFICA
           
          sudo apt update
          sudo apt ugrade
          sudo apt install xorg
            cd /etc/X11
            sudo rm xorg.conf
            sudo wget https://www.lnrd.dev/home/tvbox/h6v1/xorg.conf

        INSTALANDO A INTERFACE E ADICIONAIS (Leitor de PDFs, Captura de tela etc..)
          (INTERFACE)
          sudo apt install task-xfce-desktop xfce4-statusnotifier-plugin network-manager-gnome 
          (ADICIONAIS)
          sudo apt install thunar-volman gvfs policykit-1 xfce4-screenshooter file-roller qpdfview ristretto ttf-mscorefonts-installer catfish thunar-archive-plugin

6° Passo - Execurtar os comandos encontrados no arquivo 'Comandos TV Box.txt (Se necessário).

7° Passo - Para gravar na memória tudo o que foi feito e não depender mais do cartão SD, execute o comando 'sudo armbian-install'.

          Arquivo u-boot a ser utilizado (arquivo meson-gxl-s905x-p212.dtb) – opção 106.
          Sistema de arquivo a ser utilizado, sugere-se o ext4.
          
   Após o termino da transferẽncia, execute o seguinte comando:

          sudo shutdown -h now
