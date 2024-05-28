All taken and re-written from OpenSSL's example code : 

https://github.com/openssl/openssl/blob/master/demos/sslecho/main.c#L139



Steps to generate self signed cert/pem key files 
(You can skip all values besides 'common name', enter 'localhost' or something like that)

```
openssl req -newkey rsa:4096 -x509 -sha256 -days 3650 -nodes -out cert.pem -keyout key.pem

```