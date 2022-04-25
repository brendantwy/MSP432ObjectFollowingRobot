# csc2003-2021-Project-P1_7

Insert SD Card to PC and create 
wpa_supplicant.conf in boot directory
```
country=sg
update_config=1
ctrl_interface=/var/run/wpa_supplicant

network={
 scan_ssid=1
 ssid="MyNetworkSSID"
 psk="Pa55w0rd1234"
}
```


Install Dependencies
```
pip install opencv-python
sudo apt-get install libcblas-dev
sudo apt-get install libhdf5-dev
sudo apt-get install libhdf5-serial-dev
sudo apt-get install libatlas-base-dev
sudo apt-get install libjasper-dev 
sudo apt-get install libqtgui4 
sudo apt-get install libqt4-test
pip install -U numpy
```


System configuration required to be done to allow uart to send information
```
sudo nano /boot/config.txt
#Disable Bluetooth
dtoverlay=disable-bt

sudo systemctl disable hciuart.service
sudo systemctl disable bluealsa.service
sudo systemctl disable bluetooth.service

#Apply changes 
sudo reboot
```

VncServer password  : Password1


SSH to PI
```
ssh pi@brendan.local
Password1!
```

