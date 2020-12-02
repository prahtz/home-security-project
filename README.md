# home-security-project
A simple project for my personal home security.

## Install on Raspbian/Linux
To run this project you will need to install `wiringPi`, `libcurl` with OpenSSH enabled and `libcurlpp`.

If you want to test this program in a general Linux enviroment, do not install `wiringpi` lib.
```
sudo apt update
sudo apt install wiringpi
sudo apt install libcurl4-openssl-dev
sudo apt install libcurlpp-dev
```

Now you can simply run `make` or `make raspbian` to generate the executable file `hsp` in a Raspbian enviroment.

If you are running it from any other Linux distribution, run `make linux`.

## Run
Once you compiled the executable, run this command:

```
hsp <ip_address> <port>
```
